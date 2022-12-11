
#include "imx6ul.h"
#include "gpio.h"
#include "const.h"
#include "delay.h"

void Key0_Init()
{
    // 将UART1_CTS_B利用为GPIO01_IO18
    IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18, 0);
    // 配置电气属性
    IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18, CONST_GPIO_INPUT_PAD_VALUE);
    // 配置GPIO方向以及默认值
    GPIO_Init(GPIO1, 18, (GPIO_CONFIG){GPIO_DIRECTION_INPUT, 0});
}

int Key0_GetValue()
{
    if (GPIO_RedValue(GPIO1, 18) == 0)
    {
        //此时的电平是低电平,表示按钮是接下状态
        //为了防止电平抖动,延迟10ms再读取一下电平状态,如果还是按下表示按钮按下并且稳定
        Delay(10);
        if (GPIO_RedValue(GPIO1, 18) == 0)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        // 按钮松开
        return 0;
    }
}