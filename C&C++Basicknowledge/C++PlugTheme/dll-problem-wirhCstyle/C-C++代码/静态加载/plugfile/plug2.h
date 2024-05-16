#ifndef _PLUG2_H_
#define _PLUG2_H_

#include <iostream>
#include "public.h"
using namespace std;

    #ifdef mynamespace
    namespace plug2{
    #endif

        #ifdef myextern
            #ifdef __cplusplus
            extern "C"{
            #endif
        #endif

                DLL_PUBLIC void fun();  

        #ifdef myextern
            #ifdef __cplusplus
            }
            #endif
        #endif

    #ifdef mynamespace
    };
    #endif

#endif
