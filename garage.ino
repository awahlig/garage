#include <WiFiNINA.h>
#include <ArduinoHA.h>

#include "secrets.h"

WiFiClient client;
HADevice device;
HAMqtt mqtt(client, device);

HAButton lift1("lift1");
HAButton lift2("lift2");
HABinarySensor homeLink1("homeLink1");
HABinarySensor homeLink2("homeLink2");

void setup() {
  WiFi.setHostname("garage");

  // GPIO pins
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);

  // HA Device
  uint8_t mac[6];
  WiFi.macAddress(mac);
  device.setUniqueId(mac, sizeof(mac));
  device.enableExtendedUniqueIds();
  device.setName("Garage");
  device.setSoftwareVersion("1.0.1");
  device.setManufacturer("Arkadiusz Wahlig");
  device.setModel("G1");

  // HA Lift buttons
  lift1.setName("Lift Enter");
  lift1.setIcon("mdi:garage-variant");
  lift1.onCommand(onLift1Pressed);
  lift2.setName("Lift Leave");
  lift2.setIcon("mdi:arrow-down");
  lift2.onCommand(onLift2Pressed);

  // HA HomeLink sensors
  homeLink1.setName("HomeLink A");
  homeLink1.setIcon("mdi:alpha-a-circle");
  homeLink2.setName("HomeLink B");
  homeLink2.setIcon("mdi:alpha-b-circle");

  connectToWiFi();

  mqtt.begin(MQTT_BROKER_ADDR, MQTT_USERNAME, MQTT_PASSWORD);

  Serial.println("Ready!");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  homeLink1.setState(digitalRead(A1) == LOW);
  homeLink2.setState(digitalRead(A2) == LOW);

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
  delay(250);
  digitalWrite(pin, LOW);
}

void onLift1Pressed(HAButton* sender) {
  pulseHigh(1);
}

void onLift2Pressed(HAButton* sender) {
  pulseHigh(2);
}
