/*****************************************************************************
* | File        :   EPD_Test.h
* | Function    :   test Demo
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2021-03-16
* | Info        :   
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "LCD_1in14.h"
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "ImageData.h"
#include "Debug.h"
#include "Infrared.h"
#include <stdlib.h>		
#include <stdio.h>

int main(void)
{
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    DEV_Module_Init();
    LCD_1IN14_Init(VERTICAL);
    while (true)
    {
        LCD_1IN14_Clear(GREEN);
        sleep_ms(500);
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        LCD_1IN14_Clear(RED);
        sleep_ms(1000);
        gpio_put(PICO_DEFAULT_LED_PIN,false);
        LCD_1IN14_Clear(RED);
        //Paint_DrawPoint(0,0,BLACK,2,DOT_FILL_AROUND);
    }

    //Paint_DrawString_EN(0, 0, "Hello Pico!", &Font16, WHITE, BLACK);

    return 0;
}
