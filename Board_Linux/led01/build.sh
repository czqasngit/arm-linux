arm-none-linux-gnueabihf-gcc -c leds.s -o led.o
arm-none-linux-gnueabihf-ld -Ttext 0x87800000 led.o -o led.elf
arm-none-linux-gnueabihf-objcopy -O binary -S led.elf led.bin
./imd.o led.bin /dev/sda