// ESP32 UDP client sending datagrams to a UDP server

//  from https://github.com/evothings/evothings-examples/blob/master/examples/arduino-led-onoff-tcp/arduinowifi/arduinowifi/arduinowifi.ino

#include <WiFi.h>
#include <WiFiUdp.h>
#include "Wire.h" // This library allows you to communicate with I2C devices.


// network SSID (network name). and network password.
char ssid[] = "rcsTeam";
char pass[] = "12345678";
//char ssid[] = "TP-Link";
//char pass[] = "3588805604";
char msg[100];

void readIMU();
char* convert_int16_to_str(int16_t i);
// network key Index number (needed only for WEP).
int keyIndex = 0;

// UDP information
//local broadcast address..
IPAddress udpServer(192, 168, 0, 145);
#define UDP_PORT 3000
WiFiUDP udp;
int i = 0;
const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.

int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
int16_t temperature; // variables for temperature data

char tmp_str[7]; // temporary variable used in convert function
uint16_t dataPacket[6];

// UDP Datagram
struct __attribute__((packed)) UDPDatagram {
  uint16_t seq;                          // sequence number
  int16_t sdata;                         // 16 bit integer data
  int32_t idata;                         // 32 bit integer data
  float fdata;                           // float data
  uint16_t crc;                          // crc check
} udpDatagram = { 0, 10, 10, 3.14, 0 };  // initial values

void setup() {
  Serial.begin(9600);
  // Wait for serial port to connect. Needed for Leonardo only
  while (!Serial)
    ;
  delay(1000);
  Serial.println();
  Serial.println("ESP32 WiFi UDP client - send UDP datagrams to server");
  WiFi.mode(WIFI_STA);  // Connect to Wifi network.
  WiFi.begin(ssid, pass);
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nserver IP address: ");
  Serial.println(udpServer);
  udp.begin(UDP_PORT);
  Serial.print("server udp port ");
  Serial.println(UDP_PORT);
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void loop() {
  // Send Packet to UDP server
  readIMU();
  dataPacket[0] = accelerometer_x;
  dataPacket[1] = accelerometer_y;
  dataPacket[2] = accelerometer_z;
  dataPacket[3] = gyro_x;
  dataPacket[4] = gyro_y;
  dataPacket[5] = gyro_z;

  snprintf(msg, 100, "hello world %d", i);
  printf("%s\n", msg);

  udp.beginPacket(udpServer.toString().c_str(), UDP_PORT);
  int len = udp.write((const uint8_t *)dataPacket, 12);
  udp.endPacket();
  i++;

  delay(1000);
}


char* convert_int16_to_str(int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

void readIMU()
{
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7*2, true); // request a total of 7*2=14 registers
  
  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  //serial.print(Wire.read());
  accelerometer_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accelerometer_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  accelerometer_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  temperature = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  gyro_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gyro_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gyro_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)
  
  // print out data
  Serial.print("aX = "); Serial.print(convert_int16_to_str(accelerometer_x));
  Serial.print(" | aY = "); Serial.print(convert_int16_to_str(accelerometer_y));
  Serial.print(" | aZ = "); Serial.print(convert_int16_to_str(accelerometer_z));
  // the following equation was taken from the documentation [MPU-6000/MPU-6050 Register Map and Description, p.30]
  Serial.print(" | tmp = "); Serial.print(temperature/340.00+36.53);
  Serial.print(" | gX = "); Serial.print(convert_int16_to_str(gyro_x));
  Serial.print(" | gY = "); Serial.print(convert_int16_to_str(gyro_y));
  Serial.print(" | gZ = "); Serial.print(convert_int16_to_str(gyro_z));
  Serial.println();
  
}