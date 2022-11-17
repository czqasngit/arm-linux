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
					drivers/gpio \
					libs/delay
#源文件目录
SRC_DIRS 		:= 	src \
					drivers/clk \
					drivers/led \
					drivers/gpio \
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

#配置可以搜索查找依赖文件的路径
VPATH			:= $(SRC_DIRS) 

#忽略同级目录中的clean(如果存在),clean只是Makefile中定义的执行命令
.PHONY: clean

./product/$(TARGET).bin : $(OBJS)
	$(LD) -Timx6ull.lds -o ./product/$(TARGET).elf $^
	$(COPY) -O binary -S ./product/$(TARGET).elf $@ 
	$(DUMP) -D -m arm ./product/$(TARGET).elf > ./debug/$(TARGET).dis

#指定要生成的目标已经依赖的目标取值范围
#obj/start.o只能在SOBJS中去寻找
$(SOBJS) : obj/%.o : %.s
	$(CC) -Wall -nostdlib -O2  $(INCLUDE) -c $< -o $@ 

$(COBJS) : obj/%.o : %.c
	$(CC) -Wall -nostdlib -O2  $(INCLUDE) -c $< -o $@ 


install:
	make clean
	make
	gcc -w ./tools/imxdownload.c -o ./tools/imxdownload

write:
	./tools/imxdownload ./product/$(TARGET).bin 
	
clean:
	rm -rf ./product/* ./debug/* $(COBJS) $(SOBJS) ./tools/imxdownload

print:
	@echo $(SOBJS)