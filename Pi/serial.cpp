/******************************************************************************
 * File: uart-loopback.c
 * Description: Test UART loopback from userspace.
 *
 *****************************************************************************/

// Include files

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
#include "serial.h"
#include "messages.h"

serial::serial(char *device)
{
    dev = open(device, O_RDWR | O_NOCTTY);
    if(dev < 0)
    {
      printf("failed to open");
    }

    // Set attributes
    struct termios options;
    tcgetattr(dev, &options);
    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                    | INLCR | IGNCR | ICRNL | IXON);
    options.c_oflag &= ~OPOST;
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    options.c_cflag &= ~(CSIZE | PARENB);
    options.c_cflag |= B9600 | CS8 | CREAD | CLOCAL;
    options.c_cc[VTIME] = 1;
    options.c_cc[VMIN] = 0;
    options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
    //options.c_iflag = IGNPAR | ICRNL;
    tcflush(dev, TCIOFLUSH);
    tcsetattr(dev, TCSANOW, &options);
}
  
int serial::sendPacket(uint8_t *msg, int size)
{
    if (write(dev,msg,1) != size)
    {
        return 1;
    }
    return 0;
}

int serial::recvPacket(uint8_t *msg, int size)
{
    // read response
    int sizeRead = read(dev, msg, size);
    return sizeRead;
}

control_state_t serial::updateCoasterState()
{
    uint8_t msg[40];
    msg[0] = {REQUEST_BUTTON_STATE};
    this->sendPacket(msg, 1);
    int sendBytes = 40;
    int recieved = this->recvPacket(msg, 40);
    if (msg[0] == SEND_BUTTON_STATE)
    {
        return msg[1];
    }
    return NULL;
}

