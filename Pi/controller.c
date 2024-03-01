#include "brakeZone.h"
#include "stateMachine.h"
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>

//void* brakeZone_1()
//{
//    brakeZone1Run();
//}
//
//void* brakeZone_2()
//{
//    brakeZone2Run();
//}
//
//void* brakeZone_3()
//{
//    brakeZone3Run();
//}
//
//void* brakeZone_4()
//{
//    brakeZone4Run();
//}

unsigned int bz1Sensors[2] = {16,17};
brakeZone bz1(bz1Sensors, 2);
unsigned int bz2Sensors[2] = {16,17};
brakeZone bz2(bz2Sensors, 2);
unsigned int bz3Sensors[2] = {16,17};
brakeZone bz3(bz3Sensors, 2);
unsigned int bz4Sensors[2] = {16,17};
brakeZone bz4(bz4Sensors, 2);

int main()
{
    int err = 0;
    pthread_t brakeZone1Thread;
    pthread_t brakeZone2Thread;
    pthread_t brakeZone3Thread;
    pthread_t brakeZone4Thread;
    control_state_t state = STOP;

    //bz2 = brakeZone2::brakeZone2();
    //bz3 = brakeZone3::brakeZone3();
    //bz4 = brakeZone4::brakeZone4();

    err += bz1.brakeZoneInit();
    //err += bz2.brakeZoneInit();
    //err += bz3.brakeZoneInit();
    //err += bz4.brakeZoneInit();

    //pthread_create(&brakeZone1Thread, NULL, &brakeZone_1, NULL);
    //pthread_create(&brakeZone2Thread, NULL, &brakeZone_2, NULL);
    //pthread_create(&brakeZone3Thread, NULL, &brakeZone_3, NULL);
    //pthread_create(&brakeZone4Thread, NULL, &brakeZone_4, NULL);


    if (err != 0)
    {
        return err;
    }

    return 0;
}
