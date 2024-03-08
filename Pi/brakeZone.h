#ifndef BRAKEZONE1_H
#define BRAKEZONE1_H

#include "sensorSetup.h"
#include "stateMachine.h"
#include <stdlib.h>
#include <gpiod.h>
#include <stdio.h>


class brakeZone
{
public:
    brakeZone(unsigned int *lines, uint8_t State);

    int init();

    void updateState();

    uint8_t getState();
private:
    unsigned int sensorLines[2];
    struct gpiod_edge_event_buffer *eventBuff;
    struct  gpiod_line_request *req;
    struct gpiod_edge_event *event;
    uint8_t state;;
};

#endif
