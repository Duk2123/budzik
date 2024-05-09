#include <main.h>
#include <screens/screens.h>
#include <touch.h>
#include "screens.h"

TFT_eSprite keySprite = TFT_eSprite(&tft);
TFT_eSprite spaceKeySprite = TFT_eSprite(&tft);
TFT_eSprite specialKeySprite = TFT_eSprite(&tft);
TFT_eSprite specialKeyLargeSprite = TFT_eSprite(&tft);
TFT_eSprite keyboardBackground = TFT_eSprite(&tft);
TFT_eSprite keyboardText = TFT_eSprite(&tft);

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
        xSemaphoreTake(tftMutex, portMAX_DELAY);
        {
            keySprite.fillSprite(hexToColor("2C2E34"));
            keySprite.fillRoundRect(0, 0, 40, 40, 10, hexToColor("5D5E5F"));
            keySprite.setCursor(16, 12, 2);
            keySprite.setTextSize(1);
            keySprite.setTextColor(TFT_BLACK);
            keySprite.print(label);
            keySprite.pushSprite(coords.x, coords.y);

            delay(64);

            keySprite.fillSprite(hexToColor("2C2E34"));
            keySprite.fillRoundRect(0, 0, 40, 40, 10, hexToColor("D9D9D9"));
            keySprite.setCursor(16, 12, 2);
            keySprite.setTextSize(1);
            keySprite.setTextColor(TFT_BLACK);
            keySprite.print(label);
            keySprite.pushSprite(coords.x, coords.y);

            delay(8);
        }
        xSemaphoreGive(tftMutex);
    }
    else
    {
        xSemaphoreTake(tftMutex, portMAX_DELAY);
        {
            keySprite.fillSprite(hexToColor("2C2E34"));
            keySprite.fillRoundRect(0, 0, 40, 40, 10, hexToColor("D9D9D9"));
            keySprite.setCursor(16, 12, 2);
            keySprite.setTextSize(1);
            keySprite.setTextColor(TFT_BLACK);
            keySprite.print(label);
            keySprite.pushSprite(coords.x, coords.y);
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
}

void drawSpaceKey(bool isPressed = false)
{
    if (isPressed)
    {
        xSemaphoreTake(tftMutex, portMAX_DELAY);
        {
            spaceKeySprite.fillSprite(hexToColor("2C2E34"));
            spaceKeySprite.fillRoundRect(0, 0, 184, 40, 10, hexToColor("5D5E5F"));
            spaceKeySprite.pushSprite(148, 264);

            delay(64);

            spaceKeySprite.fillSprite(hexToColor("2C2E34"));
            spaceKeySprite.fillRoundRect(0, 0, 184, 40, 10, hexToColor("D9D9D9"));
            spaceKeySprite.pushSprite(148, 264);
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
    else
    {
        xSemaphoreTake(tftMutex, portMAX_DELAY);
        {
            spaceKeySprite.fillSprite(hexToColor("2C2E34"));
            spaceKeySprite.fillRoundRect(0, 0, 184, 40, 10, hexToColor("D9D9D9"));
            spaceKeySprite.pushSprite(148, 264);
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
        xSemaphoreTake(tftMutex, portMAX_DELAY);
        {
            specialKeySprite.fillSprite(hexToColor("2C2E34"));
            specialKeySprite.fillRoundRect(0, 0, 64, 40, 10, hexToColor("5D5E5F"));
            specialKeySprite.setCursor(8, 12, 2);
            specialKeySprite.setTextSize(1);
            specialKeySprite.setTextColor(TFT_BLACK);
            specialKeySprite.print(label);
            specialKeySprite.pushSprite(4, 216);

            delay(64);

            specialKeySprite.fillSprite(hexToColor("2C2E34"));
            specialKeySprite.fillRoundRect(0, 0, 64, 40, 10, hexToColor("D9D9D9"));
            specialKeySprite.setCursor(8, 12, 2);
            specialKeySprite.setTextSize(1);
            specialKeySprite.setTextColor(TFT_BLACK);
            specialKeySprite.print(label);
            specialKeySprite.pushSprite(4, 216);
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
    else
    {
        xSemaphoreTake(tftMutex, portMAX_DELAY);
        {
            specialKeySprite.fillSprite(hexToColor("2C2E34"));
            specialKeySprite.fillRoundRect(0, 0, 64, 40, 10, hexToColor("D9D9D9"));
            specialKeySprite.setCursor(8, 12, 2);
            specialKeySprite.setTextSize(1);
            specialKeySprite.setTextColor(TFT_BLACK);
            specialKeySprite.print(label);
            specialKeySprite.pushSprite(4, 216);
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
        xSemaphoreTake(tftMutex, portMAX_DELAY);
        {
            specialKeyLargeSprite.fillSprite(hexToColor("2C2E34"));
            specialKeyLargeSprite.fillRoundRect(0, 0, 88, 40, 10, hexToColor("5D5E5F"));
            specialKeyLargeSprite.setCursor(22, 12, 2);
            specialKeyLargeSprite.setTextSize(1);
            specialKeyLargeSprite.setTextColor(TFT_BLACK);
            specialKeyLargeSprite.print(label);
            specialKeyLargeSprite.pushSprite(4, 264);

            delay(64);

            specialKeyLargeSprite.fillSprite(hexToColor("2C2E34"));
            specialKeyLargeSprite.fillRoundRect(0, 0, 88, 40, 10, hexToColor("D9D9D9"));
            specialKeyLargeSprite.setCursor(22, 12, 2);
            specialKeyLargeSprite.setTextSize(1);
            specialKeyLargeSprite.setTextColor(TFT_BLACK);
            specialKeyLargeSprite.print(label);
            specialKeyLargeSprite.pushSprite(4, 264);
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
    else
    {
        xSemaphoreTake(tftMutex, portMAX_DELAY);
        {
            specialKeyLargeSprite.fillSprite(hexToColor("2C2E34"));
            specialKeyLargeSprite.fillRoundRect(0, 0, 88, 40, 10, hexToColor("D9D9D9"));
            specialKeyLargeSprite.setCursor(22, 12, 2);
            specialKeyLargeSprite.setTextSize(1);
            specialKeyLargeSprite.setTextColor(TFT_BLACK);
            specialKeyLargeSprite.print(label);
            specialKeyLargeSprite.pushSprite(4, 264);
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
}

void drawBackKey(bool isPressed = false)
{
    if (isPressed)
    {
        xSemaphoreTake(tftMutex, portMAX_DELAY);
        {
            specialKeySprite.fillSprite(hexToColor("2C2E34"));
            specialKeySprite.fillRoundRect(0, 0, 64, 40, 10, hexToColor("5D5E5F"));
            specialKeySprite.setCursor(12, 12, 2);
            specialKeySprite.setTextSize(1);
            specialKeySprite.setTextColor(TFT_BLACK);
            specialKeySprite.print("Back");
            specialKeySprite.pushSprite(412, 216);

            delay(64);

            specialKeySprite.fillSprite(hexToColor("2C2E34"));
            specialKeySprite.fillRoundRect(0, 0, 64, 40, 10, hexToColor("D9D9D9"));
            specialKeySprite.setCursor(12, 12, 2);
            specialKeySprite.setTextSize(1);
            specialKeySprite.setTextColor(TFT_BLACK);
            specialKeySprite.print("Back");
            specialKeySprite.pushSprite(412, 216);
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
    else
    {
        xSemaphoreTake(tftMutex, portMAX_DELAY);
        {
            specialKeySprite.fillSprite(hexToColor("2C2E34"));
            specialKeySprite.fillRoundRect(0, 0, 64, 40, 10, hexToColor("D9D9D9"));
            specialKeySprite.setCursor(12, 12, 2);
            specialKeySprite.setTextSize(1);
            specialKeySprite.setTextColor(TFT_BLACK);
            specialKeySprite.print("Back");
            specialKeySprite.pushSprite(412, 216);
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }
}

void drawEnterKey(bool isPressed = false)
{
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        specialKeyLargeSprite.fillSprite(hexToColor("2C2E34"));
        specialKeyLargeSprite.fillRoundRect(0, 0, 88, 40, 10, hexToColor("D9D9D9"));
        specialKeyLargeSprite.setCursor(24, 12, 2);
        specialKeyLargeSprite.setTextSize(1);
        specialKeyLargeSprite.setTextColor(TFT_BLACK); // TODO pozmieniać na hexToColor
        specialKeyLargeSprite.print("Enter");
        specialKeyLargeSprite.pushSprite(388, 264);
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
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        keyboardText.fillSprite(hexToColor("000000"));
        keyboardText.setCursor(0, 0, 4);
        keyboardText.setTextSize(1);
        keyboardText.setTextColor(hexToColor("FFFFFF"));
        keyboardText.print(keyboardBuffer);
        keyboardText.pushSprite(16, 32);
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

    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        keySprite.createSprite(40, 40);
        spaceKeySprite.createSprite(184, 40);
        specialKeySprite.createSprite(64, 40);
        specialKeyLargeSprite.createSprite(88, 40);
        keyboardText.createSprite(448, 38);

        keyboardBackground.createSprite(480, 360);
        keyboardBackground.fillRect(0, 0, 480, 102, hexToColor("000000"));
        keyboardBackground.fillRect(0, 102, 480, 218, hexToColor("2C2E34"));
        keyboardBackground.pushSprite(0, 0);
    }
    vTaskDelay(8);
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
    activeScreenElement = callingElement;

    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        keySprite.deleteSprite();
        spaceKeySprite.deleteSprite();
        specialKeySprite.deleteSprite();
        specialKeyLargeSprite.deleteSprite();
        keyboardBackground.deleteSprite();
        keyboardText.deleteSprite();
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
