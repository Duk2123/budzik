#include <main.h>
#include <screens/screens.h>
#include <touch.h>

TFT_eSprite keySprite = TFT_eSprite(&tft);
TFT_eSprite spaceKeySprite = TFT_eSprite(&tft);
TFT_eSprite specialKeySprite = TFT_eSprite(&tft);
TFT_eSprite specialKeyLargeSprite = TFT_eSprite(&tft);
TFT_eSprite keyboardBackground = TFT_eSprite(&tft);

char letters[29] = {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', ' ', '.'};
char symbols[29] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '@', '#', '$', '_', '&', '-', '+', '(', ')', '*', '"', '\'', ':', ';', '!', '?', ',', ' ', '.'};
char symbols_alt[29] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '~', '`', '|', '^', '=', '{', '}', '[', ']', '\\', '/', '?', ':', ';', '!', '?', '<', ' ', '>'};

bool isEndCalled = false;
int buttonPressed = -1;

void handleKeyPress()
{
    buttonPressed = -1;
    coordinates coords1 = {4, 120};
    coordinates coords2 = {44, 160};
    for (int i = 0; i < 29; i++)
    {
        if (coords1.x < touchCurrentAction[1] && touchCurrentAction[1] < coords2.x &&
            coords1.y < touchCurrentAction[2] && touchCurrentAction[2] < coords2.y)
        {
            Serial.println(letters[i]);
            buttonPressed = i;
            break;
        }
        else
        {
            coords1.x = coords2.x + 8;
            coords2.x = coords1.x + 40;
            if (i == 9)
            {
                coords1 = {28, 168};
                coords2 = {68, 208};
            }
            else if (i == 18)
            {
                coords1 = {76, 216};
                coords2 = {116, 256};
            }
            else if (i == 25)
            {
                coords1 = {100, 264};
                coords2 = {140, 304};
            }
            else if (i == 26)
            {
                coords1 = {148, 264};
                coords2 = {332, 304};
            }
        }
    }
}

void handleEnterPress()
{
    Serial.println("Enter");
    isEndCalled = true;
}

void handleShiftPress()
{
    Serial.println("Shift");
}

void handleBackPress()
{
    Serial.println("Back");
}

void handleModePress()
{
    Serial.println("Mode");
}

ScreenObject KeyboardPopUp({}, {}, {{4, 216, 68, 256}, {4, 264, 92, 304}, {412, 216, 476, 256}, {388, 264, 476, 304}, {4, 120, 476, 344}},
                           {}, {}, {handleShiftPress, handleModePress, handleBackPress, handleEnterPress, handleKeyPress});

void drawKey(coordinates coords, char label, bool isPressed = false)
{
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        keySprite.fillSprite(TFT_BLUE);
        keySprite.fillRoundRect(0, 0, 40, 40, 5, TFT_WHITE);
        keySprite.setCursor(16, 12, 2);
        keySprite.setTextSize(1);
        keySprite.setTextColor(TFT_BLACK);
        keySprite.print(label);
        keySprite.pushSprite(coords.x, coords.y);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void drawSpaceKey(bool isPressed = false)
{
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        spaceKeySprite.fillSprite(TFT_BLUE);
        spaceKeySprite.fillRoundRect(0, 0, 184, 40, 5, TFT_WHITE);
        spaceKeySprite.pushSprite(148, 264);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void drawShiftKey(bool isPressed = false)
{
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        specialKeySprite.fillSprite(TFT_BLUE);
        specialKeySprite.fillRoundRect(0, 0, 64, 40, 5, TFT_WHITE);
        specialKeySprite.setCursor(8, 12, 2);
        specialKeySprite.setTextSize(1);
        specialKeySprite.setTextColor(TFT_BLACK);
        specialKeySprite.print("Shift");
        specialKeySprite.pushSprite(4, 216);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void drawModeKey(bool isPressed = false)
{
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        specialKeyLargeSprite.fillSprite(TFT_BLUE);
        specialKeyLargeSprite.fillRoundRect(0, 0, 88, 40, 5, TFT_WHITE);
        specialKeyLargeSprite.setCursor(22, 12, 2);
        specialKeyLargeSprite.setTextSize(1);
        specialKeyLargeSprite.setTextColor(TFT_BLACK);
        specialKeyLargeSprite.print("?123");
        specialKeyLargeSprite.pushSprite(4, 264);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void drawBackKey(bool isPressed = false)
{
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        specialKeySprite.fillSprite(TFT_BLUE);
        specialKeySprite.fillRoundRect(0, 0, 64, 40, 5, TFT_WHITE);
        specialKeySprite.setCursor(12, 12, 2);
        specialKeySprite.setTextSize(1);
        specialKeySprite.setTextColor(TFT_BLACK);
        specialKeySprite.print("Back");
        specialKeySprite.pushSprite(412, 216);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void drawEnterKey(bool isPressed = false)
{
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        specialKeyLargeSprite.fillSprite(TFT_BLUE);
        specialKeyLargeSprite.fillRoundRect(0, 0, 88, 40, 5, TFT_WHITE);
        specialKeyLargeSprite.setCursor(24, 12, 2);
        specialKeyLargeSprite.setTextSize(1);
        specialKeyLargeSprite.setTextColor(TFT_BLACK);
        specialKeyLargeSprite.print("Enter");
        specialKeyLargeSprite.pushSprite(388, 264);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void keyboardPopUp(void *params)
{
    if (updateScreenElement_t != NULL && eTaskGetState(updateScreenElement_t) != 4)
    {
        vTaskSuspend(updateScreenElement_t);
    }

    ScreenObject *callingElement = activeScreenElement;
    activeScreenElement = &KeyboardPopUp;
    coordinates coords1 = {4, 120};
    coordinates coords2 = {44, 160};

    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        keySprite.createSprite(40, 40);
        spaceKeySprite.createSprite(184, 40);
        specialKeySprite.createSprite(64, 40);
        specialKeyLargeSprite.createSprite(88, 40);
        keyboardBackground.createSprite(480, 216);
        keyboardBackground.fillSprite(TFT_BLUE);
        keyboardBackground.pushSprite(0, 104);
    }
    vTaskDelay(8);
    xSemaphoreGive(tftMutex);
    for (int i = 0; i < 29; i++)
    {
        if (i != 27)
        {
            drawKey(coords1, letters[i]);
        }
        else
        {
            drawSpaceKey();
        }

        coords1.x = coords2.x + 8;
        coords2.x = coords1.x + 40;
        if (i == 9)
        {
            coords1 = {28, 168};
            coords2 = {68, 208};
        }
        else if (i == 18)
        {
            coords1 = {76, 216};
            coords2 = {116, 256};
        }
        else if (i == 25)
        {
            coords1 = {100, 264};
            coords2 = {140, 304};
        }
        else if (i == 26)
        {
            coords1 = {148, 264};
            coords2 = {332, 304};
        }
    }
    drawShiftKey();
    drawModeKey();
    drawBackKey();
    drawEnterKey();

    while (isEndCalled == false)
    {
        vTaskDelay(16);
        // TODO add press effect
    }
    isEndCalled = false;
    activeScreenElement = callingElement;

    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        keySprite.deleteSprite();
        spaceKeySprite.deleteSprite();
        specialKeySprite.deleteSprite();
        specialKeyLargeSprite.deleteSprite();
        keyboardBackground.deleteSprite();
    }
    vTaskDelay(8);
    xSemaphoreGive(tftMutex);

    if (updateScreenElement_t != NULL)
    {
        vTaskResume(updateScreenElement_t);
    }
    Serial.println("Keyboard end");

    vTaskDelete(NULL);
}