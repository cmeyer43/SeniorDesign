#include "brakeZone.h"
#include "edge-event.h"
#include <cstring>

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
    req = request_input_lines("/dev/gpiochip0", sensorLines , 2, "watch-multiple-line-values", 10);
    fd = gpiod_line_request_get_fd(req);
    eventBuff =  gpiod_edge_event_buffer_new(2);
    return 0;
}

void brakeZone::updateInternalState()
{
    // int ret = gpiod_line_request_read_edge_events(req , eventBuff ,2);
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 200000;
    int ret = select(fd + 1, &rfds, NULL, NULL, &timeout);
    if (ret == 0 || ret == -1)
    {
        timeouts++;
        //printf("timeouts %d\n", timeouts);
        ret = 0;
        //printf("error reading edge events\n");
        //printf("Internal State %u, fallLast %d\n", internalState, fallLast);
        if (timeouts >= 10 && internalState == ENTERED && fallLast == 1)
        {
            if (internalState != EMPTY)
            {
                internalState_mutex.lock();
                //printf("EMPTY\n");
                internalState = EMPTY;
                internalState_mutex.unlock();
            }
        }
        return;
    }
    else
    {
        timeouts = 0;
        ret = gpiod_edge_event_buffer_read_fd(fd, eventBuff, 2);
        //memset(eventBuff->event_data, 0,
        //       sizeof(eventBuff->event_data) * eventBuff->capacity);
        //read(fd, buf

    }
    for (int i = 0; i < ret; i++)
    {
        //printf("in loop");
        // Update this to use the fd and select statement so that we can timeout instead of infinitely blocking. We can then count timeouts and "exiting" to determine empty.
        event = gpiod_edge_event_buffer_get_event(eventBuff,i);
        if (gpiod_edge_event_get_line_offset((gpiod_edge_event*)event) == sensorLines[0])
        {
            printf("entering edge\n");
            fallLast = 0;
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
                    internalState = ENTERING;
                    internalState_mutex.unlock();
                }
            }
        } else if (gpiod_edge_event_get_line_offset((gpiod_edge_event*)event) == sensorLines[1])
        {
            printf("leaving edge\n");
            if (gpiod_edge_event_get_event_type(event) == GPIOD_EDGE_EVENT_RISING_EDGE)
            {
                fallLast = 0;
                //printf("EDGE RAISED\n");
                if (internalState != ENTERED)
                {
                    //printf("ENTERED\n");
                    internalState_mutex.lock();
                    internalState = ENTERED;
                    internalState_mutex.unlock();
                }
            } else if (gpiod_edge_event_get_event_type(event) == GPIOD_EDGE_EVENT_FALLING_EDGE)
            {
                fallLast = 1;
                //printf("EDGE Fall\n");
                //if (internalState != EMPTY)
                //{
                //    //printf("EMPTY\n");
                //    internalState_mutex.lock();
                //    internalState = EMPTY;
                //    internalState_mutex.unlock();
                //}
            }
        }
        printf("offset: %d  event #%ld\n",
               gpiod_edge_event_get_line_offset((gpiod_edge_event*)event),
               gpiod_edge_event_get_line_seqno((gpiod_edge_event*)event));
    }
}

void brakeZone::updateState()
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
    uint8_t ret = state;
    state_mutex.unlock();
    return ret;
}
