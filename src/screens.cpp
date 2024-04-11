#include <globals.h>

void menuScreen()
{
    tft.fillScreen(0);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.setCursor(20, 20, 4);
    tft.print("MENU");
    tft.setTextSize(1);

    tft.setCursor(68, 141, 4);
    tft.print("Opcja 1");

    tft.setCursor(329, 141, 4);
    tft.print("Opcja 2");

    tft.drawRect(90, 190, 40, 40, TFT_RED);
    tft.drawRect(220, 190, 40, 40, TFT_GREEN);
    tft.drawRect(350, 190, 40, 40, TFT_BLUE);
}