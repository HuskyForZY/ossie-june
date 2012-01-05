
#include <string>
#include <iostream>
#include "__IncludeFile__.h"

__Class_name__::__Class_name__(const char *uuid, omni_condition *condition) :
    Resource_impl(uuid), component_running(condition)
{
    __CONSTRUCTORS__
	__IS_ASSEMBLY_CONTROLLER__
}

__Class_name__::~__Class_name__(void)
{
    __PORT_DESTRUCTORS__
    __SIMPLE_SEQUENCE_POINTER_DESTRUCTORS__
}

// Static function for omni thread
void __Class_name__::Run( void * data )
{
    ((__Class_name__*)data)->ProcessData();
}

CORBA::Object_ptr __Class_name__::getPort( const char* portName ) throw (
    CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, __IncludeFile__, "getPort() invoked with " << portName)

    CORBA::Object_var p;

__GET_PORT__
}

void __Class_name__::start() throw (CORBA::SystemException,
    CF::Resource::StartError)
{
    DEBUG(3, __IncludeFile__, "start() invoked")
	omni_mutex_lock  l(processing_mutex);
	if( false == thread_started )
	{
		thread_started = true;
		// Create the thread for the writer's processing function
		processing_thread = new omni_thread(Run, (void *) this);

		// Start the thread containing the writer's processing function
		processing_thread->start();
	}
}

void __Class_name__::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    DEBUG(3, __IncludeFile__, "stop() invoked")
	omni_mutex_lock l(processing_mutex);
	thread_started = false;
}

void __Class_name__::releaseObject() throw (CORBA::SystemException,
    CF::LifeCycle::ReleaseError)
{
    DEBUG(3, __IncludeFile__, "releaseObject() invoked")

    component_running->signal();
}

void __Class_name__::initialize() throw (CF::LifeCycle::InitializeError,
    CORBA::SystemException)
{
    DEBUG(3, __IncludeFile__, "initialize() invoked")
}

void __Class_name__::query( CF::Properties & configProperties ) throw (CORBA::SystemException, CF::UnknownProperties)
{
	if( configProperties.length() == 0 )
	{
		configProperties.length( propertySet.length() );
		for( int i = 0; i < propertySet.length(); i++ )
		{
			configProperties[i].id = CORBA::string_dup( propertySet[i].id );
			configProperties[i].value = propertySet[i].value;
		}
		return;
	} else {
		for( int i = 0; i < configProperties.length(); i++ ) {
			for( int j = 0; j < propertySet.length(); j++ ) {
				if( strcmp(configProperties[i].id, propertySet[j].id) == 0 ) {
					configProperties[i].value = propertySet[j].value;
				}
			}
		}
	} // end if-else
}

void __Class_name__::configure(const CF::Properties& props)
throw (CORBA::SystemException,
    CF::PropertySet::InvalidConfiguration,
    CF::PropertySet::PartialConfiguration)
{
    DEBUG(3, __IncludeFile__, "configure() invoked")

    __READ_PROPS__
}

void __Class_name__::ProcessData()
{
    DEBUG(3, __IncludeFile__, "ProcessData() invoked")

    __PROCESS_DATA_DECLARATIONS__

    __PROCESS_DATA_LOOP__
}

bool __Class_name__::continue_processing()
{
	omni_mutex_lock l(processing_mutex);
	return thread_started;
}

__ACE_SVC_DEF__

