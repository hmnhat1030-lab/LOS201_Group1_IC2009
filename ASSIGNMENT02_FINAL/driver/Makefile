obj-m += sms_sensor_driver.o
ccflags-y := -march=armv7-a

KDIR := $(HOME)/embedded_lab1/kernel/linux-5.15
ARCH ?= arm
CROSS_COMPILE ?= arm-linux-gnueabihf-

all:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) clean
