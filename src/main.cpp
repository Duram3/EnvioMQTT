
/*  Hace que el dispositivo NodeMCU pueda leer del puerto serial datos de direcciones MAC
    detectadas por su par Sniffer. Estos datos se encapsulan en formato Json junto con el número de piso y 
    la zona y se envían mediante WiFi y MQTT al broker MQTT, para que los suscriptores del tópico '1/A'
    puedan recibirlos
*/


#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
//.................................................................................................................

const char* ssid = "Nombre Red";
const char* password = "contraseña";
const char* mqtt_server = "192.168.43.110";


SoftwareSerial SofSerial(D5,D6); // (Rx, Tx)
WiFiClient clienteMQTT;
PubSubClient cliente(clienteMQTT);

String datosSerial;
char topico[10] = "1/A";

//_________________________________________________________________________________________________________________

void leerPuerto() {

    if (SofSerial.available()){

        StaticJsonDocument<100> datos;
        String MAC= "";
        MAC = SofSerial.readStringUntil('\n');
        datos["piso"] = "1";
        datos["zona"] = "A";
        datos["MAC"] = MAC;
        Serial.print("\nRecibido:\n");
        Serial.print(MAC);
        datosSerial = "";
        serializeJson(datos, datosSerial); // serializando para enviar
        // Se lleva el string "datosSerial" a un arreglo de char, para que sea admitido por la funcin "publish"
        char charBuf[datosSerial.length() + 1]; // Arreglo con el tamaño del string
        datosSerial.toCharArray(charBuf, datosSerial.length() + 1); //Se llena el arreglo con la datos  del string
        cliente.publish(topico, charBuf);

  }

}
//_________________________________________________________________________________________________________________


//_________________________________________________________________________________________________________________

void setup() {

    SofSerial.begin(9600);
    Serial.begin(115200);
    delay(10);
    setupWiFi();
    cliente.setServer(mqtt_server, 1883);

}
//_________________________________________________________________________________________________________________

void loop() {
    
    if (!cliente.connected()){
        
        reconnect();
    
    }
    
    cliente.loop();

    leerPuerto();

}
