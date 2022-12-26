#include "interrupt.h"

static Interrupt_Irq_Data _irqInterruptTables[NUMBER_OF_INT_VECTORS];
/// 记录当前正在执行的中断的数量
static int Interrupt_Irq_Count;
void Init_Interrupt()
{
    Interrupt_Irq_Count = 0;
    /// 初始化IRQ中断向量表
    for (int i = 0; i < NUMBER_OF_INT_VECTORS; i++)
    {
        _irqInterruptTables[i].handler = Interrupt_Default_Irq_Handler;
        _irqInterruptTables[i].context = NULL;
    }
    GIC_Init();
    __set_VBAR(0x87800000);
}
void Interrupt_Default_Irq_Handler(unsigned int gicciar, void *context)
{
    while (1)
    {
    }
}
void Interrupt_Irq_Handler_Register(IRQn_Type type, Interrupt_Irq_Handler handler, void *context)
{
    _irqInterruptTables[type].handler = handler;
    _irqInterruptTables[type].context = context;
}
void system_irq_handler(unsigned int gicciar)
{
    uint32_t irqNum = gicciar & 0x3FF;
    if (irqNum >= NUMBER_OF_INT_VECTORS)
        return;
    Interrupt_Irq_Count++;
    Interrupt_Irq_Data iid = _irqInterruptTables[irqNum];
    iid.handler(irqNum, iid.context);
    Interrupt_Irq_Count--;
}