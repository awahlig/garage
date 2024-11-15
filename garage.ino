#include <WiFiS3.h>
#include <ArduinoHA.h>

#include "secrets.h"

WiFiClient client;
HADevice device;
HAMqtt mqtt(client, device);

HAButton relay1("relay1");
HAButton relay2("relay2");
HAButton relay3("relay3");

HABinarySensor remote1("remote1");
HABinarySensor remote2("remote2");

void setup() {
  WiFi.setHostname("garage");
  connectToWiFi();

  // GPIO pins
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(REMOTE1_PIN, INPUT_PULLUP);
  pinMode(REMOTE2_PIN, INPUT_PULLUP);

  // HA Device
  uint8_t mac[6];
  WiFi.macAddress(mac);
  device.setUniqueId(mac, sizeof(mac));
  device.enableExtendedUniqueIds();
  device.setManufacturer(DEVICE_MANUFACTURER);
  device.setModel(DEVICE_MODEL);
  device.setSoftwareVersion(DEVICE_VERSION);
  device.setName(DEVICE_NAME);

  // HA Relay buttons
  relay1.setName(RELAY1_NAME);
  relay1.setIcon(RELAY1_ICON);
  relay1.onCommand(onRelay1Pressed);
  relay2.setName(RELAY2_NAME);
  relay2.setIcon(RELAY2_ICON);
  relay2.onCommand(onRelay2Pressed);
  relay3.setName(RELAY3_NAME);
  relay3.setIcon(RELAY3_ICON);
  relay3.onCommand(onRelay3Pressed);

  // HA remote sensors
  remote1.setName(REMOTE1_NAME);
  remote1.setIcon(REMOTE1_ICON);
  remote2.setName(REMOTE2_NAME);
  remote2.setIcon(REMOTE2_ICON);

  mqtt.begin(MQTT_BROKER_ADDR, MQTT_USERNAME, MQTT_PASSWORD);

  Serial.println("Ready!");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  remote1.setState(digitalRead(REMOTE1_PIN) == LOW);
  remote2.setState(digitalRead(REMOTE2_PIN) == LOW);

  mqtt.loop();
}

void connectToWiFi() {
  while (1) {
    Serial.print("Connecting to Wifi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (1) {
      uint8_t status = WiFi.status();
      if (status == WL_CONNECTED) {
        Serial.println("\nConnected!");
        return;
      }
      if (status == WL_CONNECT_FAILED) {
        Serial.println("\nFailed!");
        break;
      }
      delay(500);
      Serial.print(".");
    }
  }
}

void pulseHigh(pin_size_t pin) {
  digitalWrite(pin, HIGH);
  delay(PULSE_DELAY);
  digitalWrite(pin, LOW);
}

void onRelay1Pressed(HAButton* sender) {
  pulseHigh(RELAY1_PIN);
}

void onRelay2Pressed(HAButton* sender) {
  pulseHigh(RELAY2_PIN);
}

void onRelay3Pressed(HAButton* sender) {
  pulseHigh(RELAY3_PIN);
}
