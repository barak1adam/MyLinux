#!/bin/sh -x
PS4='$LINENO: '

DEBUG=0

#
# Upgrading script for Prestera.
#
FLASH_TMP_DIR=/flash_tmp_dir
TMPDIR=/tmp
OTHER_PARTTION_FLASHDIR=/tmp/backup
HOST_NAME="dummyHost"

ERRFILE=upgrade.err
REMOVE_FILE_LIST="*.cramfs *.squashfs *.jffs2 merge_p.sh mupgrade_p.sh product.conf"
REMOTE_FILE=""
REMOTE_DIR=""
HOSTNAME=""
USERNAME=""
PASSWORD=""
BACKUP_IS_OLD="no"
SCRIPT_DIR=`dirname $0`
SCRIPT_NAME=`basename $0`
ACTION_TEXT="Software upgrade"
GET_SCP_SCRIPT=$SCRIPT_DIR/get_scp.sh
GET_FTP_SCRIPT=$SCRIPT_DIR/get_ftp.sh

LOCAL_ETC_DIR=/usr/local/etc
SYS_CONF=$LOCAL_ETC_DIR/System.conf

# ERRORS FROM PUPGRADE_P.SH:
############################
NONE_ERROR=0
OPERATION_NOT_PERMITTED_ERROR=2
INVALID_BOOT_PARTITION_ERROR=3
MERGE_SCRIPT_FAILURE_ERROR=4
MERGE_SCRIPT_MISSING_ERROR=5
INVALID_ACTION_ERROR=6
MISSING_PARAMETERS_ERROR=7
 SERVER_UNAVAILABLE_ERROR=8
# CANNOT_GET_GATEWAY_ERROR=9
# CANNOT_GET_NETWORK_MASK_ERROR=10
INVALID_GET_METHOD_ERROR=11
FILE_TRANSFER_FAILURE_ERROR=12
INVALID_SOFTWARE_VERSION_TYPE_ERROR=13
MUPGRADE_SCRIPT_MISSING_ERROR=14
#COPY_FAILURE=17
MOUNT_FAILURE=18
MKDIR_FAILURE=19

# ERRORS FROM GET-CONF.SH:
# ERRORS FROM PUT-CONF.SH:
##########################
# INVALID_CONFIG_FILE_TYPE_ERROR=15
# MISSING_REMOTE_USER_PARAMETER_ERROR=16

# ERRORS FROM GENERIC SOURCE:
#############################
LOGIN_FAILURE_ERROR=17
NO_SUCH_FILE_ERROR=18

# ERRORS FROM UPGRADE.C:
########################
# CANNOT_SET_BOOTPART_ERROR=19
# CANNOT_GET_BOOTPART_ERROR=20
# RESET_FAILURE_ERROR=21
# POST_RESET_FAILURE_ERROR=22
# WRONG_UPGR_TYPE_ERROR=23
# EMPTY_FILE_NAME_ERROR=24
# UNKNOWN_ERROR=25


# startProcessFailed              (26),
# getPartitionToogleFlagFailed    (27),
# setPartitionToogleFlagFailed    (28),
# tooLongCommandError             (29),
# backupCurrentStartupConfigError (30),
# unsupportedAppProtocol          (31),
# invalidFileFormat               (32),
# writeRunningConfigFileFailed    (33),

NO_SUCH_FILE_OR_DIRECTOTY_STR="No such file or directory"

#   Definitions for "Check / Clean File Transfer"
TMPDIR_DEFAULT_SIZE=""
FILE_TRANSFER_IN_PROCESS="file_transfer_in_process.tmp"
TRANSFERRED_BYTES_FILENAME="transferred_bytes.tmp"

TRANSFER_FINISHED=0
TRANSFER_IN_PROCESS=1
TRANSFER_HANGED=2
INVALID_OPERATION=3
INVALID_PARAMETERS=4
INVALID_TRANSFER_PROTOCOL=5
TRANSFER_KILLED=6

# Set logs:
###########
LOG_ERR="logger -t $SCRIPT_NAME -p daemon.err  "
LOG_WRN="logger -t $SCRIPT_NAME -p daemon.warn "
LOG_INF="logger -t $SCRIPT_NAME -p daemon.info "
LOG_DBG="logger -t $SCRIPT_NAME -p daemon.debug"


