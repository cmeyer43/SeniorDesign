#include <hardwareSerial.h>
#include "messages.h"
#include "stateMachine.h"
#define SERVO_PRESTATION_1 12  // Digiatal 10
#define SERVO_PRESTATION_2 13  // Digital 11 control servo 1
#define SERVO_RIDE_1 10  // Digital 12
#define SERVO_RIDE_2 11  // Digital 13
#define DC_STATION 6     // Digital 6
#define DC_HILL 7     // Digital 7
#define START_BUTTON 33 // Digital 28
#define FORWARD_SWITCH 27
#define BACKWARD_SWITCH 26
#define START_LED 54 // Digital 36
#define DC_A_ENABLE_FORWARD 44 // Digital 44
#define DC_A_ENABLE_BACKWARD 45 // Digital 45
#define DC_B_ENABLE_FORWARD 46 // Digital 46
#define DC_B_ENABLE_BACKWARD 47 // Digital 47
#define DC_C_ENABLE_FORWARD 36 // Digital 48
#define DC_C_ENABLE_BACKWARD 35 // Digital 49
#define DC_D_ENABLE_FORWARD 22 // Digital 50
#define DC_D_ENABLE_BACKWARD 21 // Digital 51


int bytesRead; // for incoming serial data
uint8_t msg[100];
uint8_t newMsg = 0;
uint8_t state;
uint8_t canSend;
int readState();

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600); // opens serial port, sets data rate to 9600 bps
  pinMode(START_BUTTON, INPUT);
  pinMode(SERVO_PRESTATION_1, OUTPUT);
  pinMode(SERVO_PRESTATION_2, OUTPUT);
  pinMode(SERVO_RIDE_1, OUTPUT);
  pinMode(SERVO_RIDE_2, OUTPUT);
  pinMode(DC_STATION, OUTPUT);
  pinMode(DC_HILL, OUTPUT);
  analogWrite(SERVO_PRESTATION_1, 120);
  analogWrite(SERVO_PRESTATION_2, 120);
  analogWrite(SERVO_RIDE_1, 140);
  analogWrite(SERVO_RIDE_2, 140);
  analogWrite(DC_STATION, 0);
  analogWrite(DC_HILL, 0);
  pinMode(START_LED, OUTPUT);
  digitalWrite(START_LED, HIGH);
  pinMode(DC_A_ENABLE_FORWARD, OUTPUT);
  pinMode(DC_A_ENABLE_BACKWARD, OUTPUT);
  pinMode(DC_B_ENABLE_FORWARD, OUTPUT);
  pinMode(DC_B_ENABLE_BACKWARD, OUTPUT);
  pinMode(DC_C_ENABLE_FORWARD, OUTPUT);
  pinMode(DC_C_ENABLE_BACKWARD, OUTPUT);
  pinMode(DC_D_ENABLE_FORWARD, OUTPUT);
  pinMode(DC_D_ENABLE_BACKWARD, OUTPUT);
  digitalWrite(DC_A_ENABLE_FORWARD, LOW);
  digitalWrite(DC_A_ENABLE_BACKWARD, LOW);
  digitalWrite(DC_B_ENABLE_FORWARD, LOW);
  digitalWrite(DC_B_ENABLE_BACKWARD, LOW);
  digitalWrite(DC_C_ENABLE_FORWARD, LOW);
  digitalWrite(DC_C_ENABLE_BACKWARD, LOW);
  digitalWrite(DC_D_ENABLE_FORWARD, LOW);
  digitalWrite(DC_D_ENABLE_BACKWARD, LOW);
  digitalWrite(START_LED, HIGH); // Pull up
  Serial.println("setup V1.1");
  state = MANUAL;
}

