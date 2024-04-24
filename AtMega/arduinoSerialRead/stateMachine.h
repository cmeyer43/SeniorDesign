#ifndef STATEMACHINE_H
#define STATEMACHINE_H

enum
{
    AUTOMATIC = 0,
    MANUAL,
    MAINTENANCE,
    STOP,
    RETURN_TO_SAFE
};

enum
{
    ENTERING = 0,
    ENTERED,
    EMPTY
};

#endif
