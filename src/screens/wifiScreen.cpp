#include <screens/screens.h>
#include <network.h>
#include <touch.h>

TFT_eSprite WifiBackground = TFT_eSprite(&tft);
TFT_eSprite WifiStatus = TFT_eSprite(&tft);
TFT_eSprite AvailableNetwork = TFT_eSprite(&tft);

String wifiStatuses[3] = {"Not connected", "Connecting...", ""};
int wifiStatusIndex = 0;

int wifiActivePage = 0;
std::vector<String> networks;

void WifiGoToMenu()
{
    if (degToDirection(touchCurrentAction[5]) == 3)
    {
        detectTouchSuspendCounter = 4;
        delay(16);

        WifiBackground.deleteSprite();
        WifiStatus.deleteSprite();
        AvailableNetwork.deleteSprite();

        changeToIpMode = false;
        drawWiFiStatus();

        delay(8);
        menuScreen();
    }
    else if (degToDirection(touchCurrentAction[5]) == 1)
    {
        detectTouchSuspendCounter = 4;
        delay(16);

        WifiBackground.deleteSprite();
        WifiStatus.deleteSprite();
        AvailableNetwork.deleteSprite();

        changeToIpMode = false;
        drawWiFiStatus();

        delay(8);
        clockScreen();
    }
}

void drawAvailableNetworks()
{
    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        for (int i = wifiActivePage * 6; i < wifiActivePage * 6 + 6 && i < networks.size(); i++)
        {
            AvailableNetwork.fillSprite(BLACK);
            AvailableNetwork.drawString(networks[i].length() > 11 ? networks[i].substring(0, 7) + "..." + networks[i].substring(networks[i].length() - 3, networks[i].length()) : networks[i], 0, 4, 4);
            AvailableNetwork.fillRect(180, 0, 30, 30, WHITE);
            AvailableNetwork.pushSprite(16 + (238 * ((i / 3) % 2)), 182 + (48 * (i % 3)));
        }
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void drawWifiStatus()
{
    char buffer[50];
    int temp;
    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        WifiStatus.setTextDatum(TL_DATUM);
        WifiStatus.fillSprite(BLACK);
        WifiStatus.drawCentreString(wifiStatuses[wifiStatusIndex].length() > 20 ? wifiStatuses[wifiStatusIndex].substring(0, 14) + "..." + wifiStatuses[wifiStatusIndex].substring(wifiStatuses[wifiStatusIndex].length() - 3, wifiStatuses[wifiStatusIndex].length()) : wifiStatuses[wifiStatusIndex], 206, 12, 4);
        if (WiFi.status() == WL_CONNECTED)
            WifiStatus.fillRect(363, 0, 40, 40, WHITE);

        WifiStatus.drawString("Available networks", 0, 68, 4);
        WifiStatus.fillRect(225, 62, 30, 30, WHITE);

        WifiStatus.setTextDatum(TR_DATUM);
        WifiStatus.drawString(">", 416, 68, 4);
        sprintf(buffer, "%d/%d", wifiActivePage + 1, networks.size() / 6 + 1);
        WifiStatus.drawCentreString(String(buffer), 340, 68, 4);
        WifiStatus.drawString("<", 294, 68, 4);
        WifiStatus.pushSprite(32, 64);
    }
    delay(8);
    xSemaphoreGive(tftMutex);
}

void refreshAvailableNetworks()
{
    Serial.println("refresh");

    if (!networks.empty())
    {
        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {
            WifiStatus.fillRect(225, 62, 30, 30, hexToColor("5D5E5F"));
            WifiStatus.pushSprite(32, 64);
        }
        delay(8);
        xSemaphoreGive(tftMutex);
    }

    networks.clear();
    wifiActivePage = 0;
    int temp = WiFi.scanNetworks();
    for (int i = 0; i < temp; i++)
    {
        networks.push_back(WiFi.SSID(i));
    }

    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        WifiBackground.fillSprite(BLACK);
        WifiBackground.pushSprite(0, 40);
    }
    delay(8);
    xSemaphoreGive(tftMutex);

    drawWifiStatus();
    drawAvailableNetworks();
}

void wifiChangePage()
{
    detectTouchSuspendCounter = 4;
    delay(16);

    if (degToDirection(touchCurrentAction[5]) == 2)
        wifiActivePage = wifiActivePage == 0 ? networks.size() / 6 : wifiActivePage - 1;
    else if (degToDirection(touchCurrentAction[5]) == 4)
        wifiActivePage = wifiActivePage == networks.size() / 6 ? 0 : wifiActivePage + 1;
    else
        return;

    xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        WifiBackground.fillSprite(BLACK);
        WifiBackground.pushSprite(0, 40);
    }
    delay(8);
    xSemaphoreGive(tftMutex);

    drawWifiStatus();
    drawAvailableNetworks();
}

bool wifiKeyboardActive = false;

