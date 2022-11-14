
COMPILER 	:= arm-linux-gnueabihf
CC 			:= $(COMPILER)-gcc
LD          := $(COMPILER)-ld
COPY     	:= $(COMPILER)-objcopy
DUMP		:= $(COMPILER)-objdump
NAME 		:= ledc
objs 		:= start.o main.o

$(NAME).bin:$(objs)
	$(LD) -Ttext 0x87800000 -o $(NAME).elf $^
	$(COPY) -O binary -S $(NAME).elf $@ 
	$(DUMP) -D -m arm $(NAME).elf > $(NAME).dis

%.o:%.s
	$(CC) -c $< -o $@

%.o:%.c
	$(CC) -c $< -o $@
	
clean:
	rm -rf *.o $(NAME).elf $(NAME).bin $(NAME).dis load.imx