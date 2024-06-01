//
//  Profiler.cpp
//  nestor
//
//  Created by Tommy de Roos on 4/20/13.
//
//

#include "Profiler.h"

#ifdef ENABLE_PROFILING

int ProfileBar::sCurProfileEntry = 0;
ProfileBar::ProfileEntry ProfileBar::sProfileEntries[ProfileBar::sNumProfileEntry];

void ProfileBar::sClear()
{
    sCurProfileEntry = 0;
}


float ProfileBar::sCPUCounterToMS(uint64 inCount)
{
    static double conversion = 0.0;
    
    if( conversion == 0.0 )
    {
        mach_timebase_info_data_t info;
        kern_return_t err = mach_timebase_info( &info );
        
        //Convert the timebase into seconds
        if( err == 0  )
        {
            conversion = 1e-9 * (double) info.numer / (double) info.denom;
            conversion *= 1000.0f;
        }
    }
    
    return conversion * (double) inCount;
}

#endif