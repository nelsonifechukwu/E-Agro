
//Libraries for ESP32
#include <Wire.h>
#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#else
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#endif

//Temperature Libraries
#include <DHT.h>
#include <DHT_U.h>

//LCD Libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Replace with your network credentials
const char* ssid = "EXPERIMENT";
const char* password = "Xjok23dl!?ifenel9";
String serverName = "http://192.168.0.155:5000/";  //http://127.0.0.1:5000 for local dev or set 0.0.0.0 in flask env
const char* api_key = "xdol";
String state;
String response;
String range;
int set_range[3];

//Sensor Variables
float temperature;
float humidity;
float light_intensity;
float soil_moisture;
float co2_volume;

//SENSOR PIN DEFINITIONS
#define DHTPIN 32
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
int light_intensity_pin = 36;
int soil_moisture_pin = 39;
int co2_volume_pin = 35;

//ACUTATOR PIN & STATE DEFINITIONS
int fan_pin = 33;
bool fan_state = 0;
int bulb_pin = 17;  //34
bool bulb_state = 0;
int valve_pin = 23;
bool valve_state = 0;
int button_pin = 34;
int buzzer = 16;

//Send to Cloud details
unsigned long previous_time = 0;
const unsigned long interval_to_send = 10000;

void setup() {

  dht.begin();
  Serial.begin(115200);
  lcd.begin();

  //SET PIN MODES
  pinMode(buzzer, OUTPUT);
  pinMode(fan_pin, OUTPUT);
  pinMode(bulb_pin, OUTPUT); bulb(!0);
  pinMode(valve_pin, OUTPUT); valve(!0); //! because bulb and fan are controlled by a relay with N/O configuration
  pinMode(light_intensity_pin, INPUT);
  pinMode(soil_moisture_pin, INPUT);
  pinMode(co2_volume_pin, INPUT);

  //WIFI CONNECTION
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  temperature = read_temperature();
  humidity = read_humidity();
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    //return;
  }
  light_intensity = read_light_intensity();
  soil_moisture = read_soil_moisture();
  co2_volume = read_co2_volume();

  //LCD
  display();
  //Menu: Do you want normal readings only or continuous reading with cloud integration
  int cloud = 1;
  //Send Data to cloud every interval_to_send secs.
  if (cloud) {
    if (millis() - previous_time >= interval_to_send) {
      String data = "update/key=" + String(api_key) + "/c1=" + String(int(temperature))
                    + "/c2=" + String(int(humidity)) + "/c3=" + String(int(light_intensity))
                    + "/c4=" + String(int(soil_moisture)) + "/c5="
                    + String(int(co2_volume)) + "";
      send_to_cloud(data);
      previous_time = millis();
    }
  }
  //GET STATE
  state = send_to_cloud("get_state");  // state contains e.g 1, 0, 1
  //Send state to actuators
  apply_state();
  //GET Range
  range = send_to_cloud("get_control_input/all");  //range contains e.g 3, 4, 5
  split_range();
  //SEND STATES TO ACTUATORS
  //set_range [3] = s_temp, s_light, s_moist
  check_states(set_range[0], set_range[1], set_range[2]);
}

void display() {
  lcd.setCursor(1, 0);
  lcd.print("Greenhouse Updates");
  lcd.setCursor(0, 1);
  lcd.print("T: ");
  lcd.print(temperature, 2);
  lcd.print((char)223);
  lcd.print("C ");
  lcd.print("H: ");
  lcd.print(humidity, 2);
  lcd.setCursor(0, 2);
  lcd.print("L: ");
  lcd.print(light_intensity, 2);
  lcd.print(" S: ");
  lcd.print(soil_moisture, 2);
  lcd.setCursor(0, 3);
  lcd.print("CO2(ppm): ");
  lcd.print(co2_volume, 2);
}

//READ SENSOR VALUES
float read_temperature() {
  float t = dht.readTemperature();
  return t;
}

float read_humidity() {
  float h = dht.readHumidity();
  return h;
}

float read_light_intensity() {
  float l = analogRead(light_intensity_pin);
  return l;
}

float read_soil_moisture() {
  float s = analogRead(soil_moisture_pin);
  return s;
}

float read_co2_volume() {
  float c = analogRead(co2_volume_pin);
  return c;
}

//UPDATE STATES
void check_states(float set_temp, float set_light, float set_moist) {
  if (temperature > set_temp) buzz_temp();
  if (light_intensity < set_light) buzz_light();
  if (soil_moisture < set_moist) buzz_moist();
}
//COMMAND ACTUATORS
void fan(int fstate) {
  digitalWrite(fan_pin, fstate);
}
void bulb(int bstate) {
  digitalWrite(bulb_pin, bstate);
}
void valve(int vstate) {
  digitalWrite(valve_pin, vstate);
}
void buzz_temp() {
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
  delay(1000);
}
void buzz_moist() {
  digitalWrite(buzzer, HIGH);
  delay(500);
  digitalWrite(buzzer, LOW);
  delay(500);
}
void buzz_light() {
  digitalWrite(buzzer, HIGH);
  delay(2000);
  digitalWrite(buzzer, LOW);
  delay(2000);
}


//SEND TO CLOUD
String send_to_cloud(String request) {

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Prepare your HTTP POST request data
    String httpRequestData = request;
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

    http.begin((serverName + httpRequestData).c_str());
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Send HTTP GET request
    int httpResponseCode = http.GET();
    delay(1100);  //This should give time for the server to process the request.

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      response = http.getString();
      if (request == "get_state" | request == "get_control_input/all") {
        return response;
      }

    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void apply_state() {
  fan(char(state[0]) - '0');
  bulb(!(char(state[3]) - '0'));  //! because bulb and fan are controlled by a relay with N/O configuration
  valve(!(char(state[6]) - '0'));
}

void split_range() {
  int firstComma = range.indexOf(",");
  String firstElement = range.substring(0, firstComma);
  set_range[0] = firstElement.toInt();

  // Extract the second element
  int secondComma = range.indexOf(",", firstComma + 1);
  String secondElement = range.substring(firstComma + 2, secondComma);
  set_range[1] = secondElement.toInt();

  // Extract the third element
  String thirdElement = range.substring(secondComma + 2);
  set_range[2] = thirdElement.toInt();
}