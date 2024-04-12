#ifndef SERIAL_H
#define SERIAL_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <stdarg.h>
#include <cstdint>
#include <stdint.h>
#include "messages.h"
#include "stateMachine.h"

class serial
{
public:
    serial(char *device);

    ~serial();

    int sendPacket(uint8_t* packet, int size);

    int recvPacket(uint8_t* packet, int size);

    uint8_t updateCoasterState();

    int requestControl();
    int sendCanSend(int canSend);

    void liftHillForward(int pow);
    void liftHillBackward(int pow);
    void liftHillStop();

    void stationForward(int pow);
    void stationBackward(int pow);
    void stopStation();

    void releasePreStation(int pow);
    void stopPreStation();

    void releaseRide(int pow);
    void stopRide();

private:
    int dev;
};

#endif
