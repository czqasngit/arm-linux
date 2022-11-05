## 祼机开发 - I.MX6ULL LED点亮
### 1.查看LED电路图

<img src="https://tva1.sinaimg.cn/large/008vxvgGly1h7u0qpwofpj30gq09odga.jpg" alt="drawing" style="width:400px;"/><br/>

<font color=#666666 size=2>
通过电路图可以发现LED灯是由一个发光二极管来实现的,发光二极管的输入端接了一个3.3V的电源,发光二极管的输出端接MCU管脚,通过电路图可知,管脚需要输出低电平发光二极管点亮（即LED灯亮）,LED灯名称是LED0
</font>
<br/><br/>
<img src="https://tva1.sinaimg.cn/large/008vxvgGly1h7u183ypgkj30ue0mo79y.jpg" alt="drawing"/><br/>

<font color=#666666 size=2>
查看底板原理图可以看出,LED0是复用的GPIO_3，即需要将GPIO_3输出一个底电平则LED灯亮
</font>
<br/><br/>

### 2.查看I.MX6ULL开发手册

#### 使能GPIO1时钟
<font color=#666666 size=2>
1.找到18章,Clock Control Module(CCM)模块<br/>
2.查看CCM Memory Map/Register Definition 时间门寄存器定义<br/>
3.查看CCM_CCRG0 - CCM_CCGR6,这几个寄存器用于控制I.MX6ULL所有的外设时钟使能,进一步查看发现CCM_CCGR1的第27-26位用于使能GPIO1寄存器时钟(即将27-26设置为1)<br/><img src="https://tva1.sinaimg.cn/large/008vxvgGly1h7u2bc5nefj30y004a3z1.jpg">
4.配置GPIO1_03的方向寄存器是输入还是输出
5.配置GPIO1_03的数据寄存器(即0:低电平, 1:高电平)
</font><br/><br/>

#### 复用GPIO1_03管脚为GPIO_03
<font color=#666666 size=2>
1.找到32章, IOMUX COntrol(IO复用)<br/>
2.通过配置寄存器IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03复用为GPIO_03,即将MUX_MODE(04 - 00设置为0101)<br/>
<img src="https://tva1.sinaimg.cn/large/008vxvgGly1h7u2puen2oj312x0u0n4l.jpg">
</font>

#### 配置GPIO01_03管脚的电气属性(压摆率、速度、驱动能力、等，也就是配置管脚具体的使用场景，需要根据负载的特性来配置)
<font color=#666666 size=2>
1.配置寄存器`IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO03`不同位的状态来配置其具体的电气属性<br/>
2.配置压摆率:电压上升的速度,这里我没有找到更多有意义(简单的理解)，我个人的理解是当一个信号来的时候电压能否在指定的时间内长高并输出一个高电平，那这样理解就是说如果管理输出一个高电平时压摆率设置为高，那如果管脚需要输出低电平高压摆率也就没有意义了，所以这里我将压摆率设置成了：低压摆率,即将寄存器的0位设置成0<br/>
3.配置驱动能力: 驱动能力超强电压越稳定,这里配置一个较强的驱动能力<br/>
<img src="https://tva1.sinaimg.cn/large/008vxvgGly1h7u3asbjpkj321y0nuwl7.jpg">
4.配置驱动速度:速度越高，输出数据的能力越强， 这里设置成SPEED_1_medium_100MHz_ — medium(100MHz)即中速度<br/>
<img src="https://tva1.sinaimg.cn/large/008vxvgGly1h7u3diixroj31ee0f2gpk.jpg">
5.配置输出模式: 输出模式分为推挽输出与开漏输出，推挽输出可以输出高电平与低电平，同时可以将不稳定的信号钳位在高电平(防止不稳定信号导致的干扰)，且输出的电流更大，驱动能力更强,但不能用于线与的电路；开漏输出可以输出低电平与高阻抗，可以将不稳定的信号钳位在低电平,可用于线与的电路，输出高电平需要上拉电阻，上拉电阻决定了电压。在这个实验中,由于我们是输出低电平LED亮，所以一开始我们需要将管脚保持在高电平，同时由于需要输出比较强的驱动能力，所以关闭开漏输出（即设置成推挽输出）<br/>
6.配置Pull/Keeper,很多MCU、MPU或FPGA中的IO配置都有上拉、下拉、OD配置，但有的也有Keeper这个配置。这个keeper可以称作状态保持器。在IO的输入或输出模式下，都能使能这个模式。Keeper的驱动能力很弱，它的功能是维持最后一个逻辑状态。下面以GPIO为例，讲解keeper的作用：<br/>
作为输入时，我们称它为输入状态保持器。当外部电路掉电后，而MCU/MPU/FPGA还在保持供电时，用于保持IO的输入状态，避免逻辑错误，有利于低功耗的应用。<br/>
作为输出时，我们称它为输出状态保持器。当MCU/MPU/FPGA掉电后，而外部电路还在保持供电时，用于维持外部电路的状态，避免逻辑错误，有利于低功耗的应用。<br/>
使能Pull/Keeper,同时设置成Keeper,GPIO1_03管脚的状态是由Keeper,Keeper由核心VDD供电，同时禁止了上下拉。
7.由于已经设置成了Keeper,所有PUS(15 - 14)位设置的值无效。
</font>

