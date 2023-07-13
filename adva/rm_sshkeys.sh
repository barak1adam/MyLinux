#! /bin/sh

#create dir for backup partition mount
BACKUP_MNT=/tmp/backup

if [ -f $BACKUP_MNT ]
then 
  rm -rf $BACKUP_MNT
fi

mkdir -p $BACKUP_MNT
RET=$?
if [ $RET != 0 ]; then
  echo "failed to create backup directory. exit"
  exit 1  
fi

# mount backup partition
cmd="mount | grep sda5"
$cmd
RET=$?
if [ $RET = 0 ]; then
  echo "current / is mounted on /dev/sda5 partition"
  mount /dev/sda12 $BACKUP_MNT
else
  echo "current / is mounted on /dev/sda12 partition"
  mount /dev/sda5 $BACKUP_MNT
fi

# remove keys from current partition
cmd="rm -f /etc/ssh/ssh_host*"
$cmd
RET=$?
if [ $RET != 0 ]; then
  echo "failed to remove ssh keys on running partition"
  exit 1
fi

# remove keys from backup partition
cmd="rm -f $BACKUP_MNT/etc/ssh/ssh_host*"
$cmd
RET=$?
if [ $RET = 0 ]; then
  echo "ssh keys removed successfully !"
  echo "ssh keys removed successfully !"
  echo "ssh keys removed successfully !"
else
  echo "failed to remove ssh keys"
  exit 1
fi

#unmount backup partition
umount $BACKUP_MNT

exit 0

