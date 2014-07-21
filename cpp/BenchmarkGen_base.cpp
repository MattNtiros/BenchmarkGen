#include "BenchmarkGen_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

    The following class functions are for the base class for the component class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/

BenchmarkGen_base::BenchmarkGen_base(const char *uuid, const char *label) :
    Resource_impl(uuid, label),
    ThreadedComponent()
{
    loadProperties();

    dataFloat_Out = new bulkio::OutFloatPort("dataFloat_Out");
    addPort("dataFloat_Out", dataFloat_Out);
    dataDouble_Out = new bulkio::OutDoublePort("dataDouble_Out");
    addPort("dataDouble_Out", dataDouble_Out);
    dataShort_Out = new bulkio::OutShortPort("dataShort_Out");
    addPort("dataShort_Out", dataShort_Out);
    dataOctet_Out = new bulkio::OutOctetPort("dataOctet_Out");
    addPort("dataOctet_Out", dataOctet_Out);
}

BenchmarkGen_base::~BenchmarkGen_base()
{
    delete dataFloat_Out;
    dataFloat_Out = 0;
    delete dataDouble_Out;
    dataDouble_Out = 0;
    delete dataShort_Out;
    dataShort_Out = 0;
    delete dataOctet_Out;
    dataOctet_Out = 0;
}

/*******************************************************************************************
    Framework-level functions
    These functions are generally called by the framework to perform housekeeping.
*******************************************************************************************/
void BenchmarkGen_base::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    Resource_impl::start();
    ThreadedComponent::startThread();
}

void BenchmarkGen_base::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    Resource_impl::stop();
    if (!ThreadedComponent::stopThread()) {
        throw CF::Resource::StopError(CF::CF_NOTSET, "Processing thread did not die");
    }
}

void BenchmarkGen_base::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    // This function clears the component running condition so main shuts down everything
    try {
        stop();
    } catch (CF::Resource::StopError& ex) {
        // TODO - this should probably be logged instead of ignored
    }

    Resource_impl::releaseObject();
}

void BenchmarkGen_base::loadProperties()
{
    addProperty(packet_size,
                1000,
                "packet_size",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(stream_id,
                "BenchmarkGen stream",
                "stream_id",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(avg_output_rate,
                0,
                "avg_output_rate",
                "",
                "readonly",
                "",
                "external",
                "configure");

    addProperty(packets_per_time_avg,
                30,
                "packets_per_time_avg",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(time,
                0,
                "time",
                "",
                "readonly",
                "",
                "external",
                "configure");

    addProperty(totalPackets,
                0,
                "totalPackets",
                "",
                "readonly",
                "",
                "external",
                "configure");

    addProperty(SRI,
                SRI_struct(),
                "SRI",
                "",
                "readwrite",
                "",
                "external",
                "configure");

}


