/********************************************************************************
 * Jorge De León Rivas
 * mail: jdeleonrivas@gmail.com
 * Diciembre 2019
 * Archivo: main.cpp
 * 
 * Programa que ejecuta el sistema de mediciones para un sensor ESP12-E
 * Solo es necesario definir al inicio el nombre raíz de la base de datos y
 * a continuación el nombre del nodo del cual colgarán todos los tópicos.
 * 
 * LED = ON  -> ESP12-E trabajando
 * LED = OFF -> ESP12-E deepSleep
 * 
 *******************************************************************************/ 

#include "funciones.h"

//----------------------- CONFIG PARAMETERS------------------------
//Con este parametro se construye todos los topics para el protocolo MQTT.
 //Estructura del Topic: "BaseName/NodeName/"
// Nombre de la base
String BaseName = "arboles";
// Nombre del nodo.
String NodeName = "cantero"; //naranjo
String TopicMQTT; //Variable para construir el Topic de MQTT.
//El tiempo de sueño se especifica en el fichero funciones.h (sleeptime).
//-----------------------------------------------------------------

// Declarar pin para leer la bateria
//const int bateria_pin = A0;  // ESP8266 Analog Pin ADC0 = A0

// Rate loop para Deep-Sleep
//long rate = 1000;

void setup()
{
  /* LED operate in “inverted” mode, with regard to the pin levels - 
  *  when the pin is HIGH, the LED is OFF; when the pin is LOW, the LED is ON
  */
  pinMode(LED_BUILTIN, OUTPUT); 
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883); 
  client.setCallback(callback);
  //client.subscribe("arboles/configuracion/rate");
}

void loop()
{

  // Forzar que lea el callback
  uint32_t loopStart = millis(); 
  while (millis() - loopStart < 1500) 
  { 
    if (!client.connected()) 
    { 
      reconnect(BaseName, NodeName);
    } else client.loop(); 
  } //END OF LOOP CODE

  sht_function(BaseName, NodeName);
  battery_read_function(BaseName, NodeName);
  
  sleep_function(BaseName, NodeName);

}
