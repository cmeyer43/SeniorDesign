#include "brakeZone.h"
#include "stateMachine.h"
#include "serial.h"
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <thread>

unsigned int liftHillSensors[2] = {16,17}; // Lift Hill
brakeZone liftHill(liftHillSensors, ENTERED);
unsigned int rideSensors[2] = {18,19}; // Ride into First Gravity Stop
brakeZone ride(rideSensors, ENTERED);
unsigned int waitForStationSensors[2] = {20,21}; // Second Gravity Stop
brakeZone waitForStation(waitForStationSensors, ENTERED);
unsigned int stationSensors[2] = {22,23}; // Wait on spring to be driven to hill.
brakeZone station(stationSensors, EMPTY);
uint8_t state = STOP;

serial ser("/dev/ttyAMA0");

void updateBrakeZoneState()
{
    printf("poll\n");
    liftHill.updateState();
    ride.updateState();
    waitForStation.updateState();
    station.updateState();
}

// If lift hill is empty then we can "start" the ride, move to the lift hill where it will be automated
int sendToLiftHill()
{
    return liftHill.getState() == EMPTY;
}

int stopLiftHill()
{
    return liftHill.getState() == EXITING;
}

controlRollerCoaster()
{
    int sendToLiftHill = 0;
    updateBrakeZoneState();
    state = ser.updateCoasterState();
    if (state == AUTOMATIC)
    {
        // Lift Hill
        if (liftHillEmpty() && inStation())
        {
            ser.liftHillForward(255);
        } else if (liftHillEmpty())
        {
            ser.liftHillStop();
        } else if (enteringLiftHill())
        {
            ser.liftHillForward(255)
        } else if (topLiftHill() && rideFree())
        {
            ser.liftHillForward(255);
        else if (topLiftHill() && !rideFree())
        {
            ser.liftHillStop();
        }
        // Station
        if (!inStation() && inPreStation())
        {
            ser.stationForward(128);
        else if (!inStation())
        {
            ser.stopStation();
        }
        } else if (enteringStation())
        {
            ser.stationForward(128); // Half up to roll in easy.
        } else if (inStation() && liftHillEmpty())
        {
            ser.stationForward(128);
        } else if (!liftHillEmpty())
        {
            ser.stopStation();
        }
        // Pre Station
        if (!inPreStation() && inRide())
        {
            ser.releasePreStation(128); // Half for light release
        else if (!inPreStation())
        {
            ser.stopPreStation();
        }
        } else if (enteringPreStation())
        {
            ser.releasePreStation(128); // Half up to roll in easy.
        } else if (inPreStation() && !inStation())
        {
            ser.releasePreStation(128);
        } else if (inPreStation())
        {
            ser.stopPreStation();
        }
        // Ride
        if (!inRide() && topLiftHill())
        {
            ser.releaseRide(128); // Half for light release
        else if (!inRide())
        {
            ser.stopRide();
        }
        } else if (enteringRide())
        {
            ser.releaseRide(128); // Half up to roll in easy.
        } else if (inRide() && !inPreStation())
        {
            ser.releaseRide(128);
        } else if (inRide())
        {
            ser.stopRide();
        }

    } else if (state == MANUAL)
    {
        // Lift Hill
        if (liftHillEmpty() && inStation())
        {
            ser.liftHillForward(255);
        } else if (liftHillEmpty())
        {
            ser.liftHillStop();
        } else if (enteringLiftHill())
        {
            ser.liftHillForward(255)
        } else if (topLiftHill() && rideFree())
        {
            ser.liftHillForward(255);
        else if (topLiftHill() && !rideFree())
        {
            ser.liftHillStop();
        }
        // Station
        if (!inStation() && inPreStation())
        {
            ser.stationForward(128);
        else if (!inStation())
        {
            ser.stopStation();
        }
        } else if (enteringStation())
        {
            ser.stationForward(128); // Half up to roll in easy.
        } else if (inStation() && liftHillEmpty())
        {
            int action = ser.sendCanSend(1);
            if (action)
            {
                ser.stationForward(128);
                ser.sendCanSend(0);
            } else
            {
                ser.stopStation();
            }
        } else if (!liftHillEmpty())
        {
            ser.stopStation();
        }
        // Pre Station
        if (!inPreStation() && inRide())
        {
            ser.releasePreStation(128); // Half for light release
        else if (!inPreStation())
        {
            ser.stopPreStation();
        }
        } else if (enteringPreStation())
        {
            ser.releasePreStation(128); // Half up to roll in easy.
        } else if (inPreStation() && !inStation())
        {
            ser.releasePreStation(128);
        } else if (inPreStation())
        {
            ser.stopPreStation();
        }
        // Ride
        if (!inRide() && topLiftHill())
        {
            ser.releaseRide(128); // Half for light release
        else if (!inRide())
        {
            ser.stopRide();
        }
        } else if (enteringRide())
        {
            ser.releaseRide(128); // Half up to roll in easy.
        } else if (inRide() && !inPreStation())
        {
            ser.releaseRide(128);
        } else if (inRide())
        {
            ser.stopRide();
        }
    } else if (state == MAINTENANCE)
    {
        int action = requestControl();
        if (action == 1) // Forward
        {
            ser.releaseRide(255);
            ser.releasePreStation(255);
            ser.stationForward(255);
            ser.liftHillForward(255);
        } (action == 2) // Stop
        {
            ser.stopRide();
            ser.stopPreStation();
            ser.stopStation;
            ser.stopLiftHill();

        } else if (action == 3) // Backwards
        {
            ser.releaseRide(255);
            ser.releasePreStation(255);
            ser.stationBackward(255);
            ser.liftHillBackward(255);
        }


    } else if (state == STOP)
    {
        ser.liftHillStop();
        ser.stopRide();
        ser.stopStation();
        ser.stopPreStation();
    }
    printf("%d\n", state);
}

int main()
{
    int err = 0;

    err += liftHill.init();
    err += ride.init();
    err += waitForStation.init();
    err += station.init();

    if (err != 0)
    {
        return err;
    }

    std::thread coasterThread(controlRollerCoaster);

    coasterThread.join();

    return 0;
}
