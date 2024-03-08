#ifndef SERIAL_H
#define SERIAL_H

#include "stateMachine.h"

class serial
{
public:
    serial(char *device);

    ~serial();

    int sendPacket(uint8_t* packet, int size);

    int recvPacket(uint8_t* packet, int size);

    uint8_t updateCoasterState();
private:
    int dev;
};

#endif
