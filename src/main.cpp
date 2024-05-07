#include <main.h>
#include <touch.h>
#include <screens/screens.h>
#include <time.h>
#include <network.h>

TFT_eSPI tft = TFT_eSPI();

int prevBrightness;
int brightness;
void setBrightness(int value)
{
  prevBrightness = brightness;
  brightness = value;
  analogWrite(TFT_BL, 2.55 * value);
}

int degToDirection(int degrees)
{
  if (abs(degrees) <= 45)
    return 1;
  else if (degrees > 45 && degrees < 135)
    return 2;
  else if (abs(degrees) >= 135)
    return 3;
  else if (degrees < -45 && degrees > -135)
    return 4;

  else
    return 0; // TODO dodać obsługe błędów
}

TaskHandle_t updateScreenElement_t;
TaskHandle_t handlePopUp_t; // TODO ogarnąć mechanike pop upów

SemaphoreHandle_t tftMutex = xSemaphoreCreateMutex();

void setup(void)
{
  // TODO usunąć delay i printy
  delay(2500); // do debugowania
  Serial.begin(115200);
  Serial.println("\n\nStarting...");
  setupRtc();

  Serial.println(getRtcTime());
  Serial.println(getRtcDate());

  tft.init();
  tft.setRotation(1);
  setBrightness(100);

  uint16_t calData[5] = {225, 3765, 200, 3765, 7};
  tft.setTouch(calData);

  tft.fillScreen(0);

  WiFi.onEvent(WiFiDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.onEvent(WiFiConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);

  xTaskCreate(connectToNetwork, "connectToNetwork", 20048, NULL, 1, &connectToNetwork_t); // TODO obciąć pamięć
  xTaskCreate(handleTouch, "handleTouch", 20048, NULL, 3, &handleTouch_t);
  delay(250);
  xTaskCreate(detectTouch, "detectTouch", 20048, NULL, 3, &detectTouch_t);
  delay(250);
  menuScreen();
  xTaskCreate(updateDisplay, "updateDisplay", 20048, NULL, 2, &updateDisplay_t); // TODO obciąć pamięć

  Serial.println("Running...");
}

void loop()
{
}
