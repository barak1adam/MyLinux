
# To build modules run "make"
#build=INTEL

ifeq ($(build),INTEL)

    # called from kernel build system: just declare what our modules are
    obj-m := mini2440_adc.o 

    # Assume the source tree is where the running kernel was built
    # You should set KERNELDIR in the environment if it's elsewhere
    # KERNELDIR = /lib/modules/$(shell uname -r)/build
      KERNELDIR = /usr/src/kernels/2.6.32-504.16.2.el6.x86_64
    # The current directory is passed to sub-makes as argument
    PWD := $(shell pwd)

intel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	echo "bulding module for intel architecture:" 

    
else
    # called from kernel build system: just declare what our modules are
    obj-m := mini2440_adc.o 

    CROSS_COMPILE=arm-linux- 
    ARCH=arm 
    KERNELDIR=/usr/src/kernels/linux-2.6.32.2
    PWD       := $(shell pwd)

arm_modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD)/../include modules
	echo "bulding module for arm architecture:"

    
endif



clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

.PHONY: clean


