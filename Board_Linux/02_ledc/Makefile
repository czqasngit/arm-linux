# 编译器前缀
COMPILER 		:= arm-unknown-linux-gnueabihf-
# 目录产物名称
TARGET 			:= bsp

CC 				:= $(COMPILER)gcc
LD          	:= $(COMPILER)ld
COPY     		:= $(COMPILER)objcopy
DUMP			:= $(COMPILER)objdump
OBJS 			:= start.o main.o

ledc.bin:${OBJS}
	arm-unknown-linux-gnueabihf-ld -Ttext 0x87800000 -o ledc.elf $^
	arm-unknown-linux-gnueabihf-objcopy -O binary -S ledc.elf $@ 
	arm-unknown-linux-gnueabihf-objdump -D -m arm ledc.elf > ledc.dis

%.o:%.s
	$(CC) -c $< -o $@

%.o:%.c
	$(CC) -c $< -o $@
	
clean:
	rm -rf *.o ledc.elf ledc.bin ledc.dis load.imx