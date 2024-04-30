#include <main.h>
#include <screens/screens.h>

ScreenObject *activeScreenElement;

ScreenObject Test1Screen({{-999, 999}}, {{0, -390}}, {},
                         {displaySleep}, {menuScreen}, {});

ScreenObject Test2Screen({{-999, 999}}, {{-540, 0}, {540, 0}, {0, 390}, {0, -390}}, {},
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
