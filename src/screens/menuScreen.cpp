#include <screens/screens.h>

TFT_eSprite MenuBackground = TFT_eSprite(&tft);
TFT_eSprite MenuButton = TFT_eSprite(&tft);
TFT_eSprite MenuPages = TFT_eSprite(&tft);

int activePage = 0;

const int elements = 4;
String buttonLabels[elements] = {"Alarms", "Weather", "WiFi", "Settings"};
std::array<void (*)(), elements> buttonActions = {alarmsScreen, weatherScreen, wifiScreen};
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
        MenuBackground.deleteSprite();
        MenuButton.deleteSprite();
        MenuPages.deleteSprite();
        delay(8);
        clockScreen();
        return;
    }
    else
        return;
    drawMenu();
}

void button1()
{
    if (activePage * 3 < elements)
        buttonActions[activePage * 3]();
}
void button2()
{
    if (1 + activePage * 3 < elements)
        buttonActions[1 + activePage * 3]();
}
void button3()
{
    if (2 + activePage * 3 < elements)
        buttonActions[2 + activePage * 3]();
}

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
    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        for (int i = activePage * 3; (i < activePage * 3 + 3); i++)
        {
            if (i < elements)
            {
                MenuButton.fillSprite(BLACK);
                MenuButton.fillRect(0, 0, 110, 110, hexToColor("D9D9D9")); // TODO zmienić na rysowanie ikon
                MenuButton.drawCentreString(buttonLabels[i], 55, 118, 4);
                MenuButton.pushSprite(32 + (i % 3) * 153, 86);
            }
            else
            {
                MenuButton.fillSprite(BLACK);
                MenuButton.pushSprite(32 + (i % 3) * 153, 86);
            }
        }
        MenuPages.fillSprite(BLACK);
        MenuPages.setTextSize(2);
        MenuPages.drawCentreString("<-", 20, 0, 4);
        MenuPages.drawCentreString("->", 268, 0, 4);
        MenuPages.setTextSize(1);
        MenuPages.drawCentreString(String(buffer), 144, 14, 4);
        MenuPages.pushSprite(96, 256);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void menuScreen()
{
    activePage = 0;
    detectTouchSuspendCounter = 4;
    ActiveScreenElement = &MenuScreen;
    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        MenuBackground.createSprite(480, 320);
        MenuButton.createSprite(110, 146);
        MenuPages.createSprite(288, 52);

        MenuBackground.fillScreen(BLACK);
        MenuBackground.pushSprite(0, 40);

        MenuButton.setTextColor(WHITE);
        MenuPages.setTextColor(WHITE);
    }
    delay(8);
    xSemaphoreGive(tftMutex);

    drawMenu();
}
