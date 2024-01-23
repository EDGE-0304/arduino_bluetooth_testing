#include <ArduinoBLE.h>
#include <WiFiNINA.h>

// Define the LED pin
const int LED_PIN = LED_BUILTIN;

// BLE Service and Characteristic
BLEService wifiService("991cbd4a-2e1b-4182-bc44-ca34489f6324"); // Custom UUID for WiFi service
BLECharacteristic wifiSSIDCharacteristic("abcd1234-ab12-cd34-ef56-abcdef123456", BLERead | BLEWrite, 32); // for SSID
BLECharacteristic wifiPasswordCharacteristic("abcd5678-ab12-cd34-ef56-abcdef123457", BLERead | BLEWrite, 32); // for Password

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);

  // Check for the presence of the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  // Begin BLE initialization
  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE!");
    while (1);
  }

  // Set BLE local name and service
  BLE.setLocalName("ArduinoNano33IoT");
  BLE.setAdvertisedService(wifiService);

  // Add characteristics to service
  wifiService.addCharacteristic(wifiSSIDCharacteristic);
  wifiService.addCharacteristic(wifiPasswordCharacteristic);

  // Add service
  BLE.addService(wifiService);

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

    // Once connected, stay connected until the central disconnects
    while (central.connected()) {
      if (wifiSSIDCharacteristic.written() && wifiPasswordCharacteristic.written()) {
        // Receive WiFi credentials
        int ssidLength = wifiSSIDCharacteristic.valueLength();
        int passwordLength = wifiPasswordCharacteristic.valueLength();

        char ssid[ssidLength + 1];
        char password[passwordLength + 1];

        memcpy(ssid, wifiSSIDCharacteristic.value(), ssidLength);
        ssid[ssidLength] = '\0';

        memcpy(password, wifiPasswordCharacteristic.value(), passwordLength);
        password[passwordLength] = '\0';

        // Debugging: Print received credentials
        Serial.print("Received WiFi credentials: SSID: ");
        Serial.print(ssid);
        Serial.print(", Password: ");
        Serial.println(password);

        // Restart WiFi module
        WiFi.end();
        delay(1000); // Short delay before reconnecting

        // Attempt to connect to WiFi network
        Serial.println("Connecting to WiFi...");
        WiFi.begin(ssid, password);

        int status = WiFi.status();
        while (status != WL_CONNECTED) {
          delay(1000);
          Serial.print(".");
          status = WiFi.status();
        }

        if (status == WL_CONNECTED) {
          Serial.println("Connected to WiFi!");
        } else {
          Serial.println("Failed to connect to WiFi. Check credentials and signal.");
        }
        
        // Reset the written flag, so we don't reprocess the same credentials
        wifiSSIDCharacteristic.writeValue((uint8_t*)ssid, ssidLength);
        wifiPasswordCharacteristic.writeValue((uint8_t*)password, passwordLength);

        // Do something with the WiFi connection here
        // ...

      }

      // Blink LED while connected
      digitalWrite(LED_PIN, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
      delay(500);
    }

    Serial.println("Disconnected from central");
  }
}
