#include "ESPectroBase.h"
#include "Wire.h"

#define P0 1014.0

ESPectroBase base;

void setup()
{
  Serial.begin(115200);

  //Explicitely call beginTemperaturePressureSensor in order to activate sensor
  if (base.beginTemperaturePressureSensor()) {
      Serial.println("Temperature sensor OK");
  }
  else {
      Serial.println("Temperature sensor NOT OK");
  }  
}

void loop()
{
  double T, P;
  if (base.readTemperatureAndPressure(T, P) > 0) {

    double A = base.estimateAltitude(P, 1013.25);
    Serial.print("T = \t");Serial.print(T,2); Serial.print(" degC\t");
    Serial.print("P = \t");Serial.print(P,2); Serial.print(" mBar\t");
    Serial.print("A = \t");Serial.print(A,2); Serial.println(" m");

    delay(1000);
  }
}
