#include <ESP8266WiFi.h>
#include <espnow.h>

const int ledPin = 5; // D1 (GPIO 5) for LED/bulb
bool isRelay = false; // Set to true for active-low relay

// Data structure to receive
typedef struct {
  int touchState;
} Message;

Message msg;

// Callback for received data
void onDataRecv(uint8_t *mac, uint8_t *data, uint8_t len) {
  memcpy(&msg, data, sizeof(msg));
  // Set LED/bulb state (invert for active-low relay)
  digitalWrite(ledPin, isRelay ? !msg.touchState : msg.touchState);
  Serial.println(msg.touchState == HIGH ? "LED/Bulb ON" : "LED/Bulb OFF");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\nStarting LED NodeMCU (Receiver)");
  Serial.print("Receiver MAC Address: ");
  Serial.println(WiFi.macAddress());

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, isRelay ? HIGH : LOW); // Initial state (HIGH for active-low relay)
  Serial.println("LED/Bulb Initialized on D1 (GPIO 5)");

  // Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Ensure no Wi-Fi interference
  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW Initialization Failed");
    delay(2000);
    ESP.restart();
  }

  // Set role and register callback
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(onDataRecv);

  Serial.println("ESP-NOW Initialized Successfully");
}

void loop() {
  delay(100); // Keep loop running
}