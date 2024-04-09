/******************************************************************************
 * File: uart-loopback.c
 * Description: Test UART loopback from userspace.
 *
 *****************************************************************************/

// Include files

#include "serial.h"

serial::serial(char *device)
{
    dev = open(device, O_RDWR | O_NOCTTY);
    if(dev < 0)
    {
      printf("failed to open\n");
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
    options.c_iflag = IGNPAR | ICRNL;
    tcflush(dev, TCIOFLUSH);
    tcsetattr(dev, TCSANOW, &options);
}

serial::~serial()
{
    close(dev);
}
  
int serial::sendPacket(uint8_t *msg, int size)
{
    uint8_t start[1] = {0x7b};
    uint8_t stop[1] = {0x7d};
    if (write(dev,start,1) != 1)
    {
        return 1;
    }
    if (write(dev,msg,size) != size)
    {
        return 1;
    }
    if (write(dev,stop,1) != 1)
    {
        return 1;
    }
    return 0;
}

int serial::recvPacket(uint8_t *msg, int size)
{
    // read response
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(dev, &rfds);

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 10000;

    int sizeRead = 0;
    int stat = select(dev + 1, &rfds, NULL, NULL, &timeout);
    if (stat > 0)
    {
        sizeRead = read(dev, msg, size);
    }
    return sizeRead;
}

uint8_t serial::updateCoasterState()
{
    uint8_t msg[5] = {1};
    msg[0] = {REQUEST_BUTTON_STATE};
    this->sendPacket(msg, 1);
    int sendBytes = 5;
    int recieved = this->recvPacket(msg, 5);
    if (msg[0] == SEND_BUTTON_STATE)
    {
        return msg[1];
    }
    return -1;
}

int serial::requestControl()
{
    uint8_t msg[5] = {1};
    msg[0] = {REQUEST_CONTROL};
    this->sendPacket(msg, 1);
    int sendBytes = 5;
    int recieved = this->recvPacket(msg, 5);
    return NONE;
    if (msg[0] == SEND_CONTROL)
    {
        return msg[1];
    }
    return NONE;
}

int serial::sendCanSend(int canSend)
{
    printf("can send\n");
    uint8_t msg[5] = {1,0,0,0,0};
    msg[0] = {SEND_CAN_SEND};
    msg[1] = (uint8_t) canSend;
    this->sendPacket(msg, 2);
    int sendBytes = 5;
    int recieved = this->recvPacket(msg, 5);
    if (recieved && msg[0] == RESPOND_CAN_SEND)
    {
        return msg[1];
    }
    return 0;
}

void serial::liftHillForward(int pow)
{
    uint8_t msg[5] = {1};
    msg[0] = {CONTROL_DC_2};
    msg[1] = (uint8_t)pow;
    msg[2] = (uint8_t)FORWARD;
    this->sendPacket(msg, 3);
}

void serial::liftHillBackward(int pow)
{
    uint8_t msg[5] = {1};
    msg[0] = {CONTROL_DC_2};
    msg[1] = (uint8_t)pow;
    msg[2] = (uint8_t)BACKWARD;
    this->sendPacket(msg, 3);
    return;
}

void serial::liftHillStop()
{
    uint8_t msg[5] = {1};
    msg[0] = {CONTROL_DC_2};
    msg[1] = (uint8_t)0;
    this->sendPacket(msg, 2);
    return;
}

void serial::stationForward(int pow)
{
    uint8_t msg[5] = {1};
    msg[0] = {CONTROL_DC_1};
    msg[1] = ((uint8_t)pow);
    msg[2] = (uint8_t)FORWARD;
    this->sendPacket(msg, 3);
    return;
}

void serial::stationBackward(int pow)
{
    uint8_t msg[5] = {1};
    msg[0] = {CONTROL_DC_1};
    msg[1] = (uint8_t)pow;
    msg[2] = (uint8_t)BACKWARD;
    this->sendPacket(msg, 3);
    return;
}

void serial::stopStation()
{
    uint8_t msg[5] = {1};
    msg[0] = {CONTROL_DC_1};
    msg[1] = 0;
    this->sendPacket(msg, 2);
    return;
}

void serial::releasePreStation(int pow)
{
    uint8_t msg[5] = {1};
    msg[0] = {CONTROL_SERVO_1};
    msg[1] = (uint8_t)pow;
    this->sendPacket(msg, 2);
    return;
}

void serial::stopPreStation()
{
    uint8_t msg[5] = {1};
    msg[0] = {CONTROL_SERVO_1};
    msg[1] = 160;
    this->sendPacket(msg, 2);
    return;
}

void serial::releaseRide(int pow)
{
    uint8_t msg[5] = {1};
    msg[0] = {CONTROL_SERVO_2};
    msg[1] = (uint8_t)pow;
    this->sendPacket(msg, 2);
    return;
}

void serial::stopRide()
{
    uint8_t msg[5] = {1};
    msg[0] = {CONTROL_SERVO_2};
    msg[1] = 180;
    this->sendPacket(msg, 2);
    return;
}
