#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

typedef struct accel_t {
  float x;
  float y;
  float z;
};

const int accel_union_size = sizeof(accel_t);

typedef struct gyro_t {
  float x;
  float y;
  float z;
};

const int gyro_union_size = sizeof(gyro_t);

typedef struct mag_t {
  float x;
  float y;
  float z;
};
const int mag_union_size = sizeof(mag_t);

typedef union accelPacket_t {
  accel_t structure;
  byte byteArray[accel_union_size];
};

typedef union gyroPacket_t {
  gyro_t structure;
  byte byteArray[gyro_union_size];
};

typedef union magPacket_t {
  mag_t structure;
  byte byteArray[mag_union_size];
};

accelPacket_t accel;
gyroPacket_t gyro;
magPacket_t mag;

BLEService IMUService("19B10010-E8F2-537E-4F6C-D104768A1214");  // create service

// create switch characteristic and allow remote device to read and write
BLECharacteristic accelChar("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, accel_union_size);
BLECharacteristic gyroChar("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, gyro_union_size);
BLECharacteristic magChar("19B10013-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, mag_union_size);

void setup() {
  Serial.begin(9600);
  while (!Serial)
    Serial.println("Started");

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1)
      ;
  }

  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1)
      ;
  }



  // set the local name peripheral advertises
  BLE.setLocalName("IMUService");
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(IMUService);

  // add the characteristics to the service
  IMUService.addCharacteristic(accelChar);
  IMUService.addCharacteristic(gyroChar);
  IMUService.addCharacteristic(magChar);

  // add the service
  BLE.addService(IMUService);

  accel.structure.x = 0.0;
  accel.structure.y = 0.0;
  accel.structure.z = 0.0;

  gyro.structure.x = 0.0;
  gyro.structure.y = 0.0;
  gyro.structure.z = 0.0;

  mag.structure.x = 0.0;
  mag.structure.y = 0.0;
  mag.structure.z = 0.0;

  accelChar.writeValue(accel.byteArray, accel_union_size);
  gyroChar.writeValue(gyro.byteArray, gyro_union_size);
  magChar.writeValue(mag.byteArray, mag_union_size);

  // start advertising
  BLE.advertise();
}

void loop() {
  BLE.poll();

  float x, y, z;

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);

    accel.structure.x = x;
    accel.structure.y = y;
    accel.structure.z = z;

    accelChar.writeValue(accel.byteArray, accel_union_size);
  }

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(x, y, z);

    gyro.structure.x = x;
    gyro.structure.y = y;
    gyro.structure.z = z;

    gyroChar.writeValue(gyro.byteArray, gyro_union_size);
  }

  if (IMU.magneticFieldAvailable()) {
    IMU.readMagneticField(x, y, z);

    mag.structure.x = x;
    mag.structure.y = y;
    mag.structure.z = z;

    magChar.writeValue(mag.byteArray, mag_union_size);
  }
}