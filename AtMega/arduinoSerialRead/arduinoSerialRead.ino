#include <hardwareSerial.h>
#include "messages.h"
#include "stateMachine.h"
#define SERVO_1 2
#define SERVO_2 3
#define DC_1 4
#define DC_2 5

int bytesRead; // for incoming serial data
uint8_t msg[100];
uint8_t newMsg = 0;
uint8_t state;
uint8_t canSend;
int readState();

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600); // opens serial port, sets data rate to 9600 bps
  pinMode(SERVO_1, OUTPUT);
  pinMode(SERVO_2, OUTPUT);
  pinMode(DC_1, OUTPUT);
  pinMode(DC_2, OUTPUT);
}

int readState()
{
  return MANUAL;
}

void loop()
{
start:
    state = readState();
    if (Serial1.available() > 5)
    {
      bytesRead = Serial1.readBytes(msg, 100);
      newMsg = bytesRead ? 1 : 0;
    } if (msg[0] == REQUEST_BUTTON_STATE)
    {
      msg[0] = state;
      Serial1.write(msg,1);
      newMsg = 0;
    }
    if (state == MANUAL)
    {
      goto manual;
    } else if (state == AUTOMATIC)
    {
      goto automatic;
    } 
automatic:
    canSend = 0;
    if (newMsg == 0)
    {
      goto start;
    }
    if (msg[0] == CONTROL_SERVO_1)
    {
      analogWrite(SERVO_1, msg[1]);
    } else if (msg[0] == CONTROL_SERVO_2)
    {
      analogWrite(SERVO_2, msg[1]);
    } else if (msg[0] == CONTROL_DC_1)
    {
      analogWrite(DC_1, msg[1]);
      
    } else if (msg[0] == CONTROL_DC_2)
    {
      analogWrite(DC_1, msg[2]);
    }
    newMsg = 0;
    goto start;
manual:
    if (newMsg)
    {
      if (msg[0] == SEND_CAN_SEND)
      {

      }
    }

    newMsg = 0;
    goto start;
}
