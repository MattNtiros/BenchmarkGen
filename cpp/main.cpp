#include <iostream>
#include "ossie/ossieSupport.h"

#include "BenchmarkGen.h"
int main(int argc, char* argv[])
{
    BenchmarkGen_i* BenchmarkGen_servant;
    Resource_impl::start_component(BenchmarkGen_servant, argc, argv);
    return 0;
}

