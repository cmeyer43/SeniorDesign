#include "brakeZone.h"
#include "controlFunctions.h"
#include "stateMachine.h"
#include "serial.h"
#include "controlFunctions.h"
#include "udpRecieve.h"
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <thread>

unsigned int liftHillSensors[2] = {22,27}; // Lift Hill
brakeZone liftHill(liftHillSensors, ENTERING);
unsigned int rideSensors[2] = {17,25}; // Ride into First Gravity Stop
//brakeZone ride(rideSensors, ENTERING);
brakeZone ride(rideSensors, EMPTY);
unsigned int preStationSensors[2] = {12,24}; // Second Gravity Stop
//brakeZone preStation(preStationSensors, ENTERING);
brakeZone preStation(preStationSensors, EMPTY);
unsigned int stationSensors[2] = {23,7}; // Wait on spring to be driven to hill.
brakeZone station(stationSensors, EMPTY);
uint8_t state = STOP;
uint8_t release_go = 0;
serial ser("/dev/ttyAMA0");
volatile int active = 1;

void handleWifi()
{
    udpRecieve reciever;
    char msg[1000];
    //char* msgPtr = msg;
    while (active)
    {
        reciever.recieve(&msg[0], 1000);
    }
}

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
    liftHill.updateState();
    ride.updateState();
    preStation.updateState();
    station.updateState();
};

