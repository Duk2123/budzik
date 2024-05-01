#include <main.h>
#include <screens/screens.h>

typedef struct
{
  int x;
  int y;
} coordinates;

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite clockSprite = TFT_eSprite(&tft);

/*
 touchQueuedAction[0] - type of action: //TODO (potencjalnie zastąpić event group albo task notify z rtos)
    0 - long press
    1 - swipe
    2 - press
 touchQueuedAction[1] - x coordinates
 touchQueuedAction[2] - y coordinates
 touchQueuedAction[3]* - x deviation
 touchQueuedAction[4]* - y deviation
 touchQueuedAction[5]* - angle of swipe [-180,180] 0:up, -90:left, 90:right, -180/180:down
 * only for swipe
*/
int touchCurrentAction[6]; // TODO przerobić na powiadomienia z rtos albo coś innego

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
QueueHandle_t touchQueue = xQueueCreate(43, sizeof(coordinates));
bool isTouchProcessing = false;

TaskHandle_t handleTouch_t;
// Task that reads data send from detectTouch task in queue and processes it setting
void handleTouch(void *params)
{
  coordinates coords;
  int devX, devY, firstX, firstY, i;
  for (;;)
  {
    // Wait for data
    if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) != 0)
    {
      isTouchProcessing = true; // Set flag to stop notifications
      // Processing data from queue
      xQueuePeek(touchQueue, &(coords), pdMS_TO_TICKS(150));
      i = 1;
      firstX = coords.x;
      firstY = coords.y;
      while (xQueueReceive(touchQueue, &(coords), pdMS_TO_TICKS(150)))
      {
        i++;
        devX = firstX - coords.x;
        devY = firstY - coords.y;
        // Detecting swipe
        if (i > 4 && (abs(devX) > 50 || abs(devY) > 30 || abs(devX) + abs(devY) > 58))
        {
          touchCurrentAction[0] = 1;
          touchCurrentAction[1] = firstX;
          touchCurrentAction[2] = firstY;
          touchCurrentAction[3] = -devX;
          touchCurrentAction[4] = devY;
          touchCurrentAction[5] = atan2(-devX, devY) * 180 / 3.14159265;
        }
        // Detecting long press
        else if (i > 31)
        {
          touchCurrentAction[0] = 0;
          touchCurrentAction[1] = coords.x;
          touchCurrentAction[2] = coords.y;
        }
        // Detecting press
        else
        {
          touchCurrentAction[0] = 2;
          touchCurrentAction[1] = coords.x;
          touchCurrentAction[2] = coords.y;
        }
        vTaskDelay(4);
      }
    }
    isTouchProcessing = false; // Set flag to turn on notifications
  }
}

TaskHandle_t detectTouch_t;
// Task for touch detection, upon detecting touch sends notification to handleTouch task to process data
void detectTouch(void *params)
{

  u_int16_t x, y, vx, vy, x2, y2, x3, y3, x4, y4;
  int devX, devY; // axis deviation for swipe
  for (;;)
  {
    // Serial.println("x");
    //  Check if screen is being touched and no long press detected
    if ((analogRead(8) < 1000))
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
              // Send coordinates to queue
              coordinates temp = {x, y};
              if (uxQueueSpacesAvailable(touchQueue) == 0)
              {
                xQueueReceive(touchQueue, &(temp), (TickType_t)0);
              }
              xQueueSendToBack(touchQueue, &(temp), (TickType_t)0);
            }
          }
        }
      }
      xSemaphoreGive(tftMutex);
      // Notify the handleTouch task when it isn't running
      if (isTouchProcessing == false)
      {
        vTaskNotifyGiveFromISR(handleTouch_t, NULL);
      }
      vTaskDelay(8);
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
  setBrightness(125);

  uint16_t calData[5] = {225, 3765, 200, 3765, 7};
  tft.setTouch(calData);

  tft.fillScreen(0);

  xTaskCreate(detectTouch, "detectTouch", 20048, NULL, 1, &detectTouch_t);
  xTaskCreate(handleTouch, "handleTouch", 20048, NULL, 1, &handleTouch_t);
  menuScreen(NULL);

  Serial.println("Running...");
}

void loop()
{
  if (touchCurrentAction[0] != -1)
  {
    if (touchCurrentAction[0] == 1)
    {
      Serial.printf("x: %d y: %d devx: %d devy: %d deg: %d\n", touchCurrentAction[1], touchCurrentAction[2], touchCurrentAction[3], touchCurrentAction[4], touchCurrentAction[5]);
    }
    else
    {

      Serial.printf("type: %d x: %d y: %d\n", touchCurrentAction[0], touchCurrentAction[1], touchCurrentAction[2]);
    }
    touchCurrentAction[0] = -1;
  }
  else
  {
    delay(150);
  }
}