Usage()
{
  USAGE="
Usage:
######
$SCRIPT_NAME (merge|bootpart|backupver)
or
$SCRIPT_NAME (fileTransferCheck|fileTransferClean) <TRANSFERRING_FILE_NAME> (ftp|scp)
or
$SCRIPT_NAME (ftpupgrade|tftpupgrade|sftpupgrade|scpupgrade|fileupgrade) <SERVER-IP> <REMOTE-DIR> <REMOTE-FILE> <DID-BOOTPART-TOGGLE> <IS-INTERACTIVE> [USERNAME] [PASSWORD]"

  echo "$USAGE" | $LOG_ERR
  echo "$USAGE"
  
  exit $1
}

CheckTarStatus ()
{
  rc=`cat $1`
  invalid_file_format=`grep "Invalid gzip magic" $1`

  rm -f $1

  if [ -z "$rc" ]; then
    $LOG_INF "$METHOD stage finished successfully"
###    echo "$METHOD stage finished successfully"
  else
    $LOG_ERR "$ACTION_TEXT failed - untar file failed: $rc"
    if [ -n "$invalid_file_format" ]; then
      rm -rf $FLASH_TMP_DIR 2>/dev/null	
      exit $INVALID_FILE_FORMAT
    fi
    rm -rf $FLASH_TMP_DIR 2>/dev/null	
    exit $FILE_TRANSFER_FAILURE_ERROR
  fi
}

CheckTarStatusString ()
{
  rc="$*"
  invalid_file_format=`echo $rc| grep "Invalid gzip magic"`

  if [ -z "$rc" ]; then
    $LOG_INF "$METHOD stage finished successfully"
###    echo "$METHOD stage finished successfully"
  else
    $LOG_ERR "$ACTION_TEXT failed - untar file failed: $rc"
    if [ -n "$invalid_file_format" ]; then
      rm -rf $FLASH_TMP_DIR 2>/dev/null	
      exit $INVALID_FILE_FORMAT
    fi
    rm -rf $FLASH_TMP_DIR 2>/dev/null	
    exit $FILE_TRANSFER_FAILURE_ERROR
  fi
}



IncreaseTmpDirectorySize ()
{
  TMPDIR_DEFAULT_SIZE=`grep /tmp /etc/fstab | cut -d= -f2 | cut -d, -f1`
  if grep -qs nand_mtd /proc/cmdline; then
    TMPDIR_NEW_SIZE="45m"
    ARM_KW="1"
    echo "This process might take approximately 5 minutes to finish"
  else
    TMPDIR_NEW_SIZE="60m"
    ARM_KW="0"
  fi

  if [ "$ARM_KW" == "1" ]; then
    
	#ran - no need to delete has it was deleted in restore function
        #if [ -d "$OTHER_PARTTION_FLASHDIR" ]; then
  	#  rm -rf $OTHER_PARTTION_FLASHDIR 2>/dev/null
  	#fi
	mkdir -p $OTHER_PARTTION_FLASHDIR 2>/dev/null
	

	echo -ne '.                           \r'
	mount_backup $OTHER_PARTTION_FLASHDIR rw

	echo -ne '..                          \r'
	echo -ne '..barak is remounting \r'
  	mount -o remount,rw,sync /dev/mtdblock1 / 
	echo -ne '...                         \r'

    if [ -d "$FLASH_TMP_DIR" ]; then
  	  rm -rf $FLASH_TMP_DIR 2>/dev/null
  	fi
	
	mkdir $FLASH_TMP_DIR 2>/dev/null
	
	if [ ! -d $FLASH_TMP_DIR ]; then
	  echo "$FLASH_TMP_DIR was not created"
  	  exit  $MKDIR_FAILURE
  	fi
    
    if [ "$HOST_NAME" == "$SERVER_IP" ]; then
	  cp  $TMPDIR/$REMOTE_FILE $OTHER_PARTTION_FLASHDIR
      if [ -f $OTHER_PARTTION_FLASHDIR/$REMOTE_FILE ]; then
      	rm -f $TMPDIR/$REMOTE_FILE
      else
        echo "$TMPDIR/$REMOTE_FILE not copied to $OTHER_PARTTION_FLASHDIR/$REMOTE_FILE"
        exit  $COPY_FAILURE
      fi
    fi  
  fi  
 
  mount -o remount,rw,size=$TMPDIR_NEW_SIZE $TMPDIR
  if [ $? -ne 0 ];then
    echo "The remount command (size) not succeeded exit status is : $?" 
    exit $MOUNT_FAILURE
  fi
  		
}

