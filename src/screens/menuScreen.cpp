#include <main.h>
#include <screens/screens.h>

ScreenObject MenuScreen({{0, 0, 480, 360}}, {}, {{90, 190, 130, 230}, {220, 190, 260, 230}, {350, 190, 390, 230}},
                        {displaySleep}, {}, {clockScreen, test1Screen, test2Screen});

void menuScreen()
{
    detectTouchSuspendCounter = 4;
    if (updateScreenElement_t != NULL && eTaskGetState(updateScreenElement_t) != 4)
    {
        vTaskDelete(updateScreenElement_t);
    }
    activeScreenElement = &MenuScreen;
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
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

        tft.fillRect(90, 190, 40, 40, hexToColor("FF0000"));
        tft.fillRect(220, 190, 40, 40, hexToColor("00FF00"));
        tft.fillRect(350, 190, 40, 40, hexToColor("0000FF"));
    }
    xSemaphoreGive(tftMutex);
}
