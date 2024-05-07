#include <main.h>
#include <screens/screens.h>
#include <touch.h>

TFT_eSprite keySprite = TFT_eSprite(&tft);

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
ScreenObject KeyboardPopUp({}, {}, {{388, 264, 476, 304}, {4, 120, 476, 344}},
                           {}, {}, {handleEnterPress, handleKeyPress});

void drawKey(coordinates coords, char label, bool isPressed = false)
{
    xSemaphoreTake(tftMutex, portMAX_DELAY);
    {
        keySprite.fillSprite(TFT_BLUE);
        keySprite.fillRoundRect(0, 0, 40, 40, 5, TFT_WHITE);
        keySprite.setCursor(12, 12, 2);
        keySprite.setTextSize(1);
        keySprite.setTextColor(TFT_BLACK);
        keySprite.print(label);
        keySprite.pushSprite(coords.x, coords.y);
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
            // Draw space
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

    while (isEndCalled == false)
    {
        vTaskDelay(16);
    }
    isEndCalled = false;
    activeScreenElement = callingElement;

    if (updateScreenElement_t != NULL)
    {
        vTaskResume(updateScreenElement_t);
    }
    Serial.println("Keyboard end");

    vTaskDelete(NULL);
}