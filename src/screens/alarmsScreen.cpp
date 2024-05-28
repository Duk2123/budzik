#include <screens/screens.h>
#include <time.h>
#include <main.h>

TFT_eSprite AlarmsBackground = TFT_eSprite(&tft);
TFT_eSprite AlarmsInfo = TFT_eSprite(&tft);
TFT_eSprite AlarmsNav = TFT_eSprite(&tft);

int alarmsCurrentPage = 0;

void drawAlarms();
void drawAlarmsNav();

void alarmsChangePage()
{
    detectTouchSuspendCounter = 4;
    delay(16);
    if (72 < touchCurrentAction[2] && touchCurrentAction[2] < 102 && alarmsCurrentPage != 0)
        alarmsCurrentPage = alarmsCurrentPage - 1;
    else if (256 < touchCurrentAction[2] && touchCurrentAction[2] < 286 && alarmsCurrentPage != (int(alarms.size()) - 1) / 5)
        alarmsCurrentPage = alarmsCurrentPage + 1;
    else
        return;

    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        AlarmsBackground.fillSprite(BLACK);
        AlarmsBackground.pushSprite(0, 40);
    }
    delay(8);
    xSemaphoreGive(tftMutex);

    drawAlarms();
    drawAlarmsNav();
}

void alarmsGoToMenu()
{
    if (degToDirection(touchCurrentAction[5]) == 3)
    {
        detectTouchSuspendCounter = 4;
        vTaskDelete(updateScreenElement_t);
        delay(16);

        AlarmsBackground.deleteSprite();
        AlarmsInfo.deleteSprite();
        AlarmsNav.deleteSprite();

        delay(8);
        menuScreen();
    }
    else if (degToDirection(touchCurrentAction[5]) == 1)
    {
        detectTouchSuspendCounter = 4;
        vTaskDelete(updateScreenElement_t);
        delay(16);

        AlarmsBackground.deleteSprite();
        AlarmsInfo.deleteSprite();
        AlarmsNav.deleteSprite();

        delay(8);
        clockScreen();
        return;
    }
    else
        return;
}

bool alarmsKeyboardActive = false;

void addAlarm()
{
    alarmsKeyboardActive = true;
    xTaskCreate(keyboardPopUp, "keyboardPopUp", 20048, NULL, 3, &handlePopup_t);
}

void removeAlarm()
{
    coordinates coords1 = {32, 72};
    coordinates coords2 = {62, 102};
    for (int i = alarmsCurrentPage * 5; i < alarms.size() && i < alarmsCurrentPage * 5 + 5; i++)
    {
        if (coords1.y < touchCurrentAction[2] && touchCurrentAction[2] < coords2.y)
        {
            alarms.erase(alarms.begin() + i);
            xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
            {
                AlarmsBackground.fillSprite(BLACK);
                AlarmsBackground.pushSprite(0, 40);
            }
            delay(8);
            xSemaphoreGive(tftMutex);

            drawAlarms();
            drawAlarmsNav();
            return;
        }
        else
        {
            coords1.y = coords2.y + 16;
            coords2.y += 46;
        }
    }
}

void toggleAlarm()
{
    coordinates coords1 = {32, 72};
    coordinates coords2 = {62, 102};
    for (int i = alarmsCurrentPage * 5; i < alarms.size() && i < alarmsCurrentPage * 5 + 5; i++)
    {
        if (coords1.y < touchCurrentAction[2] && touchCurrentAction[2] < coords2.y)
        {
            alarms[i].toggleAlarm();
            xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
            {
                AlarmsBackground.fillSprite(BLACK);
                AlarmsBackground.pushSprite(0, 40);
            }
            delay(8);
            xSemaphoreGive(tftMutex);

            drawAlarms();
            drawAlarmsNav();
            return;
        }
        else
        {
            coords1.y = coords2.y + 16;
            coords2.y += 46;
        }
    }
}

ScreenObject AlarmsScreen({{32, 72, 62, 286}, {0, 0, 480, 360}}, {{0, 0, 480, 320}}, {{32, 72, 62, 286}, {418, 165, 448, 195}, {418, 72, 448, 286}},
                          {removeAlarm, displaySleep}, {alarmsGoToMenu}, {toggleAlarm, addAlarm, alarmsChangePage});

void updateAlarmsScreen(void *params)
{
    for (;;)
    {
        if (alarmsKeyboardActive && ulTaskNotifyTake(pdTRUE, portMAX_DELAY) != 0)
        {
            alarmsKeyboardActive = false;
            if (keyboardInput != "")
            {
                String time = keyboardInput.substring(0, 5);
                std::array<bool, 7> days;

                for (int i = 6; i < 13; i++)
                {
                    days[i - 6] = keyboardInput[i] == 't' ? true : false;
                }

                UserAlarm alarm(String(time), days);
                alarms.push_back(alarm);

                xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
                {
                    AlarmsBackground.fillSprite(BLACK);
                    AlarmsBackground.pushSprite(0, 40);
                }
                delay(8);
                xSemaphoreGive(tftMutex);

                drawAlarms();
                drawAlarmsNav();
            }
        }
        vTaskDelay(250);
    }
}

void drawAlarms()
{
    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        for (int i = alarmsCurrentPage * 5; i < alarms.size() && i < alarmsCurrentPage * 5 + 5; i++)
        {
            AlarmsInfo.fillSprite(BLACK);
            AlarmsInfo.fillRect(0, 0, 30, 30, alarms[i].isActive() ? WHITE : hexToColor("5D5E5F"));
            AlarmsInfo.drawString(alarms[i].getTime(), 38, 6);
            AlarmsInfo.drawString(alarms[i].getOccurrences(), 108, 6);
            AlarmsInfo.pushSprite(32, 72 + (46 * (i % 5)));
        }
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void drawAlarmsNav()
{
    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        AlarmsNav.fillSprite(BLACK);
        if (alarmsCurrentPage > 0)
        {
            AlarmsNav.setTextDatum(TC_DATUM);
            AlarmsNav.drawString("/\\", 15, 0, 4);
        }
        AlarmsNav.fillRect(0, 107, 30, 2, WHITE);
        AlarmsNav.fillRect(14, 93, 2, 30, WHITE);
        if (alarmsCurrentPage < (int(alarms.size()) - 1) / 5)
        {
            AlarmsNav.setTextDatum(BC_DATUM);
            AlarmsNav.drawString("\\/", 15, 214, 4);
        }
        AlarmsNav.pushSprite(418, 72);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void alarmsScreen()
{
    detectTouchSuspendCounter = 4;
    ActiveScreenElement = &AlarmsScreen;

    alarmsCurrentPage = 0;

    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        AlarmsBackground.createSprite(480, 320);
        AlarmsBackground.fillSprite(BLACK);
        AlarmsBackground.pushSprite(0, 40);

        AlarmsInfo.createSprite(320, 30);
        AlarmsNav.createSprite(30, 214);

        AlarmsInfo.setTextColor(WHITE);
        AlarmsNav.setTextColor(WHITE);

        AlarmsInfo.setTextFont(4);
        AlarmsNav.setTextFont(4);
    }
    delay(8);
    xSemaphoreGive(tftMutex);

    drawAlarms();
    drawAlarmsNav();

    xTaskCreate(updateAlarmsScreen, "updateWeatherScreen", 20048, NULL, 3, &updateScreenElement_t);
}