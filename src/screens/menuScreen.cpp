#include <main.h>
#include <screens/screens.h>

ScreenObject MenuScreen({{-999, 999}}, {}, {{110, 210}, {240, 210}, {370, 210}},
                        {displaySleep}, {}, {clockScreen, test1Screen, test2Screen});

void menuScreen()
{
    if (updateDisplay_t != NULL && eTaskGetState(updateDisplay_t) != 4)
    {
        vTaskDelete(updateDisplay_t);
    }
    activeScreenElement = &MenuScreen;
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
