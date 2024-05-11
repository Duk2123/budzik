#include <main.h>
#include <touch.h>

/*
 touchQueuedAction[0] - type of action: //TODO (potencjalnie zastąpić event group albo task notify z rtos)
   -1 - no action pending
    0 - long press
    1 - swipe
    2 - press
 touchQueuedAction[1] - x coordinates
 touchQueuedAction[2] - y coordinates
 touchQueuedAction[3]* - x deviation
 touchQueuedAction[4]* - y deviation
 touchQueuedAction[5]* - angle of swipe [-180,180] 0:up, -90:left, 90:right, -+180:down
 * only for swipe
*/
int touchCurrentAction[6] = {-1, 0, 0, 0, 0, 0}; // TODO przerobić na powiadomienia z rtos albo coś innego

QueueHandle_t touchQueue = xQueueCreate(43, sizeof(coordinates));
bool isTouchProcessing = false;

TaskHandle_t handleTouch_t;
// Task that reads data send from detectTouch task in queue processes it and updates touchCurrentAction
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
            xQueuePeek(touchQueue, &(coords), pdMS_TO_TICKS(128));
            i = 1;
            firstX = coords.x;
            firstY = coords.y;
            while (xQueueReceive(touchQueue, &(coords), pdMS_TO_TICKS(128)))
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
                    touchCurrentAction[5] = atan2(-devX, devY) * 180 / M_PI;
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
                vTaskDelay(8);
            }
        }
        isTouchProcessing = false; // Set flag to turn on notifications
    }
}

// Suspend for n*50ms
int detectTouchSuspendCounter = 0;

TaskHandle_t detectTouch_t;
// Task for touch detection, upon detecting touch sends notification to handleTouch task to process data
void detectTouch(void *params)
{

    u_int16_t x, y, vx, vy, x2, y2, x3, y3, x4, y4;
    int devX, devY, i; // axis deviation for swipe
    i = 0;
    for (;;)
    {
        // Serial.println("x");
        //  Check if screen is being touched and no long press detected
        if ((analogRead(8) < 1000) && detectTouchSuspendCounter-- == 0)
        {
            i = 0;

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
            // Notify the handleTouch task that there is data available for processing
            if (isTouchProcessing == false)
            {
                vTaskNotifyGiveFromISR(handleTouch_t, NULL);
            }
            vTaskDelay(8);
        }
        // Screen was not touched
        else if (detectTouchSuspendCounter > 0)
        {
            vTaskDelay(50);
        }
        else
        {
            vTaskDelay(16 + pow(i++, 2.71828));
        }
        i = i > 6 ? 6 : i;
        detectTouchSuspendCounter = detectTouchSuspendCounter < 0 ? 0 : detectTouchSuspendCounter;
    }
}