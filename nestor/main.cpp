//
//  main.cpp
//  nestor
//
//  Created by Tommy de Roos on 03/03/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "nestor.h"

int main(int argc, const char * argv[])
{
//    nestor nes("/Users/tderoos/Projects/nestor/smb1.nes");
    nestor nes("/Users/tderoos/Projects/nestor/nestest.nes");
    
    while (true)
        nes.RunToVBlank();
  
    return 0;
}
