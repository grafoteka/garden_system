/***************************************************************************************
 * Jorge De León Rivas
 * mail:jdeleonrivas@gmail.com
 * Diciembre 2019
 * Archivo: funciones.h
 * 
 * Este archivo esta configurado para un sensor SHT30 y SHT31. 
 * Estos sensores funcionan por I2C.
 * D1 -> CLK
 * D2 -> Data
 * 
 *  Fichero de funciones necesarias para el funcionamiento del riego
 *
 *  -void setup_wifi()
 *       Función que se conecta a la red wifi indicada
 *
 *  -void callback (char* topic, byte* payload, unsigned int length)
 *       Función que se suscribe a los tópicos indicados 
 *
 *  -void reconnect(String bName, String nName)
 *       Función que realiza la reconexión al servidor MQTT 
 *       y también hace la llamada al callback
 *
 *  -void enviar_dato(String dato, String bName, String nName, String tipoDato)
 *       Función para publicar un mensaje en un tópico indicado
 *
 *  -void sht_function(String BName, String NName)
 *       Función que hace la medición del sensor. 
 *       Luego realiza una llamada a la publicación en el tópico indicado
 *
 *  -void battery_read_function(String BName, String NName)
 *       Función que hace la medición del voltaje en el pin Vin y con la función getVcc
 *       Luego realiza una llamada a la publicación en el tópico indicado
 *
 *  -void sleep_function(String bName, String nName)
 *       Función para poner en SLEEP durante un intervalo de tiempo indicado
 *
 ****************************************************************************************/

#include <ESP8266WiFi.h>
#include <WEMOS_SHT3X.h>
#include <PubSubClient.h>

//----------------------- NEWTORK PARAMETERS------------------------
// ARAFO
const char* ssid = "cantero_router";
const char* password = "Jjdeleon_56";
// Valle Guerra
//const char* ssid = "MOVISTAR_ED06";
//const char* password = "MFaHPyfbaExE4tRC6Ctk";
// La Laguna
//const char* ssid = "linksys";
//const char* password = "";
// RPI3
const char* mqtt_server = "192.168.1.3";
// RPI2
//const char* mqtt_server = "192.168.1.2";
//-----------------------------------------------------------------

WiFiClient espClient;
PubSubClient client(espClient);

// Variables del mensaje
//char msg_temp[50];
//char msg_hum [50];

// Declarar sensor temperatura y humedad
SHT3X sht30(0x44);

// Tiempo de deepsleep. Cambiando el primer parametro. La cuenta o convierte en uS.
const int sleeptime = 60 * 10e5; // En segundos //20 * 60 * 1000000;
int rate = 1000;
//----------------------- WIFI CONNECTION ------------------------
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
//-----------------------------------------------------------------

//----------------------- WIFI RE-CONNECTION ------------------------
void reconnect(String bName, String nName) {
  // Loop until we're reconnected
  int reconnect_try = 0;
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      delay(1000);
      // Once connected, publish an announcement...
      String TopicMQTT = bName + "/" + nName;
      String payload = "";
      TopicMQTT += "/info";
      payload += "Conectado";
      //client.publish("arboles/melocotonero/info", "Conectado");
      client.publish((char*) TopicMQTT.c_str(), (char*) payload.c_str());
      // ... and resubscribe
      //client.subscribe("arboles/configuracion/rate");
      if(client.subscribe("arboles/configuracion/rate"))
      {
        Serial.println("Subscribed to arboles/configuracion/rate");  
      } else {
      Serial.println("Failed to subscribe to arboles/configuracion/rate");    
      }
      
      //client.subscribe("despacho/luz");
    } else {
      reconnect_try++;
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
      if(reconnect_try == 5)
      {
        // Dormir durante una hora
        ESP.deepSleep(3.6e9, WAKE_RF_DEFAULT);
      }
    }
  }
}
//-----------------------------------------------------------------

//----------------------- CALLBACK ------------------------
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.print("El valor entero es: "); 
  payload[length] = '\0';
  rate = atoi((char*)payload); //Char to int
  rate = rate * 1000;
  Serial.println(rate);
  //EEPROM.put(rate_address, rate);
}
//-----------------------------------------------------------------

