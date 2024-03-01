#include "brakeZone1.h"
#include "brakeZone2.h"
#include "brakeZone3.h"
#include "brakeZone4.h"
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>

void* brakeZone_1_2()
{
}

void* brakeZone_3_4()
{
}

int main()
{
    int err = 0;
    pthread_t brakeZone_1_2_thread;
    pthread_t brakeZone_3_4_thread;

    err += brakeZone1Init();
    err += brakeZone2Init();
    err += brakeZone3Init();
    err += brakeZone4Init();

    if (err != 0)
    {
        return err;
    }

    return 0;
}
