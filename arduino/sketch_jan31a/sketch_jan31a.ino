#include <ArduinoBLE.h>
#include <Wire.h>
#include <Arduino_LSM6DSOX.h>

// Define BLE Service UUID (must be a valid 128-bit UUID)
#define SERVICE_UUID "12345678-1234-5678-1234-56789abcdef0"

// Define BLE Characteristic UUIDs
#define ACCEL_X_UUID "00002A98-0000-1000-8000-00805f9b34fb"
#define ACCEL_Y_UUID "00002A99-0000-1000-8000-00805f9b34fb"
#define ACCEL_Z_UUID "00002A9A-0000-1000-8000-00805f9b34fb"

#define GYRO_X_UUID "00002A9B-0000-1000-8000-00805f9b34fb"
#define GYRO_Y_UUID "00002A9C-0000-1000-8000-00805f9b34fb"
#define GYRO_Z_UUID "00002A9D-0000-1000-8000-00805f9b34fb"

// Create BLE service
BLEService imuService(SERVICE_UUID);

// Create BLE characteristics
BLEFloatCharacteristic accelXCharacteristic(ACCEL_X_UUID, BLERead | BLENotify);
BLEFloatCharacteristic accelYCharacteristic(ACCEL_Y_UUID, BLERead | BLENotify);
BLEFloatCharacteristic accelZCharacteristic(ACCEL_Z_UUID, BLERead | BLENotify);

BLEFloatCharacteristic gyroXCharacteristic(GYRO_X_UUID, BLERead | BLENotify);
BLEFloatCharacteristic gyroYCharacteristic(GYRO_Y_UUID, BLERead | BLENotify);
BLEFloatCharacteristic gyroZCharacteristic(GYRO_Z_UUID, BLERead | BLENotify);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize IMU
  IMU.begin();
  Serial.println("IMU initialization attempted.");

  // Check if IMU is returning valid values
  float ax, ay, az;
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(ax, ay, az);
    Serial.print("IMU Check: Accel X=");
    Serial.print(ax);
    Serial.print(" Y=");
    Serial.print(ay);
    Serial.print(" Z=");
    Serial.println(az);
  } else {
    Serial.println("IMU acceleration not available. Check wiring.");
  }

  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("Starting Bluetooth failed!");
    while (1)
      ;
  }

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
            float ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0;

            // Read acceleration and gyroscope data - force reading
            if (IMU.accelerationAvailable()) {
                IMU.readAcceleration(ax, ay, az);
            } else {
                Serial.println("IMU Acceleration data not available!");
            }

            if (IMU.gyroscopeAvailable()) {
                IMU.readGyroscope(gx, gy, gz);
            } else {
                Serial.println("IMU Gyroscope data not available!");
            }

            // Send data over BLE
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
if (millis() % 10000 < 100) {
        resetIMU();
      }
            delay(200);  // Shorter delay for smoother updates
        }
        Serial.println("BLE Disconnected");
    }
}


void resetIMU() {
  Serial.println("Resetting IMU...");
  IMU.begin();
}
