#include <ESP8266WiFi.h>
#include <WEMOS_SHT3X.h>
#include <PubSubClient.h>
#include <EEPROM.h>

//----------------------- NEWTORK PARAMETERS------------------------
// ARAFO
//const char* ssid = "cantero_router";
//const char* password = "Jjdeleon_56";
// Valle Guerra
//const char* ssid = "MOVISTAR_ED06";
//const char* password = "MFaHPyfbaExE4tRC6Ctk";
// La Laguna
const char* ssid = "linksys";
const char* password = "";
// RPI3
//const char* mqtt_server = "192.168.1.3";
// RPI2
const char* mqtt_server = "192.168.1.3";
//-----------------------------------------------------------------

WiFiClient espClient;
PubSubClient client(espClient);

// Variables del mensaje
char msg_temp[50];
char msg_hum [50];

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
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("arboles/naranjo/info", "Conectado");
      // ... and resubscribe
      //client.subscribe("arboles/configuracion/rate");
      client.subscribe("arboles/configuracion/rate");
      //client.subscribe("despacho/luz");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
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