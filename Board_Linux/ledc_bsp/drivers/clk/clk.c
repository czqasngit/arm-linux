#define CCM_CCGR1 *((volatile unsigned int *)0x020C406C)

void Clk_Init()
{
    CCM_CCGR1 = 0xFFFFFFFF;
}