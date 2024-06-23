SUMMARY:
======== 
ZynqMP part number: XCZU17EG-ffvc1760-2-i

to run petalinux tools:
source /opt/pkg/petalinux/v2022.1/settings.sh

files needed to boot from JTAGm the following files should be updated in /home/barak/xilinx-zcu102-2022.1/images/linux:

pmufw.elf
---------
/home/barak/xilinx-zcu102-2022.1/images/linux/pmufw.elf
not changed yet, same as built by Petalinux for the ZCU102 EVB
will have to change it to support PS warm reset

fsbl:
------
/home/barak/xilinx-zcu102-2022.1/images/linux/zynqmp_fsbl.elf
I built the last one on Vitis: barak_ws2 workspace
based on latest XSA done with Guy: /home/barak/edt/test_fsbl_guy/system_wrapper.xsa
all source code + XSA are uploaded to bitbucket: /home/barak/edt/test_fsbl_guy/system_wrapper.xsa
cloned to: /home/barak/edt/test_fsbl_guy/system_wrapper.xsa

read MyLinx/xilinx/xo126_fsbl_build.txt

dtb:
-----
when we are booting from JTAG with: petalinux-boot --jtag --u-boot
then a u-boot dtb is used.

/home/barak/xilinx-zcu102-2022.1/images/linux/system.dtb

to generate this u-boot dtb, I cp the u-boot dts file to ~/XO126_DT and build the dtb there.

~/XO126_DT is an independent folder I created for external builds of a DTB.
it has all required dtsi files for ZynqMP but simply flattened, without sub folders.
external build means: outside petalinux, outside u-boot, outside kernel.
I did it because we had many troubles with Petalinux dtb generation.

cp ~/u-boot-xlnx/xo126_uboot.dts ~/XO126_DT/system-top.dts; gcc -I my_dts -E -nostdinc -undef -D__DTS__ -x assembler-with-cpp -o system-top.dts.tmp system-top.dts; sleep 1; dtc -I dts -O dtb -o system-top.dtb system-top.dts.tmp; sleep 1; cp ~/XO126_DT/system-top.dtb ~/xilinx-zcu102-2022.1/images/linux/system.dtb

note: the final blob is named system.dtb as required by petalinux-boot and as listed in the bif file for creating the BOOT bin file.


to genreate it fo the kernel dtb:
cp ~/linux-xlnx/xo126_kernel.dts ~/XO126_DT/system-top.dts; gcc -I my_dts -E -nostdinc -undef -D__DTS__ -x assembler-with-cpp -o system-top.dts.tmp system-top.dts; sleep 1; dtc -I dts -O dtb -o system-top.dtb system-top.dts.tmp; sleep 1; cp ~/XO126_DT/system-top.dtb ~/xilinx-zcu102-2022.1/images/linux/system-top.dtb; cp ~/XO126_DT/system-top.dtb /tftpboot/system-top.dtb;

note: the final blob is named system-top.dtb
this is the one I cp to /tftpboot for booting the kerenl and to Maoz to place it in the kernel AOS build.

examples of cp:
#cp ~/XO126_DT/system-top.dtb /tftpboot/system-kernel.dtb
#scp ~/XO126_DT/system-top.dtb maozy@10.32.8.71:~/barak/xo126.dtb

u-boot:
--------
/home/barak/xilinx-zcu102-2022.1/images/linux/u-boot.elf

please refer to u-boot-outside-petalinux.txt of how to build the u-boot.elf

bl31.elf
---------                                                                         
/home/barak/xilinx-zcu102-2022.1/images/linux/bl31.elf



how to create the BIN file
----------------------------
at /home/barak/xilinx-zcu102-2022.1/images/linux/bootgen.bif
//arch = zynqmp; split = false; format = BIN
the_ROM_image:
{
    [bootloader, destination_cpu = a53-0]zynqmp_fsbl.elf
    [pmufw_image]pmufw.elf
    [destination_cpu = a53-0,  exception_level=el-3, trustzone]bl31.elf
    [load = 0x00100000, destination_cpu = a53-0]system.dtb
    [destination_cpu = a53-0,  exception_level=el-2]u-boot.elf
}

