base       = hrtimers-tester
obj-ko     = $(base).ko
obj-m     += $(base).o
ccflags-y += -g -DDEBUG

all:
	@make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
build:
	@reset; make all
clean:
	@make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
install:
	@sync; sudo insmod $(obj-ko);
ls-mod:
	@lsmod | grep timertest
log-show:
	@dmesg
log-tail:
	@dmesg -w
mod-info:
	@modinfo $(obj-ko)
mod-probe:
	@sudo modprobe $(base)
uninstall:
	@sudo rmmod $(obj-ko)
