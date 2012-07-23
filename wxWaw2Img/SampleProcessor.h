/*
        SampleProcessor.h
 */

#ifndef _SAMPLE_PROCESSOR_H_
#define _SAMPLE_PROCESSOR_H_

class SampleProcessor
{
public:

	SampleProcessor( void );
	virtual ~SampleProcessor( void );

public:

	virtual void ProcessInitializer() = 0;
	virtual void ProcessFinalizer()	  = 0;
};

class ProcessorHolder
{
public:

	ProcessorHolder( SampleProcessor& processor ):
		m_processor( processor )
	{
		processor.ProcessInitializer();
	}

	~ProcessorHolder()
	{
		m_processor.ProcessFinalizer();
	}

protected:

	SampleProcessor& m_processor;
};

#endif;

