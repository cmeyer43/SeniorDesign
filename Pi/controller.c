#include "brakeZone.h"
#include "controlFunctions.h"
#include "stateMachine.h"
#include "serial.h"
#include "controlFunctions.h"
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <thread>

unsigned int liftHillSensors[2] = {16,26}; // Lift Hill
brakeZone liftHill(liftHillSensors, ENTERING);
unsigned int rideSensors[2] = {18,19}; // Ride into First Gravity Stop
brakeZone ride(rideSensors, ENTERING);
unsigned int preStationSensors[2] = {20,21}; // Second Gravity Stop
brakeZone preStation(preStationSensors, ENTERING);
unsigned int stationSensors[2] = {22,23}; // Wait on spring to be driven to hill.
brakeZone station(stationSensors, EMPTY);
uint8_t state = STOP;
serial ser("/dev/ttyAMA0");
volatile int active = 1;

void bz1Monitor()
{
    while (active)
    {
        liftHill.updateInternalState();
    }
}

void bz2Monitor()
{
    while (active)
    {
        ride.updateInternalState();
    }
}

void bz3Monitor()
{
    while (active)
    {
        preStation.updateInternalState();
    }
}

void bz4Monitor()
{
    while (active)
    {
        station.updateInternalState();
    }
}

void updateBrakeZoneState()
{
    printf("poll\n");
    liftHill.updateState();
    ride.updateState();
    preStation.updateState();
    station.updateState();
};

void controlRollerCoaster()
{
    int sendToLiftHill = 0;
    updateBrakeZoneState();
    state = ser.updateCoasterState();
    if (state == AUTOMATIC)
    {
        // Lift Hill
        if (liftHill.getState() == EMPTY && station.getState() == ENTERED)
        {
            ser.liftHillForward(255);
        } else if (liftHill.getState() == EMPTY)
        {
            ser.liftHillStop();
        } else if (liftHill.getState() == ENTERING)
        {
            ser.liftHillForward(255);
        } else if (liftHill.getState() == ENTERED && ride.getState() == EMPTY)
        {
            ser.liftHillForward(255);
        } else if (liftHill.getState() == ENTERED && ride.getState() != EMPTY)
        {
            ser.liftHillStop();
        }
        // Station
        if (station.getState() != ENTERED && preStation.getState() == ENTERED)
        {
            ser.stationForward(128);
        } else if (station.getState() == EMPTY)
        {
            ser.stopStation();
        } else if (station.getState() == ENTERING)
        {
            ser.stationForward(128); // Half up to roll in easy.
        } else if (station.getState() == ENTERED && liftHill.getState() == EMPTY)
        {
            ser.stationForward(128);
        } else if (liftHill.getState() != EMPTY)
        {
            ser.stopStation();
        }
        // Pre Station
        if (preStation.getState() == EMPTY && ride.getState() == ENTERED)
        {
            ser.releasePreStation(128); // Half for light release
        } else if (preStation.getState() == EMPTY)
        {
            ser.stopPreStation();
        } else if (preStation.getState() == ENTERING)
        {
            ser.releasePreStation(128); // Half up to roll in easy.
        } else if (preStation.getState() == ENTERED && station.getState() == EMPTY)
        {
            ser.releasePreStation(128);
        } else if (preStation.getState() == ENTERED)
        {
            ser.stopPreStation();
        }
        // Ride
        if (ride.getState() == EMPTY && liftHill.getState() == ENTERED)
        {
            ser.releaseRide(128); // Half for light release
        } else if (ride.getState() == EMPTY)
        {
            ser.stopRide();
        } else if (ride.getState() == ENTERING)
        {
            ser.releaseRide(128); // Half up to roll in easy.
        } else if (ride.getState() == ENTERED && preStation.getState() == EMPTY)
        {
            ser.releaseRide(128);
        } else if (ride.getState() == ENTERED)
        {
            ser.stopRide();
        }
    } else if (state == MANUAL)
    {
        // Lift Hill
        if (liftHill.getState() == EMPTY && station.getState() == ENTERED)
        {
            ser.liftHillForward(255);
        } else if (liftHill.getState() == EMPTY)
        {
            ser.liftHillStop();
        } else if (liftHill.getState() == ENTERING)
        {
            ser.liftHillForward(255);
        } else if (liftHill.getState() == ENTERED && ride.getState() == EMPTY)
        {
            ser.liftHillForward(255);
        } else if (liftHill.getState() == ENTERED && ride.getState() != EMPTY)
        {
            ser.liftHillStop();
        }
        // Station
        if (station.getState() != EMPTY && preStation.getState() == ENTERED)
        {
            ser.stationForward(128);
        } else if (station.getState() == EMPTY)
        {
            ser.stopStation();
        } else if (station.getState() == ENTERING)
        {
            ser.stationForward(128); // Half up to roll in easy.
        } else if (station.getState() == ENTERED && liftHill.getState() == EMPTY)
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
        } else if (liftHill.getState() != EMPTY)
        {
            ser.stopStation();
        }
        // Pre Station
        if (preStation.getState() != EMPTY && ride.getState() == ENTERED)
        {
            ser.releasePreStation(128); // Half for light release
        } else if (preStation.getState() == EMPTY)
        {
            ser.stopPreStation();
        } else if (preStation.getState() == ENTERING)
        {
            ser.releasePreStation(128); // Half up to roll in easy.
        } else if (preStation.getState() == ENTERED && station.getState() == EMPTY)
        {
            ser.releasePreStation(128);
        } else if (preStation.getState() == ENTERED)
        {
            ser.stopPreStation();
        }
        // Ride
        if (ride.getState() == EMPTY && liftHill.getState() == ENTERED)
        {
            ser.releaseRide(128); // Half for light release
        } else if (ride.getState() == EMPTY)
        {
            ser.stopRide();
        } else if (ride.getState() == ENTERING)
        {
            ser.releaseRide(128); // Half up to roll in easy.
        } else if (ride.getState() == ENTERED && preStation.getState() == EMPTY)
        {
            ser.releaseRide(128);
        } else if (ride.getState() == ENTERED)
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
        } else if (action == 2) // Stop
        {
            ser.stopRide();
            ser.stopPreStation();
            ser.stopStation();
            ser.liftHillStop();

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
};

int main()
{
    int err = 0;

    err += liftHill.init();
    err += ride.init();
    err += preStation.init();
    err += station.init();

    if (err != 0)
    {
        return err;
    }

    std::thread coasterThread(controlRollerCoaster);
    std::thread bz1Thread(bz1Monitor);
    std::thread bz2Thread(bz2Monitor);
    std::thread bz3Thread(bz3Monitor);
    std::thread bz4Thread(bz4Monitor);

    coasterThread.join();
    bz1Thread.join();
    bz2Thread.join();
    bz3Thread.join();
    bz4Thread.join();

    return 0;
}

