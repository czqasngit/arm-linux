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

/*
    fiq 比 irq 的优先级高
    FIQ 向量位于向量表的最末端，异常处理不需要跳转
    FIQ 比 IRQ 多5个私有的寄存器(r8-r12)，在中断操作时，压栈出栈操作的少。
 */
/*
    异常发生后，ARM核的操作步骤可以总结为4大步3小步。
    1.保存执行状态：将CPSR复制到发生的异常模式下SPSR中;
    2.模式切换：
        CPSR模式位强制设置为与异常类型相对应的值，
        处理器进入到ARM执行模式，
        禁止所有IRQ中断，当进入FIQ快速中断模式时禁止FIQ中断;
    3.保存返回地址：将下一条指令的地址(被打断程序)保存在LR(异常模式下LR_excep)中。
    4.跳入异常向量表：强制设置PC的值为相应异常向量地址，跳转到异常处理程序中。
 */
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
    isb                                  /* 这里涉及到了改变读取内存的地址起始地址,需要使用内存屏障指令进隔断,保证前后读取指令都是正常的地址 */
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
    /* 
        中断发生时, IRQ模式下的lr(LR_svc物理)寄存器保存中断时刻的PC寄存器
        通过使用push命令将lr的值压入栈,这样的目的是为了在执行完当前中断服务函数
        后可以顺利的返回到中断前的执行位置,因为在执行中断服务函数的时候lr里面的值可能发生变化
        比如: 在内部使用了blx调用其它函数,新的IRQ中断进入
    */
    push {lr}    
    /*
        保存中断发生时的执行现场(r0-r12)
        从User/Sys模式切换到IRQ模式，r0-r12寄存器是通用的,所以需要将这些寄存器都压入栈保存起来，
        由于在执行IRQ中断函数时模式已经切换，此时的sp指针已经是IRQ模式下的栈地址了,所以r0-r12保存到了
        IRQ对应的栈空间中,恢复现场的时候只需要入栈即可
     */   
    push {r0-r12}
   // push {r0-r3, r12}
   /* 
        将spsr(SPSR_irq)寄存器的值压入栈,spsr保存了中断发生时的cpsr寄存器的值，
        中断执行完成之后需要恢复
    */
    mrs r0, spsr        
    push {r0}

    /* GIC寄存器组的基地址(起始地址,通过起始地址可以访问得所有的GIC寄存器) */
    /* 将GIC基地址读取到r1寄存器中 */
    MRC p15, 4, r1, c15, c0, 0          // Read Configuration Base Address Register

    /* 0x2000 - 0x3FFF 是GIC中CPU Interface的范围 */
    /* 将r1中保存的GIC基地址偏移0x2000再保存到r1中 */
    /* 此时r1中保存的是CPU Interface的基地址 */
    add r1, r1, #0x2000
    /*
        r1(CPU Interface基地址)偏移0x0c得到GICC_IAR寄存器地址,
        将GICC_IAR寄存器的值读取到r0中,
        GICC_IAR保存了IRQ中断的中断号与CPU号(多核时使用),
        通过中断号即可明确具体的中断来源并对中断进行响应
     */
    ldr r0, [r1, #0x0c]
    /*
        由于要进入到SVC模式了,需要将r0, r1两个通用寄存器的数据保存到栈里,
        防止在SVC模式下后r0,r1数据丢失
        此时r0, r1保存到的是IRQ模式下的栈空间,
     */
    push {r0, r1}
    /*
        将CPSR寄存器的M[4:0]值改成10011, 让CPU进入到SVC模式,
        进行SVC模式之后,当我们处理当前中断时,
        系统可以再次响应IRQ中断
     */
    cps #0x13                       // 进入到SVC
    /*
        进入到svc模式后先将lr的数据压入栈,执行完后再恢复
        因为接下来要使用blx调用C语言函数,会改变lr寄存器的数据
     */
    push {lr} 
    ldr r2, =system_irq_handler     // 将C语言写的中断服务函数的地址加载到r2寄存器
    blx r2                          // 调用C语言的中断处理函数, r0为函数参数
    pop {lr}                        // 调用完具体中断处理函数后,lr恢复
    cps #0x12                       // 进入到IRQ,执行完中断服务函数后进入IRQ不能再次响应IRQ中断,直到当前的IRQ中断完成
    pop {r0, r1}                    // 恢复IRQ模式下r0,r1寄存器的数据
    /*
        此时r0保存的是GICC_IAR寄存器的数据,
        将GICC_IAR的数据写入到GICC_EOIR寄存器,表示当前IRQ中断处理完成
     */
    str r0, [r1, #0x10]

    pop {r0}                        // 将栈顶的数据(此时栈顶保存的是spsr寄存器的值)出栈到r0寄存器
    /// spsr_cxsf其中(cxsf表示4个不同的8bit位数据,后续表示此次命令影响的数据位), spsr_cxsf等于spsr
    msr spsr_cxsf, r0               // 恢复spsr寄存器数据
    pop {r0-r12}                    // 恢复r0-r12寄存器的数据
    pop {lr}                        // 恢复lr寄存器的数据
    subs pc, lr, #4                 // 将lr - 4字节赋值给lc, 恢复中断前的执行命令继续执行
FIQ_Handler:
    ldr r0, =FIQ_Handler
    bx r0
    