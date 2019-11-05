/*
  Programa que publica la temperatura leída con el DHT11
  casa/despacho/temperatura
  casa/despacho/humedad
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"

//----------------------- CONFIG PARAMETERS------------------------
//Con este parametro se construye todos los topics para el protocolo MQTT.
// Nombre del nodo.
String NodeName = "Testing";
String TopicMQTT; //Variable para construir el Topic de MQTT.
//El tiempo de sueño se especifica en el fichero funciones.h (sleeptime).
//-----------------------------------------------------------------


DHTesp dht;

// Update these with values suitable for your network.
const char* ssid = "george";
const char* password = "RVVBQTHCTJTWTB";
const char* mqtt_server = "192.168.1.10";

//On board LED Connected to GPIO2
#define LED 2

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg_temp[50];
char msg_hum [50];
int value = 0;

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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("despacho/temperatura", "Enviando el primer mensaje");
      // ... and resubscribe
      client.subscribe("despacho/luz");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  dht.setup(0, DHTesp::DHT11); // Connect DHT sensor to GPIO 17

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 60000) {
    lastMsg = now;
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();

    char valTemperatura[5];
    char valHumedad[5];

    dtostrf(humidity, 3, 1, valHumedad);
    String payload; //Variable para contener la info de la trama que se envia.
     //Creamos la trama. Sera del estilo: "sensors/NodeName/temp"

    TopicMQTT = "sensors/" + NodeName + "/temperatura";
    //Creamos el mensaje. Sera del estilo: "NodeName temperature=";
    payload = ""; //Limpiamos la variable.
    // Para node-red necesito quitar las palabras y dejar solo el valor como string
    //payload += NodeName;
    //payload += " temperatura=";
    payload += valTemperatura;
    //Enviamos la trama via MQTT.
    client.publish((char*) TopicMQTT.c_str(), (char*) payload.c_str());


    dtostrf(temperature, 3, 1, valTemperatura);
    //Creamos la trama. Sera del estilo: "sensors/NodeName/humidity"
    TopicMQTT = "sensors/" + NodeName + "/humedad";
    //Creamos el mensaje. Sera del estilo: "NodeName humidity=";
    payload = ""; //Limpiamos la variable.
    //payload += NodeName;
    //payload += " Humedad=";
    payload += valHumedad;
    //Enviamos la trama via MQTT.
    client.publish((char*) TopicMQTT.c_str(), (char*) payload.c_str());

    //long now = millis();
    //if (now - lastMsg > 2000) {
    //  lastMsg = now;
    //  ++value;
    //snprintf (msg_temp, 75, "temperatura=%.f", temperature);
    //snprintf(msg_hum, 75, "humedad=%.f", humidity);
    // Serial.print("Publish message: ");
    // Serial.println(msg_temp);
    //client.publish("despacho/temperatura", msg_temp);
    //client.publish("despacho/humedad", msg_hum);
    //}
  }
  delay(dht.getMinimumSamplingPeriod());
}
