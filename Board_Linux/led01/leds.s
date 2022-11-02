.global _start /* start entry */

_start:
    // enable CCM_CCGR1 for GPIO01
    ldr r0, =0x020C406C //set r0 CCM_CCGR1(GPIO1) address
    ldr r1, =0xFFFFFFFF //set r1 value
    str r1, [r0]        //r1 value to r0 address CCGR resiter, enable GPIO1 enable(26-27 bits)

    // mux GPIO1_IO03

    ldr r0, =0x020E0068
    ldr r1, =0x5        //0101
    str r1, [r0]

    // 配置电气属性
    
    ldr r0, =0x020E02F4
    /**
        bit0:           0       设置速率为低速率
        bit5:3          110     设置驱动能力为R0/6，比较强的驱动能力
        bit7:6          10      设置速度为中速度
        bit11           0       关闭开路输出
        bit12           1       全能pull/keeper
        bit13           0       保持上拉
        bit15:14        00      100k下拉
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