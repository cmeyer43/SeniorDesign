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

    zone_state_t getState();
private:
    const unsigned int sensorLines[2];
    struct gpiod_edge_event_buffer *eventBuff;
    struct  gpiod_line_request *req;
    struct gpiod_edge_event *event;
    zone_state_t state;;
};

#endif
