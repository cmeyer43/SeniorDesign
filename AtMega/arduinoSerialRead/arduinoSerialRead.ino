#include <hardwareSerial.h>
#include "messages.h"
#include "stateMachine.h"
#define SERVO_1 2
#define SERVO_2 3
#define DC_1 4
#define DC_2 5
#define START_BUTTON 17
#define FORWARD_SWITCH 18
#define BACKWARD_SWITCH 19


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
  pinMode(START_BUTTON, INPUT);
  digitalWrite(START_BUTTON, HIGH); // Pull up
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
      msg[0] = SEND_BUTTON_STATE;
      msg[1] = state;
      Serial1.write(msg,2);
      newMsg = 0;
    }
    if (state == MANUAL)
    {
      goto manual;
    } else if (state == AUTOMATIC)
    {
      goto automatic;
    } else if (state == MAINTENANCE)
    { 
      goto maintenance;
    } else 
    {
      goto start;
    }

automatic:
    canSend = 0;
    if (newMsg)
    {
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
    }

    newMsg = 0;
    goto start;

manual:
    if (newMsg)
    {
      if (msg[0] == SEND_CAN_SEND)
      {
        msg[0] = RESPOND_CAN_SEND;
        if (msg[1] == 1)
        {
            // Turn on Green LED
          if (digitalRead(START_BUTTON))
          {
            msg[1] = 1;
          } else
          {
            msg[1] = 0;
          }
        } else
        {
            // Turn off Green LED
          msg[1] = 0;
        }
        Serial1.write(msg, 2);
      }
      else if (msg[0] == CONTROL_SERVO_1)
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
    }

    newMsg = 0;
    goto start;

maintenance:
    if (newMsg)
    {
      if (msg[0] = REQUEST_CONTROL)
      {
        msg[0] = SEND_CONTROL;
        if (digitalRead(FORWARD_SWITCH))
        {
          msg[1] = FORWARD;
        } else if (digitalRead(BACKWARD_SWITCH))
        {
          msg[1] = BACKWARD;
        } else
        {
          msg[1] = NONE;
        }
        Serial1.write(msg, 2);
      }
      else if (msg[0] == CONTROL_SERVO_1)
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
    }

    goto start;
}
