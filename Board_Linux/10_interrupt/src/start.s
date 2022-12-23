.global _start  		/* 全局标号 */

/*
 * 描述：	_start函数，程序从此函数开始执行，此函数主要功能是设置C
 *		 运行环境。
 */
_start:
    /* 从链接起始地址开始,8条4字节的指令组成了ARM的中断向量表 */
    /* 中断向量表放在最开始的位置,每一条指令对应了具体的中断处理 */
    /* 当发生对应中断时,硬件会把对应的地址设置到pc寄存器,从而执行对应的中断服务函数 */
    ldr pc, =Reset_Handler                          /* 0x00: 复位中断 */
    ldr pc, =Undefine_Instruction_Handler           /* 0x04: 未定义中断指令 */
    ldr pc, =Software_Interrupt_Handler             /* 0x08: 软中断, SVC特权模式 */
    ldr pc, =Prefetch_Abort_Handler                 /* 0x0c: 指令预取中止中断 */
    ldr pc, =Data_Abort_Handler                     /* 0x10: 数据访问中止中断 */
    ldr pc, =Not_Used_Handler                       /* 0x14: 未使用的中断 */
    ldr pc, =IRQ_Handler                            /* 0x18: 外部设备中断 */
    ldr pc, =FIQ_Handler                            /* 0x1c: 快速中断 */

Reset_Handler:
    cpsid i                             /* 关闭IRQ,  此时IRQ还没有配置完成，所以关闭*/

    /* 
        在设备上电启动时,执行的代码访问的外设都是实际地址,
        mmu与cache此时的意义不大,
        这个时候为了防止cache与mmu可能导致的问题会先将mmu与cache关闭 
    */
    /* CP15: SCTLR */
    /* 关闭 I-Cache, D-Cache, MMU */
    MRC p15, 0, r0, c1, c0, 0            /* 将SCTLR寄存器读取到r0寄存器 */
    bic r0, r0, #(1 << 0)                /* 关闭MMU */
    bic r0, r0, #(1 << 1)                /* 关闭对齐 */
    bic r0, r0, #(1 << 11)               /* 关闭分支预测 */
    bic r0, r0, #(1 << 12)               /* 关闭i-cache */
    bic r0, r0, #(1 << 2)                /* 关闭d-cache */
    MCR p15, 0, r0, c1, c0, 0            /* 将r0寄存器数据写入到SCTLR寄存器 */

    /* 设置中断向量偏移,在发生中断之前设置即可,也可以在C语言中设置 */

    ldr r0, =0x87800000                 /* 将0x87800000这个立即数写入到 r0寄存器, 也就是链接起始地址*/
    dsb                                 /* 这里涉及到了改变读取内存的地址起始地址,需要使用内存屏障指令进隔断,保证前后读取指令都是正常的地址 */
    isb                                 /* 这里涉及到了改变读取内存的地址起始地址,需要使用内存屏障指令进隔断,保证前后读取指令都是正常的地址 */
    MCR p15, 0, r0, c12, c0, 0          /* 将r0的数据写入到VBAR寄存器中,表示向量偏移地址是0x87800000 */
    dsb                                 /* 这里涉及到了改变读取内存的地址起始地址,需要使用内存屏障指令进隔断,保证前后读取指令都是正常的地址 */
    isb                                 /* 这里涉及到了改变读取内存的地址起始地址,需要使用内存屏障指令进隔断,保证前后读取指令都是正常的地址 */

    /* 设置不同模式下的sp指针,每一个模式的sp对应不同的物理地址,当进入不同工作模式时C语言会在不同的物理sp指针指向的栈内存上工作 */
    /* 进入IRQ模式 */
	mrs r0, cpsr
	bic r0, r0, #0x1f 	                /* 将r0寄存器中的低5位清零，也就是cpsr的M0~M4 	*/
	orr r0, r0, #0x12 	                /* r0或上0x12,表示使用IRQ模式					*/
	msr cpsr, r0		                /* 将r0 的数据写入到cpsr_c中 					*/
    ldr sp, =0x80600000                 /* 设置栈指针			 */

    /* 进入SYS模式 */
	mrs r0, cpsr
	bic r0, r0, #0x1f 	                /* 将r0寄存器中的低5位清零，也就是cpsr的M0~M4 	*/
	orr r0, r0, #0x1f 	                /* r0或上0x1f,表示使用SYS模式					*/
	msr cpsr, r0		                /* 将r0 的数据写入到cpsr_c中 					*/
    ldr sp, =0x80400000                 /* 设置栈指针			 */

    /* 进入SVC模式 */
	mrs r0, cpsr
	bic r0, r0, #0x1f 	                /* 将r0寄存器中的低5位清零，也就是cpsr的M0~M4 	*/
	orr r0, r0, #0x13 	                /* r0或上0x13,表示使用SVC模式					*/
	msr cpsr, r0		                /* 将r0 的数据写入到cpsr_c中 					*/
    ldr sp, =0x80200000                 /* 设置栈指针			 */

    cpsie i                             /* 打开IRQ */
	b main				                /* 跳转到C语言main函数 		 */

Undefine_Instruction_Handler:
    ldr r0, =Undefine_Instruction_Handler
    bx r0

Software_Interrupt_Handler:
    ldr r0, =Software_Interrupt_Handler
    bx r0

Prefetch_Abort_Handler:
    ldr r0, =Prefetch_Abort_Handler
    bx r0

Data_Abort_Handler:
    ldr r0, =Data_Abort_Handler
    bx r0

Not_Used_Handler:
    ldr r0, =Not_Used_Handler
    bx r0

IRQ_Handler:
    ldr r0, =IRQ_Handler
    bx r0

FIQ_Handler:
    ldr r0, =FIQ_Handler
    bx r0