void enviar_dato(String dato, String bName, String nName, String tipoDato)
{
    //Creamos el topic. Sera del estilo: "BName/NName/TipoDato"
     String TopicMQTT = bName + "/" + nName + "/" + tipoDato;
    //Creamos el mensaje. Sera del estilo: "NName TipoDato=";
     String payload; //Variable para contener la info de la trama que se envia.
     payload = ""; //Limpiamos la variable.
     //payload += NName;
     //payload += " " + TipoDato + "=";
     payload += dato;
     //Enviamos la trama via MQTT.
     client.publish((char*) TopicMQTT.c_str(), (char*) payload.c_str());
     delay(3000);
}

void sht_function(String BName, String NName)
{
    if(sht30.get()==0)
    {
        float temperatura = sht30.cTemp;
        float humedad     = sht30.humidity;
        Serial.print ("Temperatura = "); Serial.println(temperatura);
        Serial.print ("Humedad = "); Serial.println(humedad);
            
        char temperatura_str[6];
        char humedad_str[6];
        //char* dtostrf(float number, int tamanyo, int decimales, char* buffer);
        /*                    |         |             |               \_ buffer donde almacenaremos la cadena
                                |         |             \_ Precisión (nº decimales)
                                |         \_Tamaño del número en carácteres
                                \_ Número a convertir
        */

        dtostrf(temperatura,  4, 2, temperatura_str);
        dtostrf(humedad,      4, 2, humedad_str);

        enviar_dato(temperatura_str, BName, NName, "temperatura");
        enviar_dato(humedad_str, BName, NName, "humedad");
    }
    else
    {
        Serial.println("Error! Not sensor detected");
    }
}

void battery_read_function(String BName, String NName)
{
    // Lectura voltaje bateria
    int bateria_voltaje_actual = ESP.getVcc();//analogRead(bateria_pin);//bateria_voltaje_actual = bateria_voltaje_actual * 0.3125;
    bateria_voltaje_actual = map(bateria_voltaje_actual, 0, 65535, 0, 100);  //bateria_voltaje_actual = bateria_voltaje_actual/1024.0;
    Serial.print ("Bateria = "); Serial.println(bateria_voltaje_actual);
    // map it to the range of the PWM out
    char bateria_voltaje_str[6];
    dtostrf(bateria_voltaje_actual,  4, 2, bateria_voltaje_str);
    enviar_dato(bateria_voltaje_str, BName, NName, "Bateria");
}

void sleep_function(String bName, String nName)
{
  /* LIGHT SLEEP */
  //WiFi.disconnect(); // DO NOT DISCONNECT WIFI IF YOU WANT TO LOWER YOUR POWER DURING LIGHT_SLEEP_T DELLAY !
  //Serial.println(WiFi.status());  
  Serial.println("Go to sleep");
  /*client.publish("arboles/melocotonero/info", "Estado deep-sleep");
  digitalWrite(LED_BUILTIN, HIGH);
  wifi_set_sleep_type(LIGHT_SLEEP_T);
  delay(6000-800); // loop every 3 minutes
  */
  Serial.println("INFO: Closing the Wifi connection");
  WiFi.disconnect();
  while (client.connected() || (WiFi.status() == WL_CONNECTED))
  {
    Serial.println("Waiting for shutdown before sleeping");
    delay(10);
  }
  delay(10);  
  //ESP.deepSleep(5e6, WAKE_RF_DEFAULT);
  Serial.print("Voy a dormir "); Serial.print(rate/(6e7)); Serial.println(" minutos");
  //client.publish("arboles/melocotonero/info", "Voy dormir");
  //Creamos el topic. Sera del estilo: "BName/NName/TipoDato"
  String TopicMQTT = bName + "/" + nName + "/info";
  //Creamos el mensaje. Sera del estilo: "NName TipoDato=";
  String payload; //Variable para contener la info de la trama que se envia.
  payload = "Voy a dormir"; //Limpiamos la variable.
  client.publish((char*) TopicMQTT.c_str(), (char*) payload.c_str());
  ESP.deepSleep(rate, WAKE_RF_DEFAULT);
  //delay(500); // wait for deep sleep to happen

  /* DEEP SLEEP */
  //ESP.deepSleep(9e8);
}