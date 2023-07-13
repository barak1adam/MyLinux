#!/bin/sh -x
dmesg -n8
mkdir /flash_tmp_dir
mount -o remount,rw /dev/mtdblock5 /tmp/backup
mount -o remount,rw /dev/mtdblock1 /
if [ -f ./out.txt ]; then rm ./out.txt; fi
sync
ftpget 172.21.11.14 /tmp/backup/OS940-1_5_9.19411-os606.ver ./OS940-1_5_9.19411-os606.ver  2>/dev/null; 1>./out.txt
if [ ! -z "`grep -v Transferring ./out.txt`" ]; then
  exit 1
fi
rm ./out.txt
sync
tar xz -f /tmp/backup/OS940-1_5_9.19411-os606.ver -C /flash_tmp_dir 2>./out.txt
if [ ! -z "`cat ./out.txt`" ]; then
  exit 1
fi
if [ ! -z "`dmesg | grep untar`" ]; then
  exit 1
fi
rm -f /tmp/backup/OS940-1_5_9.19411-os606.ver
exit 0
