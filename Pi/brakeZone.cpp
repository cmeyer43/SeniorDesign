#include "brakeZone.h"

brakeZone::brakeZone(unsigned int *lines, uint8_t state):
    numSensors(numLines)
{
    this->state = state;
}

int brakeZone::init()
{
    req = request_input_lines("/dev/gpiochip0", sensorLines , numSensors , "watch-multiple-line-values");
    eventBuff =  gpiod_edge_event_buffer_new(numSensors );
    return 0;
}

void brakeZone::updateState()
{
    int ret = gpiod_line_request_read_edge_events(req , eventBuff ,
							  numSensors );
    if (ret == -1)
    {
        printf("error reading edge events\n");
    }
    for (int i = 0; i < ret; i++)
    {
        event = gpiod_edge_event_buffer_get_event(eventBuff,i);
        if (gpiod_edge_event_get_line_offset((gpiod_edge_event*)event) == sensorLines[0])
        {
            if (gpiod_edge_event_get_event_type(event) == GPIOD_EDGE_EVENT_RISING_EDGE)
            {
                state = ENTERING;
            } else if (gpiod_edge_event_get_event_type(event) == GPIOD_EDGE_EVENT_FALLING_EDGE)
            {
                state = ENTERED;
            }
        } else if (gpiod_edge_event_get_line_offset((gpiod_edge_event*)event) == sensorLines[1])
        {
            if (gpiod_edge_event_get_event_type(event) == GPIOD_EDGE_EVENT_RISING_EDGE)
            {
                state = EXITING;
            } else if (gpiod_edge_event_get_event_type(event) == GPIOD_EDGE_EVENT_FALLING_EDGE)
            {
                state = EMPTY;
            }
        }
        printf("offset: %d  event #%ld\n",
               gpiod_edge_event_get_line_offset((gpiod_edge_event*)event),
               gpiod_edge_event_get_line_seqno((gpiod_edge_event*)event));
    }
}

zone_state_t brakeZone::getState()
{
    return state;
}