### 编辑LED点亮实验代码

```
.global _start /* start entry */

_start:
    // 找到控制GPIO1_IO03寄存器的使能时钟
    // 开启使能时钟
    ldr r0, =0x020C406C //set r0 CCM_CCGR1(GPIO1) address
    ldr r1, =0xFFFFFFFF //set r1 value
    str r1, [r0]        //r1 value to r0 address CCGR resiter, enable GPIO1 enable(26-27 bits)

    // mux GPIO1_IO03
    // SW_MUX_CTL_PAD_GPIO1_IO03
    ldr r0, =0x020E0068
    ldr r1, =0x5        //0101
    str r1, [r0]

    // 配置电气属性
    // SW_PAD_CTL_PAD_GPIO1_IO03
    ldr r0, =0x020E02F4
    /**
        电器参数位详解: https://z.itpub.net/article/detail/578088950F3DA3F701FB5454715771BB     
        上拉/下拉电阻: https://www.bilibili.com/video/BV1nf4y1y7Xx/?spm_id_from=333.337.search-card.all.click&vd_source=89f42a8ea34957e3baa89069cfc1bb28
                      https://www.bilibili.com/video/BV1W34y1579U/?spm_id_from=333.788.recommend_more_video.-1&vd_source=89f42a8ea34957e3baa89069cfc1bb28
        bit0:           0       设置压摆率为低压摆率
        bit5:3          110     设置驱动能力为R0/5-7，比较强的驱动能力(驱动能力则是输出电流的能力,驱动能力不足或者负载电流过大导致驱动能力不足时可能导致逻辑电路电压不稳定或者不足,从而导致逻辑电路紊乱)
        bit7:6          10      设置速度为中速度
        bit11           0       关闭开漏输出(使用推挽输出： 驱动能力强)
        bit12           1       使能（开启）上下拉/保持器功能 保持器: 但是它在输入模式下，叫输入状态保持器，用于当外部电路断电时（此时 MCU 还是供电的），维持 IO 口的状态，有利于低功耗应用。而在输出模式下，它又叫输出状态保持器，用于当内核断电时，维持 IO 口的输出状态，同样有利于低功耗应用。
        bit13           0       设置为使用保持器
        bit15:14        00      不配置
        bit16           0       关闭hys
     */
     ldr r1, =0x10B0
     str r1, [r0]

     /* 配置GPIO1_IO03的管脚数据方向为输出 */
     ldr r0, =0x0209C004        //  
     ldr r1, =0x08
     str r1, [r0]
    /* 配置GPIO1_IO03 的管脚数据为低电平 */
     ldr r0, =0x0209C000
     ldr r1, =0
     str r1, [r0]
loop:
    b loop
```

### 在Linux下编译

- 编译leds.s,生成leds.o文件
```
arm-linux-gnueabihf-gcc -c leds.s -o leds.o
```
- 将编译好的leds.o文件链接生成linux可执行文件 leds.elf,同时指定了链接起始地址。
```
arm-linux-gnueabihf-ld -Ttext 0x87800000 leds.o -o leds.elf
```

- 通过copy命令指定elf生成bin文件，同时通过-S去掉了所有的称号表以及重写向信息,减少二进制文件的大小
```
arm-linux-gnueabihf-copyobj -O binary -S leds.elf leds.bin
```

-将leds.bin烧写到SDK(/dev/sdb)目录中，在烧写前imxdownload工具会在bin文件前面添加一些必要的头信息(IVT+DCD+Boot Data)生成leds.imx文件，最终烧写到SD卡中的是leds.imx文件。头文件信息中IVT数据记录了在链接生成elf时指定的链接地址(0x87800000)(代码段第一条指令的地址,也祼机运行代码时第一条指令的地址)。
```
imxdownload leds.bin /dev/sdb
```

<font color=#666666 size=2>
I.MX6ULL boot rom(固定在SOC上的程序启动器)选择从SD卡启动后会将解析,会从1KB的位置开始读取bin的头文件信息(IVT+DCD+BOOT DATA)，并做一些初始化的操作，如初始化DDR3。leds.elf指定了链接地址是0x87800000,这个地址记录在了IVT中的entry中，当boot rom启动后会将bin文件具体的代码段加载到DDR3中的0x87800000地址，并在初始化完之后从DDR3的0x87800000地址开始执行程序。
</font>
<br/><br/>
### 启动验证LED

- 将I.MX6ULL开发板的启动开关设置成1与7打开，其它关闭，表示从SD卡启动。
- 接通电源，此时电源LED亮绿灯，按下RESET按钮后几秒LED0红灯亮