
#include "xc.h"
#include "delayHeader.h"
#include <p24Fxxxx.h>

void wait(int t)
{
    int i = 0;
    while(i<t)
    {
        delay_1ms();
        i++;
    }
}