RestoreDefaultTmpDirectorySize ()
{
  if [ -z "$TMPDIR_DEFAULT_SIZE" ]; then
    echo "Size of directory $$TMPDIR is not changed"
  else
    #   Sleep before restore default directory size
    #   because size of files in the directory isn't
    #   updated immediatly after file remove
    sleep 1
    if [ "$ARM_KW" == "1" ]; then
      #ran - keep it mounted, why removing it ? we will reboot any way, mount will be dropped - we also keep mount always on 
      #rm -rf $OTHER_PARTTION_FLASHDIR/*	 2>/dev/null
      #umount $OTHER_PARTTION_FLASHDIR    2>/dev/null
      echo "Keeping mount point"
      mount -o remount,rw,size=$TMPDIR_DEFAULT_SIZE $TMPDIR
      #barak
	  #mount
    else
      mount -o remount,size=$TMPDIR_DEFAULT_SIZE $TMPDIR
      if [ $? -ne 0 ];then
    	echo "The remount command not succeeded (size) exit status is : $?" 
    	exit $MOUNT_FAILURE
      fi		
    fi	    
  fi
}

FileTransferCheck ()
{
 
  cd $TMPDIR
  
###  $LOG_INF "Operation CHECK"
  if [ -f $FILE_TRANSFER_IN_PROCESS ]; then
    if [ -f $TRANSFERRING_FILE_NAME ]; then
      LS_STRING=`ls -l $TRANSFERRING_FILE_NAME`
      LS_STRING1=`echo $LS_STRING | sed s/"  "/" "/`
      while [ "$LS_STRING" != "$LS_STRING1" ]
      do
        LS_STRING=$LS_STRING1
        LS_STRING1=`echo $LS_STRING | sed s/"  "/" "/`
      done
      NEW_FILE_SIZE=`echo $LS_STRING | cut -d' ' -f5`
      if [ -f $TRANSFERRED_BYTES_FILENAME ]; then
        PREV_FILE_SIZE=`cat $TRANSFERRED_BYTES_FILENAME`
      else
        PREV_FILE_SIZE=""
      fi
      if [ "$NEW_FILE_SIZE" != "$PREV_FILE_SIZE" ]; then
        echo $NEW_FILE_SIZE > $TRANSFERRED_BYTES_FILENAME
###        $LOG_INF "Size of file the $TRANSFERING_FILE is changed"
        exit $TRANSFER_IN_PROCESS
      else
###        $LOG_INF "Size of file the $TRANSFERING_FILE is not changed"
        if [ "$TRANSFER_FILE_PROTOCOL" == "ftp" ]; then
          killall ftpget
          killall ftpput
        elif [ "$TRANSFER_FILE_PROTOCOL" == "scp" ]; then
          killall scp
        else
          $LOG_ERR "Transfer file $TRANSFERING_FILE hanged. Failed to continue due invalid file transfer protocol: $TRANSFER_FILE_PROTOCOL"
          exit $INVALID_TRANSFER_PROTOCOL
        fi

        CheckVtyshKilledAndExit $TRANSFER_HANGED $TRANSFER_KILLED
      fi
    else
###      $LOG_INF "File $TRANSFERRING_FILE_NAME not exist"
      if [ "$TRANSFER_FILE_PROTOCOL" == "ftp" ]; then
        killall ftpget
        killall ftpput
      elif [ "$TRANSFER_FILE_PROTOCOL" == "scp" ]; then
        killall scp
      else
        $LOG_ERR "Transfer file $TRANSFERING_FILE handed (file not exist). Failed to continue due invalid file transfer protocol: $TRANSFER_FILE_PROTOCOL"
        exit $INVALID_TRANSFER_PROTOCOL
      fi

      CheckVtyshKilledAndExit $TRANSFER_HANGED $TRANSFER_KILLED
    fi
  else
###    $LOG_INF "File Transfer finished"
    CheckVtyshKilledAndExit $TRANSFER_FINISHED $TRANSFER_KILLED
  fi
}

FileTransferClean ()
{
  cd $TMPDIR
###  $LOG_INF "Operation CLEAN"
  rm -rf $FILE_TRANSFER_IN_PROCESS $TRANSFERRING_FILE_NAME $TRANSFERRED_BYTES_FILENAME
  exit $NO_ERROR
}

CheckVtyshKilledAndExit ()
{
#   Routine CheckVtyshKilledAndExit has two parameters:
#      $1 - exit code if VTYSH ID = 0 VTYSH with VTYSH ID is exist 
#      $2 - exit code if VTYSH with VTYSH ID isn't exist (was killed)
  if [ "$TRANSFER_FILE_VTYSH_ID" != "0" ]; then
    PARENT_VTYSH_EXIST=`ps | grep "$TRANSFER_FILE_VTYSH_ID " | grep /usr/local/nbase/bin/vtysh`
    if [ -z "$PARENT_VTYSH_EXIST" ]; then
      cd $TMPDIR
###  $LOG_INF "Operation CLEAN"
      rm -rf $FILE_TRANSFER_IN_PROCESS $TRANSFERRING_FILE_NAME $TRANSFERRED_BYTES_FILENAME
      exit $2
    fi
  fi
  
  exit $1
}

