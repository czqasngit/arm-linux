# 编译器前缀
COMPILER 		:= arm-unknown-linux-gnueabihf-
# 目录产物名称
TARGET 			:= bsp

CC 				:= $(COMPILER)gcc
LD          	:= $(COMPILER)ld
COPY     		:= $(COMPILER)objcopy
DUMP			:= $(COMPILER)objdump


#头文件目录
INCLUDE_DIRS 	:= 	common \
					drivers/clk \
					drivers/led \
					libs/delay
#源文件目录
SRC_DIRS 		:= 	src \
					drivers/clk \
					drivers/led \
					libs/delay

#INCLUDE_DIRS中所有的目录添加-I 前续,用于编译时增加链接头文件目录
INCLUDE			:= 	$(patsubst %, -I %, $(INCLUDE_DIRS))
#遍历文件夹, 将文件夹里面的文件全部展开并拼接上文件夹
SFILES			:=  $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.s))
CFILES			:=  $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))
# 去掉数组中文件路径部分,只保留文件名
SFILE_NAMES		:= $(notdir $(SFILES))
CFILE_NAMES		:= $(notdir $(CFILES))

#替换生成obj
SOBJS			:= $(patsubst %, obj/%, $(SFILE_NAMES:.s=.o))
COBJS			:= $(patsubst %, obj/%, $(CFILE_NAMES:.c=.o))

OBJS 			:= $(SOBJS) $(COBJS)

#make在编译的时候会首先查找当前目录下是否有依赖的目标(.o)文件
#make在编译的时候可以从VPATH中去搜索查找依赖的目标(.o)文件
VPATH			:= $(SRC_DIRS) 

#忽略同级目录中的clean(如果存在),clean只是Makefile中定义的执行命令
.PHONY: clean

$(TARGET).bin : $(OBJS)
	$(LD) -Timx6ull.lds -o $(TARGET).elf $^
	$(COPY) -O binary -S $(TARGET).elf $@ 
	$(DUMP) -D -m arm $(TARGET).elf > $(TARGET).dis

$(SOBJS) : obj/%.o : %.s
	$(CC) -Wall -nostdlib -O2  $(INCLUDE) -c $< -o $@ 

$(COBJS) : obj/%.o : %.c
	$(CC) -Wall -nostdlib -O2  $(INCLUDE) -c $< -o $@ 
	
clean:
	rm -rf $(TARGET).elf $(TARGET).dis $(TARGET).bin $(COBJS) $(SOBJS)

print:
	@echo $(VPATH)