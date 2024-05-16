#include <iostream>
#include "plug.h"
#include "plug2.h"
using namespace std;

int main(){
    #ifdef mynamespace
    plug1::
    #endif
    fun();

    #ifdef mynamespace
    plug2::
    #endif
    fun();

    return 0;
}