int readState()
{
  return AUTOMATIC;
}
// Station
void stationForwardA()
{
  digitalWrite(DC_A_ENABLE_FORWARD, LOW);
  digitalWrite(DC_A_ENABLE_BACKWARD, LOW);
  digitalWrite(DC_A_ENABLE_FORWARD, HIGH);
  digitalWrite(DC_A_ENABLE_BACKWARD, LOW);
  
}
// STATION
void stationBackwardA()
{
  digitalWrite(DC_A_ENABLE_FORWARD, LOW);
  digitalWrite(DC_A_ENABLE_BACKWARD, LOW);
  digitalWrite(DC_A_ENABLE_FORWARD, LOW);
  digitalWrite(DC_A_ENABLE_BACKWARD, HIGH);
}

void hillForwardB()
{
  digitalWrite(DC_B_ENABLE_FORWARD, LOW);
  digitalWrite(DC_B_ENABLE_BACKWARD, LOW);
  digitalWrite(DC_B_ENABLE_FORWARD, LOW);
  digitalWrite(DC_B_ENABLE_BACKWARD, HIGH);
}

void hillBackwardB()
{
  digitalWrite(DC_B_ENABLE_FORWARD, LOW);
  digitalWrite(DC_B_ENABLE_BACKWARD, LOW);
  digitalWrite(DC_B_ENABLE_FORWARD, HIGH);
  digitalWrite(DC_B_ENABLE_BACKWARD, LOW);
}

void setForwardC()
{
  digitalWrite(DC_C_ENABLE_FORWARD, LOW);
  digitalWrite(DC_C_ENABLE_BACKWARD, LOW);
  digitalWrite(DC_C_ENABLE_FORWARD, HIGH);
  digitalWrite(DC_C_ENABLE_BACKWARD, LOW);
}

void setBackwardC()
{
  digitalWrite(DC_C_ENABLE_FORWARD, LOW);
  digitalWrite(DC_C_ENABLE_BACKWARD, LOW);
  digitalWrite(DC_C_ENABLE_FORWARD, LOW);
  digitalWrite(DC_C_ENABLE_BACKWARD, HIGH);
}

void setForwardD()
{
  digitalWrite(DC_D_ENABLE_FORWARD, LOW);
  digitalWrite(DC_D_ENABLE_BACKWARD, LOW);
  digitalWrite(DC_D_ENABLE_FORWARD, HIGH);
  digitalWrite(DC_D_ENABLE_BACKWARD, LOW);
}

