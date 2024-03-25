#ifndef A_H
#define A_H

inline int liftHillEmpty(brakeZone bz)
{
    return bz.getState() == EMPTY;
};
inline int enteringLiftHill(brakeZone bz)
{
    return bz.getState() == ENTERING;
};
inline int topLiftHill(brakeZone bz)
{
    return bz.getState() == ENTERED;
};

inline int inStation(brakeZone bz)
{
    return bz.getState() == ENTERED;
};
inline int enteringStation(brakeZone bz)
{
    return 0;
};
inline int sendToLiftHill(brakeZone bz)
{
    return 0;
}

inline int enteringPreStation(brakeZone bz)
{
    return 0;
};
inline int inPreStation(brakeZone bz)
{
    return 0;
}

inline int rideFree(brakeZone bz)
{
    return 0;
}
inline int enteringRide(brakeZone bz)
{
    return 0;
};
inline int inRide(brakeZone bz)
{
    return 0;
};

inline int requestControl()
{
    return 0;
};

void controlRollerCoaster();



#endif
