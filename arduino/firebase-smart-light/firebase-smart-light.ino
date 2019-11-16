//
// Copyright 2019 Orestes Carracedo https://orestes.io
//
// This work is licensed under the Creative Commons Attribution 4.0 International License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or
// send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

#include <ESP8266WiFi.h> // ESP8266 dependency
#include <FirebaseESP8266.h> // Firebase dependency
#include <Adafruit_NeoPixel.h> // NeoPixel dependency

// Firebase Project
#define FIREBASE_HOST "PROJECT ID GOES HERE.firebaseio.com"
#define FIREBASE_AUTH "DATABASE SECRET GOES HERE"
#define WIFI_SSID "WIFI SSID GOES HERE"
#define WIFI_PASSWORD "WIFI PASSWORD GOES HERE"

#define DEVICE_ID "light-1"

// NeoPixel connection
const int LED_PIN = D4;
const int LED_COUNT = 1;

// State variables
bool on = true;
int i = 127;
int r = 255;
int g = 0;
int b = 0;

// Firebase Data API
FirebaseData firebaseData;
FirebaseJson json;

// NeoPixel API
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  
  strip.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.print("Connected: ");
  Serial.println(WiFi.localIP());
  
  Serial.print("Connecting to Firebase: ");
  Serial.println(FIREBASE_HOST);

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  if (!Firebase.beginStream(firebaseData, "/devices/" + String(DEVICE_ID) + "/state"))
  {
      Serial.println("Could not begin stream");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println();
  }
}

void loop() {
  if (!Firebase.readStream(firebaseData)) {
    Serial.println("Can't read stream data");
    Serial.println("REASON: " + firebaseData.errorReason());
    delay(1000);
  }

  if (firebaseData.streamTimeout()) {
    Serial.println();
    Serial.println("Stream timeout, will resume streaming...");
  }

  updateDataFromFirebase();
  
  if (on) {
    strip.setBrightness(i);
    strip.setPixelColor(0, strip.Color(r, g, b));
  } else {
    strip.setBrightness(0);
    strip.setPixelColor(0, strip.Color(0, 0, 0));  
    strip.clear();
  }
  
  strip.show();
}

void updateDataFromFirebase() {
  if (!firebaseData.streamAvailable()) {
    return;
  }
  
  Serial.println("Stream Data...");
  Serial.println(firebaseData.streamPath());
  Serial.println(firebaseData.dataPath());
  Serial.println(firebaseData.dataType());

 if (firebaseData.dataType() == "int")
    Serial.println(firebaseData.intData());
  else if (firebaseData.dataType() == "float")
    Serial.println(firebaseData.floatData(), 5);
  else if (firebaseData.dataType() == "double")
    printf("%.9lf\n", firebaseData.doubleData());
  else if (firebaseData.dataType() == "boolean")
    Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
  else if (firebaseData.dataType() == "string")
    Serial.println(firebaseData.stringData());
  else if (firebaseData.dataType() == "json")
    Serial.println(firebaseData.jsonString());
    
  const String path = firebaseData.dataPath();

  if (path == "/") {
    FirebaseJson &json = firebaseData.jsonObject();
    FirebaseJsonData data;
    
    json.get(data, "on");
    if (data.success && data.type == "bool")
      setOn(data.boolValue); 

    json.get(data, "brightness");
    if (data.success && data.type == "int")
      setBrightness(data.intValue);

    int tmpR;
    int tmpG;
    int tmpB;
    
    json.get(data, "color/rgb/r");
    if (data.success && data.type == "int")
      tmpR = data.intValue;
    
    json.get(data, "color/rgb/g");
    if (data.success && data.type == "int")
      tmpG = data.intValue;
    
    json.get(data, "color/rgb/b");
    if (data.success && data.type == "int")
      tmpB = data.intValue;
    
    setColor(tmpR, tmpG, tmpB);
    return;
  }

  if (path == "/on") {
    setOn(firebaseData.boolData());
    return;
  }

  if (path == "/brightness") {
    setBrightness(firebaseData.intData());
    return;
  }

  if (path == "/color") {    
    FirebaseJson &json = firebaseData.jsonObject();
    FirebaseJsonData data;
    
    int tmpR;
    int tmpG;
    int tmpB;
    
    json.get(data, "rgb/r");
    if (data.success && data.type == "int")
      tmpR = data.intValue;
    
    json.get(data, "rgb/g");
    if (data.success && data.type == "int")
      tmpG = data.intValue;
    
    json.get(data, "rgb/b");
    if (data.success && data.type == "int")
      tmpB = data.intValue;
    
    setColor(tmpR, tmpG, tmpB);
    return;
  }
}

void setOn(bool value) {
  on = value;
  if (on) {
    Serial.println("Turn light ON");
  } else {
    Serial.println("Turn light OFF");
  }
}
void setBrightness(int value) {
  i = map(value, 0, 100, 0, 255);
  Serial.print("Set brightness to: ");
  Serial.println(i);
}

void setColor(int red, int green, int blue) {
  r = constrain(red, 0, 255);
  g = constrain(green, 0, 255);
  b = constrain(blue, 0, 255);

  Serial.print("Set R to: ");
  Serial.println(r);
  Serial.print("Set G to: ");
  Serial.println(g);
  Serial.print("Set B to: ");
  Serial.println(b);
}
