#include <main.h>
#include <screens/screens.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite clockSprite = TFT_eSprite(&tft);

/*
 touchQueuedAction[0] - type of action: //TODO (potencjalnie zastąpić event group albo task notify z rtos)
    0 - long press
    1 - swipe
    2 - press
 touchQueuedAction[1] - x coordinates or deviation in case of swipe
 touchQueuedAction[2] - y coordinates or deviation in case of swipe
*/
int touchQueuedAction[3];
int prevBrightness;
int brightness;
void setBrightness(int value)
{
  prevBrightness = brightness;
  brightness = value;
  analogWrite(TFT_BL, 2.55 * value);
}

TaskHandle_t updateDisplay_t;
TaskHandle_t handlePopUp_t;

SemaphoreHandle_t tftMutex = xSemaphoreCreateMutex();

void handleTouch(int *touchQueuedAction)
{
  xSemaphoreTake(tftMutex, portMAX_DELAY);
  {
    activeScreenElement->processTouch(touchQueuedAction);
  }
  xSemaphoreGive(tftMutex);
}

TaskHandle_t detectTouch_t;
// Task for touch detection, upon detecting touch calls handleTouch function with touch type and coordinates or deviation as arguments.
void detectTouch(void *params)
{

  u_int16_t x, y, vx, vy, x2, y2, x3, y3, x4, y4;
  std::pair<int, int> touchBufor[43];
  std::fill(std::begin(touchBufor), std::end(touchBufor), std::pair<int, int>(0, 0));
  int i = 0;      // touchBufor index
  int devX, devY; // axis deviation for swipe
  for (;;)
  {
    // Serial.println("x");
    //  Check if screen is being touched and no long press detected
    if ((analogRead(8) < 1000) && i != 43)
    {

      // Touch threshold
      xSemaphoreTake(tftMutex, portMAX_DELAY);
      {
        if (tft.getTouchRawZ() > 1)
        {
          // Check for raw data errors
          tft.getTouchRaw(&x, &y);
          tft.getTouchRaw(&x2, &y2);
          tft.getTouchRaw(&x3, &y3);
          tft.getTouchRaw(&x4, &y4);
          vx = x - x2 + x3 - x4;
          vy = y - y2 + y3 - y4;
          if (abs(vx) < 250 && abs(vy) < 250)
          {
            // Normalize raw data by adding deviation
            x += vx / 2;
            y += vy / 2;
            tft.convertRawXY(&x, &y);
            // Throw warning for conversion error //TODO add proper warn/error handling
            if (x < 0 || y < 0 || x > 480 || y > 360)
            {
              Serial.println("Coordinates conversion error");
            }
            else
            {
              touchBufor[i] = {x, y};
              i++;
            }
          }
        }
      }
      xSemaphoreGive(tftMutex);

      vTaskDelay(7);
    }
    // If screen is no longer touched or touched for max amount of time (touchBufor[size] * vTaskDelay(ms))
    else if (i > 0)
    {
      // Detect long press
      if (i >= 32)
      {
        touchQueuedAction[0] = 0;
        touchQueuedAction[1] = touchBufor[i - 1].first;
        touchQueuedAction[2] = touchBufor[i - 1].second;
      }
      else
      {
        // Detect swipe
        devX = touchBufor[0].first - touchBufor[i - 1].first;
        devY = touchBufor[0].second - touchBufor[i - 1].second;
        if (abs(devX) > 60 || abs(devY) > 30 || abs(devX) + abs(devY) > 67)
        {
          touchQueuedAction[0] = 1;
          touchQueuedAction[1] = devX;
          touchQueuedAction[2] = devY;
        }

        // Detect press
        else
        {
          touchQueuedAction[0] = 2;
          touchQueuedAction[1] = touchBufor[i - 1].first;
          touchQueuedAction[2] = touchBufor[i - 1].second;
        }
      }
      // TODO Wywołać przerwanie i podać argumenty do taska
      handleTouch(touchQueuedAction);

      // Clear touchBufor and reset its index
      std::fill(std::begin(touchBufor), std::end(touchBufor), std::pair<int, int>(0, 0));
      i = 0;
      vTaskDelay(500);
    }
    // Screen was not touched
    else
    {
      vTaskDelay(150);
    }
  }
}

void setup(void)
{
  // TODO usunąć delay i printy
  delay(2500); // do debugowania
  Serial.begin(115200);
  Serial.println("\n\nStarting...");

  tft.init();
  tft.setRotation(1);
  setBrightness(100);

  uint16_t calData[5] = {225, 3765, 200, 3765, 7};
  tft.setTouch(calData);

  tft.fillScreen(0);

  xTaskCreate(detectTouch, "touch", 10024, NULL, 1, &detectTouch_t);
  menuScreen();

  Serial.println("Running...");
}

void loop()
{
}
