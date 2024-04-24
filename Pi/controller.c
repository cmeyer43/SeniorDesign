#include "brakeZone.h"
#include "controlFunctions.h"
#include "stateMachine.h"
#include "serial.h"
#include "controlFunctions.h"
#include "udpRecieve.h"
#include "printDefs.h"
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <thread>
#include <ncurses.h>
#include <mutex>

std::mutex curses_lock;
unsigned int preStationSensors[2] = {12,24}; // Second Gravity Stop
brakeZone preStation(preStationSensors, EMPTY);
unsigned int liftHillSensors[2] = {22,27}; // Lift Hill
brakeZone liftHill(liftHillSensors, EMPTY);
unsigned int rideSensors[2] = {17,25}; // Ride into First Gravity Stop
brakeZone ride(rideSensors, EMPTY);
unsigned int stationSensors[2] = {23,7}; // Wait on spring to be driven to hill.
brakeZone station(stationSensors, EMPTY);
uint8_t state = STOP;
uint8_t release_go = 0;
serial ser("/dev/ttyAMA0");
volatile int active = 1;

void handleWifi()
{
    udpRecieve reciever;
    uint8_t msg[1000];
    //char* msgPtr = msg;
    curses_lock.lock();
    mvprintw(2,0,"Train 1");
    mvprintw(3,0,"\tAccelerometer");
    mvprintw(4,0,"\t\tX: 0 Y: 0 Z: 0");
    mvprintw(5,0,"\tGyroscope");
    mvprintw(6,0,"\t\tX: 0 Y: 0 Z: 0");

    mvprintw(7,0,"Train 2");
    mvprintw(8,0,"\tAccelerometer");
    mvprintw(9,0,"\t\tX: 0 Y: 0 Z: 0");
    mvprintw(10,0,"\tGyroscope");
    mvprintw(11,0,"\t\tX: 0 Y: 0 Z: 0");

    mvprintw(12,0,"Train 3");
    mvprintw(13,0,"\tAccelerometer");
    mvprintw(14,0,"\t\tX: 0 Y: 0 Z: 0");
    mvprintw(15,0,"\tGyroscope");
    mvprintw(16,0,"\t\tX: 0 Y: 0 Z: 0");
    refresh();
    curses_lock.unlock();

    while (active)
    {
        int recv = reciever.recieve(&msg[0], 1000);
        if (recv != 14)
        {
            curses_lock.lock();
            int16_t *vals = (int16_t*)msg;
            if (vals[0] == 1)
            {
                move(2,0);
                clrtoeol();
                mvprintw(2,0,"Train 1");
                move(3,0);
                clrtoeol();
                mvprintw(3,0,"\tAccelerometer");
                move(4,0);
                clrtoeol();
                mvprintw(4,0,"\t\tX: %f Y: %f Z: %f", vals[1] / 16384.0, vals[2] / 16384.0, vals[3] / 16384.0);
                move(5,0);
                clrtoeol();
                mvprintw(5,0,"\tGyroscope");
                move(6,0);
                clrtoeol();
                mvprintw(6,0,"\t\tX: %f Y: %f Z: %f", vals[4] / 131.0, vals[5] / 131.0, vals[6] / 131.0);
            }
            if (vals[0] == 2)
            {
                move(7,0);
                clrtoeol();
                mvprintw(7,0,"Train 2");
                move(8,0);
                clrtoeol();
                mvprintw(8,0,"\tAccelerometer");
                move(9,0);
                clrtoeol();
                mvprintw(9,0,"\t\tX: %f Y: %f Z: %f", vals[1] / 16384.0, vals[2] / 16384.0, vals[3] / 16384.0);
                move(10,0);
                clrtoeol();
                mvprintw(10,0,"\tGyroscope");
                move(11,0);
                clrtoeol();
                mvprintw(11,0,"\t\tX: %f Y: %f Z: %f", vals[4] / 131.0, vals[5] / 131.0, vals[6] / 131.0);
            } else if (vals[0] == 3)
            {
                move(12,0);
                clrtoeol();
                mvprintw(12,0,"Train 3");
                move(13,0);
                clrtoeol();
                mvprintw(13,0,"\tAccelerometer");
                move(14,0);
                clrtoeol();
                mvprintw(14,0,"\t\tX: %f Y: %f Z: %f", vals[1] / 16384.0, vals[2] /16384.0, vals[3] / 16384.0);
                move(15,0);
                clrtoeol();
                mvprintw(15,0,"\tGyroscope");
                move(16,0);
                clrtoeol();
                mvprintw(16,0,"\t\tX: %f Y: %f Z: %f", vals[4] / 131.0, vals[5] / 131.0, vals[6] / 131.0);
            }
            else
            {
                curses_lock.unlock();
                continue;
            }
            refresh();
            curses_lock.unlock();


        }
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
        }
        curses_lock.lock();
        move(0,0);
        clrtoeol();
        mvprintw(0,0,"Mode is  %s", getModeString(state));
        move(1,0);
        clrtoeol();
        mvprintw(1,0,"Lift Hill: %s \tStation: %s \tPre-Station: %s\tRide: %s\n", getStateString(liftHill.getState()), getStateString(station.getState()), getStateString(preStation.getState()), getStateString(ride.getState()));
        refresh();
        curses_lock.unlock();
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
                ser.releasePreStation(PRESTATION_STOP); // Move Forward
            } else if (preStation.getState() == EMPTY)
            {
                ser.stopPreStation();
            } else if (preStation.getState() == ENTERING)
            {
                ser.releasePreStation(PRESTATION_STOP); //  START STOP
            } else if (preStation.getState() == ENTERED && station.getState() == EMPTY)
            {
                ser.releasePreStation(PRESTATION_GO); // MOVE FORWARD
            } else if (preStation.getState() == ENTERED)
            {
                ser.stopPreStation();
            }
            // Ride
            if (ride.getState() == EMPTY && liftHill.getState() == ENTERED)
            {
                ser.releaseRide(RIDE_STOP); // MOVE FORWARD
            } else if (ride.getState() == EMPTY)
            {
                ser.stopRide();
            } else if (ride.getState() == ENTERING)
            {
                ser.releaseRide(RIDE_STOP); // STOP
            } else if (ride.getState() == ENTERED && preStation.getState() == EMPTY)
            {
                ser.releaseRide(RIDE_GO); // MOVE FORWARD
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
                    ser.stationForward(128);
                } else
                {
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
                ser.releasePreStation(PRESTATION_STOP);
            } else if (preStation.getState() == EMPTY)
            {
                ser.stopPreStation();
            } else if (preStation.getState() == ENTERING)
            {
                ser.releasePreStation(PRESTATION_STOP); //  START STOP
            } else if (preStation.getState() == ENTERED && station.getState() == EMPTY)
            {
                ser.releasePreStation(PRESTATION_GO); // MOVE FORWARD
            } else if (preStation.getState() == ENTERED)
            {
                ser.stopPreStation();
            }
            // Ride
            if (ride.getState() == EMPTY && liftHill.getState() == ENTERED)
            {
                ser.releaseRide(RIDE_STOP); // MOVE FORWARD
            } else if (ride.getState() == EMPTY)
            {
                ser.stopRide();
            } else if (ride.getState() == ENTERING)
            {
                ser.releaseRide(RIDE_STOP); // STOP
            } else if (ride.getState() == ENTERED && preStation.getState() == EMPTY)
            {
                ser.releaseRide(RIDE_GO); // MOVE FORWARD
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
                ser.releasePreStation(PRESTATION_STOP); //  START STOP
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
                ser.releaseRide(RIDE_STOP); // STOP
            }
        } else if (state == RETURN_TO_SAFE)
        {
            // Lift Hill
            if (liftHill.getState() == EMPTY)
            {
                ser.liftHillStop();
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
                ser.releasePreStation(PRESTATION_STOP); //  START STOP
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
                ser.releaseRide(RIDE_STOP); // STOP
            }
        }
    }
};

//int main()
int main()
{
    int err = 0;

    initscr();
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
    std::thread wifiTalk(handleWifi);

    coasterThread.join();
    bz1Thread.join();
    bz2Thread.join();
    bz3Thread.join();
    bz4Thread.join();
    wifiTalk.join();
    endwin();

    return 0;
}