void controlRollerCoaster()
{
    while(active)
    {
        int sendToLiftHill = 0;
        updateBrakeZoneState();
        uint8_t tmpState;
        tmpState = ser.updateCoasterState();
        if (tmpState != 255)
        {
            state = tmpState;
            printf("state %d\n", state);
        }
        printf("liftHill %d, station %d, prestation %d, ride %d\n", liftHill.getState(), station.getState(), preStation.getState(), ride.getState());
        if (state == AUTOMATIC)
        {
            // Lift Hill
            printf(" AUTO liftHill %d, station %d, prestation %d, ride %d\n", liftHill.getState(), station.getState(), preStation.getState(), ride.getState());
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
            } else
            {
                ser.liftHillStop();
            }
            // Station
            if (station.getState() == EMPTY && preStation.getState() == ENTERED)
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
            } else if (station.getState() == ENTERED)
            {
                release_go = 0;
                ser.stationForward(0);
            } else if (liftHill.getState() != EMPTY)
            {
                release_go = 0;
                ser.stopStation();
            } else
            {
                release_go = 0;
                ser.stopStation();
            }
            // Pre Station
            if (preStation.getState() == EMPTY && ride.getState() == ENTERED)
            {
                ser.releasePreStation(130); // Move Forward
            } else if (preStation.getState() == EMPTY)
            {
                ser.stopPreStation();
            } else if (preStation.getState() == ENTERING)
            {
                ser.releasePreStation(130); //  START STOP
            } else if (preStation.getState() == ENTERED && station.getState() == EMPTY)
            {
                ser.releasePreStation(220); // MOVE FORWARD
            } else if (preStation.getState() == ENTERED)
            {
                ser.stopPreStation();
            }
            // Ride
            if (ride.getState() == EMPTY && liftHill.getState() == ENTERED)
            {
                ser.releaseRide(120); // MOVE FORWARD
            } else if (ride.getState() == EMPTY)
            {
                ser.stopRide();
            } else if (ride.getState() == ENTERING)
            {
                ser.releaseRide(160); // STOP
            } else if (ride.getState() == ENTERED && preStation.getState() == EMPTY)
            {
                ser.releaseRide(120); // MOVE FORWARD
            } else if (ride.getState() == ENTERED)
            {
                ser.stopRide(); // STOP
            }
        } else if (state == MANUAL)
        {
            // Lift Hill
            if (liftHill.getState() == EMPTY && station.getState() == ENTERED && release_go)
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
            } else
            {
                ser.liftHillStop();
            }
            // Station
            if (station.getState() == EMPTY && preStation.getState() == ENTERED)
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
                if (!release_go)
                {
                    release_go = ser.sendCanSend(1);
                }

                if (release_go)
                {
                    //printf("release go\n");
                    ser.stationForward(128);
                } else
                {
                    //printf("no go\n");
                    ser.stopStation();
                }
            } else if (station.getState() == ENTERED)
            {
                release_go = 0;
                ser.stationForward(0);
            } else if (liftHill.getState() != EMPTY)
            {
                release_go = 0;
                ser.stopStation();
            } else
            {
                release_go = 0;
                ser.stopStation();
            }
            // Pre Station
            if (preStation.getState() == EMPTY && ride.getState() == ENTERED)
            {
                ser.releasePreStation(130); // Move Forward
            } else if (preStation.getState() == EMPTY)
            {
                ser.stopPreStation();
            } else if (preStation.getState() == ENTERING)
            {
                ser.releasePreStation(130); //  START STOP
            } else if (preStation.getState() == ENTERED && station.getState() == EMPTY)
            {
                ser.releasePreStation(220); // MOVE FORWARD
            } else if (preStation.getState() == ENTERED)
            {
                ser.stopPreStation();
            }
            // Ride
            if (ride.getState() == EMPTY && liftHill.getState() == ENTERED)
            {
                ser.releaseRide(120); // MOVE FORWARD
            } else if (ride.getState() == EMPTY)
            {
                ser.stopRide();
            } else if (ride.getState() == ENTERING)
            {
                ser.releaseRide(160); // STOP
            } else if (ride.getState() == ENTERED && preStation.getState() == EMPTY)
            {
                ser.releaseRide(120); // MOVE FORWARD
            } else if (ride.getState() == ENTERED)
            {
                ser.stopRide(); // STOP
            }
        } else if (state == MAINTENANCE)
        {
            // Arduino handles this its just simpler
            continue;
        } else if (state == STOP)
        {
            // Lift Hill
            ser.liftHillStop();
            // Station
            ser.stopStation();
            release_go = 0;
            // Pre Station
            if (preStation.getState() == EMPTY)
            {
                ser.stopPreStation();
            } else if (preStation.getState() == EMPTY)
            {
                ser.stopPreStation();
            } else if (preStation.getState() == ENTERING)
            {
                ser.releasePreStation(130); //  START STOP
            }
            // Ride
            if (ride.getState() == EMPTY)
            {
                ser.stopRide(); // MOVE FORWARD
            } else if (ride.getState() == EMPTY)
            {
                ser.stopRide();
            } else if (ride.getState() == ENTERING)
            {
                ser.releaseRide(160); // STOP
            }
        } else if (state == RETURN_TO_SAFE)
        {
            // Lift Hill
            if (liftHill.getState() == EMPTY)
            {
                ser.liftHillStop();
            } else if (liftHill.getState() == ENTERING)
            {
                ser.liftHillForward(255);
            } else if (liftHill.getState() == ENTERED)
            {
                ser.liftHillStop();
            }
            // Station
            if (station.getState() == EMPTY)
            {
                ser.stopStation();
            } else if (station.getState() == EMPTY)
            {
                ser.stopStation();
            } else if (station.getState() == ENTERING)
            {
                ser.stationForward(128); // Half up to roll in easy.
            } else if (station.getState() == ENTERED)
            {
                release_go = 0;
                ser.stationForward(0);
            }
            // Pre Station
            if (preStation.getState() == EMPTY)
            {
                ser.stopPreStation();
            } else if (preStation.getState() == EMPTY)
            {
                ser.stopPreStation();
            } else if (preStation.getState() == ENTERING)
            {
                ser.releasePreStation(130); //  START STOP
            }
            // Ride
            if (ride.getState() == EMPTY)
            {
                ser.stopRide(); // MOVE FORWARD
            } else if (ride.getState() == EMPTY)
            {
                ser.stopRide();
            } else if (ride.getState() == ENTERING)
            {
                ser.releaseRide(160); // STOP
            }
        }
    }
};

//int main()
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
    //std::thread wifiTalk(handleWifi);

    coasterThread.join();
    bz1Thread.join();
    bz2Thread.join();
    bz3Thread.join();
    bz4Thread.join();
    //wifiTalk.join();

    return 0;
}

