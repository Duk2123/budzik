#include <main.h>
#include <touch.h>
#include <screens/screens.h>
#include <time.h>
#include <network.h>
#include <climateSensor.h>

TFT_eSPI tft = TFT_eSPI();

/// @brief converts text special letters to ?
/// @param text String to convert
/// @return String
String convertSpecialLetters(String text) // TODO usunąć po dodaniu czcionek
{
  String result;
  for (int i = 0; i < text.length(); i++)
  {
    if (int(text[i]) > 126)
    {
      result += '?';
    }
    else
    {
      result += text[i];
    }
  }
  return result;
}

int prevBrightness;
int brightness;
/** @brief Sets display brightens
 * @param value brightness % [0,100]
 */
void setBrightness(int value)
{
  prevBrightness = brightness;
  brightness = value;
  analogWrite(TFT_BL, 2.55 * value);
}

/** @brief Converts degrees from atan2 to direction
 * @param degrees int representing degrees [-180,180]
 * @return int representing directions:  1 - up, 2 - right, 3 - down, 4 - left
 */
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

/** @brief Converts a hex to color
 * @param hex a String with hex color
 * @return uint16_t with r5g6b5 formatted color
 */
uint16_t hexToColor(String hex)
{
  uint8_t r = strtol(hex.substring(0, 2).c_str(), NULL, 16);
  uint8_t g = strtol(hex.substring(2, 4).c_str(), NULL, 16);
  uint8_t b = strtol(hex.substring(4, 6).c_str(), NULL, 16);

  uint16_t b5 = b >> 3;
  uint16_t g6 = g >> 2;
  uint16_t r5 = r >> 3;

  return (r5 << 11) | (g6 << 5) | b5;
}

TaskHandle_t updateScreenElement_t;
TaskHandle_t handlePopup_t;

SemaphoreHandle_t tftMutex = xSemaphoreCreateMutex();

TaskHandle_t debug_t;
void debug(void *params) // TODO usunąć debug / dodać funkcje monitorującą
{
  for (;;)
  {
    Serial.println(esp_get_free_heap_size());
    vTaskDelay(10000);
  }
}

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
  setBrightness(0);

  uint16_t calData[5] = {225, 3765, 200, 3765, 7};
  tft.setTouch(calData);

  tft.fillScreen(0);

  WiFi.onEvent(WiFiDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.onEvent(WiFiConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);

  setupClimateSensor();

  xTaskCreate(debug, "debug", 2048, NULL, 10, &debug_t);                                  // TODO obciąć pamięć
  xTaskCreate(connectToNetwork, "connectToNetwork", 20048, NULL, 1, &connectToNetwork_t); // TODO obciąć pamięć
  xTaskCreate(handleTouch, "handleTouch", 20048, NULL, 4, &handleTouch_t);                //
  delay(250);                                                                             // TODO poprawić delay
  xTaskCreate(detectTouch, "detectTouch", 20048, NULL, 5, &detectTouch_t);                //
  xTaskCreate(updateDisplay, "updateDisplay", 20048, NULL, 3, &updateDisplay_t);          //
  xTaskCreate(statusBar, "statusBar", 20048, NULL, 2, &statusBar_t);                      //
  clockScreen();

  delay(1000);
  setBrightness(100);
  Serial.println("Running...");
}

void loop()
{
}
