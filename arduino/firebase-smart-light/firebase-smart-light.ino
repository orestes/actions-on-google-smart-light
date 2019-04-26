#include <FirebaseCloudMessaging.h>
#include <Firebase.h>
#include <FirebaseHttpClient.h>
#include <FirebaseArduino.h>
#include <FirebaseError.h>
#include <FirebaseObject.h>

#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>

// Wi-fi access
#define WIFI_SSID "orestes_LTE"
#define WIFI_PASSWORD "hell0wifi"

// NeoPixel connection
#define LED_STRIP_PIN = 2;
#define LED_STRIP_COUNT = 1;

// State variables
bool on = true;
int i = 127;
int r = 255;
int g = 0;
int b = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_STRIP_COUNT, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin("toy-home.firebaseio.com");
  Firebase.stream("/devices/light-1/state"); // TODO: Use your device ID if you change it
}

void loop() {
  if (Firebase.failed()) {
    Serial.println("streaming error");
    Serial.println(Firebase.error());

    Serial.println(Firebase.error());
  }

  updateDataFromFirebase();
  
  if (on) {
    strip.setBrightness(i);
    strip.setPixelColor(0, strip.Color(r, g, b));
  } else {
    strip.setBrightness(0);
    strip.setPixelColor(0, strip.Color(0, 0, 0));  
  }
  
  strip.show();
}

void updateDataFromFirebase() {
  if (!Firebase.available()) {
    return;
  }
  
  FirebaseObject event = Firebase.readEvent();
  String eventType = event.getString("type");
  eventType.toLowerCase();

  if (eventType == "") {
   Serial.println("discarded");
   return;
  }

  // Let's output some debugging data
  Serial.print("event: ");
  Serial.println(eventType);
  
  Serial.println("path: " + event.getString("path"));
  
  Serial.print("data: ");
  JsonVariant variant = event.getJsonVariant("data");
  variant.prettyPrintTo(Serial);

  // We only care act on  put events
  if (eventType != "put") {
    return;
  }

  if (path == "/") {
    setOn(data.on);
    setBrightness(data.brightness);
    setColor(data.color.rgb.r, data.color.rgb.g, data.color.rgb.b);
  }

  if (path == "state/on") {
    setOn(data.on);
  }

  if (path == "state/brightness") {
    setBrightness(data.brightness);
  }

  if (path == "state/color" || path == "state/color/rgb") {
    setColor(data.color.rgb.r, data.color.rgb.g, data.color.rgb.b);
  }
}

void setOn(value) {
  on = value;
}
void setBrightness(value) {
  i = map(value, 0, 100, 0, 255);
}
void setColor(red, green, blue) {
  r = constrain(red, 0, 255);
  g = constrain(green, 0, 255),
  b = constrain(blue, 0, 255)
}
