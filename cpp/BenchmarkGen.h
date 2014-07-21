#ifndef TEST110_IMPL_H
#define TEST110_IMPL_H

#include "BenchmarkGen_base.h"
#include <omp.h> // include flag when making project "-lgomp" in CXXFLAG
#include <math.h>

using std::vector;
using std::complex;
using std::string;
using std::cout;
using std::endl;

typedef unsigned char octet;


class BenchmarkGen_i : public BenchmarkGen_base
{
    ENABLE_LOGGING
    public:
        BenchmarkGen_i(const char *uuid, const char *label);
        ~BenchmarkGen_i();
        int serviceFunction();


    private:
        bool mode;
        bool sriUpdate;
        bool resetTime;
        BULKIO::StreamSRI sri;

        unsigned long last_packet_size;
        unsigned long sentPackets;
        unsigned long last_packets_per_time;

        void updateInfo(double &t);
        void updateSRI(const struct SRI_struct *oldSRI, const struct SRI_struct *newSRI);
        void resizeData(const unsigned long &size);
        void reset_all();

        BULKIO::PrecisionUTCTime t_start;
        double t_b;

        vector<float> dataFloat;
        vector<octet> dataOctet;
        vector<short> dataShort;
        vector<double> dataDouble;
};

#endif // TEST110_IMPL_H
