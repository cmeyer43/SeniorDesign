#include <hardwareSerial.h>
int incomingByte = 0; // for incoming serial data
                      //
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600); // opens serial port, sets data rate to 9600 bps
}

void loop()
{
    // Serial1.write(2);
    // Serial.print("sent\n");
// send data only when you receive data:
    if (Serial1.available() > 5)
    {
        // read the incoming byte:
        incomingByte = Serial1.read();

        // say what you got:
        Serial.print("I received: ");
        Serial.println(incomingByte, DEC);
    }
}
