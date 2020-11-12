/*
Este código envía y escribe un valor en uno de los campos o fields del canal 
definido en "Config.h" cada 15 segundos. El fin de este código es asegurarse 
de que hay una correcta comunicación entre la ESP y Thinkspeak para escribir
datos.

CONSIDERACIONES IMPORTANTES
    -Se debe modificar el archivo "Config.h" con los datos de su proyecto.
    -Si tiene una cuenta de Thinkspeak diferente a la gratuita, podrá disminuir 
     el tiempo de escritura hasta 1 segundo.
*/

#include "ThingSpeak.h"
#include "Config.h"
#include <ESP8266WiFi.h>

char ssid[] = MY_SSID;  // Nombre de la red WiFi definida en Config.h
char pass[] = MY_PASS;  // Contraseña de la red WiFi definida en Config.h
int keyIndex = 0;       // Número del índice de claves de su red (Solo necesaria 
                        // para Redes WEP)
WiFiClient  client;     // Creación del cliente TCP

unsigned long channelID = CH_ID;            // ID del canal definido en Config.h
const char * writeAPIKey = WRITE_API_KEY;   // Clave de la API de escritura 
                                            // definido en Config.h

int num = 0; // Valor inicial del contador

void setup() {
    Serial.begin(115200);   // Iniciar comunicación serial
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
  
    /* 
    Enviar y escribir los datos en ThingSpeak. Se pueden tener hasta 8 campos en un
    canal, que te permitirán almacenar hasta 8 piezas de información diferente en 
    un canal. En este ejemplo se escribirá la información solo en el canal 1.
    */ 
    int x = ThingSpeak.writeField(channelID, 1, num, writeAPIKey);  // La variable x se 
                                                                    // encargará de recibir
                                                                    // la respuesta HTTP al 
                                                                    // enviar la solicitud 
                                                                    // de escritura.
    if(x == 200){ // El código 200 indica un OK o éxito en la escritura.
        Serial.println("Canal actualizado correctamente.");
    }
    else{
        Serial.println("Problema actualizando el canal. Código de error HTTP " + String(x));
    }
    /*
    Los posibles códigos de error son:
        > 404: Clave incorrecta de la API (o dirección del servidor Thinkspeak incorrecto)
        > -101: El valor está fuera del rango o el string es muy largo (Mayor a 255 caracteres)
        > -201: El número del campo del canal especificado es erróneo.
        > -210: setField() no fue llamado antes de writeFields().
        > -301: Falló la conexión a Thinkspeak.
        > -302: Fallo inesperado durante la escritura en Thinkspeak.
        > -303: No se pudo analizar la respuesta recibida.
        > -304: Tiempo de espera agotado para la respuesta del servidor.
        > -401: No se pudo escribir el valor. La causa más probable es el límite de frecuencia 
                de escritura, que puede ser una vez cada 15 segundos (Para cuentas gratuitas,
                en las cuentas de pago se podría cada segundo).
        > 0: Otro error.
    */
    // Incremento del dato simulado para escritura en Thinkspeak
    num++;
    if(num > 99){
        num = 0;
    }  
    delay(15000);   // Espera de 15 segundos para actualizar el canal de nuevo
}