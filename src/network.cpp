#include <network.h>
// TODO nie działa w hederze naprawić

int offset = 2; // TODO zrobić automatyczną zmiane stref czasowej

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.pl.pool.ntp.org", 3600 * offset, 28800000);

const char *ssid = "Netianet"; // TODO
const char *password = "Na765432";

void WiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    timeClient.end();
    Serial.println("Disconnected");
    xTaskCreate(connectToNetwork, "connectToNetwork", 20048, NULL, 1, &connectToNetwork_t); // TODO obciąć pamięć
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
    }
}

void WiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    delay(1000);
    timeClient.begin();
    timeClient.update();
    Serial.println("Connected");
}

TaskHandle_t connectToNetwork_t;
void connectToNetwork(void *params)
{
    WiFi.begin(ssid, password);

    int i = 0;

    while (WiFi.status() != WL_CONNECTED && i < 60)
    {
        vTaskDelay((1000 + pow((i++ - 1), 3)) < 60000 ? (1000 + pow((i++ - 1), 3)) : 60000);
    }

    if (i == 60)
        Serial.println("Failed to connect");

    vTaskDelete(NULL);
}