$pwd
~/xilinx-zcu102-2022.1/images/linux$
$cp /home/barak/edt/barak_ws2/fsbl/Release/fsbl.elf zynqmp_fsbl.elf; /home/barak/Vitis/2022.1/bin/bootgen -w -image /home/barak/xilinx-zcu102-2022.1/images/linux/bootgen.bif -arch zynqmp -o /home/barak/xilinx-zcu102-2022.1/images/linux/BOOT_0607.img; sleep 2; cp BOOT_0607.img /tftpboot/;

how to burn the BIN to NOR and the SW to eMMC:
----------------------------------------------
Please refer to new_board_programming.txt

how to build the kernel:
------------------------
please refer to xo126_kernel_build.txt

to run the kernel with its dtb please do:
-----------------------------------------
#set of ethaddr is not needed if EEPROM is already set
setenv ethaddr 3C:A7:2B:00:D8:17
setenv ipaddr 10.32.8.250
setenv serverip 10.32.8.145
ping 10.32.8.145

#not saved yet to eMMC, use tftp
tftpboot 0x00200000 /tftpboot/Image_19_06
tftpboot 0x00010000 /tftpboot/system-top.dtb
tftpboot 0x04000000 /tftpboot/initramfs.img

#if already saved in eMMC:
ext4load mmc 0:5 0x00200000 kernel.uimg
ext4load mmc 0:5 0x00010000 xo126.dtb  
ext4load mmc 0:5 0x04000000 initramfs

setenv setbootargs "setenv bootargs ethaddr=$ethaddr panic=5  $bootargs craft=$baudrate craft_port=ttyPS0"
run setbootargs; 
setenv bootargs "$bootargs uio_pdrv_genirq.of_id=uio_pdrv_genirq"
setenv bootargs "$bootargs cs.0=/dev/mmcblk1p1 csfs=ext3 csmo=ro cs.1=/dev/mmcblk1p2"
booti 0x00200000 0x04000000 0x00010000

#the following is to boot only the kernel without the AOS FS
setenv bootargs "$bootargs uio_pdrv_genirq.of_id=uio_pdrv_genirq init=/bin/bash"

how do build i2c.bit:
---------------------
in Vivado, document with guy.


Procedure for Genadi:
=====================
Install Vitis and import / opend the right workspace to zcu102

1. Change bootmode to JTAG and reset:

connect
targets -set -filter {name =~ "Cortex-A53 #0"}
rst -processor; 
mwr 0xff5e0200 0x0100
rst -system

2. Load PMU FW
targets -set -filter {name =~ "PSU"}; mwr 0xffca0038 0x1ff; after 500; targets; targets -set -filter {name =~ "MicroBlaze PMU"}; targets; after 100; dow {/home/barak/xilinx-zcu102-2022.1/images/linux/pmufw.elf}; con; after 500

3. Load FSBL
targets -set -filter {name =~ "Cortex-A53 #0"}; targets; rst -processor; dow /home/barak/xilinx-zcu102-2022.1/images/linux/zynqmp_fsbl.elf; con; after 5000

4. Load DTB
dow -data {/home/barak/xilinx-zcu102-2022.1/images/linux/system.dtb} 0x00100000

5. Load U-boot (the one that is built for JTAG - no env and no autoboot). 
dow {/home/barak/xilinx-zcu102-2022.1/images/linux/u-boot.elf}; con;

6. now burn BIN to NOR by u-boot tfttp or get it by jtag (stop; dow -data {/home/barak/edt/zcu102_for_bringup/BOOT.bin} 0x2000000; con)
Note:
#if we like to make the u-boot working with ETH connectivity for BOOT.bin tftpboot, we need to load also the dtb.
#so we need to load u-boot by "petalinux-boot --jtag --u-boot".
