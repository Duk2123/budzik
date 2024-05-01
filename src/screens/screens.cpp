#include <main.h>
#include <screens/screens.h>

ScreenObject *activeScreenElement;

ScreenObject Test1Screen({{0, 0, 480, 360}}, {{0, -390, 0, 0}}, {},
                         {displaySleep}, {menuScreen}, {});

ScreenObject Test2Screen({{0, 0, 480, 360}}, {{0, 0, 480, 360}}, {},
                         {menuScreen}, {testDirection}, {});

void displaySleep(int *touchQueuedAction)
{
    if (brightness > 0)
    {
        setBrightness(0);
    }
    else
    {
        setBrightness(prevBrightness);
    }
}

void test1Screen(int *touchQueuedAction)
{
    if (updateDisplay_t != NULL && eTaskGetState(updateDisplay_t) != 4)
    {
        vTaskDelete(updateDisplay_t);
    }
    activeScreenElement = &Test1Screen;
    tft.fillScreen(0);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.setCursor(20, 20, 4);
    tft.print("test1");
}

void testDirection(int *touchQueuedAction)
{
    const int devX = 480;
    const int devY = 360;

    int x = touchQueuedAction[1];
    int y = touchQueuedAction[2];

    Serial.printf("x: %d y: %d devx: %d devy: %d deg: %d\n", touchQueuedAction[1], touchQueuedAction[2], touchQueuedAction[3], touchQueuedAction[4], touchQueuedAction[5]);
}

void test2Screen(int *touchQueuedAction)
{
    if (updateDisplay_t != NULL && eTaskGetState(updateDisplay_t) != 4)
    {
        vTaskDelete(updateDisplay_t);
    }
    activeScreenElement = &Test2Screen;
    tft.fillScreen(0);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.setCursor(20, 20, 4);
    tft.print("test2");
}
