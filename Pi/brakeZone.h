#ifndef BRAKEZONE1_H
#define BRAKEZONE1_H

#include "sensorSetup.h"
#include <stdlib.h>
#include <gpiod.h>
#include <stdio.h>

class brakeZone
{
public:
    brakeZone(unsigned int *lines, unsigned int numLines);

    int brakeZoneInit();

    void brakeZoneRun();
private:
    const unsigned int *sensorLines;
    const unsigned int numSensors;
    struct gpiod_edge_event_buffer *eventBuff;
    struct  gpiod_line_request *req;
    struct gpiod_edge_event *event;
};

#endif
