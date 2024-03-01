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
    brakeZone(unsigned int *lines, unsigned int numLines);

    int init();

    void updateState();
private:
    const unsigned int *sensorLines;
    const unsigned int numSensors;
    struct gpiod_edge_event_buffer *eventBuff;
    struct  gpiod_line_request *req;
    struct gpiod_edge_event *event;
    zone_state_t state = EXITED;
};

#endif
