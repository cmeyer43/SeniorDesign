#include <hardwareSerial.h>
#include <Ticker.h>
#include "messages.h"
#include "stateMachine.h"
#define SERVO_PRESTATION_1 12  // Digiatal 10
#define SERVO_PRESTATION_2 13  // Digital 11 control servo 1
#define SERVO_RIDE_1 10  // Digital 12
#define SERVO_RIDE_2 11  // Digital 13
#define DC_STATION 6     // Digital 6
#define DC_HILL 7     // Digital 7
#define DC_A_ENABLE_FORWARD 44 // Digital 44
#define DC_A_ENABLE_BACKWARD 45 // Digital 45
#define DC_B_ENABLE_FORWARD 46 // Digital 46
#define DC_B_ENABLE_BACKWARD 47 // Digital 47

#define ESTOP_LED 43 
#define IGNITION_LED 42
#define IGNITION_SWITCH 34 // NOT WORKING
#define ACKNOWLEDGE_LED 41
#define ACKNOWLEDGE_BUTTON 33
#define LIFT_START_LED 36
#define LIFT_START_BUTTON 28
#define RIDE_START_LED 37
#define RIDE_START_BUTTON 29
#define RIDE_STOP_LED 38
#define RIDE_STOP_BUTTON 30
#define LIFT_STOP_LED 39
#define LIFT_STOP_BUTTON 31
#define RESET_LED 40
#define RESET_BUTTON 32
#define ESTOP 35
#define HILL_SWITCH 26
#define STATION_SWITCH 27
#define MANUAL_MODE 22
#define MAINTAINCE_MODE 25

int bytesRead; // for incoming serial data
uint8_t msg[100];
uint8_t newMsg = 0;
uint8_t state;
uint8_t canSend;
uint8_t toggleState = 0;
int checkoutBlinking = 0;
int returnToSafe = 1;
int toggleStartBlinking = 0;
int liftMoving = 0;

void toggleCheckoutLeds();
void turnOffLeds();
void turnOnLeds();
void toggleStartLeds();
void toggleEmergencyLeds();
Ticker ledToggle(toggleCheckoutLeds, 1000);
Ticker toggleStart(toggleStartLeds, 1000);
Ticker toggleEmergency(toggleEmergencyLeds, 1000);

int readState();

void setup() {

  Serial.begin(9600);
  Serial1.begin(9600); // opens serial port, sets data rate to 9600 bps
  ledToggle.start();
  ledToggle.pause();  
  toggleStart.start();
  toggleStart.pause();
  toggleEmergency.start();
  toggleEmergency.pause();
  pinMode(IGNITION_SWITCH, INPUT);
  pinMode(ESTOP_LED, OUTPUT);
  pinMode(IGNITION_LED, OUTPUT);
  digitalWrite(ESTOP_LED, HIGH);
  digitalWrite(IGNITION_LED, HIGH);
  pinMode(ACKNOWLEDGE_LED, OUTPUT);
  digitalWrite(ACKNOWLEDGE_LED, HIGH);
  pinMode(LIFT_START_BUTTON, INPUT);
  pinMode(LIFT_START_LED, OUTPUT);
  digitalWrite(LIFT_START_LED, HIGH);
  pinMode(RIDE_START_BUTTON, INPUT);
  pinMode(RIDE_START_LED, OUTPUT);
  digitalWrite(RIDE_START_LED, HIGH);
  pinMode(RIDE_STOP_BUTTON, INPUT);
  pinMode(RIDE_STOP_LED, OUTPUT);
  digitalWrite(RIDE_STOP_LED, HIGH);
  pinMode(LIFT_STOP_BUTTON, INPUT);
  pinMode(LIFT_STOP_LED, OUTPUT);
  digitalWrite(LIFT_STOP_LED, HIGH);
  pinMode(RESET_BUTTON, INPUT);
  pinMode(RESET_LED, OUTPUT);
  digitalWrite(RESET_LED, HIGH);
  pinMode(ESTOP, INPUT);
  pinMode(HILL_SWITCH, INPUT);
  pinMode(STATION_SWITCH, INPUT);
  pinMode(MANUAL_MODE, INPUT);
  pinMode(MAINTAINCE_MODE, INPUT);
  pinMode(RIDE_START_BUTTON, INPUT);
  pinMode(SERVO_PRESTATION_1, OUTPUT);
  pinMode(SERVO_PRESTATION_2, OUTPUT);
  pinMode(SERVO_RIDE_1, OUTPUT);
  pinMode(SERVO_RIDE_2, OUTPUT);
  pinMode(DC_STATION, OUTPUT);
  pinMode(DC_HILL, OUTPUT);
  analogWrite(SERVO_PRESTATION_1, 130);
  analogWrite(SERVO_PRESTATION_2, 130);
  analogWrite(SERVO_RIDE_1, 160);
  analogWrite(SERVO_RIDE_2, 160);
  analogWrite(DC_STATION, 0);
  analogWrite(DC_HILL, 0);
  pinMode(RIDE_START_LED, OUTPUT);
  pinMode(DC_A_ENABLE_FORWARD, OUTPUT);
  pinMode(DC_A_ENABLE_BACKWARD, OUTPUT);
  pinMode(DC_B_ENABLE_FORWARD, OUTPUT);
  pinMode(DC_B_ENABLE_BACKWARD, OUTPUT);
  digitalWrite(DC_A_ENABLE_FORWARD, LOW);
  digitalWrite(DC_A_ENABLE_BACKWARD, LOW);
  digitalWrite(DC_B_ENABLE_FORWARD, LOW);
  digitalWrite(DC_B_ENABLE_BACKWARD, LOW);
  turnOffLeds();
  // Serial.println("setup V1.1");
}

