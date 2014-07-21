/**************************************************************************

    This is the component code. This file contains the child class where
    custom functionality can be added to the component. Custom
    functionality to the base class can be extended here. Access to
    the ports can also be done from this class

**************************************************************************/

#include "BenchmarkGen.h"

PREPARE_LOGGING(BenchmarkGen_i)

BenchmarkGen_i::BenchmarkGen_i(const char *uuid, const char *label) :
BenchmarkGen_base(uuid, label)
{
	mode = false;
	resetTime = true;
	sri = bulkio::sri::create(stream_id);
	sri.blocking = true;
	sri.mode = SRI.mode;
	sri.subsize = SRI.subsize;
	sri.xdelta = SRI.xdelta;
	sriUpdate = true;

	last_packet_size = packet_size;
	last_packets_per_time = packets_per_time_avg;
	sentPackets = 0;

	resizeData(packet_size);
	t_b = 0.0;
	addPropertyChangeListener("SRI", this, &BenchmarkGen_i::updateSRI);
}

BenchmarkGen_i::~BenchmarkGen_i()
{
}

/***********************************************************************************************

    Basic functionality:

        The service function is called by the serviceThread object (of type ProcessThread).
        This call happens immediately after the previous call if the return value for
        the previous call was NORMAL.
        If the return value for the previous call was NOOP, then the serviceThread waits
        an amount of time defined in the serviceThread's constructor.
        
    SRI:
        To create a StreamSRI object, use the following code:
                std::string stream_id = "testStream";
                BULKIO::StreamSRI sri = bulkio::sri::create(stream_id);

	Time:
	    To create a PrecisionUTCTime object, use the following code:
                BULKIO::PrecisionUTCTime tstamp = bulkio::time::utils::now();

        
    Ports:

        Data is passed to the serviceFunction through the getPacket call (BULKIO only).
        The dataTransfer class is a port-specific class, so each port implementing the
        BULKIO interface will have its own type-specific dataTransfer.

        The argument to the getPacket function is a floating point number that specifies
        the time to wait in seconds. A zero value is non-blocking. A negative value
        is blocking.  Constants have been defined for these values, bulkio::Const::BLOCKING and
        bulkio::Const::NON_BLOCKING.

        Each received dataTransfer is owned by serviceFunction and *MUST* be
        explicitly deallocated.

        To send data using a BULKIO interface, a convenience interface has been added 
        that takes a std::vector as the data input

        NOTE: If you have a BULKIO dataSDDS or dataVITA49  port, you must manually call 
              "port->updateStats()" to update the port statistics when appropriate.

        Example:
            // this example assumes that the component has two ports:
            //  A provides (input) port of type bulkio::InShortPort called short_in
            //  A uses (output) port of type bulkio::OutFloatPort called float_out
            // The mapping between the port and the class is found
            // in the component base class header file

            bulkio::InShortPort::dataTransfer *tmp = short_in->getPacket(bulkio::Const::BLOCKING);
            if (not tmp) { // No data is available
                return NOOP;
            }

            std::vector<float> outputData;
            outputData.resize(tmp->dataBuffer.size());
            for (unsigned int i=0; i<tmp->dataBuffer.size(); i++) {
                outputData[i] = (float)tmp->dataBuffer[i];
            }

            // NOTE: You must make at least one valid pushSRI call
            if (tmp->sriChanged) {
                float_out->pushSRI(tmp->SRI);
            }
            float_out->pushPacket(outputData, tmp->T, tmp->EOS, tmp->streamID);

            delete tmp; // IMPORTANT: MUST RELEASE THE RECEIVED DATA BLOCK
            return NORMAL;

        If working with complex data (i.e., the "mode" on the SRI is set to
        true), the std::vector passed from/to BulkIO can be typecast to/from
        std::vector< std::complex<dataType> >.  For example, for short data:

            bulkio::InShortPort::dataTransfer *tmp = myInput->getPacket(bulkio::Const::BLOCKING);
            std::vector<std::complex<short> >* intermediate = (std::vector<std::complex<short> >*) &(tmp->dataBuffer);
            // do work here
            std::vector<short>* output = (std::vector<short>*) intermediate;
            myOutput->pushPacket(*output, tmp->T, tmp->EOS, tmp->streamID);

        Interactions with non-BULKIO ports are left up to the component developer's discretion

    Properties:
        
        Properties are accessed directly as member variables. For example, if the
        property name is "baudRate", it may be accessed within member functions as
        "baudRate". Unnamed properties are given a generated name of the form
        "prop_n", where "n" is the ordinal number of the property in the PRF file.
        Property types are mapped to the nearest C++ type, (e.g. "string" becomes
        "std::string"). All generated properties are declared in the base class
        (BenchmarkGen_base).
    
        Simple sequence properties are mapped to "std::vector" of the simple type.
        Struct properties, if used, are mapped to C++ structs defined in the
        generated file "struct_props.h". Field names are taken from the name in
        the properties file; if no name is given, a generated name of the form
        "field_n" is used, where "n" is the ordinal number of the field.
        
        Example:
            // This example makes use of the following Properties:
            //  - A float value called scaleValue
            //  - A boolean called scaleInput
              
            if (scaleInput) {
                dataOut[i] = dataIn[i] * scaleValue;
            } else {
                dataOut[i] = dataIn[i];
            }
            
        Callback methods can be associated with a property so that the methods are
        called each time the property value changes.  This is done by calling 
        addPropertyChangeListener(<property name>, this, &BenchmarkGen_i::<callback method>)
        in the constructor.

        Callback methods should take two arguments, both const pointers to the value
        type (e.g., "const float *"), and return void.

        Example:
            // This example makes use of the following Properties:
            //  - A float value called scaleValue
            
        //Add to BenchmarkGen.cpp
        BenchmarkGen_i::BenchmarkGen_i(const char *uuid, const char *label) :
            BenchmarkGen_base(uuid, label)
        {
            addPropertyChangeListener("scaleValue", this, &BenchmarkGen_i::scaleChanged);
        }

        void BenchmarkGen_i::scaleChanged(const float *oldValue, const float *newValue)
        {
            std::cout << "scaleValue changed from" << *oldValue << " to " << *newValue
                      << std::endl;
        }
            
        //Add to BenchmarkGen.h
        void scaleChanged(const float* oldValue, const float* newValue);
        
        
************************************************************************************************/
int BenchmarkGen_i::serviceFunction()
{
	LOG_DEBUG(BenchmarkGen_i, "serviceFunction() example log message");



	//if sri has been updated, update sri in all output ports
	if(sriUpdate)
	{
		dataDouble_Out->pushSRI(sri);
		dataFloat_Out->pushSRI(sri);
		dataShort_Out->pushSRI(sri);
		dataOctet_Out->pushSRI(sri);
		if(sri.mode)
			mode = true;
		else
			mode = false;
		sriUpdate = false;
	}

	if(last_packets_per_time != packets_per_time_avg)
	{
		last_packets_per_time = packets_per_time_avg;
		totalPackets = 0;
		reset_all();
		return NOOP;
	}

	//if packet size changed, resize data
	if(last_packet_size != packet_size)
	{
		last_packet_size = packet_size;
		resizeData(last_packet_size);
		totalPackets = 0;
		reset_all();
		return NOOP;
	}
	if(resetTime)
	{
		t_b = omp_get_wtime();
		resetTime = false;
	}

	if(mode)
		resizeData(last_packet_size*2);
	for(unsigned int i=0;i<dataFloat.size();i++)
	{
		dataDouble[i] = 		(sqrt(2)*5);
		dataFloat[i] = 	(float)	(sqrt(2)*5);
		dataShort[i] = 	(short) (sqrt(2)*5);
		dataOctet[i] = 	(octet) (sqrt(2)*5);
	}

	BULKIO::PrecisionUTCTime tstamp = bulkio::time::utils::now();
	dataDouble_Out->pushPacket(dataDouble, tstamp, false, stream_id);
	dataFloat_Out->pushPacket(dataFloat, tstamp, false, stream_id);
	dataShort_Out->pushPacket(dataShort, tstamp, false, stream_id);
	dataOctet_Out->pushPacket(dataOctet, tstamp, false, stream_id);
	double t_t = omp_get_wtime();
	sentPackets++;
	totalPackets++;
	if(sentPackets == last_packets_per_time)
		updateInfo(t_t);

	return NORMAL;
}


void BenchmarkGen_i::reset_all()
{
	sentPackets = 0;
	resetTime = true;
}

void BenchmarkGen_i::updateInfo(double &t)
{
	double elapsedTime = t - t_b;
	time = elapsedTime;
	avg_output_rate = (sentPackets*last_packet_size)/elapsedTime;
	reset_all();
}

void BenchmarkGen_i::updateSRI(const struct SRI_struct *oldSRI, const struct SRI_struct *newSRI)
{
	if(oldSRI->mode != newSRI->mode)
		sri.mode = newSRI->mode;
	if(oldSRI->subsize != newSRI->subsize)
		sri.subsize = newSRI->subsize;
	if(oldSRI->xdelta != newSRI->xdelta)
		sri.xdelta = newSRI->xdelta;

	sriUpdate = true;
}


void BenchmarkGen_i::resizeData(const unsigned long &size)
{
	dataDouble.resize(size);
	dataFloat.resize(size);
	dataShort.resize(size);
	dataOctet.resize(size);
}
