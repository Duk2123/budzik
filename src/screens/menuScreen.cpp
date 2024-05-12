#include <screens/screens.h>

TFT_eSprite menuBackground = TFT_eSprite(&tft);
TFT_eSprite menuButton = TFT_eSprite(&tft);
TFT_eSprite menuPages = TFT_eSprite(&tft);

int activePage = 0;

const int elements = 4;
String buttonLabels[elements] = {"Alarms", "Weather", "WiFi", "Settings"};
std::array<void (*)(), elements> buttonActions = {};
// TODO add button icons

void drawMenu();

void changePage()
{
    detectTouchSuspendCounter = 4;
    delay(16);
    if (degToDirection(touchCurrentAction[5]) == 2)
        activePage = activePage == 0 ? 1 : activePage - 1;
    else if (degToDirection(touchCurrentAction[5]) == 4)
        activePage = activePage == 1 ? 0 : activePage + 1;
    else if (degToDirection(touchCurrentAction[5]) == 1)
    {
        detectTouchSuspendCounter = 4;
        delay(16);
        menuBackground.deleteSprite();
        menuButton.deleteSprite();
        menuPages.deleteSprite();
        delay(8);
        clockScreen();
        return;
    }
    else
        return;
    drawMenu();
}

void button1() {}
void button2() {}
void button3() {}

void leftArrow()
{
    activePage = activePage == 0 ? 1 : activePage - 1;
    drawMenu();
}
void rightArrow()
{
    activePage = activePage == 1 ? 0 : activePage + 1;
    drawMenu();
}

ScreenObject MenuScreen({{0, 0, 480, 360}}, {{0, 0, 480, 360}}, {{32, 86, 142, 196}, {185, 86, 295, 196}, {338, 86, 448, 196}, {96, 256, 136, 308}, {344, 256, 384, 308}},
                        {displaySleep}, {changePage}, {button1, button2, button3, leftArrow, rightArrow});

void drawMenu()
{
    char buffer[50];
    sprintf(buffer, "%d/%d", activePage + 1, elements / 3 + 1);
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        for (int i = activePage * 3; (i < activePage * 3 + 3); i++)
        {
            if (i < elements)
            {
                menuButton.fillSprite(BLACK);
                menuButton.fillRect(0, 0, 110, 110, hexToColor("D9D9D9")); // TODO zmienić na rysowanie ikon
                menuButton.drawCentreString(buttonLabels[i], 55, 118, 4);
                menuButton.pushSprite(32 + (i % 3) * 153, 86);
            }
            else
            {
                menuButton.fillSprite(BLACK);
                menuButton.pushSprite(32 + (i % 3) * 153, 86);
            }
        }
        menuPages.fillSprite(BLACK);
        menuPages.setTextSize(2);
        menuPages.drawCentreString("<-", 20, 0, 4);
        menuPages.drawCentreString("->", 268, 0, 4);
        menuPages.setTextSize(1);
        menuPages.drawCentreString(String(buffer), 144, 14, 4);
        menuPages.pushSprite(96, 256);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void menuScreen()
{
    activePage = 0;
    detectTouchSuspendCounter = 4;
    activeScreenElement = &MenuScreen;
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        menuBackground.createSprite(480, 320);
        menuButton.createSprite(110, 146);
        menuPages.createSprite(288, 52);

        menuBackground.fillScreen(BLACK);
        menuBackground.pushSprite(0, 40);

        menuButton.setTextColor(WHITE);

        menuPages.setTextColor(WHITE);
    }
    delay(8);
    xSemaphoreGive(tftMutex);

    drawMenu();
}
