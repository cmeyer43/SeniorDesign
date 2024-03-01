#include "brakeZone1.h"
#include "brakeZone2.h"
#include "brakeZone3.h"
#include "brakeZone4.h"
#include "stateMachine.h"
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>

void* brakeZone_1()
{
    brakeZone1Run();
}

void* brakeZone_2()
{
    brakeZone2Run();
}

void* brakeZone_3()
{
    brakeZone3Run();
}

void* brakeZone_4()
{
    brakeZone4Run();
}

int main()
{
    int err = 0;
    pthread_t brakeZone1Thread;
    pthread_t brakeZone2Thread;
    pthread_t brakeZone3Thread;
    pthread_t brakeZone4Thread;
    states_t state = STOP;

    err += brakeZone1Init();
    err += brakeZone2Init();
    err += brakeZone3Init();
    err += brakeZone4Init();

    pthread_create(&brakeZone1Thread, NULL, &brakeZone_1, NULL);
    pthread_create(&brakeZone2Thread, NULL, &brakeZone_2, NULL);
    pthread_create(&brakeZone3Thread, NULL, &brakeZone_3, NULL);
    pthread_create(&brakeZone4Thread, NULL, &brakeZone_4, NULL);


    if (err != 0)
    {
        return err;
    }

    return 0;
}
