#include "brakeZone.h"
#include "edge-event.h"
#include "sensorSetup.h"
#include "stateMachine.h"
#include <stdlib.h>
#include <gpiod.h>
#include <stdio.h>
#include <mutex>
#include <cstring>

int hmain()
{
    struct gpiod_edge_event_buffer *eventBuff;
    struct gpiod_line_request *req;
    struct gpiod_edge_event *event;
    volatile uint8_t internalState;
    volatile uint8_t state;
    int timeouts;
    int fallLast;
    int fd;
    //unsigned int sensorLines[21] = {4,17,27,22,10,9,11,5,13,19,26,18,23,24,25,8,7,12,16,20,21};
    unsigned int sensorLines[21] = {4,17,27,22,10,9,11,5,13,19,26,18,23,24,25,12,16,20,21,7,8};
    req = request_input_lines("/dev/gpiochip0", sensorLines, 21, "watch-multiple-line-values", 1000);
    //fd = gpiod_line_request_get_fd(req);
    eventBuff =  gpiod_edge_event_buffer_new(21);
    int ret = 0;
    while (1)
    {
        ret = gpiod_line_request_read_edge_events(req , eventBuff ,21);
        if (ret > 0)
        {
            for (int i = 0; i < ret; i++)
            {
                event = gpiod_edge_event_buffer_get_event(eventBuff,i);
                printf("offset: %d  event #%ld\n",
                       gpiod_edge_event_get_line_offset((gpiod_edge_event*)event),
                       gpiod_edge_event_get_line_seqno((gpiod_edge_event*)event));
            }
        }
    }
    
}
