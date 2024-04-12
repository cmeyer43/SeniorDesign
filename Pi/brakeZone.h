#ifndef BRAKEZONE_H
#define BRAKEZONE_H

#include "sensorSetup.h"
#include "stateMachine.h"
#include <stdlib.h>
#include <gpiod.h>
#include <stdio.h>
#include <mutex>


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
    volatile uint8_t internalState;
    volatile uint8_t state;
    int timeouts;
    int fallLast;
    int fd;
    std::mutex state_mutex;
    std::mutex internalState_mutex;
};

#endif
