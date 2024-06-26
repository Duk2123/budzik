#include <main.h>
#include <screens/screens.h>
#include <touch.h>
#include "screens.h"

#define KEY_COLOR hexToColor("D9D9D9")
#define KEY_COLOR_ALT hexToColor("5D5E5F")
#define BG_COLOR hexToColor("2C2E34")

TFT_eSprite KeySprite = TFT_eSprite(&tft);
TFT_eSprite SpaceKeySprite = TFT_eSprite(&tft);
TFT_eSprite SpecialKeySprite = TFT_eSprite(&tft);
TFT_eSprite SpecialKeyLargeSprite = TFT_eSprite(&tft);
TFT_eSprite KeyboardBackground = TFT_eSprite(&tft);
TFT_eSprite KeyboardText = TFT_eSprite(&tft);

String keyboardBuffer;
String keyboardInput;

char *activeKeys;
char letters[29] = {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', ' ', '.'};
char letters_capitalized[29] = {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', ' ', '.'};
char symbols[29] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '@', '#', '$', '_', '&', '-', '+', '(', ')', '*', '"', '\'', ':', ';', '!', '?', ',', ' ', '.'};
char symbols_alt[29] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '~', '`', '|', '^', '=', '{', '}', '[', ']', '\\', '/', '?', ':', ';', '!', '?', '<', ' ', '>'};

bool isEndCalled = false;
int buttonPressed = -1;

void drawKeyboard();

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
            keyboardBuffer += activeKeys[i];
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
    Serial.println(keyboardBuffer);
    keyboardInput = keyboardBuffer;
    keyboardBuffer = "";
    isEndCalled = true;
}

void handleShiftPress()
{
    Serial.println("Shift");
    buttonPressed = 29;
    delay(16);
    if (activeKeys == letters)
    {
        activeKeys = letters_capitalized;
    }
    else if (activeKeys == letters_capitalized)
    {
        activeKeys = letters;
    }
    else if (activeKeys == symbols)
    {
        activeKeys = symbols_alt;
    }
    else
    {
        activeKeys = symbols;
    }

    drawKeyboard();
}

void handleBackPress()
{
    keyboardBuffer.remove(keyboardBuffer.length() - 1);
    buttonPressed = 30;
}

void handleBackHold()
{
    keyboardBuffer = "";
    buttonPressed = 30;
}

void handleModePress()
{
    Serial.println("Mode");
    buttonPressed = 31;
    delay(16);
    if (activeKeys == letters || activeKeys == letters_capitalized)
    {
        activeKeys = symbols;
    }
    else
    {
        activeKeys = letters;
    }

    drawKeyboard();
}

ScreenObject KeyboardPopUp({
                               {412, 216, 476, 256},
                           },
                           {}, {{4, 216, 68, 256}, {4, 264, 92, 304}, {412, 216, 476, 256}, {388, 264, 476, 304}, {4, 120, 476, 344}}, {handleBackHold}, {}, {handleShiftPress, handleModePress, handleBackPress, handleEnterPress, handleKeyPress});

