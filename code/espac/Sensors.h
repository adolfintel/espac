#include<Wire.h>
#include<SPI.h>
#include<Adafruit_Sensor.h>
#include<Adafruit_BME280.h>
#include<DS3231.h>

Adafruit_BME280 bme; //Temp sensor
DS3231 rtc; //RTC

float temperature, humidity, pressure, apparentTemperature;
uint16_t year; uint8_t month, day, hour, minute, dayOfWeek;

void initSensors(){
  bme.begin(0x76);
  bme.setSampling(Adafruit_BME280::MODE_FORCED,Adafruit_BME280::SAMPLING_X1,Adafruit_BME280::SAMPLING_X1,Adafruit_BME280::SAMPLING_X1,Adafruit_BME280::FILTER_OFF);
  Wire.begin();
}
void updateSensors(){
  bme.takeForcedMeasurement();
  temperature=bme.readTemperature()-1.0F;
  pressure=bme.readPressure()/100.0F;
  humidity=bme.readHumidity();
  apparentTemperature=temperature+0.348F*((humidity/100.0F)*6.105F*pow(2.71828F,((17.27F*temperature)/(237.7F+temperature))))/*-0.7F*windSpeed*/-4.25F; //formula adapted from https://planetcalc.com/2089/
  bool b;
  year=rtc.getYear()+2000;
  month=rtc.getMonth(b);
  day=rtc.getDate();
  hour=rtc.getHour(b,b);
  minute=rtc.getMinute();
  dayOfWeek=rtc.getDoW();
}
