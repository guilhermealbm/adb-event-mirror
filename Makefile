CPU_ABI := $(shell adb -s $(SERIAL) shell getprop ro.product.cpu.abi)
	
ifneq (,$(findstring x86,$(CPU_ABI)))
all: build/x86/sendevents
else
all: build/arm/sendevents
endif

build/x86:
	mkdir -p build/x86

build/arm:
	mkdir -p build/arm

build/x86/sendevents: sendevents.c build/x86
	gcc -Wall -static -O3 $< -o $@

build/arm/sendevents: sendevents.c build/arm
	arm-linux-gnueabi-gcc -Wall -static -O3 $< -o $@

clean:
	rm -rf build