/*
 Programa completo para implementar WebServer de Control para Tarjeta Smart Home

 En este programa se tomó como base el ejemplo SimpleWebServer que ya tenía la tarjeta Feather ESP32 de Adafruit. Se implementó un ejemplo que puede desde un webserver con 4 botones probar
 el buzzer, el relé izquierdo, los LEDs y extraer informacion del sensor BME280.

 Creado por Ángel Isidro y Oscar Rodas / Tesla Lab
 Guatemala
 Universidad Galileo 2019
 
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */

//Librerias necesarias para WiFi
#include <WiFi.h>

//Librerias y seteo para BME280
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;


//Librerias necesarias para NEOPIXEL
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

//Configuracion de Red WiFi

const char* ssid     = "PSI_2_4";
const char* password = "PSI02019";

//const char* ssid     = "Minion2";
//const char* password = "minion6543";

//Definicion de pin para rele
int releIzquierdo = 12;

//Definicion de pin para buzzer
int pinBuzzer = 14;     // Numero del pin digital conectado el buzzer
int frecuencia = 2000;  // Esta en Hertz
int duracion = 2000;    // Valor en milisegundos

///Definicion de webserver
WiFiServer server(80);

//Definicion de pines para LEDS
#define PIN 15 //PIN en el cual esta conectado el NEO PIXEL

#define NUMPIXELS 10 // Numero de NEO PIXEL conectados

// Creamos un objeto de tipo NEO PIXEL
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); 


void setup()
{
    Serial.begin(115200);
    

//    pinMode(5, OUTPUT);

//  Seteamos pin para rele izquierdo    
    pinMode(releIzquierdo,OUTPUT);

    delay(10);

// Inicializamos la libreria de NEO PIXEL
    pixels.begin(); 

// Setear brillo de LEDs
    pixels.setBrightness(64);    

//  Iniciamos conexion a red WiFi

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        // Mostrar intento de conexion
        pixels.setPixelColor(9, pixels.Color(255,0,0));   //Color LED color rojo
        pixels.show(); //Refrescamos los pixeles
        delay(250); //Retardo de encendido en cada pixel;
        pixels.clear();   //Color LED color rojo
        pixels.show(); //Refrescamos los pixeles
        delay(250); //Retardo de encendido en cada pixel;
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

//  Indicar que se conecto al Internet
    pixels.setPixelColor(9, pixels.Color(0,255,0));
    pixels.show(); //Refrescamos los pixeles
    delay(1000); //Retardo de encendido en cada pixel;

//  Apagar indicacion de conexion a Internet
    pixels.clear();
    pixels.show();
    delay(500);
    
//  Inicializamos el webserver    
    server.begin();

//  Aseguramos que rele izquierdo arranque apagado
    digitalWrite(releIzquierdo,LOW);
    delay(500);

    //NO TOCAR ESTE CODIGO 
    #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
    #endif
    //NO TOCAR ESTE CODIGO 

// Inicializamos la libreria de NEO PIXEL
    pixels.begin(); 

// Verificar si BME está encendido
if (!bme.begin(0x77)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

}

int value = 0;

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            //Determinar temperatura
            int tempParaDesplegar = bme.readTemperature();
            
            // the content of the HTTP response follows the header:
            client.print("<h1 align=\"center\"> Taller de IoT - Ing. Electr&oacute;nica - UGAL </h1>");
            client.print("<form method=\"get\" action=\"/H\"> <button style=\"background-color: #0000CC; font-size:50px; color:#FFFFFF; height:120px;width:100%\" type=\"submit\"> Rel&eacute; ON </button> </form>");
            client.print("<form method=\"get\" action=\"/L\"> <button style=\"background-color: #00CC00; font-size:50px; color:#FFFFFF; height:120px;width:100%\" type=\"submit\"> Rel&eacute; OFF </button> </form>");
            client.print("<form method=\"get\" action=\"/B\"> <button style=\"background-color: #CC0000; font-size:50px; color:#FFFFFF; height:120px;width:100%\" type=\"submit\"> Probar Buzzer </button> </form>");
            client.print("<form method=\"get\" action=\"/N\"> <button style=\"background-color: #9955CC; font-size:50px; color:#FFFFFF; height:120px;width:100%\" type=\"submit\"> Probar LEDS </button> </form>");
            client.print("<h1 align=\"center\">La temperatura en el ambiente es ");
            client.print(tempParaDesplegar); 
            client.print(" &#176;C </h1>");

            Serial.print("Temp = ");
            Serial.print(bme.readTemperature());
            Serial.println(" ºC");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;

            // Verificar valor de Temperatura en el Monitor Serial
            Serial.print("Temperatura = ");
            Serial.print(bme.readTemperature());
            Serial.println("ºC");
                        
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Verifica si el client request fue "GET /H" para encender el rele
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(releIzquierdo,HIGH);
          delay(1000);
        }
        // Verifica si el client request fue "GET /L" para apagar el rele
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(releIzquierdo,LOW);
          delay(1000);
        }
        // Verifica si el client request fue "GET /B" para testear el buzzer
        if (currentLine.endsWith("GET /B")) {         
          myTone(pinBuzzer,frecuencia,duracion);
        }
        // Verifica si el client request fue "GET /N" para testear los LEDs
        if (currentLine.endsWith("GET /N")) {         
          testNeopixels();
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void myTone(byte pin, uint16_t frequency, uint16_t duration)
{ // Parametros de entrada: Numero de pin, frequency en Hz, duration en milisegundos
  unsigned long startTime=millis();
  unsigned long halfPeriod= 1000000L/frequency/2;
  pinMode(pin,OUTPUT);
  while (millis()-startTime< duration)
  {
    digitalWrite(pin,HIGH);
    delayMicroseconds(halfPeriod);
    digitalWrite(pin,LOW);
    delayMicroseconds(halfPeriod);
  }
  pinMode(pin,INPUT);
}

void testNeopixels ()
{
  // Animacion de leds con un color especifico
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i, pixels.Color(23,161,165));  // Color en leds turquesa
    pixels.show();                                      //Refrescamos los pixeles
    delay(500);                                         //Retardo de encendido en cada pixel;
  }
  // Apagar leds con amimacion
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i, pixels.Color(0,0,0));       //Apagamos los LEDs
    pixels.show(); 
    delay(100);
  }
}
