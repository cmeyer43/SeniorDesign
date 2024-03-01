#ifndef SERIAL_H
#define SERIAL_H

class serial
{
public:
    serial(char *device);

    int sendPacket(char* packet, int &size);

    int recvPacket(char* packet, int &size);
private:
    int dev;
};

#endif
