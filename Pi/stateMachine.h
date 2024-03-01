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
    INZONE = 0,
    OUTZONE
} zone_state_t;

#endif
