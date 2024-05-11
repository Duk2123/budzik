#include <globals.h>
#include <climateSensor.h>

TwoWire I2C_BME280 = TwoWire(1);

Adafruit_BME280 bme;
bool bmeStatus = false;

bool setupClimateSensor()
{
    I2C_BME280.begin(BME280_SDA, BME280_SCL);
    bmeStatus = bme.begin(0x76, &I2C_BME280);

    if (!bmeStatus)
    {
        Serial.println("Could not find a valid BME280 sensor"); // TODO error handling
        return false;
    }

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