function mount_backup
{
    RES1=$(mount | grep -c $1)
    RES2=$(mount | grep -c /tmp/backup)
    if [ `uname -m | grep arm` ]; then
	  if grep -qs nand_mtd /proc/cmdline; then
	    if [ $RES2 -ne 0 ]; then
	      mount -o remount,$2 /dev/mtdblock5 /tmp/backup
	    fi
#	    if [ "$1" != "/tmp/backup" ]; then
	      if [ $RES1 -eq 0 ]; then	
	    	mount -t jffs2 -o $2 /dev/mtdblock5 $1
    	    if [ $? -ne 0 ];then
              echo "The mount command jffs2 not succeeded exit status is : $? res1=$RES1 res2=$RES2" 
              exit $MOUNT_FAILURE
            fi
    	  fi
#	    fi
	  else
#	    if [ "$1" != "/tmp/backup" ]; then
	      if [ $RES1 -eq 0 ]; then	
	        mount -t squashfs -o $2 /dev/mtdblock5 $1
    	    if [ $? -ne 0 ];then
              echo "The mount command squashfs not succeeded exit status is : $? res1=$RES1 res2=$RES2" 
              exit $MOUNT_FAILURE
            fi
    	  fi
#	    fi
	  fi
    else
	  mount -t cramfs -o $2 /dev/mtdblock3 $1
  	  if [ $? -ne 0 ];then
        echo "The mount command cramfs not succeeded exit status is : $? res1=$RES1 res2=$RES2" 
        exit $MOUNT_FAILURE
      fi
    fi
    
}

function check_new_firm_for_multi_sys_conf
{
    mkdir -p /tmp/backup
    mount_backup /tmp/backup rw
    
    #barak - check data integrity of upgrade sw
    sudo /usr/local/nbase/bin/domd5sum.sh
    RET=$?
    if [ $RET -ne 0 ]
    then
      echo "SW upgrade: data integrity failed !!"
      echo "trying to write SW to flash again"
      exit $RET
    fi

    if [ ! -d /tmp/backup/$LOCAL_ETC_DIR/sys ]; then
    	cp $SYS_CONF $SYS_CONF.tmp
    	mv $SYS_CONF /tmp/
    	mv $SYS_CONF.tmp $SYS_CONF
    	chown root:admin $SYS_CONF
    	chmod 664 $SYS_CONF
    	/sbin/flconfig save $LOCAL_ETC_DIR local_etc
    	mv /tmp/System.conf $LOCAL_ETC_DIR/
    	BACKUP_IS_OLD="yes"
    fi

    #we keep /tmp/backup mounted 
    #umount /tmp/backup 2>/dev/null
    #rm -rf /tmp/backup
}

PURPOSE="$1"

case $PURPOSE in
"fileTransferCheck")

   if [ $# -eq 4 ]; then
     TRANSFERRING_FILE_NAME=$2
     TRANSFER_FILE_PROTOCOL=$3
     TRANSFER_FILE_VTYSH_ID=$4
###     $LOG_INF "$SCRIPT_NAME $PURPOSE $TRANSFERRING_FILE_NAME $TRANSFER_FILE_PROTOCOL $TRANSFER_FILE_VTYSH_ID"
   else
     $LOG_ERR "$SCRIPT_NAME: Option fileTransferCheck - invalid number of parameters = $#"
     exit $INVALID_PARAMETERS
   fi
   FileTransferCheck
;;
"fileTransferClean")
   if [ $# -eq 4 ]; then
     TRANSFERRING_FILE_NAME=$2
     TRANSFER_FILE_PROTOCOL=$3
     TRANSFER_FILE_VTYSH_ID=$4
###     $LOG_INF "$SCRIPT_NAME $PURPOSE $TRANSFERRING_FILE_NAME $TRANSFER_FILE_PROTOCOL $TRANSFER_FILE_VTYSH_ID"
   else
     $LOG_ERR "$SCRIPT_NAME: Option fileTransferCheck - invalid number of parameters = $#"
     exit $INVALID_PARAMETERS
   fi
   FileTransferClean
;;
"bootpart")
    ACTION_TEXT="Change boot partition"

    # This is where we get the old bootpart value:
    bootpart=`/usr/sbin/fw_printenv  -n  bootpart`
    
    if [ $bootpart -eq 1 ] || [ $bootpart -eq 3 ]; then
    
        # NEW(BP)   = (OLD(BP) + 2)      % 4:
        new_bootpart=`expr     $bootpart + 2`
        new_bootpart=`expr $new_bootpart % 4`
        
        # This is where we set the new bootpart value:
        /usr/sbin/fw_setenv   bootpart   $new_bootpart
        
        $LOG_INF "The system is rebooting from backup partition (# $new_bootpart)"
        echo "The system is rebooting from backup partition (# $new_bootpart)"
        sleep 5
        exit $NONE_ERROR
    else
        $LOG_ERR  "$ACTION_TEXT failed - Could not find a valid bootpart value"
        echo "$ACTION_TEXT failed - Could not find a valid bootpart value"
        sleep 5
        exit $INVALID_BOOT_PARTITION_ERROR
    fi        
