#include "brakeZone.h"

brakeZone::brakeZone(unsigned int *lines, uint8_t state)
{
    // Need to add check to see if should be in entered
    this->state = state;
    this->internalState = state;
    this->sensorLines[0] = lines[0];
    this->sensorLines[1] = lines[1];
}

int brakeZone::init()
{
    req = request_input_lines("/dev/gpiochip0", sensorLines , 2, "watch-multiple-line-values", 100000);
    eventBuff =  gpiod_edge_event_buffer_new(2);
    return 0;
}

void brakeZone::updateInternalState()
{
    printf("get ret\n");
    int ret = gpiod_line_request_read_edge_events(req , eventBuff ,
							  2);
    printf("ret\n");
    if (ret == -1)
    {
        printf("error reading edge events\n");
    }
    for (int i = 0; i < ret; i++)
    {
        printf("in loop");
        event = gpiod_edge_event_buffer_get_event(eventBuff,i);
        if (gpiod_edge_event_get_line_offset((gpiod_edge_event*)event) == sensorLines[0])
        {
            if (gpiod_edge_event_get_event_type(event) == GPIOD_EDGE_EVENT_RISING_EDGE)
            {
                if (internalState != ENTERING)
                {
                    internalState_mutex.lock();
                    internalState = ENTERING;
                    internalState_mutex.unlock();
                }
            } else if (gpiod_edge_event_get_event_type(event) == GPIOD_EDGE_EVENT_FALLING_EDGE)
            {
                if (internalState != ENTERING)
                {
                    internalState_mutex.lock();
                    internalState = ENTERED;
                    internalState_mutex.unlock();
                }
            }
        } else if (gpiod_edge_event_get_line_offset((gpiod_edge_event*)event) == sensorLines[1])
        {
            if (gpiod_edge_event_get_event_type(event) == GPIOD_EDGE_EVENT_RISING_EDGE)
            {
                if (internalState != ENTERED)
                {
                    internalState_mutex.lock();
                    internalState = ENTERED;
                    internalState_mutex.unlock();
                }
            } else if (gpiod_edge_event_get_event_type(event) == GPIOD_EDGE_EVENT_FALLING_EDGE)
            {
                if (internalState != EMPTY)
                {
                    internalState_mutex.lock();
                    internalState = EMPTY;
                    internalState_mutex.unlock();
                }
            }
        }
        printf("offset: %d  event #%ld\n",
               gpiod_edge_event_get_line_offset((gpiod_edge_event*)event),
               gpiod_edge_event_get_line_seqno((gpiod_edge_event*)event));
    }
}

void updateState()
{
    internalState_mutex.lock();
    state_mutex.lock();
    state = internalState;
    internalState_mutex.unlock();
    state_mutex.unlock();
}

uint8_t brakeZone::getState()
{
    state_mutex.lock();
    return state;
    state_mutex.unlock();
}
