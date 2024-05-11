#include <main.h>
#include <screens/screens.h>

ScreenObject *activeScreenElement;

ScreenObject Test1Screen({{0, 0, 480, 360}}, {{0, 0, 480, 360}}, {},
                         {displaySleep}, {menuScreen}, {});

ScreenObject Test2Screen({{0, 0, 480, 360}}, {{0, 0, 480, 360}}, {},
                         {menuScreen}, {testDirection}, {});

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

void test1Screen()
{
    if (updateScreenElement_t != NULL && eTaskGetState(updateScreenElement_t) != 4)
    {
        vTaskDelete(updateScreenElement_t);
    }
    activeScreenElement = &Test1Screen;
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        tft.fillScreen(0);
        tft.setTextColor(WHITE);
        tft.setTextSize(3);
        tft.setCursor(20, 20, 4);
        tft.print("test1");
    }
    xSemaphoreGive(tftMutex);
}

void testDirection()
{
    const int devX = 480;
    const int devY = 360;

    int x = touchCurrentAction[1];
    int y = touchCurrentAction[2];

    Serial.printf("x: %d y: %d devx: %d devy: %d deg: %d\n", touchCurrentAction[1], touchCurrentAction[2], touchCurrentAction[3], touchCurrentAction[4], touchCurrentAction[5]);
}

void test2Screen()
{
    if (updateScreenElement_t != NULL && eTaskGetState(updateScreenElement_t) != 4)
    {
        vTaskDelete(updateScreenElement_t);
    }
    activeScreenElement = &Test2Screen;
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        tft.fillScreen(0);
        tft.setTextColor(WHITE);
        tft.setTextSize(3);
        tft.setCursor(20, 20, 4);
        tft.print("test2");
    }
    xSemaphoreGive(tftMutex);
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