void turnOffLeds()
{
  digitalWrite(ESTOP_LED, LOW);
  digitalWrite(IGNITION_LED, LOW);
  digitalWrite(ACKNOWLEDGE_LED, LOW);
  digitalWrite(LIFT_START_LED, LOW);
  digitalWrite(RIDE_START_LED, LOW);
  digitalWrite(RIDE_STOP_LED, LOW);
  digitalWrite(LIFT_STOP_LED, LOW);
  digitalWrite(RESET_LED, LOW);
}

void turnOnLeds()
{
  digitalWrite(ESTOP_LED, LOW);
  digitalWrite(IGNITION_LED, LOW);
  digitalWrite(ACKNOWLEDGE_LED, LOW);
  digitalWrite(LIFT_START_LED, LOW);
  digitalWrite(RIDE_START_LED, LOW);
  digitalWrite(RIDE_STOP_LED, LOW);
  digitalWrite(LIFT_STOP_LED, LOW);
  digitalWrite(RESET_LED, LOW);
}

void toggleCheckoutLeds()
{
  if (toggleState)
  {
    digitalWrite(ESTOP_LED, HIGH);
    digitalWrite(ACKNOWLEDGE_LED, LOW);
    digitalWrite(LIFT_START_LED, HIGH);
    digitalWrite(RIDE_START_LED, HIGH);
    digitalWrite(RIDE_STOP_LED, HIGH);
    digitalWrite(LIFT_STOP_LED, HIGH);
    digitalWrite(RESET_LED, HIGH);
  } else
  {
    digitalWrite(ESTOP_LED, LOW);
    digitalWrite(ACKNOWLEDGE_LED, HIGH);
    digitalWrite(LIFT_START_LED, LOW);
    digitalWrite(RIDE_START_LED, LOW);
    digitalWrite(RIDE_STOP_LED, LOW);
    digitalWrite(LIFT_STOP_LED, LOW);
    digitalWrite(RESET_LED, LOW);
  }
  toggleState = !toggleState;
}

void toggleStartLeds()
{
  if (toggleState)
  {
    digitalWrite(RIDE_START_LED, HIGH);
  } else
  {
    digitalWrite(RIDE_START_LED, LOW);
  }
  toggleState = !toggleState;
}


void checkout()
{
  ledToggle.update();
  if (!digitalRead(IGNITION_SWITCH))
  {
    ledToggle.pause();
    checkoutBlinking = 0;
    turnOffLeds();
    return;
  }
  digitalWrite(IGNITION_LED, HIGH);
  if (!checkoutBlinking)
  {
    ledToggle.resume();
    checkoutBlinking = 1;
  }
  if (digitalRead(ACKNOWLEDGE_BUTTON))
  {
    ledToggle.pause();
    turnOffLeds();
    returnToSafe = 0;
    checkoutBlinking = 0;
    digitalWrite(ACKNOWLEDGE_LED, LOW);
    return;
  }
}

void toggleEmergencyLeds()
{
  if (toggleState)
  {
    digitalWrite(RESET_LED, HIGH);
  } else
  {
    digitalWrite(RESET_LED, LOW);
  }
  toggleState = !toggleState;
}
int emergencyStopBlinking = 0;
int recoverEstop = 0;
void emergencyStop()
{
  digitalWrite(ESTOP_LED, HIGH);
  toggleEmergency.update();
  if (digitalRead(ESTOP))
  {
    toggleEmergency.pause();
    emergencyStopBlinking = 0;
    digitalWrite(ESTOP_LED, HIGH);
    return;
  }
  if (!digitalRead(ESTOP))
  {
    if (!emergencyStopBlinking)
    {
      toggleEmergency.resume();
      emergencyStopBlinking = 1;
    }
    if (digitalRead(RESET_BUTTON))
    {
      toggleEmergency.pause();
      digitalWrite(ESTOP_LED, LOW);
      digitalWrite(RESET_LED, LOW);
      recoverEstop = 0;
      emergencyStopBlinking = 0;
      return;
    }
  }
}