;;       
"merge")        
    ACTION_TEXT="Copy startup configuration to backup partition"

    if [ -f "$SCRIPT_DIR/merge_p.sh" ] ; then
        check_new_firm_for_multi_sys_conf
        $SCRIPT_DIR/merge_p.sh || exit $MERGE_SCRIPT_FAILURE_ERROR
        if [ $BACKUP_IS_OLD == "yes" ]; then
          /sbin/flconfig save $LOCAL_ETC_DIR local_etc
        fi
        exit $NONE_ERROR
    else
        $LOG_ERR  "$ACTION_TEXT failed - '$SCRIPT_DIR/merge_p.sh': $NO_SUCH_FILE_OR_DIRECTOTY_STR"
        echo "$ACTION_TEXT failed - '$SCRIPT_DIR/merge_p.sh': $NO_SUCH_FILE_OR_DIRECTOTY_STR"
        exit $MERGE_SCRIPT_MISSING_ERROR
    fi
;;      
"backupver")
    mkdir -p /tmp/backup
               
    mount_backup /tmp/backup ro
    VERSION=`grep VERSION /tmp/backup/usr/local/etc/product.conf | awk -F= '{ print $2}'`       
    cp /tmp/backup/usr/local/etc/product.conf /tmp/backup_product.conf
    chown admin /tmp/backup_product.conf
    
    
    #umount /tmp/backup 2>/dev/null
    #rm -rf /tmp/backup 
    exit $NONE_ERROR
;;
*)
;;
esac

$LOG_INF "$ACTION_TEXT started"
echo "$ACTION_TEXT started"

############################################
if [ "$DEBUG" == 1 ] ; then
  echo "$ACTION_TEXT is in DEBUG mode" | $LOG_DBG
  echo "$ACTION_TEXT is in DEBUG mode"
  NEW_SCRIPT_DIR=$SCRIPT_DIR
else 
  NEW_SCRIPT_DIR="."
fi
############################################

# Check user name:
##################
user=`id`
if [ "$user" != "uid=0(root) gid=0(root)" ]; then
    $LOG_ERR  "$ACTION_TEXT failed - Invalid permissions. This script requires root permissions."
    echo "$ACTION_TEXT failed - Invalid permissions. This script requires root permissions."
    exit $OPERATION_NOT_PERMITTED_ERROR
fi

umask 0077

# Get params:
#############
###$LOG_DBG  "Purpose = '$PURPOSE'"

case $PURPOSE in
"ftpupgrade")
METHOD="ftp"
;;
#"tftpupgrade")
#    METHOD="tftp"
#;;
#"sftpupgrade")
#    METHOD="sftp"
#;;
"scpupgrade")
METHOD="scp"
;;
"fileupgrade")
METHOD="from file"
;;
*)
    $LOG_ERR  "$ACTION_TEXT failed - Wrong usage"
    echo "$ACTION_TEXT failed - Wrong usage"
    Usage $INVALID_ACTION_ERROR
;;
esac