void drawKey(coordinates coords, char label, bool isPressed = false)
{
    if (isPressed)
    {
        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {
            KeySprite.fillSprite(BG_COLOR);
            KeySprite.fillRoundRect(0, 0, 40, 40, 10, KEY_COLOR_ALT);
            KeySprite.drawCentreString(String(label), 20, 13, 2);
            KeySprite.pushSprite(coords.x, coords.y);

            delay(64);

            KeySprite.fillSprite(BG_COLOR);
            KeySprite.fillRoundRect(0, 0, 40, 40, 10, KEY_COLOR);
            KeySprite.drawCentreString(String(label), 20, 13, 2);
            KeySprite.pushSprite(coords.x, coords.y);

            delay(8);
        }
        xSemaphoreGive(tftMutex);
    }
    else
    {
        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {
            KeySprite.fillSprite(BG_COLOR);
            KeySprite.fillRoundRect(0, 0, 40, 40, 10, KEY_COLOR);
            KeySprite.drawCentreString(String(label), 20, 13, 2);
            KeySprite.pushSprite(coords.x, coords.y);
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
}

void drawSpaceKey(bool isPressed = false)
{
    if (isPressed)
    {
        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {
            SpaceKeySprite.fillSprite(BG_COLOR);
            SpaceKeySprite.fillRoundRect(0, 0, 184, 40, 10, KEY_COLOR_ALT);
            SpaceKeySprite.pushSprite(148, 264);

            delay(64);

            SpaceKeySprite.fillSprite(BG_COLOR);
            SpaceKeySprite.fillRoundRect(0, 0, 184, 40, 10, KEY_COLOR);
            SpaceKeySprite.pushSprite(148, 264);
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
    else
    {
        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {
            SpaceKeySprite.fillSprite(BG_COLOR);
            SpaceKeySprite.fillRoundRect(0, 0, 184, 40, 10, KEY_COLOR);
            SpaceKeySprite.pushSprite(148, 264);
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
}

void drawShiftKey(bool isPressed = false)
{
    String label;
    if (activeKeys == letters || activeKeys == letters_capitalized)
    {
        label = "Shift";
    }
    else if (activeKeys == symbols)
    {
        label = "=/<";
    }
    else
    {
        label = "?123";
    }

    if (isPressed)
    {
        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {
            SpecialKeySprite.fillSprite(BG_COLOR);
            SpecialKeySprite.fillRoundRect(0, 0, 64, 40, 10, KEY_COLOR_ALT);
            SpecialKeySprite.drawCentreString(String(label), 32, 13, 2);
            SpecialKeySprite.pushSprite(4, 216);

            delay(64);

            SpecialKeySprite.fillSprite(BG_COLOR);
            SpecialKeySprite.fillRoundRect(0, 0, 64, 40, 10, KEY_COLOR);
            SpecialKeySprite.drawCentreString(String(label), 32, 13, 2);
            SpecialKeySprite.pushSprite(4, 216);
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
    else
    {
        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {
            SpecialKeySprite.fillSprite(BG_COLOR);
            SpecialKeySprite.fillRoundRect(0, 0, 64, 40, 10, KEY_COLOR);
            SpecialKeySprite.drawCentreString(String(label), 32, 13, 2);
            SpecialKeySprite.pushSprite(4, 216);
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
}

void drawModeKey(bool isPressed = false)
{
    String label;
    if (activeKeys == letters || activeKeys == letters_capitalized)
    {
        label = "?123";
    }
    else
    {
        label = "ABC";
    }

    if (isPressed)
    {
        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {
            SpecialKeyLargeSprite.fillSprite(BG_COLOR);
            SpecialKeyLargeSprite.fillRoundRect(0, 0, 88, 40, 10, KEY_COLOR_ALT);
            SpecialKeyLargeSprite.drawCentreString(String(label), 44, 13, 2);
            SpecialKeyLargeSprite.pushSprite(4, 264);

            delay(64);

            SpecialKeyLargeSprite.fillSprite(BG_COLOR);
            SpecialKeyLargeSprite.fillRoundRect(0, 0, 88, 40, 10, KEY_COLOR);
            SpecialKeyLargeSprite.drawCentreString(String(label), 44, 13, 2);
            SpecialKeyLargeSprite.pushSprite(4, 264);
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
    else
    {
        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {
            SpecialKeyLargeSprite.fillSprite(BG_COLOR);
            SpecialKeyLargeSprite.fillRoundRect(0, 0, 88, 40, 10, KEY_COLOR);
            SpecialKeyLargeSprite.drawCentreString(String(label), 44, 13, 2);
            SpecialKeyLargeSprite.pushSprite(4, 264);
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
}

void drawBackKey(bool isPressed = false)
{
    if (isPressed)
    {
        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {
            SpecialKeySprite.fillSprite(BG_COLOR);
            SpecialKeySprite.fillRoundRect(0, 0, 64, 40, 10, KEY_COLOR_ALT);
            SpecialKeySprite.drawCentreString("<--", 32, 13, 2);
            SpecialKeySprite.pushSprite(412, 216);

            delay(64);

            SpecialKeySprite.fillSprite(BG_COLOR);
            SpecialKeySprite.fillRoundRect(0, 0, 64, 40, 10, KEY_COLOR);
            SpecialKeySprite.drawCentreString("<--", 32, 13, 2);
            SpecialKeySprite.pushSprite(412, 216);
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
    else
    {
        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {
            SpecialKeySprite.fillSprite(BG_COLOR);
            SpecialKeySprite.fillRoundRect(0, 0, 64, 40, 10, KEY_COLOR);
            SpecialKeySprite.drawCentreString("<--", 32, 13, 2);
            SpecialKeySprite.pushSprite(412, 216);
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
}

void drawEnterKey(bool isPressed = false)
{
    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        SpecialKeyLargeSprite.fillSprite(BG_COLOR);
        SpecialKeyLargeSprite.fillRoundRect(0, 0, 88, 40, 10, KEY_COLOR);
        SpecialKeyLargeSprite.drawCentreString("Enter", 44, 13, 2);
        SpecialKeyLargeSprite.pushSprite(388, 264);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void drawKeyboard()
{
    coordinates coords1 = {4, 120};
    coordinates coords2 = {44, 160}; // TODO przerobić ?
    for (int i = 0; i < 29; i++)
    {
        if (i != 27)
        {
            drawKey(coords1, activeKeys[i]);
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
    drawShiftKey(false);
    drawModeKey(false);
    drawBackKey(false);
    drawEnterKey(false);
}

void drawKeyboardText()
{
    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        KeyboardText.fillSprite(BLACK);
        KeyboardText.drawString(keyboardBuffer, 0, 0, 4);
        KeyboardText.pushSprite(16, 32);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void keyboardPopUp(void *params)
{
    vTaskSuspend(statusBar_t);
    if (updateScreenElement_t != NULL && eTaskGetState(updateScreenElement_t) != 4)
    {
        vTaskSuspend(updateScreenElement_t);
    }

    ScreenObject *callingElement = ActiveScreenElement;
    ActiveScreenElement = &KeyboardPopUp;

    vTaskDelay(32);
    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        KeySprite.createSprite(40, 40);
        SpaceKeySprite.createSprite(184, 40);
        SpecialKeySprite.createSprite(64, 40);
        SpecialKeyLargeSprite.createSprite(88, 40);
        KeyboardText.createSprite(448, 38);

        KeyboardBackground.createSprite(480, 360);
        KeyboardBackground.fillRect(0, 0, 480, 102, BLACK);
        KeyboardBackground.fillRect(0, 102, 480, 218, BG_COLOR);
        KeyboardBackground.pushSprite(0, 0);

        SpecialKeySprite.setTextSize(1);
        SpecialKeySprite.setTextColor(BLACK);

        KeyboardText.setTextSize(1);
        KeyboardText.setTextColor(WHITE);

        SpecialKeyLargeSprite.setTextSize(1);
        SpecialKeyLargeSprite.setTextColor(BLACK);

        KeySprite.setTextSize(1);
        KeySprite.setTextColor(BLACK);
    }
    vTaskDelay(32);
    xSemaphoreGive(tftMutex);
    activeKeys = letters;
    drawKeyboard();

    while (isEndCalled == false)
    {

        if (buttonPressed != -1)
        {
            if (buttonPressed < 10)
            {
                drawKey(coordinates{4 + (48 * buttonPressed), 120}, activeKeys[buttonPressed], true);
            }
            else if (buttonPressed < 19)
            {
                drawKey(coordinates{28 + (48 * (buttonPressed - 10)), 168}, activeKeys[buttonPressed], true);
            }
            else if (buttonPressed < 26)
            {
                drawKey(coordinates{76 + (48 * (buttonPressed - 19)), 216}, activeKeys[buttonPressed], true);
            }
            else if (buttonPressed == 26)
            {
                drawKey(coordinates{100, 264}, activeKeys[buttonPressed], true);
            }
            else if (buttonPressed == 27)
            {
                drawSpaceKey(true);
            }
            else if (buttonPressed == 28)
            {
                drawKey(coordinates{340, 264}, activeKeys[buttonPressed], true);
            }
            else if (buttonPressed == 29)
            {
                drawShiftKey(true);
            }
            else if (buttonPressed == 30)
            {
                drawBackKey(true);
            }
            else if (buttonPressed == 31)
            {
                drawModeKey(true);
            }
            drawKeyboardText();
            buttonPressed = -1;
        }

        vTaskDelay(16);
    }
    isEndCalled = false;
    ActiveScreenElement = callingElement;

    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        KeySprite.deleteSprite();
        SpaceKeySprite.deleteSprite();
        SpecialKeySprite.deleteSprite();
        SpecialKeyLargeSprite.deleteSprite();
        KeyboardBackground.deleteSprite();
        KeyboardText.deleteSprite();
    }
    vTaskDelay(8);
    xSemaphoreGive(tftMutex);

    if (updateScreenElement_t != NULL)
    {
        vTaskResume(updateScreenElement_t);
        vTaskNotifyGiveFromISR(updateScreenElement_t, NULL);
    }
    Serial.println("Keyboard end");

    vTaskResume(statusBar_t);
    drawWiFiStatus();
    drawStatusBarClock();

    vTaskDelete(NULL);
}
