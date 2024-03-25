#ifndef BRAKEZONE_H
#define BRAKEZONE_H

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
    void updateInternalState();
    uint8_t getState();
private:
    unsigned int sensorLines[2];
    struct gpiod_edge_event_buffer *eventBuff;
    struct  gpiod_line_request *req;
    struct gpiod_edge_event *event;
    uint8_t internalState;
    uint8_t state;
    std::mutex state_mutex;
    std::mutex interalState_mutex;
};

#endif
