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
     ldr r1, =0x0
     str r1, [r0]
loop:
    b loop