int readState()
{
  if (returnToSafe)
  {
    return RETURN_TO_SAFE;
  }
  if (recoverEstop)
  {
    return STOP;
  }
  if (!digitalRead(IGNITION_SWITCH))
  {
    returnToSafe = 1;
    return RETURN_TO_SAFE;
  }
  if (digitalRead(ESTOP))
  {
    recoverEstop = 1;
    return STOP;
  } else if (digitalRead(MAINTAINCE_MODE))
  {
    return MAINTENANCE;
  } else if (digitalRead(MANUAL_MODE))
  {
    return MANUAL;
  } else
  {
    return AUTOMATIC;
  }
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

void loop()
{
// test:
//   toggleStart.update();
//   // digitalWrite(ESTOP_LED, digitalRead(IGNITION_SWITCH));
//   // Serial.println(digitalRead(ESTOP));
//   stationForwardA();
//   analogWrite(DC_STATION, 255);
//   hillForwardB();
//   analogWrite(DC_HILL, 255);
//   if (digitalRead(ESTOP))
//   {
//     emergencyStop();
//   }
//   analogWrite(SERVO_PRESTATION_1, 120); // control servo 1
//   analogWrite(SERVO_PRESTATION_2, 120);
//   // SERVO_PRESTATION 170 MAX
//   // SERVO_PRESTATION 150 HALF
//   analogWrite(SERVO_RIDE_1, 160);  // control servo 2
//   analogWrite(SERVO_RIDE_2, 160);
//   // SERVO_RIDE
// goto test;
start:
    digitalWrite(IGNITION_LED, digitalRead(IGNITION_SWITCH));
    state = readState();
    if (Serial1.available())
    {
      memset(msg, 0, 100);
      uint8_t tmp = 0;
      bytesRead = Serial1.readBytes(&tmp, 1);
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
      }
    }
    if (newMsg && msg[0] == REQUEST_BUTTON_STATE)
    {
      msg[0] = SEND_BUTTON_STATE;
      msg[1] = state;
      Serial1.write(msg,2);
      Serial1.flush();
      newMsg = 0;
    }
    if (state == RETURN_TO_SAFE)
    {
      goto getSafe;
    } else if (state == STOP)
    {
      goto stop;
    }
    else if (state == MANUAL)
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
          toggleStart.update();
            // Turn on Green LED
          digitalWrite(RIDE_START_LED, HIGH);
          if (!toggleStartBlinking)
          {
            toggleStartBlinking = 1;
            toggleStart.resume();
          }
          if (digitalRead(RIDE_START_BUTTON))
          {
            msg[1] = 1;
            toggleStart.pause();
            toggleStartBlinking = 0;

          } else
          {
            msg[1] = 0;
          }
        } else
        {
          // Turn off Green LED
          digitalWrite(RIDE_START_LED, LOW);
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
      newMsg = 0;
    }
    analogWrite(SERVO_RIDE_1, 220);
    analogWrite(SERVO_RIDE_2, 220);
    analogWrite(SERVO_PRESTATION_1, 120);
    analogWrite(SERVO_PRESTATION_2, 120);
    if (digitalRead(LIFT_STOP_BUTTON))
    {
      if (digitalRead(STATION_SWITCH))
      {
        analogWrite(DC_STATION, 0);
      } else if (digitalRead(HILL_SWITCH))
      {
        analogWrite(DC_HILL, 0);
      }
    }
    if (digitalRead(LIFT_START_BUTTON))
    {
      if (digitalRead(STATION_SWITCH))
      {
        stationForwardA();
        analogWrite(DC_STATION, 255);
      } else if (digitalRead(HILL_SWITCH))
      {
        hillForwardB();
        analogWrite(DC_HILL, 255);
      }
    }


    goto start;
getSafe:
    canSend = 0;
    if (newMsg)
    {
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
    checkout();
    if (digitalRead(ESTOP))
    {
      recoverEstop = 1;
    }
    goto start;
stop:
    canSend = 0;
    if (newMsg)
    {
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
    emergencyStop();
    if (!digitalRead(IGNITION_SWITCH))
    {
      returnToSafe = 1;
    }
    goto start;
}
