#include <Arduino.h>
#include <SPIFFS.h>
#include <Audio.h>  // ESP32-audioI2S library
#include <WiFi.h>
#include <HTTPClient.h>

#define FEATUREPIN 14

Audio audio;  // main audio object

unsigned long lastCheck = 0;
const unsigned long checkInterval = 600000; // 10 min

bool isPlaying = false; 

const char* ssid = "";
const char* password = "";

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed!");
    while (true);
  }
  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    Serial.println(WiFi.status());
  }
  Serial.println("Connected to WiFi");
  pinMode(FEATUREPIN, OUTPUT);


  // Init I2S for MAX98357A
  // Args: BCLK, LRC, DOUT
  audio.setPinout(26, 25, 22);  

  // Volume: 0.0 to 21.0
  audio.setVolume(21);  

  // Play from SPIFFS
  // audio.connecttoFS(SPIFFS, "/WAKE THE FUCK UP SONG.mp3");
}

void loop() {
  audio.loop(); // must be called continuously


  if (millis() - lastCheck > checkInterval) {
    lastCheck = millis();
    String payload = "";
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String serverPath = "https://decapi.me/twitch/uptime/soymilk"; // Replace with your target URL

      http.begin(serverPath.c_str()); // Specify the URL

      int httpResponseCode = http.GET(); // Send the GET request

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString(); // Get the response payload
        Serial.println(payload);
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }

      http.end(); // Free resources
    } else {
      Serial.println("WiFi Disconnected");
    }

    if (payload == "soymilk is offline") {
      Serial.println("Soymilk aint on");
      digitalWrite(FEATUREPIN, LOW);
    }
    else {
      Serial.println("Soymilk be on");
      digitalWrite(FEATUREPIN, HIGH);
      if (!isPlaying) {
        audio.connecttoFS(SPIFFS, "/WAKE THE FUCK UP SONG.mp3");
        isPlaying = true;
        delay(200);
      }
      if (isPlaying && !audio.isRunning()) {  // playback finished
        Serial.println("Playback finished");
        isPlaying = false;
      }
    }
  }
}