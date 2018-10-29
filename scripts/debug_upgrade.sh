#!/bin/sh
umount /dev/mtdblock5
/usr/bin/flash_erase --jffs2 /dev/mtd5 0 0 1>/dev/null
/usr/bin/nandwrite /dev/mtd5 /flash_tmp_dir/root.jffs2 -p 1>/dev/null
/usr/bin/flash_erase --jffs2 /dev/mtd6 0 0 1>/dev/null
/usr/bin/nandwrite /dev/mtd6 /flash_tmp_dir/kernel.jffs2 -p 1>/dev/null
mount -o rw /dev/mtdblock5 /tmp/backup
if [ -f ./out.txt ]; then rm ./out.txt; fi
#find /tmp/backup -type f 1>/dev/null; 2>/dev/null
find /tmp/backup -type f | while read f 1>/dev/null; do md5sum $f 1>/dev/null 2>./out.txt;  done;
if [ ! -z "`cat ./out.txt`" ]; then
 exit 1
fi
fw_setenv bootpart 3
exit 0
