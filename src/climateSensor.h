#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME280_SDA 47
#define BME280_SCL 48

extern Adafruit_BME280 bme;
extern bool bmeStatus;

bool setupClimateSensor();