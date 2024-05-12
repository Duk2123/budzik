#include <screens/screens.h>

ScreenObject *activeScreenElement;

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
                activeScreenElement->processTouch();
            }
            else if (touchCurrentAction[0] == 2)
            {
                while (isTouchProcessing == true && touchCurrentAction[0] == 2)
                {
                    vTaskDelay(8);
                }
                if (isTouchProcessing == false && touchCurrentAction[0] == 2)
                {
                    activeScreenElement->processTouch();
                }
            }
            else
            {
                activeScreenElement->processTouch();
            }

            touchCurrentAction[0] = -1;
        }
        vTaskDelay(8);
    }
}