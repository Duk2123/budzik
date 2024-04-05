#include <SPI.h>
#include <TFT_eSPI.h>
#include <scenes/menuTest.cpp>
TFT_eSPI tft = TFT_eSPI();

String action = "";

TaskHandle_t touch_handle;
void touch(void *params)
{
  Serial.println("task created");
  uint16_t x, y, z, x2, y2;
  int status;

  for (;;)
  {
    status = analogRead(8);
    // Check if screen is being touched
    if (status < 100)
    {
      z = tft.getTouchRawZ();
      // Touch threshold
      if (z > 350)
      {
        tft.getTouchRaw(&x, &y);
        vTaskDelay(5);
        tft.getTouchRaw(&x2, &y2);
        // Check for raw data errors
        if (abs(x2 - x) < 100 && abs(y2 - y) < 100)
        {
          tft.convertRawXY(&x, &y);
          Serial.printf("\nx: %d, y: %d, z: %d | mem: %d", x, y, z, uxTaskGetStackHighWaterMark(NULL));
        }
      }
      vTaskDelay(10);
    }
    else
    {
      vTaskDelay(25);
    }
  }
}

TaskHandle_t displayRefresh_handle;
void displayRefresh(void *params)
{
  return;
}

void setup(void)
{
  delay(2500);
  Serial.begin(115200);
  Serial.println("\n\nStarting...");

  tft.init();
  tft.setRotation(1);
  analogWrite(TFT_BL, 100 * 2.55);

  uint16_t calData[5] = {225, 3765, 200, 3765, 7};
  tft.setTouch(calData);
  // https://github.com/PaulStoffregen/XPT2046_Touchscreen
  // jak będą problemy z wykrywaniem dotyku

  tft.fillScreen(0);

  tft.pushImage(0, 0, 480, 320, test);

  xTaskCreate(touch, "touch", 2048, NULL, 1, &touch_handle);

  Serial.println("Running...");
}

void loop()
{
}
// rgb
// brg