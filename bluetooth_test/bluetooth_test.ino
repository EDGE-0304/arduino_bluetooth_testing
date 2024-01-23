#include <ArduinoBLE.h>

// Define the LED pin
const int LED_PIN = LED_BUILTIN; // The built-in LED on your Arduino Nano 33 IoT

// BLE Service and Characteristic
BLEService ledService("991cbd4a-2e1b-4182-bc44-ca34489f6324"); // Custom UUID
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

void setup() {
  pinMode(LED_PIN, OUTPUT);

  // Begin BLE initialization
  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE!");
    while (1);
  }

  // Set BLE local name and service
  BLE.setLocalName("ArduinoNano33IoT");
  BLE.setAdvertisedService(ledService);

  // Add characteristic to service
  ledService.addCharacteristic(switchCharacteristic);

  // Add service
  BLE.addService(ledService);

  // Start advertising
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  // Check for BLE connections
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    // Blink LED while connected
    while (central.connected()) {
      digitalWrite(LED_PIN, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
      delay(500);
    }

    Serial.println("Disconnected from central");
  }
}