if [ $# -lt 6 ]; then
  $LOG_ERR  "$ACTION_TEXT failed - Invalid number of parameters."
  echo "$ACTION_TEXT failed - Invalid number of parameters"
  exit $MISSING_PARAMETERS_ERROR
fi



SERVER_IP="$2"
REMOTE_DIR="$3"
REMOTE_FILE="$4"
export DID_BOOTPART_TOGGLE="$5"
INTERACTIVE_MODE="$6"
if [ $# -gt 6 ]; then
  USERNAME="$7"
  if [ $# -gt 7 ]; then
    PASSWORD="$8"
  fi
fi

###$LOG_DBG  "PURPOSE              = '$PURPOSE'            "
###$LOG_DBG  "SERVER               = '$SERVER_IP'          "
###$LOG_DBG  "REMOTE_DIR           = '$REMOTE_DIR'         "
###$LOG_DBG  "REMOTE_FILE          = '$REMOTE_FILE'        "
###$LOG_DBG  "DID_BOOTPART_TOGGLE  = '$DID_BOOTPART_TOGGLE'"
###$LOG_DBG  "INTERACTIVE_MODE     = '$INTERACTIVE_MODE'   "
###$LOG_DBG  "USERNAME             = '$USERNAME'           "
###$LOG_DBG  "PASSWORD             = '$PASSWORD'           "

# Verify params:
################
if [ -z "$SERVER_IP" ] || [ -z "$REMOTE_DIR" ] || [ -z "$REMOTE_FILE" ] ; then
    Usage $MISSING_PARAMETERS_ERROR
fi

# GOTO tmp
##########
cd $TMPDIR

###echo "$METHOD stage started"

if [ "$METHOD" == "ftp" ]; then
  # Arrange ftp parameters:
  #########################
  if [ -z "$USERNAME" ] ; then
    MINUS_U="" 
  else
    MINUS_U="-u" 
  fi
  if [ -z "$PASSWORD" ] ; then
    MINUS_P=""
  else
    MINUS_P="-p"
  fi
  # Documentation:
  ################
  IncreaseTmpDirectorySize
  touch $FILE_TRANSFER_IN_PROCESS
  rm -f $TRANSFERRED_BYTES_FILENAME
  if [ $INTERACTIVE_MODE -ne 0 ]; then
###    $LOG_DBG  "Running 'ftpget $MINUS_U $USERNAME $MINUS_P $PASSWORD $SERVER_IP - $REMOTE_DIR/$REMOTE_FILE | tar xz -'"
    logger -t $SCRIPT_NAME -p daemon.info "Starting ftpget"
  
    #barak: try ftp up to 5 times
    for i in {1..5}
    do
      if [ "$ARM_KW" == "1" ]; then
            ftpget $MINUS_U $USERNAME $MINUS_P $PASSWORD $SERVER_IP $OTHER_PARTTION_FLASHDIR/$REMOTE_FILE $REMOTE_DIR/$REMOTE_FILE 
            echo -ne '....                        \r'
      else
            ftpget $MINUS_U $USERNAME $MINUS_P $PASSWORD $SERVER_IP $REMOTE_FILE             $REMOTE_DIR/$REMOTE_FILE
      fi  
      RET=$?

      if [ $RET = 0 ] 
      then
        break;
      fi

    done
     
    if [ $RET != 0 ] 
    then
      exit  $FILE_TRANSFER_FAILURE_ERROR
    fi

    logger -t $SCRIPT_NAME -p daemon.info "Finishing ftpget"
    
    rm -f $FILE_TRANSFER_IN_PROCESS $TRANSFERRED_BYTES_FILENAME
    if [ $RET = 0 ]; then

      if [ "$ARM_KW" == "1" ]; then
 		tar xz -f ${OTHER_PARTTION_FLASHDIR}/${REMOTE_FILE} -C $FLASH_TMP_DIR  2> $ERRFILE.tar	
 		rm -f ${OTHER_PARTTION_FLASHDIR}/${REMOTE_FILE}
		echo -ne '.....                       \r'
		echo -ne '\n'
 	  else
 	    tar xz -f $REMOTE_FILE 2> $ERRFILE.tar
 		rm -f $REMOTE_FILE
 	  fi
      #barak
      RestoreDefaultTmpDirectorySize
      CheckTarStatus $ERRFILE.tar
    else
      $LOG_ERR "$ACTION_TEXT failed - $METHOD failure"  
      rm -f  $REMOTE_FILE
      RestoreDefaultTmpDirectorySize
      exit $FILE_TRANSFER_FAILURE_ERROR
    fi
  else
###    $LOG_DBG  "Running 'ftpget $MINUS_U $USERNAME $MINUS_P $PASSWORD $SERVER_IP - $REMOTE_DIR/$REMOTE_FILE 2> $ERRFILE.ftpget | tar xz - 2> $ERRFILE.tar'"
    
  	if [ "$ARM_KW" == "1" ]; then
		ftpget $MINUS_U $USERNAME $MINUS_P $PASSWORD $SERVER_IP $OTHER_PARTTION_FLASHDIR/$REMOTE_FILE $REMOTE_DIR/$REMOTE_FILE 2> $ERRFILE.ftpget
  		echo -ne '....                        \r'
  	else
    	ftpget $MINUS_U $USERNAME $MINUS_P $PASSWORD $SERVER_IP $REMOTE_FILE $REMOTE_DIR/$REMOTE_FILE 2> $ERRFILE.ftpget
  	fi  
   
    RET=$?
    rm -f $FILE_TRANSFER_IN_PROCESS $TRANSFERRED_BYTES_FILENAME
    if [ $RET = 0 ]; then
    	
    logger -t $SCRIPT_NAME -p daemon.info "Starting to write zip file to Flash..."
	echo "Starting to write zip file to Flash..."
	if [ "$ARM_KW" == "1" ]; then
      errstr=`tar xz -f ${OTHER_PARTTION_FLASHDIR}/${REMOTE_FILE} -C $FLASH_TMP_DIR  2>&1 1>/dev/null`	
	  rm -f ${OTHER_PARTTION_FLASHDIR}/${REMOTE_FILE}
	  echo -ne '.....                       \r'
	  echo -ne '\n'
    else	
      errstr=`tar xz -f $REMOTE_FILE                                                 2>&1 1>/dev/null`
      rm -f $REMOTE_FILE
    fi
    	
      RestoreDefaultTmpDirectorySize
      CheckTarStatusString $errstr
    else
      $LOG_ERR "$ACTION_TEXT failed - $METHOD failure"
      echo "$ACTION_TEXT failed - $METHOD failure"

      if [ -n "`grep "PASS error:" $ERRFILE.ftpget`" ]; then      #   Check login error
        RET=$LOGIN_FAILURE_ERROR
      elif [ -n "`grep "Unknown host" $ERRFILE.ftpget`" ]; then   #   Check invalid host error
        RET=$SERVER_UNAVAILABLE_ERROR
      elif [ -n "`grep "Host name lookup failure" $ERRFILE.ftpget`" ]; then
        RET=$SERVER_UNAVAILABLE_ERROR
      elif [ -n "`grep "Unable to connect to remote host" $ERRFILE.ftpget`" ]; then
        RET=$SERVER_UNAVAILABLE_ERROR
      elif [ -n "`grep "Failed to open file" $ERRFILE.ftpget`" ]; then   #   Check invalid filename
        RET=$NO_SUCH_FILE_ERROR
      elif [ -n "`grep "No such file" $ERRFILE.ftpget`" ]; then   #   Check invalid filename
        RET=$NO_SUCH_FILE_ERROR
      elif [ -n "`grep "Error accessing file" $ERRFILE.ftpget`" ]; then   #   Check invalid filename
        RET=$NO_SUCH_FILE_ERROR
      else
        RET=$FILE_TRANSFER_FAILURE_ERROR
      fi
      rm -f $ERRFILE.ftpget $ERRFILE.tar $REMOVE_FILE_LIST  $REMOTE_FILE
      RestoreDefaultTmpDirectorySize
      exit $RET
    fi
  fi
elif [ "$METHOD" == "scp" -o "$METHOD" == "from file" ]; then
  IncreaseTmpDirectorySize
  if [ "$METHOD" == "scp" ]; then
    touch $FILE_TRANSFER_IN_PROCESS
    rm -f $TRANSFERRED_BYTES_FILENAME
###    $LOG_INF "$GET_SCP_SCRIPT $TMPDIR $REMOTE_FILE $SERVER_IP $REMOTE_DIR $REMOTE_FILE $INTERACTIVE_MODE $USERNAME $PASSWORD"
    
    if [ "$ARM_KW" == "1" ]; then
	  #rm -f $OTHER_PARTTION_FLASHDIR/*.ver
	  $GET_SCP_SCRIPT $OTHER_PARTTION_FLASHDIR $REMOTE_FILE $SERVER_IP $REMOTE_DIR $REMOTE_FILE $INTERACTIVE_MODE $USERNAME $PASSWORD
      echo -ne '....                        \r'
    else
      $GET_SCP_SCRIPT $TMPDIR                  $REMOTE_FILE $SERVER_IP $REMOTE_DIR $REMOTE_FILE $INTERACTIVE_MODE $USERNAME $PASSWORD
    fi  
    
    RET=$?
    rm -f $FILE_TRANSFER_IN_PROCESS $TRANSFERRED_BYTES_FILENAME
    if [ $RET -ne $NONE_ERROR ]; then
      if [ "$ARM_KW" == "1" ]; then
	    rm -f ${OTHER_PARTTION_FLASHDIR}/${REMOTE_FILE}	
      else
		rm -f $REMOTE_FILE
      fi
      RestoreDefaultTmpDirectorySize
      exit $RET
    fi
  fi
    	
    logger -t $SCRIPT_NAME -p daemon.info "Starting to write zip file to Flash..."
	echo "Starting to write zip file to Flash..."
					
    if [ "$ARM_KW" == "1" ]; then
 	  errstr=`tar xz -f ${OTHER_PARTTION_FLASHDIR}/${REMOTE_FILE} -C $FLASH_TMP_DIR  2>&1 1>/dev/null`	
      rm -f ${OTHER_PARTTION_FLASHDIR}/${REMOTE_FILE} 
      echo -ne '.....                       \r'
      echo -ne '\n'
    else
      errstr=`tar xz -f $REMOTE_FILE                                                 2>&1 1>/dev/null`				
      rm -f $REMOTE_FILE
    fi
    
      
  RestoreDefaultTmpDirectorySize
  CheckTarStatusString $errstr
else
	$LOG_ERR  "$ACTION_TEXT failed - $METHOD stage wasn't implemented yet"
	echo "$ACTION_TEXT failed - $METHOD stage wasn't implemented yet"
	exit $INVALID_GET_METHOD_ERROR
fi
	 
# Execute mupgrade:
###################
  
  if [ "$ARM_KW" == "1" ]; then
  	cd $FLASH_TMP_DIR
  fi

#barak: if upgrade fails, try again.

$LOG_INF  "pwd=$PWD Running $NEW_SCRIPT_DIR/mupgrade_p.sh"
if [ -f "$NEW_SCRIPT_DIR/mupgrade_p.sh" ] ; then
    #barak
    echo "show mount before  mupgrade"
    mount
    $NEW_SCRIPT_DIR/mupgrade_p.sh 
	if [ $? -ne 0 ]; then
        $LOG_ERR  "$ACTION_TEXT failed - Invalid software version."
        echo "$ACTION_TEXT failed - Invalid software version."
  		rm -f $REMOVE_FILE_LIST 
  		if [ "$ARM_KW" == "1" ]; then
  			cd /
  			rm -rf $FLASH_TMP_DIR 2>/dev/null
			mount -o remount,ro /dev/mtdblock1 /
                        RES=$(mount | grep -c /dev/mtdblock5)
                        #no need to umount - we will reboot anyway 
			#if [ $RES -ne 0 ]; then
			 # umount /dev/mtdblock5
			#fi	
		fi
    	exit $INVALID_SOFTWARE_VERSION_TYPE_ERROR
 	fi
else
    $LOG_ERR  "$ACTION_TEXT failed - $NEW_SCRIPT_DIR/mupgrade_p.sh: $NO_SUCH_FILE_OR_DIRECTOTY_STR"
    echo "$ACTION_TEXT failed - $NEW_SCRIPT_DIR/mupgrade_p.sh: $NO_SUCH_FILE_OR_DIRECTOTY_STR"
    rm -f $REMOVE_FILE_LIST
    if [ "$ARM_KW" == "1" ]; then
      cd / 	
  	  rm -rf $FLASH_TMP_DIR 2>/dev/null
	  mount -o remount,ro /dev/mtdblock1 /
      
      RES=$(mount | grep -c /dev/mtdblock5)  
      #no need to umount - we will reboot anyway 
			#if [ $RES -ne 0 ]; then
			 # umount /dev/mtdblock5
			#fi	
	    
     fi
    exit $MUPGRADE_SCRIPT_MISSING_ERROR
fi

#######################################################################
# Check wether the new firmware do not use multi System configuration #
#######################################################################
check_new_firm_for_multi_sys_conf
if [ $BACKUP_IS_OLD == "yes" ]; then
    ./merge_p.sh
    /sbin/flconfig save $LOCAL_ETC_DIR local_etc
fi
#######################################################################

rm -f $REMOVE_FILE_LIST

if [ "$ARM_KW" == "1" ]; then
  cd /	
  rm -rf $FLASH_TMP_DIR 2>/dev/null
  mount -o remount,ro /dev/mtdblock1 /
  
  RES=$(mount | grep -c /dev/mtdblock5)
  #no need to umount - we will reboot anyway 
  #if [ $RES -ne 0 ] ; then
  #  umount /dev/mtdblock5
  #fi	
  
fi
 
$LOG_INF "$ACTION_TEXT finished successfully"
##echo "$ACTION_TEXT finished successfully"
exit $NONE_ERROR
