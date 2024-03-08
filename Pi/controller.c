#include "brakeZone.h"
#include "stateMachine.h"
#include "serial.h"
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <thread>

unsigned int bz1Sensors[2] = {16,17};
brakeZone bz1(bz1Sensors, ENTERED);
unsigned int bz2Sensors[2] = {18,19};
brakeZone bz2(bz2Sensors, ENTERED);
unsigned int bz3Sensors[2] = {20,21};
brakeZone bz3(bz3Sensors, ENTERED);
unsigned int bz4Sensors[2] = {22,23};
brakeZone bz4(bz4Sensors, EMPTY);
uint8_t state = STOP;



void brakeZoneWork(serial ser)
{
    while (1)
    {
        //bz1.updateState();
        //bz2.updateState();
        //bz3.updateState();
        //bz4.updateState();
        uint8_t tmpState = ser.updateCoasterState();
        state = tmpState != 255 ? tmpState : state;
        printf("state is %u\n", state);
        if (state == MANUAL)
        {
            // Send if allowed to go
            continue;
        } else if (state == OFF)
        {
            // Do nothing
            continue;
        } else if (state == MAITNENCE)
        {
            // Not sure
            continue;
        } else if (state == AUTOMATIC)
        {
            // logic
            continue;
        }
    }

}


int main()
{
    serial ser("/dev/ttyAMA0");
    int err = 0;

    err += bz1.init();
    err += bz2.init();
    err += bz3.init();
    err += bz4.init();

    if (err != 0)
    {
        return err;
    }

    std::thread breakZoneThread(brakeZoneWork, ser);

    breakZoneThread.join();

    return 0;
}

