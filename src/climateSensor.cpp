#include <globals.h>
#include <climateSensor.h>
#include <time.h>

TwoWire I2C_BME280 = TwoWire(1);

Adafruit_BME280 bme;
bool bmeStatus = false;

TaskHandle_t climateLogStatistics_t;
void climateLogStatistics(void *params)
{
    char buffer[64];
    while (!rtcRunning)
    {
        vTaskDelay(500);
    }
    for (;;)
    {
        if (rtc.now().minute() % 15 == 0)
        {
            File file = SD.open("/logs/climate.csv", FILE_APPEND);
            sprintf(buffer, "%d,%.2f,%.0f,%.0f", rtc.now().unixtime(), bme.readTemperature(), bme.readHumidity(), bme.readPressure() / 100.0F);
            file.println(buffer);
            file.close();
            vTaskDelay(600000);
        }
        vTaskDelay(20000);
    }
}

/** @brief climate sensor setup
 * @return true if successful
 */
bool setupClimateSensor()
{
    I2C_BME280.begin(BME280_SDA, BME280_SCL);
    bmeStatus = bme.begin(0x76, &I2C_BME280);

    if (!bmeStatus)
    {
        Serial.println("Could not find a valid BME280 sensor"); // TODO error handling
        return false;
    }

    bme.setTemperatureCompensation(-0.62);
    bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                    Adafruit_BME280::SAMPLING_X4, // temperature
                    Adafruit_BME280::SAMPLING_X8, // pressure
                    Adafruit_BME280::SAMPLING_X2, // humidity
                    Adafruit_BME280::FILTER_X16,
                    Adafruit_BME280::STANDBY_MS_0_5);

    xTaskCreate(climateLogStatistics, "climateLogStatistics", 4096, NULL, 1, &climateLogStatistics_t);

    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" °C");
    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
    return true;
}
