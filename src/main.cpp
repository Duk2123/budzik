#include <main.h>
#include <touch.h>
#include <screens/screens.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite clockSprite = TFT_eSprite(&tft);

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
TaskHandle_t handlePopUp_t; // TODO

SemaphoreHandle_t tftMutex = xSemaphoreCreateMutex();

void setup(void)
{
  // TODO usunąć delay i printy
  delay(2500); // do debugowania
  Serial.begin(115200);
  Serial.println("\n\nStarting...");

  tft.init();
  tft.setRotation(1);
  setBrightness(125);

  uint16_t calData[5] = {225, 3765, 200, 3765, 7};
  tft.setTouch(calData);

  tft.fillScreen(0);

  xTaskCreate(handleTouch, "handleTouch", 2048, NULL, 2, &handleTouch_t);
  delay(250);
  xTaskCreate(detectTouch, "detectTouch", 2048, NULL, 2, &detectTouch_t);
  delay(250);
  xTaskCreate(updateDisplay, "detectTouch", 20048, NULL, 1, &updateDisplay_t);

  Serial.println("Running...");
}

void loop()
{
  // if (touchCurrentAction[0] != -1)
  // {
  //   if (touchCurrentAction[0] == 1)
  //   {
  //     Serial.printf("x: %d y: %d devx: %d devy: %d deg: %d\n", touchCurrentAction[1], touchCurrentAction[2], touchCurrentAction[3], touchCurrentAction[4], touchCurrentAction[5]);
  //   }
  //   else
  //   {

  //     Serial.printf("type: %d x: %d y: %d\n", touchCurrentAction[0], touchCurrentAction[1], touchCurrentAction[2]);
  //   }
  //   touchCurrentAction[0] = -1;
  // }
  // else
  // {
  //   delay(150);
  // }
}