void wifiHandleButtonPress()
{
    coordinates coords1 = {196, 182};
    coordinates coords2 = {226, 212};
    for (int i = wifiActivePage * 6; i < wifiActivePage * 6 + 6 && i < networks.size(); i++)
    {
        if (coords1.x < touchCurrentAction[1] && touchCurrentAction[1] < coords2.x &&
            coords1.y < touchCurrentAction[2] && touchCurrentAction[2] < coords2.y)
        {
            networkSSID = networks[i];

            xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
            {
                AvailableNetwork.fillSprite(BLACK);
                AvailableNetwork.drawString(networks[i].length() > 11 ? networks[i].substring(0, 7) + "..." + networks[i].substring(networks[i].length() - 3, networks[i].length()) : networks[i], 0, 4, 4);
                AvailableNetwork.fillRect(180, 0, 30, 30, hexToColor("5D5E5F"));
                AvailableNetwork.pushSprite(16 + (238 * ((i / 3) % 2)), 182 + (48 * (i % 3)));
            }
            delay(16);
            xSemaphoreGive(tftMutex);

            delay(200);
            wifiKeyboardActive = true;
            xTaskCreate(keyboardPopUp, "keyboardPopUp", 20048, NULL, 3, &handlePopup_t);
            return;
        }
        else
        {
            if (i % 6 != 2)
            {
                coords1.y += 48;
                coords2.y += 48;
            }
            else
            {
                coords1 = {434, 182};
                coords2 = {464, 212};
            }
        }
    }
}

void wifiDisconnectButtonPress()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        wifiStatusIndex = 0;
        networkPassword = "";
        networkSSID = "";
        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {
            WifiStatus.fillRect(363, 0, 40, 40, hexToColor("5D5E5F"));
            WifiStatus.pushSprite(32, 64);
        }
        delay(8);
        xSemaphoreGive(tftMutex);

        WiFi.disconnect();

        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {
            WifiBackground.fillSprite(BLACK);
            WifiBackground.pushSprite(0, 40);
        }
        delay(8);
        xSemaphoreGive(tftMutex);

        drawWifiStatus();
        drawAvailableNetworks();
    }
}

ScreenObject WifiScreen({{0, 0, 480, 360}}, {{0, 0, 480, 160}, {0, 0, 480, 360}}, {{257, 126, 287, 156}, {16, 182, 464, 308}, {395, 64, 435, 104}},
                        {displaySleep}, {WifiGoToMenu, wifiChangePage}, {refreshAvailableNetworks, wifiHandleButtonPress, wifiDisconnectButtonPress});

void wifiScreenUpdate(void *params)
{
    for (;;)
    {

        if (wifiKeyboardActive && ulTaskNotifyTake(pdTRUE, portMAX_DELAY) != 0)
        {
            wifiKeyboardActive = false;
            networkPassword = keyboardInput;
            wifiStatusIndex = 1;
            xTaskCreate(connectToNetwork, "connectToNetwork", 10024, NULL, 1, &connectToNetwork_t);
        }
        else if (wifiStatusIndex == 1 && ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(10000)) != 0)
        {
            wifiStatusIndex = 2;
            wifiStatuses[wifiStatusIndex] = WiFi.SSID();
        }
        else
        {
            if (wifiStatusIndex == 1)
            {
                networkPassword = "";
                networkSSID = "";
                wifiStatusIndex = 0;

                xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
                {
                    WifiBackground.fillSprite(BLACK);
                    WifiBackground.pushSprite(0, 40);
                }
                delay(8);
                xSemaphoreGive(tftMutex);

                drawWifiStatus();
                drawAvailableNetworks();
            }
            else if (wifiStatusIndex == 2 && WiFi.status() != WL_CONNECTED)
            {
                wifiStatusIndex = 0;
                xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
                {
                    WifiBackground.fillSprite(BLACK);
                    WifiBackground.pushSprite(0, 40);
                }
                delay(8);
                xSemaphoreGive(tftMutex);

                drawWifiStatus();
                drawAvailableNetworks();
            }

            vTaskDelay(250);
            continue;
        }
        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {
            WifiBackground.fillSprite(BLACK);
            WifiBackground.pushSprite(0, 40);
        }
        delay(8);
        xSemaphoreGive(tftMutex);

        drawWifiStatus();
        drawAvailableNetworks();
    }
}
void wifiScreen()
{
    detectTouchSuspendCounter = 4;
    ActiveScreenElement = &WifiScreen;

    changeToIpMode = true;
    drawWiFiStatus();

    if (wifiActivePage == 0)

        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
    {
        WifiBackground.createSprite(480, 320);
        WifiBackground.fillSprite(BLACK);
        WifiBackground.pushSprite(0, 40);

        WifiStatus.createSprite(416, 96);
        AvailableNetwork.createSprite(210, 30);

        WifiStatus.setTextColor(WHITE);
        AvailableNetwork.setTextColor(WHITE);
    }
    delay(8);
    xSemaphoreGive(tftMutex);

    if (networks.size() == 0)
        refreshAvailableNetworks();
    else
    {
        xSemaphoreTake(tftMutex, pdMS_TO_TICKS(30000));
        {
            WifiBackground.fillSprite(BLACK);
            WifiBackground.pushSprite(0, 40);
        }
        delay(8);
        xSemaphoreGive(tftMutex);

        drawWifiStatus();
        drawAvailableNetworks();
    }

    xTaskCreate(wifiScreenUpdate, "wifiScreenUpdate", 4096, NULL, 2, &updateScreenElement_t);
}
