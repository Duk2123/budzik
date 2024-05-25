#include <screens/screens.h>

ScreenObject *ActiveScreenElement;

// TODO known issue flickering screen that fixes on screen change

void displaySleep()
{
    detectTouchSuspendCounter = 6;
    if (brightness > 0)
    {
        setBrightness(0);
    }
    else
    {
        setBrightness(prevBrightness);
    }
}

TaskHandle_t updateDisplay_t;
void updateDisplay(void *params)
{
    for (;;)
    {
        if (touchCurrentAction[0] != -1)
        {
            if (touchCurrentAction[0] == 0)
            {
                detectTouchSuspendCounter = 4;
                ActiveScreenElement->processTouch();
            }
            else if (touchCurrentAction[0] == 2)
            {
                while (isTouchProcessing == true && touchCurrentAction[0] == 2)
                {
                    vTaskDelay(8);
                }
                if (isTouchProcessing == false && touchCurrentAction[0] == 2)
                {
                    ActiveScreenElement->processTouch();
                }
            }
            else
            {
                ActiveScreenElement->processTouch();
            }

            touchCurrentAction[0] = -1;
        }
        vTaskDelay(8);
    }
}