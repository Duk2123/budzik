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

TaskHandle_t debug_t;
void debug(void *params) // TODO usunąć debug / dodać funkcje monitorującą
{
  char buffer[1024];
  for (;;)
  {
    Serial.println();
    Serial.println("heap size|detect touch|handle touch|update display|update element|handle popup|connect wifi|status bar|sync rtc");
    sprintf(buffer, "%9d|%12d|%12d|%14d|%14d|%12d|%12d|%10d|%8d",
            esp_get_free_heap_size(), detectTouch_t != NULL ? uxTaskGetStackHighWaterMark(detectTouch_t) : -1, handleTouch_t != NULL ? uxTaskGetStackHighWaterMark(handleTouch_t) : -1,
            updateDisplay_t != NULL ? uxTaskGetStackHighWaterMark(updateDisplay_t) : -1, updateScreenElement_t != NULL ? uxTaskGetStackHighWaterMark(updateScreenElement_t) : -1,
            -2, -2,
            statusBar_t != NULL ? uxTaskGetStackHighWaterMark(statusBar_t) : -1, autoSyncRtc_t != NULL ? uxTaskGetStackHighWaterMark(autoSyncRtc_t) : -1);
    Serial.println(buffer);
    sprintf(buffer, "status   |%12d|%12d|%14d|%14d|%12d|%12d|%10d|%8d", detectTouch_t != NULL ? eTaskGetState(detectTouch_t) : -1, handleTouch_t != NULL ? eTaskGetState(handleTouch_t) : -1,
            updateDisplay_t != NULL ? eTaskGetState(updateDisplay_t) : -1, updateScreenElement_t != NULL ? eTaskGetState(updateScreenElement_t) : -1, handlePopup_t != NULL ? eTaskGetState(handlePopup_t) : -1,
            connectToNetwork_t != NULL ? eTaskGetState(connectToNetwork_t) : -1, statusBar_t != NULL ? eTaskGetState(statusBar_t) : -1, autoSyncRtc_t != NULL ? eTaskGetState(autoSyncRtc_t) : -1);
    Serial.println(buffer);
    Serial.println();
    vTaskDelay(10000);
  }
}

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
  audio.connecttoFS(SD, "/test.mp3");
  audio.setFileLoop(true);
  for (;;)
  {
    audio.loop();
  }
}

File myFile;

void saveVectorToFile(const char *filename, std::vector<UserAlarm> &data)
{
  myFile = SD.open(filename, FILE_WRITE);
  if (myFile)
  {
    for (auto &obj : data)
    {
      obj.serialize(myFile);
    }
    myFile.close();
    Serial.println("vector saved");
  }
  else
  {
    Serial.println("save error");
  }
}

void loadVectorFromFile(const char *filename, std::vector<UserAlarm> &data)
{
  myFile = SD.open(filename, FILE_READ);
  if (myFile)
  {
    data.clear();
    while (myFile.available())
    {
      UserAlarm obj;
      obj.deserialize(myFile);
      data.push_back(obj);
    }
    myFile.close();
    Serial.println("vector loaded");
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

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC)
  {
    Serial.println("MMC");
  }
  else if (cardType == CARD_SD)
  {
    Serial.println("SDSC");
  }
  else if (cardType == CARD_SDHC)
  {
    Serial.println("SDHC");
  }
  else
  {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  Serial.println(SD.exists("/test.mp3"));

  audio.setPinout(41, 40, 42);
  audio.setVolume(12); // 0...21

  if (SD.exists("/alarms"))
    loadVectorFromFile("/alarms", alarms);

  delay(2500);
  setBrightness(100);
  Serial.println("Running...");
}

void loop()
{
}
