/*
 *  Toilet occupation notifier with PIR sensor.
 *
 *  It sends notification to HipChat when PIR sensor detects motion.
 *  To make the program work, you need to add wifi ssid, password, hipchat room ID and token.
 *
 *  Written by Sean Kang
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid     = "REPLACE_WITH_SSID";
const char* password = "REPLACE_WITH_PASSWORD";
int vacancyThreshold = 180;
int pirState = LOW;
boolean isVacant = true;
unsigned int vacancyCounter = 0;

void setup() {
  Serial.begin(9600);
  delay(10);

  // connect to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // set up PIR sensor input and LED output
  pinMode(D5, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);
}

void sendNotification(const char* postData) {
  HTTPClient http;
  http.begin("http://api.hipchat.com/v2/room/REPLACE_WITH_ROOM_ID/notification");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer REPLACE_WITH_TOKEN");
  int result = http.POST(postData);
  // Serial.println(http.errorToString(result));
  http.end();
}

void loop() {
  int detected = digitalRead(D5);

  if (detected == HIGH) {
    vacancyCounter = 0;
    if (isVacant == true) {
      sendNotification("{\"message_format\":\"text\",\"message\":\"(failed) occupied\"}");
      isVacant = false;

      if (pirState == LOW) {
        digitalWrite(BUILTIN_LED, LOW);
        pirState = HIGH;
      }
    }
  } else {
    vacancyCounter++;
    if (vacancyCounter > vacancyThreshold && isVacant == false) {
      sendNotification("{\"message_format\":\"text\",\"color\":\"green\",\"message\":\"(successful) vacant\"}");
      vacancyCounter = vacancyThreshold;
      isVacant = true;

      if (pirState == HIGH) {
        digitalWrite(BUILTIN_LED, HIGH);
        pirState = LOW;
      }
    }
  }
  delay(1000);
}
