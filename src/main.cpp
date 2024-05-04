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
  setBrightness(125);

  uint16_t calData[5] = {225, 3765, 200, 3765, 7};
  tft.setTouch(calData);

  tft.fillScreen(0);

  xTaskCreate(connectToNetwork, "connectToNetwork", 20048, NULL, 1, &connectToNetwork_t); // TODO obciąć pamięć
  xTaskCreate(handleTouch, "handleTouch", 20048, NULL, 3, &handleTouch_t);
  delay(250);
  xTaskCreate(detectTouch, "detectTouch", 20048, NULL, 3, &detectTouch_t);
  delay(250);
  xTaskCreate(updateDisplay, "detectTouch", 20048, NULL, 2, &updateDisplay_t); // TODO obciąć pamięć

  Serial.println("Running...");
}

void loop()
{ // TODO zrobić taska monitorującego inne taski, do resetowania jak się wysypią
  Serial.println();
  if (connectToNetwork_t != NULL)
  {
    Serial.println("connectToNetwork_t");
    Serial.println(eTaskGetState(connectToNetwork_t));
  }

  if (handleTouch_t != NULL)
  {
    Serial.println("handleTouch_t");
    Serial.println(eTaskGetState(handleTouch_t));
  }

  if (detectTouch_t != NULL)
  {
    Serial.println("detectTouch_t");
    Serial.println(eTaskGetState(detectTouch_t));
    if (eTaskGetState(detectTouch_t) == 3)
    {
      vTaskDelete(detectTouch_t);
      xTaskCreate(detectTouch, "detectTouch", 20048, NULL, 3, &detectTouch_t);
    }
  }

  if (updateDisplay_t != NULL)
  {
    Serial.println("updateDisplay_t");
    Serial.println(eTaskGetState(updateDisplay_t));
  }

  if (updateScreenElement_t != NULL)
  {
    Serial.println("updateScreenElement_t");
    Serial.println(eTaskGetState(updateScreenElement_t));
  }

  delay(1000);
}
