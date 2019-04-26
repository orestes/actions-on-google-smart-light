#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>

// Set these to run example.
#define WIFI_SSID "orestes_LTE"
#define WIFI_PASSWORD "hell0wifi"

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
  Firebase.stream("/devices/light-1/state");
}

void loop() {
  if (Firebase.failed()) {
    Serial.println("streaming error");
    Serial.println(Firebase.error());

    Serial.println(Firebase.error());
  }

  if (Firebase.available()) {
     FirebaseObject event = Firebase.readEvent();
     String eventType = event.getString("type");
     eventType.toLowerCase();

     Serial.print("event: ");
     Serial.println(eventType);

     Serial.println("path: " + event.getString("path"));

     Serial.print("data: ");
     JsonVariant variant = event.getJsonVariant("data");
     variant.prettyPrintTo(Serial);

     // if (eventType == "put") {}
     if (eventType == "") {
      Serial.println("discarded");
     }


  }
}
