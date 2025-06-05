#include <ESP8266WiFi.h>
#include <espnow.h>

// Receiver MAC Address
uint8_t receiverMac[] = {0x60, 0x01, 0x94, 0x51, 0x2B, 0x70};

const int touchPin = 14; // D5 (GPIO 14) for touch sensor
int lastTouchState = -1; // Track last state to avoid redundant sends

// Data structure to send
typedef struct {
  int touchState;
} Message;

Message msg;

// Callback for send status
void onDataSent(uint8_t *mac_addr, uint8_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == 0 ? "Success" : "Failed");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\nStarting Touch Sensor NodeMCU (Sender)");
  Serial.print("Sender MAC Address: ");
  Serial.println(WiFi.macAddress());

  pinMode(touchPin, INPUT);
  Serial.println("Touch Sensor Initialized on D5 (GPIO 14)");

  // Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Ensure no Wi-Fi interference
  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW Initialization Failed");
    delay(2000);
    ESP.restart();
  }

  // Set role and register peer
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(onDataSent);
  if (esp_now_add_peer(receiverMac, ESP_NOW_ROLE_SLAVE, 1, NULL, 0) != 0) {
    Serial.println("Failed to Add Peer");
    delay(2000);
    ESP.restart();
  }

  Serial.println("ESP-NOW Initialized Successfully");
}

void loop() {
  int touchState = digitalRead(touchPin);
  if (touchState != lastTouchState) {
    msg.touchState = touchState;
    esp_now_send(receiverMac, (uint8_t *)&msg, sizeof(msg));
    Serial.println(touchState == HIGH ? "Touched!" : "Not Touched");
    lastTouchState = touchState;
  }
  delay(100); // Check every 100ms
}