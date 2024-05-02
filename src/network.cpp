#include <network.h>
// TODO nie działa w hederze naprawić

int offset = 2; // TODO zrobić automatyczną zmiane stref czasowej

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.pl.pool.ntp.org", 3600 * offset, 28800000);

const char *ssid = "Netianet"; // TODO
const char *password = "Na765432";

TaskHandle_t connectToNetwork_t;
void connectToNetwork(void *params)
{
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(1000);
    }

    Serial.println("Connected");
    timeClient.begin();
    timeClient.update();
    vTaskDelete(NULL);
}