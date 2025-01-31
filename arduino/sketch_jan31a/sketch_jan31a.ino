#include <ArduinoBLE.h>
#include <Wire.h>
#include <Arduino_LSM6DSOX.h>

// Define BLE Service and Characteristics
BLEService imuService("180A");  

BLEFloatCharacteristic accelXCharacteristic("2A98", BLERead | BLENotify);
BLEFloatCharacteristic accelYCharacteristic("2A99", BLERead | BLENotify);
BLEFloatCharacteristic accelZCharacteristic("2A9A", BLERead | BLENotify);

BLEFloatCharacteristic gyroXCharacteristic("2A9B", BLERead | BLENotify);
BLEFloatCharacteristic gyroYCharacteristic("2A9C", BLERead | BLENotify);
BLEFloatCharacteristic gyroZCharacteristic("2A9D", BLERead | BLENotify);

void setup() {
    Serial.begin(115200);
    Wire.begin();

    // Initialize IMU
    Serial.println("IMU Initialized."+ IMU.begin());

    // Initialize BLE
    if (!BLE.begin()) {
        Serial.println("Starting Bluetooth failed!");
        while (1);
    }

    // Set up BLE characteristics
    BLE.setLocalName("UNO-R4-IMU");
    BLE.setAdvertisedService(imuService);

    imuService.addCharacteristic(accelXCharacteristic);
    imuService.addCharacteristic(accelYCharacteristic);
    imuService.addCharacteristic(accelZCharacteristic);
    imuService.addCharacteristic(gyroXCharacteristic);
    imuService.addCharacteristic(gyroYCharacteristic);
    imuService.addCharacteristic(gyroZCharacteristic);

    BLE.addService(imuService);
    BLE.advertise();
    Serial.println("BLE Advertising...");
}

void loop() {
    BLEDevice central = BLE.central();

    if (central) {
        Serial.print("Connected to ");
        Serial.println(central.address());

        while (central.connected()) {
            float ax, ay, az, gx, gy, gz;

            // Read acceleration and gyroscope data
            if (IMU.accelerationAvailable()) {
                IMU.readAcceleration(ax, ay, az);
            }

            if (IMU.gyroscopeAvailable()) {
                IMU.readGyroscope(gx, gy, gz);
            }

            // Send raw data over BLE
            accelXCharacteristic.writeValue(ax);
            accelYCharacteristic.writeValue(ay);
            accelZCharacteristic.writeValue(az);

            gyroXCharacteristic.writeValue(gx);
            gyroYCharacteristic.writeValue(gy);
            gyroZCharacteristic.writeValue(gz);

            // Print to Serial Monitor for debugging
            Serial.print("Accel: X=");
            Serial.print(ax, 4);
            Serial.print(", Y=");
            Serial.print(ay, 4);
            Serial.print(", Z=");
            Serial.print(az, 4);
            Serial.print(" | Gyro: X=");
            Serial.print(gx, 4);
            Serial.print(", Y=");
            Serial.print(gy, 4);
            Serial.print(", Z=");
            Serial.println(gz, 4);

            delay(2000);  // 100ms sampling rate
        }
        Serial.println("BLE Disconnected");
    }
}
