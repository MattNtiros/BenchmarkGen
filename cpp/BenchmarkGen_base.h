#ifndef BENCHMARKGEN_IMPL_BASE_H
#define BENCHMARKGEN_IMPL_BASE_H

#include <boost/thread.hpp>
#include <ossie/Resource_impl.h>
#include <ossie/ThreadedComponent.h>

#include <bulkio/bulkio.h>
#include "struct_props.h"

class BenchmarkGen_base : public Resource_impl, protected ThreadedComponent
{
    public:
        BenchmarkGen_base(const char *uuid, const char *label);
        ~BenchmarkGen_base();

        void start() throw (CF::Resource::StartError, CORBA::SystemException);

        void stop() throw (CF::Resource::StopError, CORBA::SystemException);

        void releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

        void loadProperties();

    protected:
        // Member variables exposed as properties
        CORBA::ULong packet_size;
        std::string stream_id;
        double avg_output_rate;
        CORBA::ULong packets_per_time_avg;
        double time;
        CORBA::ULong totalPackets;
        SRI_struct SRI;

        // Ports
        bulkio::OutFloatPort *dataFloat_Out;
        bulkio::OutDoublePort *dataDouble_Out;
        bulkio::OutShortPort *dataShort_Out;
        bulkio::OutOctetPort *dataOctet_Out;

    private:
};
#endif // BENCHMARKGEN_IMPL_BASE_H
