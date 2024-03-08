#ifndef STATEMACHINE_H
#define STATEMACHINE_H

typedef enum
{
    AUTOMATIC = 0,
    MANUAL,
    MAITNENCE,
    STOP
} control_state_t;

typedef enum
{
    ENTERING = 0,
    ENTERED,
    EXITING,
    EMPTY
} zone_state_t;

#endif
