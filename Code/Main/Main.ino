#include "ThingSpeak.h"
#include "Config.h"
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 sensorObj; // Creación del objeto que representa al sensor.

long lastBeat = 0;      // Tiempo del último latido
float beatsPerMinute;   // Variable que guardará el valor del ritmo cardíaco

char ssid[] = MY_SSID; // Nombre de la red WiFi definida en Config.h
char pass[] = MY_PASS; // Contraseña de la red WiFi definida en Config.h
int keyIndex = 0;   // Número del índice de claves de su red (Solo necesaria 
                    // para Redes WEP)
WiFiClient  client; // Creación del cliente TCP

unsigned long channelID = CH_ID;            // ID del canal definido en Config.h
const char * writeAPIKey = WRITE_API_KEY;   // Clave de la API de escritura definido en Config.h

int periodo = 15000;    // Cada 15 segundos se envía el valor del ritmo cardíaco a Thinkspeak. Este valor 
                        // se puede reducir hasta 1 segundo si se tiene un plan diferente al gratuito en 
                        // la cuenta de Thinkspeak.
unsigned long TiempoActual = 0; // Valor para almacenar la última vez que se envió un dato a Thinkspeak.

void setup() {
    Serial.begin(115200);   // Iniciar comunicación serial
    Serial.println("Inicializando..."); 
    // Inicializar el sensor
    if (!sensorObj.begin(Wire, I2C_SPEED_FAST)) //Usa el puerto I2C por defecto, 400kHz de velocidad
    {
      Serial.println("MAX30102 no encontrado. Por favor revise la conexión y la alimentación del sensor");
      while (1);
    }
    Serial.println("Coloque su dedo índice sobre el sensor con presión constante.");

    sensorObj.setup(); // Configuración del sensor con las configuraciones por defecto
    sensorObj.setPulseAmplitudeGreen(0); // Se apaga el LED verde, ya que el MAX30102 no dispone de dicho LED.
    
    WiFi.mode(WIFI_STA);    // Definir la ESP8266 como estación, es decir, será 
                            // un dispositivo que se conecta a un punto de acceso 
                            // (AP, Access Point)
    ThingSpeak.begin(client);   // Iniciar la conexión IP con Thinkspeak. Si tiene 
                                // un puerto personalizado para usar Thinkspeak debe
                                // usar ThingSpeak.begin(client, port);
                                // El valor de port debe ser numérico.
}

void loop() {
    // Conectarse o reconectarse a la red WiFi
    // WL_CONNECTED indica que se conectó correctamente a la red indicada en Config.h
    if(WiFi.status() != WL_CONNECTED){     
        Serial.print("Intentando conectarse a la red: ");
        Serial.println(MY_SSID);
        while(WiFi.status() != WL_CONNECTED){
            WiFi.begin(ssid, pass); // Conectarse a la red WPA/WPA2. Cambiar esta línea 
                                    // si se usa una red abierta o WEP.
            Serial.print("Reconectándose a la red: ");
            Serial.println(MY_SSID);
            delay(5000);     
        } 
        Serial.println("\nConectado.");
    }

    //Obtener la lectura de datos
    long irValue = sensorObj.getIR();

    if (checkForBeat(irValue) == true) //Si se detecta el dedo y el pulso.
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
  
    /* 
    Enviar y escribir el ritmo cardiaco por minuto, en el canal 1 de ThingSpeak.
    */ 
    if (millis() > TiempoActual+periodo && checkForBeat(irValue) == true){
        TiempoActual = millis();
        int x = ThingSpeak.writeField(channelID, 1, beatsPerMinute, writeAPIKey);  // La variable x se 
        // encargará de recibir la respuesta HTTP al enviar la solicitud de escritura.

        if(x == 200){ // El código 200 indica un OK o éxito en la escritura.
            Serial.println("Canal actualizado correctamente.");
        }
        else{
            Serial.println("Problema actualizando el canal. Código de error HTTP " + String(x));
        }
    }
}
