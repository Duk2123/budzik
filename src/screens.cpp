#include <globals.h>

Screen *activeScreen;

Screen MenuScreen({{-999, 999}}, {}, {{110, 210}, {240, 210}, {370, 210}},
                  {displaySleep}, {}, {clockScreen, test1Screen, test2Screen});

Screen ClockScreen({{-999, 999}}, {{0, -390}}, {},
                   {displaySleep}, {menuScreen}, {});

Screen Test1Screen({{-999, 999}}, {{0, -390}}, {},
                   {displaySleep}, {menuScreen}, {});

Screen Test2Screen({{-999, 999}}, {{-540, 0}, {540, 0}, {0, 390}, {0, -390}}, {},
                   {menuScreen}, {testR, testL, testU, testD}, {});

void displaySleep()
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

void test1Screen()
{
    activeScreen = &Test1Screen;
    tft.fillScreen(0);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.setCursor(20, 20, 4);
    tft.print("test1");
}

void testR()
{
    Serial.println("Right");
}
void testL()
{
    Serial.println("Left");
}
void testU()
{
    Serial.println("Up");
}
void testD()
{
    Serial.println("Down");
}

void test2Screen()
{
    activeScreen = &Test2Screen;
    tft.fillScreen(0);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.setCursor(20, 20, 4);
    tft.print("test2");
}

void updateClock(void *params)
{
    Serial.println("task");
    int i = 0;
    for (;;)
    { // TODO wprowadzić mutex an obiekty tft
        while (testFlag)
        {
            vTaskDelay(15);
        }
        testFlag = true;
        clockSprite.fillSprite(TFT_BLACK);
        clockSprite.setTextColor(TFT_WHITE);
        clockSprite.drawString(String(i), 0, 0);
        clockSprite.pushSprite(60, 144);
        testFlag = false;
        i++;

        vTaskDelay(100);
    }
}

void clockScreen()
{
    activeScreen = &ClockScreen;
    tft.fillScreen(0);
    delay(100);
    clockSprite.createSprite(360, 72);
    clockSprite.setColorDepth(8);
    clockSprite.setTextColor(TFT_WHITE);
    clockSprite.setTextSize(10);

    xTaskCreate(updateClock, "updateClock", 10024, NULL, 2, &updateDisplay_t);
}

void menuScreen()
{
    activeScreen = &MenuScreen;
    tft.fillScreen(0);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.setCursor(20, 20, 4);
    tft.print("MENU");
    tft.setTextSize(1);

    tft.setCursor(68, 141, 4);
    tft.print("Opcja 1");

    tft.setCursor(198, 141, 4);
    tft.print("Opcja 2");

    tft.setCursor(328, 141, 4);
    tft.print("Opcja 3");

    tft.fillRect(90, 190, 40, 40, TFT_RED);
    tft.fillRect(220, 190, 40, 40, TFT_GREEN);
    tft.fillRect(350, 190, 40, 40, TFT_BLUE);
}
