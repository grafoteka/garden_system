# garden_system
Sistema de riego con módulos ESP13-E y sensores SHT10, SHT30 y SHT31.

  El sensor SHT10 funciona con 2 pines digitales, no con I2C.
  
    D6 -> CLK
    
    D5 -> Data
  
  Los sensores SHT30 y SHT31 funcionan por I2C.
  
    D1 -> CLK
  
    D2 -> Data

El programa consta de 2 ficheros: main.cpp y funciones.h

- Main.cpp:
  Programa que ejecuta el sistema de mediciones para un sensor ESP12-E.
  Solo es necesario definir al inicio el nombre raíz de la base de datos y a continuación el nombre del nodo del cual colgarán todos los tópicos. 
  
  
    LED = ON  -> ESP12-E trabajando
    
    LED = OFF -> ESP12-E deepSleep
    
  
- Funciones.h
  El fichero de funciones necesarias para el funcionamiento del riego
 
   - void setup_wifi()
        Función que se conecta a la red wifi indicada
 
   - void callback (char* topic, byte* payload, unsigned int length)
        Función que se suscribe a los tópicos indicados 
 
   - void reconnect(String bName, String nName)
        Función que realiza la reconexión al servidor MQTT 
        y también hace la llamada al callback
 
   - void enviar_dato(String dato, String bName, String nName, String tipoDato)
        Función para publicar un mensaje en un tópico indicado
 
   - void sht_function(String BName, String NName)
        Función que hace la medición del sensor. 
        Luego realiza una llamada a la publicación en el tópico indicado
 
   - void battery_read_function(String BName, String NName)
        Función que hace la medición del voltaje en el pin Vin y con la función getVcc
        Luego realiza una llamada a la publicación en el tópico indicado
 
   - void sleep_function(String bName, String nName)
        Función para poner en SLEEP durante un intervalo de tiempo indicado
