#include <main.h>
#include <touch.h>
#include <screens/screens.h>
#include <network.h>
#include <climateSensor.h>
#include <time.h>

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

std::vector<UserAlarm> alarms = {};

TaskHandle_t alarmInterrupt_t;
void alarmInterrupt(void *params)
{
  String time = "00:00";
  for (;;)
  {
    if (time != getRtcTime().substring(0, 5))
    {
      time = getRtcTime().substring(0, 5);
      for (int i = 0; i < alarms.size(); i++)
      {
        if (alarms[i].isActive() && alarms[i].timeUntilAlarm() == "0 minutes")
        {
          alarms[i].activateAlarm();
          vTaskDelay(1000);
          saveVectorToFile("/bin/alarms", alarms);
        }
      }
    }
    vTaskDelay(20000);
  }
}

Audio audio;

TaskHandle_t alarmAudio_t;
void alarmAudio(void *params)
{
  Serial.println("Audio");
  audio.stopSong();
  audio.connecttoFS(SD, "/audio/default_alarm.mp3");
  audio.setFileLoop(true);
  for (;;)
  {
    audio.loop();
  }
}

void saveVectorToFile(const char *path, std::vector<UserAlarm> &data)
{
  File file;
  file = SD.open(path, FILE_WRITE);
  if (file)
  {
    for (auto &obj : data)
    {
      obj.serialize(file);
    }
    file.close();
  }
  else
  {
    Serial.println("save error");
  }
}

void loadVectorFromFile(const char *path, std::vector<UserAlarm> &data)
{
  File file;
  file = SD.open(path, FILE_READ);
  if (file)
  {
    data.clear();
    while (file.available())
    {
      UserAlarm obj;
      obj.deserialize(file);
      data.push_back(obj);
    }
    file.close();
  }
  else
  {
    Serial.println("load error");
  }
}

SPIClass spi2(HSPI);

void setup(void)
{
  // TODO usunąć delay i printy
  delay(2500); // do debugowania
  Serial.begin(115200);
  Serial.println("\n\nStarting...");

  setupRtc();

  Serial.println(getRtcTime());
  Serial.println(getRtcDate());

  digitalWrite(TOUCH_CS, HIGH);
  digitalWrite(TFT_CS, HIGH);
  digitalWrite(SD_CS, HIGH);

  audio.setPinout(41, 40, 42);
  audio.setVolume(12); // 0...21

  tft.init();
  setBrightness(0);

  spi2.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);

  if (!SD.begin(SD_CS, spi2, 8000000))
  {
    Serial.println("Card Mount Failed");
    return;
  }

  tft.setRotation(1);

  uint16_t calData[5] = {225, 3765, 200, 3765, 7};
  tft.setTouch(calData);

  tft.fillScreen(0);

  WiFi.onEvent(WiFiDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.onEvent(WiFiConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);

  setupClimateSensor();

  // xTaskCreate(debug, "debug", 20048, NULL, 1, &debug_t);                         // TODO obciąć pamięć
  xTaskCreate(handleTouch, "handleTouch", 4096, NULL, 4, &handleTouch_t);       // TODO obciąć pamięć
  delay(250);                                                                   // TODO poprawić delay
  xTaskCreate(detectTouch, "detectTouch", 4096, NULL, 5, &detectTouch_t);       // TODO obciąć pamięć
  xTaskCreate(updateDisplay, "updateDisplay", 4096, NULL, 3, &updateDisplay_t); // TODO obciąć pamięć
  xTaskCreate(statusBar, "statusBar", 4096, NULL, 2, &statusBar_t);             // TODO obciąć pamięć
  xTaskCreate(autoSyncRtc, "autoSyncRtc(", 4096, NULL, 1, &autoSyncRtc_t);      // TODO obciąć pamięć
  xTaskCreate(alarmInterrupt, "alarmInterrupt", 4096, NULL, 6, &alarmInterrupt_t);
  clockScreen();

  if (SD.exists("/bin/alarms"))
    loadVectorFromFile("/bin/alarms", alarms);

  delay(2500);
  setBrightness(100);
  Serial.println("Running...");
}

void loop() { vTaskDelete(NULL); };