#!/sbin/sh
cd /tmp/
/tmp/dd if=/dev/block/mmcblk0p9 of=/tmp/boot.img
/tmp/unpackbootimg /tmp/boot.img
/tmp/mkbootimg --kernel /tmp/zImage --ramdisk /tmp/boot.img-ramdisk.gz --cmdline 'console=ttyHSL0,115200,n8 androidboot.hardware=vee3 androidboot.selinux=permissive' --base 0x00200000  --ramdiskaddr 0x01500000 -o /tmp/newboot.img
/tmp/dd if=/tmp/newboot.img of=/dev/block/mmcblk0p9
busybox chmod 644 /system/lib/modules/*
