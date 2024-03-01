// ESP32 UDP client sending datagrams to a UDP server

//  from https://github.com/evothings/evothings-examples/blob/master/examples/arduino-led-onoff-tcp/arduinowifi/arduinowifi/arduinowifi.ino

#include <WiFi.h>
#include <WiFiUdp.h>

// network SSID (network name). and network password.
// char ssid[] = "rcsTeam";
// char pass[] = "12345678";
char ssid[] = "TP-Link";
char pass[] = "3588805604";
char msg[100];


// network key Index number (needed only for WEP).
int keyIndex = 0;

// UDP information
//local broadcast address..
IPAddress udpServer(192, 168, 0, 145);
#define UDP_PORT 3000
WiFiUDP udp;
int i = 0;

// UDP Datagram
struct __attribute__((packed)) UDPDatagram {
  uint16_t seq;                          // sequence number
  int16_t sdata;                         // 16 bit integer data
  int32_t idata;                         // 32 bit integer data
  float fdata;                           // float data
  uint16_t crc;                          // crc check
} udpDatagram = { 0, 10, 10, 3.14, 0 };  // initial values

void setup() {
  Serial.begin(115200);
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
}

void loop() {
  // Send Packet to UDP server
  snprintf(msg, 100, "hello world %d", i);
  printf("%s\n", msg);

  udp.beginPacket(udpServer.toString().c_str(), UDP_PORT);
  int len = udp.write((const uint8_t *)msg, sizeof(msg));
  udp.endPacket();
  i++;

  delay(1000);
}