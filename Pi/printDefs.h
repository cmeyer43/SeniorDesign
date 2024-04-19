#ifndef PRINTDEFS_H
#define PRINTDEFS_H
char* getModeString(int state)
{
    if (state == AUTOMATIC)
    {
        return "Automatic";
    } else if (state == MANUAL)
    {
        return "Manual";
    } else if (state == MAINTENANCE)
    {
        return "Maintenance";
    } else if (state == STOP)
    {
        return "Stop";
    } else if (state == RETURN_TO_SAFE)
    {
        return "Powered Off";
    }
    return "";
}

char* getStateString(int state)
{
    if (state == ENTERING)
    {
        return "Entering";
    } else if (state == ENTERED)
    {
        return "Entered";
    } else if (state == EMPTY)
    {
        return "Empty";
    }
}

#endif
