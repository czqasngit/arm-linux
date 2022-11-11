arm-linux-gnueabihf-gcc -c leds.s -o led.o
arm-linux-gnueabihf-ld -Ttext 0x87800000 led.o -o led.elf
arm-linux-gnueabihf-objcopy -O binary -S led.elf led.bin
./imd led.bin /dev/sdb