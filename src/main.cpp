#include <SPI.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

TaskHandle_t touch_handle;
void touch(void *params)
{
  Serial.println("task created");
  uint16_t x, y, z;
  int status;

  for (;;)
  {
    status = analogRead(8);
    vTaskDelay(25);
    if (status == 0)
    {

      z = tft.getTouchRawZ();
      if (z > 600)
      {
        tft.getTouch(&x, &y);
        Serial.printf("\nx: %d, y: %d, z: %d", x, y, z);
      }
    }
  }
}

void setup(void)
{
  delay(5000);
  Serial.begin(115200);
  Serial.println("\n\nStarting...");

  tft.init();
  tft.setRotation(1);

  uint16_t calData[5] = {279, 3617, 186, 3668, 7};
  tft.setTouch(calData);
  // https://github.com/PaulStoffregen/XPT2046_Touchscreen
  // jak będą problemy z wykrywaniem dotyku

  xTaskCreate(touch, "touch", 10024, NULL, 1, &touch_handle);

  Serial.println("Running...");
}

void loop()
{
}
