#!/bin/sh

 

is_connected()

{

  local ret=`send-at scact|cut -d "," -f 2`

  [ "$ret" -eq "0" ] && return 1

  return 0

}

 

get_pdp ()

{

  send-at pdp | grep -q "^+CGCONTRDP: "

}

 

disconnect ()

{

  #send-at pdp_disconnect > /dev/null

  send-at reset > /dev/null

}

 

connect ()

{

  if ! is_connected; then

    send-at connect >> /dev/null

    sleep 1

  fi

}

 

wan_if="eth0"

get_wan_interface ()

{

    ifs=`ip link | grep "<" | awk '{print $2}' | sed -e 's/://'`

    for i in $ifs; do

        msg=`sudo udevadm info -a -p /sys/class/net/$i`

        echo "$msg" | egrep 'ATTRS{idVendor}=="1199"' > /dev/null

        ret=$?

        if [ "$ret" -eq "0" ]; then

            echo $msg | egrep 'ATTRS{bInterfaceNumber}=="08"' > /dev/null

            ret=$?

            if [ "$ret" -eq "0" ]; then

                wan_if=$i

                break

            fi

        fi

    done

}

 

 

in_iteration()

{ 

  #check if environment variable is deinfed

  printenv CUR_APN

  ret=$?

}

 

 

##########################

# read one line

# use    $filename

# use    #linenum - which line to read

# output $line

read_line()

  

{

   #if requested line is not valid retur error in $error

   #start by making error empty

   #compare wc-l to line num

   #exit with error if larger

   #read line

   while   

 

 

 

}

 

 

#check if LTE should be operative

if [ -e /tmp/.mrv_lte_wan_disable ]; then

  logger -p local0.debug "mrv-lte-daemon: wan disabled"

  send-at pdp_disconnect > /dev/null

  while true; do

    sleep 42

  done

  exit 0

fi

 

logger -p local0.debug "mrv-lte-daemon: wan enabled"

 

 

 

 

#############################################################################

#first time create profile. this code should be modified.

# check if cache is valid.

# compare run-time-profile-table md5 cashe with default.

# if valid use the run-time

# can no longer use the mrv_lte_modem - note taht is the only place

# that it is used.

#############################################################################

 

#check if iteration variable is defined

# numOfRetries=1;

# startIteration=checkIteration

# if (InIteration); then;

   ## use the current iteration number

# elseif (LastCachValid) then;

    ## first iteration from cache

    #iteration=LastCacheValid;

    # numberOfRetries=10;

# else;

    ## start from first iteration

    #iteration=1;

 

# fi

 

 

#use flock to lock this section .

#(

#  flock -e 200

#  echo "In critical section"

#  sleep 5

#) 200>/tmp/blah.lockfile

 

## now that iteration is starting create a profile based on iteration number

#profile_create (iteration);

 

 

if [ ! -e /tmp/.mrv_lte_modem ]; then

  #create default profile

  send-at profile_create > /dev/null

  sudo touch /tmp/.mrv_lte_modem

fi

 

#############################################################################

#  validate that the modem is not waiting for any password

#############################################################################

 

 

#check pin

msg=`send-at pin`

echo "$msg" | grep -i "READY" > /dev/null

ret=$?

if [ ! "$ret" -eq "0" ]; then

  logger -p local0.error "mrv-lte-daemon: $msg"

  exit 0

fi

 

 

#############################################################################

# set the band

#############################################################################

 

 

#force LTE band

send-at band_lte > /dev/nul

 

 

 

#############################################################################

# get_wan_interface checks which of the created the wan_if

#   lo:

#   enp3s0:

#   br1:

#   br4092:

#   enp0s20u1i8:

#   enp0s20u1i10:

#   enp3s0.4092@enp3s0:

#############################################################################

 

connected=0

get_wan_interface

 

 

#############################################################################

# Connect iterations  using the wan_if

#############################################################################

for i in `seq 1 30`;

do

  connect

 

  ip a s $wan_if | grep "inet " > /dev/null

  ret=$?

  if [ "$ret" -eq "0" ]; then

    #modem connected

    connected="1"

    logger -p local0.debug "mrv-lte-daemon: initial connect"

    break

  fi

 

  if is_connected; then

    #connection established

    if get_pdp; then

      #ip assigned

      ip a show $wan_if | grep "DOWN" > /dev/null

      ret=$?

      if [ "$ret" -eq "0" ]; then

        #ip assigned but interface is still down

        sudo ifdown $wan_if > /dev/null

        sudo ifup $wan_if > /dev/null

      fi

    fi

  fi

 

  sleep 1

done

 

if [ "$connected" -eq "0" ]; then

  logger -p local0.error "mrv-lte-daemon: modem is not up. disconnect"

  disconnect

  exit 0

fi

logger -p local0.debug "mrv-lte-daemon: modem is up"

 

sleep 5

 

while :

do

  if ! is_connected; then

    logger -p local0.error "mrv-lte-daemon: modem not connected. reset"

    disconnect

    exit 0

  fi

  if ! get_pdp; then

    logger -p local0.error "mrv-lte-daemon: no pdp context. reset"

    disconnect

    exit 0

  fi

 

  sleep 30

done

 

exit 0
