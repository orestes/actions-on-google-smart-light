#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, 2, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.clear(); // Set all pixel colors to 'off'
  strip.setBrightness(255);
  Serial.begin(9600);
}

void loop() {
    strip.setPixelColor(0, strip.Color(255, 0, 0));
    Serial.println("red");
    strip.show();
    delay(500);
    
    strip.setPixelColor(0, strip.Color(0, 255, 0));
    Serial.println("green");
    strip.show();
    delay(500);
    
    strip.setPixelColor(0, strip.Color(00, 0, 255));
    Serial.println("blue");
    strip.show();
    delay(500);
}