void setBackwardD()
{
  digitalWrite(DC_D_ENABLE_FORWARD, LOW);
  digitalWrite(DC_D_ENABLE_BACKWARD, LOW);
  digitalWrite(DC_D_ENABLE_FORWARD, LOW);
  digitalWrite(DC_D_ENABLE_BACKWARD, HIGH);
}
// SERVO
void loop()
{
// test:
//   stationForwardA();
//   analogWrite(DC_STATION, 0);
//   hillForwardB();
//   analogWrite(DC_HILL, 0);
//   analogWrite(SERVO_PRESTATION_1, 120); // control servo 1
//   analogWrite(SERVO_PRESTATION_2, 120);
//   // SERVO_PRESTATION 170 MAX
//   // SERVO_PRESTATION 150 HALF
//   analogWrite(SERVO_RIDE_1, 160);  // control servo 2
//   analogWrite(SERVO_RIDE_2, 160);
//   // SERVO_RIDE
// goto test;
start:
    if (Serial1.available())
    {
      // Serial.print("got message ");
      // Serial.println(Serial1.available());
      memset(msg, 0, 100);
      uint8_t tmp = 0;
      // Serial.print("tmp ");
      bytesRead = Serial1.readBytes(&tmp, 1);
      // Serial.print(tmp);
      // Serial.print(",");
      newMsg = 0;
      if (tmp == 0x7b)
      {
        tmp = 0;
        while (1)
        {
          if (Serial1.available())
          {
            int count = Serial1.readBytes(&msg[tmp], 1);
            if (count)
            {
              // Serial.print(msg[tmp]);
              // Serial.print(",");
              if (msg[tmp] == 0x7d)
              {
                newMsg = 1;
                break;
              }
              tmp++;
              if (tmp > 5)
              {
                break;
              }
            }
          }
        }
        // Serial.println(((uint32_t*)msg)[0]);
      } else
      {
        // Serial.println("bad start byte");
      } 
    } 
    if (newMsg && msg[0] == REQUEST_BUTTON_STATE)
    {
      state = readState();
      // Serial.print("send state ");
      // Serial.println(state);
      msg[0] = SEND_BUTTON_STATE;
      msg[1] = state;
      Serial1.write(msg,2);
      Serial1.flush();
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

      // if (msg[0] == SEND_CAN_SEND)
      // {
      //   msg[0] = RESPOND_CAN_SEND;
      //   if (msg[1] == 1)
      //   {
      //       // Turn on Green LED
      //     digitalWrite(START_LED, HIGH);
      //     if (digitalRead(START_BUTTON))
      //     {
      //       msg[1] = 1;
      //     } else
      //     {
      //       msg[1] = 0;
      //     }
      //   } else
      //   {
      //       // Turn off Green LED
      //     digitalWrite(START_LED, LOW);
      //     msg[1] = 0;
      //   }
      //   Serial1.write(msg, 2);
      //   Serial1.flush();
      // }
      // else 
      if (msg[0] == CONTROL_SERVO_1) // prestation
      {

        analogWrite(SERVO_PRESTATION_1, msg[1]);
        analogWrite(SERVO_PRESTATION_2, msg[1]);

      } else if (msg[0] == CONTROL_SERVO_2)
      {
        analogWrite(SERVO_RIDE_1, msg[1]);
        analogWrite(SERVO_RIDE_2, msg[1]);
      } else if (msg[0] == CONTROL_DC_1)
      {
        stationForwardA();
        analogWrite(DC_STATION, msg[1]);
        
      } else if (msg[0] == CONTROL_DC_2)
      {
        hillForwardB();
        analogWrite(DC_HILL, msg[1]);
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
          digitalWrite(START_LED, HIGH);
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
          digitalWrite(START_LED, LOW);
          msg[1] = 0;
        }
        Serial1.write(msg, 2);
        Serial1.flush();
      }
      else if (msg[0] == CONTROL_SERVO_1) // prestation
      {

        analogWrite(SERVO_PRESTATION_1, msg[1]);
        analogWrite(SERVO_PRESTATION_2, msg[1]);

      } else if (msg[0] == CONTROL_SERVO_2)
      {
        analogWrite(SERVO_RIDE_1, msg[1]);
        analogWrite(SERVO_RIDE_2, msg[1]);
      } else if (msg[0] == CONTROL_DC_1)
      {
        stationForwardA();
        analogWrite(DC_STATION, msg[1]);
        
      } else if (msg[0] == CONTROL_DC_2)
      {
        hillForwardB();
        analogWrite(DC_HILL, msg[1]);
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
        Serial1.flush();
      }
      else if (msg[0] == CONTROL_SERVO_1)
      {
        analogWrite(SERVO_PRESTATION_1, msg[1]);
        analogWrite(SERVO_PRESTATION_2, msg[1]);
      } else if (msg[0] == CONTROL_SERVO_2)
      {
        analogWrite(SERVO_RIDE_1, msg[1]);
        analogWrite(SERVO_RIDE_2, msg[1]);
      } else if (msg[0] == CONTROL_DC_1)
      {
        if (msg[2] = BACKWARD)
        {
          stationBackwardA();
        } else
        {
          stationForwardA();
        }
          analogWrite(DC_STATION, msg[1]);
        
      } else if (msg[0] == CONTROL_DC_2)
      {
        if (msg[2] = BACKWARD)
        {
          hillBackwardB();
        } else
        {
          hillForwardB();
        }
        analogWrite(DC_HILL, msg[1]);
      }
    }

    goto start;
}
