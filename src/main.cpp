#include <Arduino.h>
#include <SPIFFS.h>
#include <Audio.h>  // ESP32-audioI2S library
#include <WiFi.h>
#include <HTTPClient.h>


Audio audio;  // main audio object


const char* ssid = "";
const char* password = "";

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed!");
    while (true);
  }

  // Init I2S for MAX98357A
  // Args: BCLK, LRC, DOUT
  audio.setPinout(26, 25, 22);  

  // Volume: 0.0 to 21.0
  audio.setVolume(21);  

  // Play from SPIFFS
  audio.connecttoFS(SPIFFS, "/WAKE THE FUCK UP SONG.mp3");
}

void loop() {
  audio.loop(); // must be called continuously
}