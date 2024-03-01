#include "brakeZone.h"
#include "stateMachine.h"
#include "serial.h"
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <thread>

unsigned int bz1Sensors[2] = {16,17};
brakeZone bz1(bz1Sensors, 2);
unsigned int bz2Sensors[2] = {18,19};
brakeZone bz2(bz2Sensors, 2);
unsigned int bz3Sensors[2] = {20,21};
brakeZone bz3(bz3Sensors, 2);
unsigned int bz4Sensors[2] = {22,23};
brakeZone bz4(bz4Sensors, 2);

serial ser("/dev/ttyAMA0");

void brakeZoneWork()
{
    bz1.updateState();
    bz2.updateState();
    bz3.updateState();
    bz4.updateState();
}

int main()
{
    int err = 0;
    control_state_t state = STOP;

    err += bz1.init();
    err += bz2.init();
    err += bz3.init();
    err += bz4.init();

    if (err != 0)
    {
        return err;
    }

    std::thread breakZoneThread(brakeZoneWork);

    return 0;
}
