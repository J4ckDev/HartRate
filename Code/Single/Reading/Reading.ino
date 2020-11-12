/*
Este código lee el último valor de uno de los campos o fields del canal 
definido en "Config.h" cada 15 segundos. El fin de este código es asegurarse 
de que hay una correcta comunicación entre la ESP y Thinkspeak para leer
datos.

CONSIDERACIONES IMPORTANTES
    -Se debe modificar el archivo "Config.h" con los datos de su proyecto.
    -Se puede disminuir el tiempo de lectura hasta 1 segundo, pero si usa este 
     código para realizar la lectura a un canal que está asociado a una cuenta 
     gratuita, el tiempo para que ese canal actualice su información será cada
     15 segundos, por lo que un tiempo menor de lectura no tendría sentido 
     ya que recibiría información repetida.
*/

#include "ThingSpeak.h"
#include "Config.h"
#include <ESP8266WiFi.h>

char ssid[] = MY_SSID;  // Nombre de la red WiFi definida en Config.h
char pass[] = MY_PASS;  // Contraseña de la red WiFi definida en Config.h
int keyIndex = 0;       // Número del índice de claves de su red (Solo necesaria 
                        // para Redes WEP) 
WiFiClient  client;     // Creación del cliente TCP
unsigned long channelID = CH_ID;        // ID del canal definido en Config.h
const char * readAPIKey = READ_API_KEY; // Clave de la API de lectura
unsigned int fieldNumber = 1;           // Número del campo o field del canal que desea leer,
                                        // debe ser un valor entre 1 y 8

void setup() {
  Serial.begin(115200);     // Iniciar la comunicación serial
  WiFi.mode(WIFI_STA);      // Definir la ESP8266 como estación, es decir, será un dispositivo 
                            // que se conecta a un punto de acceso (AP, Access Point)
  ThingSpeak.begin(client); // Iniciar la conexión IP con Thinkspeak. Si tiene un puerto 
                            // personalizado para usar Thinkspeak debe usar 
                            // ThingSpeak.begin(client, port); El valor de port debe ser numérico.
}

void loop() {

  int statusCode = 0; // Variable para guardar la respuesta HTTP del servidor.

  // Conectarse o reconectarse a la red WiFi
  // WL_CONNECTED indica que se conectó correctamente a la red indicada en Config.h
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Intentando conectarse a la red: ");
    Serial.println(MY_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Conectarse a la red WPA/WPA2. Cambiar esta línea si se usa una 
                              // red abierta o WEP.
      Serial.print("Reconectándose a la red: ");
      Serial.println(MY_SSID);
      delay(5000);     
    } 
    Serial.println("\nConectado.");
  }
  /*
    Consultar datos en Thinkspeak. Se accederá al canal por medio de la API de lectura definida en
    el archivo Config.h y se consultará el último dato del canal definido en fieldNumber, donde 
    fielNumber puede ser un valor entre 1 y 8.
  */
  long value = ThingSpeak.readLongField(channelID, fieldNumber, readAPIKey);  // La variable value
                                                                              // guardará el último 
                                                                              // valor del canal que 
                                                                              // está consultando.
  statusCode = ThingSpeak.getLastReadStatus();  // Se obtiene la respuesta HTTP de la solicitud de 
                                                // lectura realizada.
  if(statusCode == 200){  // El código 200 indica un Ok o éxito en la lectura de datos.
    Serial.println("El valor obtenido del canal es: " + String(value));
  }
  else{
    Serial.println("Problema leyendo el canal. Código de error HTTP " + String(statusCode)); 
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
  delay(15000);   
}
