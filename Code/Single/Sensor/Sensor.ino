/*
    El fin de este código, es obtener el ritmo cardíaco para comprobar el buen
    funcionamiento del sensor MAX30102. Se hace uso de la librería de SparkFun 
    que se puede encontrar en el repositorio
    https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library
*/
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 sensorObj; // Creación del objeto que representa al sensor.

long lastBeat = 0;      // Tiempo del último latido
float beatsPerMinute;   // Variable que guardará el valor del ritmo cardíaco

void setup() {
    Serial.begin(115200);   // Iniciar comunicación serial
    Serial.println("Inicializando..."); 
    /* 
        Inicializar el sensor usando los pines I2C por defecto a 400KHz de velocidad. 
        Los puertos I2C para la NodeMCU por defecto son:
        - D1 para SCL
        - D2 para SDA
    */
    if (!sensorObj.begin(Wire, I2C_SPEED_FAST))
    {
      Serial.println("MAX30102 no encontrado. Por favor revise la conexión y la alimentación del sensor");
      while (1);
    }
    Serial.println("Coloque su dedo índice sobre el sensor con presión constante.");

    sensorObj.setup(); // Configuración del sensor con las configuraciones por defecto
    sensorObj.setPulseAmplitudeGreen(0); // Se apaga el LED verde, ya que el MAX30102 no dispone de dicho LED.
}

void loop() {
    
    long irValue = sensorObj.getIR();   // Obtener la lectura del sensor
    /*
        Sí se detecta el dedo y el pulso, se procede a realizar la estimación del ritmo cardíaco a partir del 
        tiempo transcurrido.
    */
    if (checkForBeat(irValue) == true)
    {
        long delta = millis() - lastBeat;
        lastBeat = millis();
        beatsPerMinute = 60000 / delta; 
        Serial.print(beatsPerMinute);
        Serial.print(" Latidos por minuto");
    }       

    if (irValue < 50000){
        Serial.print("No se detecta el dedo.");
    }
    Serial.println();    
}
