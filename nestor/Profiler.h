//
//  Profiler.h
//  nestor
//
//  Created by Tommy de Roos on 4/20/13.
//
//

#pragma once
#define ENABLE_PROFILING

#ifdef ENABLE_PROFILING

#include "types.h"
#include <mach/mach_time.h>
#include <stdio.h>
#include <utility>

#define PROFILEBAR(id)    ProfileBar __PBAR##__LINE__(id)

struct ProfileBar
{
    ProfileBar(const char* inID)
    {
        id = inID;
        start = mach_absolute_time();
    }
    
    ~ProfileBar()
    {
        uint64 delta = mach_absolute_time() - start;
        sProfileEntries[sCurProfileEntry++] = std::make_pair(id, delta);
        
        printf("%10s %.4f\n", id, sCPUCounterToMS(delta));
    }
    
    void Tick()
    {
        uint64 current = mach_absolute_time();
        uint64 delta = current - start;
        
        printf("%10s %.4f\n", id, sCPUCounterToMS(delta));
        
        start = current;
    }

    
    static float sCPUCounterToMS(uint64 inCount);
    static void sGetTimingInfo();
    static void sClear();
    
    const char* id;
    uint64 start;
    
    static const int sNumProfileEntry = 1000;
    static int sCurProfileEntry;
    typedef std::pair<const char*, uint64> ProfileEntry;
    static ProfileEntry sProfileEntries[sNumProfileEntry];
};


#else 

#define PROFILEBAR()

#endif
