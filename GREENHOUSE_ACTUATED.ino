//DHT SENSOR 32
//LDR 33
//FAN CONTROL 21
//LIGHT CONTROL 23


#include <Blynk.h>
#include <BlynkSimpleEsp32.h>
#define BLYNK_PRINT Serial

const char* ssid = "";
const char* password = "";
char auth[] = ""; 

//#ifdef ESP32
  #include <WiFi.h>
  #include <HTTPClient.h>

#include <WiFiClient.h>
//#else
//  #include <ESP8266WiFi.h>
//  #include <ESP8266HTTPClient.h>
//  #include <WiFiClient.h>
//#endif

#include <Wire.h>


// Set the LCD address to 0x27 for a 16 chars and 2 line display
//LiquidCrystal_I2C lcd(0x27, 16, 2);

//Include DHT libraries
#include <DHT.h>
#define DHTPIN 32    
#define DHTTYPE DHT11     
DHT dht(DHTPIN, DHTTYPE);


float humidity = 0;
float setTemperature = 0;
float setLightIntensity = 0;
int lightIntensity = 0;

#define LDR 33
#define fan 21
#define light 23
#define redlight 22
//#define button1 34
//#define button2 35
//#define resetbutton 39


//bool check = 1; 

BLYNK_WRITE(V0)
{
   setTemperature = param.asInt();

}


BLYNK_WRITE(V1)
{
   setLightIntensity = param.asInt();

}
void setup() {
  // put your setup code here, to run once:
  pinMode(fan, OUTPUT);
  pinMode(light, OUTPUT);
  pinMode(redlight, OUTPUT);
  digitalWrite(redlight, HIGH);
//  pinMode(button1, INPUT);
//  pinMode(button2, INPUT);
//  pinMode(resetbutton, INPUT);
  Serial.begin(9600);
//  EEPROM.begin(EEPROM_SIZE);
  dht.begin();

  //CONNECT TO WIFI

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

    //BLYNK START
  Blynk.begin(auth, ssid, password);
 
  
  
}

void loop() {
  // put your main code here, to run repeatedly:
//  if (check){
//    threshold_set();
//    }

 Blynk.run();
 
    //READ FROM DHT11 SENSOR
  float h = dht.readHumidity();  //Read the humidity
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
     //READ LIGHT INTENSITY FROM LDR
     
lightIntensity = analogRead(LDR);

  

  
     //COMPARE PARAMETERS WITH SET VALUES
  if (t > setTemperature)
  {
    digitalWrite (fan, HIGH); //Turn fan ON
    digitalWrite (redlight, LOW);//Turn alarm light ON, PULL-UP resistor
    } else{
          digitalWrite (fan, LOW); //Turn fan OFF
    digitalWrite (redlight, HIGH); //Turn alarm light OFF
      }
  if (lightIntensity < setLightIntensity)
  {
    digitalWrite (light, HIGH); //Turn light ON
    } 
else{
         
    digitalWrite (light, LOW); //Turn  light OFF
      }
    

      //SEND TO BLYNK 
  Blynk.virtualWrite(V2, t); 
  Blynk.virtualWrite(V3, h); 
  Blynk.virtualWrite(V4, lightIntensity);
  delay(1500);
}

//void threshold_set(){
//      while(check == 1){
//        int counter = 0;
//        if (digitalRead(button1 == HIGH))
//        {counter1++;
//        Serial.println(counter1);
//        }
//        if (digitalRead(button2 == HIGH))
//        {
//          counter1--;
//          Serial.println(counter1);
//          }
//        if (digitalRead(resetbutton == HIGH))
//        {
//          EEPROM.write(0, counter);
//          check = 0;
//          }
//        }
//  
//  }
//


