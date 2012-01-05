/****************************************************************************

Copyright 2008, Virginia Polytechnic Institute and State University

This file is part of the OSSIE Core Framework.

OSSIE Core Framework is free software; you can redistribute it and/or modify
it under the terms of the Lesser GNU General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

OSSIE Core Framework is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Lesser GNU General Public License for more details.

You should have received a copy of the Lesser GNU General Public License
along with OSSIE Core Framework; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/
#include <iostream>
#include <string>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifndef HAVE_UNISTD_H
#include <process.h>
#endif

#include "ossie/debug.h"
#include "ossie/Application_impl.h"
#include "ossie/ossieSupport.h"
#include "ossie/portability.h"

Application_impl::Application_impl (const char *_id, const char *_name,
                                    const char *_profile, DEV_SEQ * _devSeq,
                                    ELEM_SEQ * _implSeq, ELEM_SEQ * _ncSeq,
                                    PROC_ID_SEQ * _pidSeq,
                                    std::vector <ossieSupport::ConnectionInfo *> _connectionData,
                                    CF::Resource_ptr _controller,
                                    CF::DomainManager::ApplicationSequence *_appseq) : Resource_impl(_id)
{

    appName = _name;
    sadProfile = _profile;

    connectionData = _connectionData;
    _orb = new ossieSupport::ORB();

    appseq = _appseq;

    if (_devSeq != NULL) {
        this->appComponentDevices = new DEV_SEQ ();
        this->appComponentDevices->length (_devSeq->length ());

        for (unsigned i = 0; i < _devSeq->length (); i++)
            (*appComponentDevices)[i] = (*_devSeq)[i];
    } else {
        this->appComponentDevices = new DEV_SEQ (0);
    }

    if (_implSeq != NULL) {
        this->appComponentImplementations = new ELEM_SEQ ();
        this->appComponentImplementations->length (_implSeq->length ());

        for (unsigned int i = 0; i < _implSeq->length (); i++)
            (*appComponentImplementations)[i] = (*_implSeq)[i];
    } else {
        this->appComponentImplementations = new ELEM_SEQ (0);
    }

    if (_ncSeq != NULL) {
        this->appComponentNamingContexts = new ELEM_SEQ ();
        this->appComponentNamingContexts->length (_ncSeq->length ());

        for (unsigned int i = 0; i < _ncSeq->length (); i++)
            (*appComponentNamingContexts)[i] = (*_ncSeq)[i];
    } else {
        this->appComponentNamingContexts = new ELEM_SEQ (0);
    }

    if (_pidSeq != NULL) {
        this->appComponentProcessIds = new PROC_ID_SEQ ();
        this->appComponentProcessIds->length (_pidSeq->length ());

        for (unsigned int i = 0; i < _pidSeq->length (); i++)
            (*appComponentProcessIds)[i] = (*_pidSeq)[i];
    } else {
        this->appComponentProcessIds = new PROC_ID_SEQ (0);
    }

    this->assemblyController = _controller;
};

Application_impl::~Application_impl ()
{

    delete this->appComponentDevices;
    this->appComponentDevices = NULL;

    delete this->appComponentImplementations;
    this->appComponentImplementations = NULL;

    delete this->appComponentNamingContexts;
    this->appComponentNamingContexts = NULL;

    delete this->appComponentProcessIds;
    this->appComponentProcessIds = NULL;
};

void Application_impl::start ()
throw (CORBA::SystemException, CF::Resource::StartError)
{
/* 
    try {
        assemblyController->start ();
    } catch ( CF::Resource::StartError& se ) {
        std::cout << "[Application::start] \"assmeblyController->start\" failed with CF:Resource::StartError\n";
        // THROW CF:Resource::StartError ??
        exit(EXIT_FAILURE);
    } catch ( CORBA::SystemException& ex ) {
        std::cout << "[Application::start] \"assemblyController->start\" failed with CORBA::SystemException\n";
        // THROW CORBA::SystemException ??
        exit(EXIT_FAILURE);
    } catch ( ... ) {
        std::cout << "[Application::start] \"assemblyController->start\" failed with Unknown Exception\n";
        exit(EXIT_FAILURE);
    }
*/
 // NOTE: this is not supposed to be here
 // Remove this section and replace with the above section
 // when proper port impls are available
    CORBA::Object_ptr _rscobj = CORBA::Object::_nil();
    for( unsigned int i = 0; i < appComponentNamingContexts->length(); i++ )
    {
     _rscobj = _orb->get_object_from_name((*appComponentNamingContexts)[i].elementId);
     CF::Resource_ptr _rsc = CF::Resource::_narrow(_rscobj);
     _rsc->start();
     std::cout << "[Application::start] Component Naming Context " << i << ": " << (*appComponentNamingContexts)[i].elementId << std::endl;
     CORBA::release(_rsc);
    }
    CORBA::release(_rscobj);
}


void Application_impl::stop ()
throw (CORBA::SystemException, CF::Resource::StopError)
{
/*
    try {
        assemblyController->stop ();
    } catch ( CF::Resource::StopError& se ) {
        std::cout << "[Application::stop] \"assemblyController->stop\" failed with CF::Resource::StopError\n";
        // THROW CF::Resource::StopError ??
        exit(EXIT_FAILURE);
    } catch ( CORBA::SystemException& se ) {
        std::cout << "[Application::stop] \"assemblyController->stop\" failed with CORBA::SystemException\n";
        // THROW CORBA::SystemException ??
        exit(EXIT_FAILURE);
    } catch ( ... ) {
        std::cout << "[Application::stop] \"assemblyController->stop\" failed with Unknown Exception\n";
        exit(EXIT_FAILURE);
    }
*/
 // NOTE: this is not supposed to be here
 // Remove this section and replace with the above section
 // when proper port impls are available
    CORBA::Object_ptr _rscobj = CORBA::Object::_nil();
    for( unsigned int i = 0; i < appComponentNamingContexts->length(); i++ )
    {
     _rscobj = _orb->get_object_from_name((*appComponentNamingContexts)[i].elementId);
     CF::Resource_ptr _rsc = CF::Resource::_narrow(_rscobj);
     _rsc->stop();
     std::cout << "[Application::stop] Component Naming Context " << i << ": " << (*appComponentNamingContexts)[i].elementId << std::endl;
     CORBA::release(_rsc);
    }
    CORBA::release(_rscobj);
}


void Application_impl::initialize ()
throw (CORBA::SystemException, CF::LifeCycle::InitializeError)
{
    try {
        assemblyController->initialize ();
    } catch ( CF::LifeCycle::InitializeError& ie ) {
        std::cout << "[Application::initialize] \"assemblyController->initialize\" failed with CF::LifeCycle::InitializeError\n";
        // THROW CF::LifeCycle::InitializeError ??
        exit(EXIT_FAILURE);
    } catch ( CORBA::SystemException& se ) {
        std::cout << "[Application::initialize] \"assemblyController->initialize\" failed with CORBA::SystemException\n";
        // THROW CORBA::SystemException ??
        exit(EXIT_FAILURE);
    } catch ( ... ) {
        std::cout << "[Application::initialize] \"assemblyController->initialize\" failed with Unknown Exception\n";
        exit(EXIT_FAILURE);
    }
}


CORBA::Object_ptr Application_impl::getPort (const char* _id)
throw (CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    try {
        return assemblyController->getPort (_id);
    } catch ( CF::PortSupplier::UnknownPort& up ) {
        std::cout << "[Application::getPort] \"assemblyController->getPort\" failed with CF::PortSupplier::UnknownPort for ID " << _id << "\n";
        // THROW CF::PortSupplier::UnknownPort ??
        exit(EXIT_FAILURE);
    } catch ( CORBA::SystemException& se ) {
        std::cout << "[Application::getPort] \"assemblyController->getPort\" failed with CORBA::SystemException for ID " << _id << "\n";
        exit(EXIT_FAILURE);
    } catch ( ... ) {
        std::cout << "[Application::getPort] \"assemblyController->getPort\" failed with Unknown Exception for ID " << _id << "\n";
        exit(EXIT_FAILURE);
    }
}


void Application_impl::runTest (CORBA::ULong _testId, CF::Properties& _props)
throw (CORBA::SystemException, CF::UnknownProperties, CF::TestableObject::UnknownTest)

{
    try {
        assemblyController->runTest (_testId, _props);
    } catch ( CF::UnknownProperties& up ) {
        std::cout << "[Application::runTest] \"assemblyController->runTest\" failed with CF::UnknownProperties for Test ID " << _testId << "\n";
        // THROW CF::UnknownProperties ??
        exit(EXIT_FAILURE);
    } catch ( CF::TestableObject::UnknownTest& ut ) {
        std::cout << "[Application::runTest] \"assemblyController->runTest\" failed with CF::TestableObject::UnknownTest for Test ID " << _testId << "\n";
        // THROW CF::TestableObject::UnknownTest ??
        exit(EXIT_FAILURE);
    } catch ( ... ) {
        std::cout << "[Application::runTest] \"assemblyController->runTest\" failed with Unknown Exception for Test ID " << _testId << "\n";
        exit(EXIT_FAILURE);
    }
}


void Application_impl::releaseObject ()
throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    ossieSupport::ORB *orb;
    orb = new ossieSupport::ORB();

    CF::DomainManager_ptr dmnMgr;
    CORBA::Object_ptr obj;
    try {
        obj = orb->get_object_from_name("DomainName1/DomainManager");
    } catch ( CORBA::SystemException& se ) {
        std::cout << "[Application::releaseObject] \"orb->get_object_from_name\" failed with CORBA::SystemException\n";
        exit(EXIT_FAILURE);
    } catch ( ... ) {
        std::cout << "[Application::releaseObject] \"orb->get_object_from_name\" failed with Unknown Exception\n";
        exit(EXIT_FAILURE);
    }

    try {
        dmnMgr = CF::DomainManager::_narrow (obj);
    } catch ( ... ) {
        std::cout << "[Application::releaseObject] \"CF::DomainManager::_narrow\" failed with Unknown Exception\n";
        exit(EXIT_FAILURE);
    }

    CF::FileManager_var fileMgr;
    try {
        fileMgr = dmnMgr->fileMgr();
    } catch ( ... ) {
        std::cout << "[Application::releaseObject] \"dmnMgr->fileMgr\" failed with Unknown Exception\n";
        exit(EXIT_FAILURE);
    }

// Remove reference for this application from the ApplicationSequence in DomainManager
    int old_length = appseq->length();
    int index_found_app = -1;
    for (int i=0; i<old_length; i++) {
        if (!strcmp((*(*appseq)[i]->componentNamingContexts())[0].elementId, \
                    (*appComponentNamingContexts)[0].elementId)) {
            index_found_app = i;
            break;
        }
    }

    if (index_found_app == -1) {
        std::cout << "The DomainManager's application list is corrupt" << std::endl;
    } else {
        for (int i=index_found_app; i<old_length-1; i++) {
            (*appseq)[i] = (*appseq)[i+1];
        }

        appseq->length(old_length-1);
    }

    std::vector <CF::Device_ptr> registeredDevices;

    assemblyController = CF::Resource::_nil ();
    CF::File_var _sad;
    try {
        _sad = fileMgr->open( sadProfile.c_str(), true );
    } catch ( CF::InvalidFileName &_ex ) {
        std::cout << "[Application::releaseObject] While opening SAD: " << _ex.msg << "\n";
        exit(EXIT_FAILURE);
    } catch ( CF::FileException &_ex ) {
        std::cout << "[Application::releaseObject] While opening SAD: " << _ex.msg << "\n";
        exit(EXIT_FAILURE);
    } catch ( ... ) {
        std::cout << "[Application::releaseObject] While opening SAD: Unknown Exception\n";
        exit(EXIT_FAILURE);
    }

    sadParser = new SADParser ( _sad );

    try {
        _sad->close();
    } catch ( CORBA::SystemException& se ) {
        std::cout << "[Application::releaseObject] \"sad->close\" failed with CORBA::SystemException\n";
        exit(EXIT_FAILURE);
    } catch ( ... ) {
        std::cout << "[Application::releaseObject] \"sad->close\" failed with Unknown Exception\n";
        exit(EXIT_FAILURE);
    }

// Break all connections in the application
    for (unsigned int i = 0; i < connectionData.size(); i++) {
        CF::Port_var port;
        try {
            port = connectionData[i]->getPort();
        } catch ( CORBA::SystemException& se ) {
            std::cout << "[Application::releaseObject] \"connectionData[" << i << "]->getPort\" failed with CORBA::SystemException\n";
            exit(EXIT_FAILURE);
        } catch ( ... ) {
            std::cout << "[Application::releaseObject] \"connectionData[" << i << "]->getPort\" failed with Unknown Exception\n";
            exit(EXIT_FAILURE);
        }

        try {
            port->disconnectPort(connectionData[i]->getID());
        } catch ( CORBA::SystemException& se ) {
            std::cout << "[Application::releaseObject] \"port->disconnectPort\" failed with CORBA::SystemException for ID " << connectionData[i]->getID() << "\n";
            exit(EXIT_FAILURE);
        } catch ( ... ) {
            std::cout << "[Application::releaseObject] \"port->disconnectPort\" failed with Unknown Exception for ID " << connectionData[i]->getID() << "\n";
            exit(EXIT_FAILURE);
        }
    }

    DEBUG(2, App, "app->releaseObject finished disconnecting ports")

// Release all resources
// Before releasing the components, all executed processes should be terminated,
// all software components unloaded, and all capacities deallocated
// search thru all waveform components
// if there is a match with the deviceAssignmentSequence, then get a reference to the device
// unload and deallocate

    CF::DomainManager::DeviceManagerSequence* devMgrs;
    try {
        devMgrs = dmnMgr->deviceManagers ();
    } catch ( CORBA::SystemException& se ) {
        std::cout << "[Application::releaseObject] \"dmnMgr->deviceManagers\" failed with CORBA::SystemException\n";
        exit(EXIT_FAILURE);
    } catch ( ... ) {
        std::cout << "[Application::releaseObject] \"dmnMgr->deviceManagers\" failed with Unknown Exception\n";
        exit(EXIT_FAILURE);
    }

    for (unsigned int i=0; i<devMgrs->length(); i++) {
        CF::DeviceSequence* devices = (*devMgrs)[i]->registeredDevices();

        for (unsigned int j=0; j<devices->length(); j++) {
            registeredDevices.push_back((*devices)[j]);
        }
    }

// Search thru all waveform components
// if there is a match with the deviceAssignmentSequence, then get a reference to the device
// unload and deallocate
    std::vector < SADComponentPlacement * >*ComponentsVector = sadParser->getComponents ();

    std::string application_naming_context("");

    for (unsigned int _numComponentVector = 0; _numComponentVector < \
            ComponentsVector->size (); _numComponentVector++) {
        std::vector < SADComponentInstantiation * >*ComponentInstantiationVector = \
                (*ComponentsVector)[_numComponentVector]->getSADInstantiations ();
        int mem = -1;
        for (unsigned int i = 0; i < appComponentImplementations->length (); i++) {
            char *value1;
            char *value2;
            value1 =
                CORBA::string_dup ((*ComponentInstantiationVector)[0]->getID ());
            value2 =
                CORBA::string_dup ((*appComponentImplementations)[i].componentId);
            if (strcmp (value1, value2) == 0) {
                mem = i;
                break;
            }
        }

        if (mem == -1) continue;

        CORBA::Object_var _rscObj = CORBA::Object::_nil ();

        char *value3;
        value3 = CORBA::string_dup ((*appComponentNamingContexts)[mem].elementId);

        // This code assumes that all components are deployed under the same waveform naming context
        std::string my_component_name(CORBA::string_dup ((*appComponentNamingContexts)[mem].elementId));
        size_t location_first_slash = my_component_name.find('/', 0);
        size_t location_second_slash = my_component_name.find('/', location_first_slash+1);
        application_naming_context = "";
        application_naming_context.append(my_component_name, location_first_slash+1, \
                                          location_second_slash-location_first_slash-1);

        while (CORBA::is_nil (_rscObj)) {
            try {
                _rscObj = _orb->get_object_from_name ((*appComponentNamingContexts)[mem].elementId);
            } catch ( CORBA::SystemException& se ) {
                std::cout << "[Application::releaseObject] \"orb->get_object_from_name\" failed with CORBA::SystemException for appComponentNamingContexts[" << mem << "]." << value3 << "\n";
                exit(EXIT_FAILURE);
            } catch ( ... ) {
                std::cout << "[Application::releaseObject] \"orb->get_object_from_name\" failed with Unknown Exception for appComponentNamingContexts[" << mem << "]." << value3 << "\n";
                exit(EXIT_FAILURE);
            }
        }


        char *localFileName;
        localFileName = new char[256];
        localFileName[0]=0;
        CF::File_var _spd;
        try {
            _spd = fileMgr->open( sadParser->getSPDById( (*ComponentsVector)[_numComponentVector]->getFileRefId() ), true );
        } catch ( CF::InvalidFileName &_ex ) {
            std::cout << "[Application::releaseObject] While opening SPD: " << _ex.msg << "\n";
            exit(EXIT_FAILURE);
        } catch ( CF::FileException &_ex ) {
            std::cout << "[Application::releaseObject] While opening SPD: " << _ex.msg << "\n";
            exit(EXIT_FAILURE);
        } catch ( ... ) {
            std::cout << "[Application::releaseObject] While opening SPD: Unknown Exception\n";
            exit(EXIT_FAILURE);
        }

        SPDParser _spdParser ( _spd );
        try {
            _spd->close();
        } catch ( CORBA::SystemException& se ) {
            std::cout << "[Application::releaseObject] \"spd->close\" failed with CORBA::SystemException\n";
            exit(EXIT_FAILURE);
        } catch ( ... ) {
            std::cout << "[Application::releaseObject] \"spd->close\" failed with Unknown Exception\n";
            exit(EXIT_FAILURE);
        }

        std::vector <SPDImplementation *> *spd_i = _spdParser.getImplementations();

        CF::FileManager::MountSequence_var mts;
        try {
            mts = fileMgr->getMounts();
        } catch ( CORBA::SystemException& se ) {
            std::cout << "[Application::releaseObject] \"fileMgr->getMounts\" failed with CORBA::SystemException\n";
            exit(EXIT_FAILURE);
        } catch ( ... ) {
            std::cout << "[Application::releaseObject] \"fileMgr->getMounts\" failed with Unknown Exception\n";
            exit(EXIT_FAILURE);
        }

        int idx = -1;

        for ( int num_mounts = 0; num_mounts < mts->length(); num_mounts++ ) {
            try {
                if ( mts[num_mounts].fs->exists( (*spd_i)[0]->getCodeFile() ) ) {
                    idx = num_mounts;
                    break;
                }
            } catch ( CORBA::SystemException& se ) {
                std::cout << "[Application::releaseObject] \"mts[" << num_mounts << "].fs->exists\" for SPD " << (*spd_i)[0]->getCodeFile() << " failed with CORBA::SystemException\n";
                exit(EXIT_FAILURE);
            } catch ( ... ) {
                std::cout << "[Application::releaseObject] \"mts[" << num_mounts << "].fs->exists\" for SPD " << (*spd_i)[0]->getCodeFile() << " failed with Unknown Exception\n";
                exit(EXIT_FAILURE);
            }
        }

        strcpy (localFileName, CORBA::string_dup( mts[idx].mountPoint ) );
        strcat (localFileName, (*spd_i)[0]->getCodeFile());

        if ((*ComponentInstantiationVector)[0]->isResourceFactoryRef ()) {
            char *localName;
            localName = new char[256];
            localName[0]=0;
            CF::File_var _spd;
            try {
                _spd = fileMgr->open( sadParser->getSPDById( (*ComponentsVector)[_numComponentVector]->getFileRefId() ), true );
            } catch ( CF::InvalidFileName &_ex ) {
                std::cout << "[Application::releaseObject] While opening SPD for RscFac: " << _ex.msg << "\n";
                exit(EXIT_FAILURE);
            } catch ( CF::FileException &_ex ) {
                std::cout << "[Application::releaseObject] While opening SPD for RscFac: " << _ex.msg << "\n";
                exit(EXIT_FAILURE);
            } catch ( ... ) {
                std::cout << "[Application::releaseObject] While opening SPD for RscFac: Unknown Exception\n";
                exit(EXIT_FAILURE);
            }
            SPDParser _spdParser ( _spd );

            try {
                _spd->close();
            } catch ( CORBA::SystemException& se ) {
                std::cout << "[Application::releaseObject] \"spd->close\" 2 failed with CORBA::SystemException\n";
                exit(EXIT_FAILURE);
            } catch ( ... ) {
                std::cout << "[Application::releaseObject] \"spd->close\" 2 failed with Unknown Exception\n";
                exit(EXIT_FAILURE);
            }

            strcpy (localName, _spdParser.getSoftPkgName ());
            CF::ResourceFactory_ptr _rscFac;
            try {
                _rscFac = CF::ResourceFactory::_narrow (_rscObj);
            } catch ( ... ) {
                std::cout << "[Application::releaseObject] \"CF::ResourceFactory::_narrow\" failed with Unknown Exception\n";
                exit(EXIT_FAILURE);
            }

            try {
                _rscFac->releaseResource (CORBA::string_dup (_spdParser.getSoftPkgName ()));
            } catch ( CORBA::SystemException& se ) {
                std::cout << "[Application::releaseObject] \"rscFac->releaseResource\" failed with CORBA::SystemException for SPD " << _spdParser.getSoftPkgName() << "\n";
                exit(EXIT_FAILURE);
            } catch ( ... ) {
                std::cout << "[Application::releaseObject] \"rscFac->releaseResource\" failed with Unknown Exception for SPD " << _spdParser.getSoftPkgName() << "\n";
                exit(EXIT_FAILURE);
            }
        } else {
            CF::Resource_ptr _rsc;
            try {
                _rsc = CF::Resource::_narrow (_rscObj);
            } catch ( ... ) {
                std::cout << "[Application::releaseObject] \"CF::Resource::_narrow\" failed with Unknown Exception\n";
                exit(EXIT_FAILURE);
            }
            DEBUG(2, App, "about to release Object")

            try {
                _rsc->releaseObject ();
            } catch ( CORBA::SystemException& se ) {
                std::cout << "[Application::releaseObject] \"rsc->releaseObject\" failed with CORBA::SystemException\n";
                //exit(EXIT_FAILURE);
            } catch ( ... ) {
                std::cout << "[Application::releaseObject' \" rsc->releaseObject\" failed with Unknown Exception\n";
                //exit(EXIT_FAILURE);
            }
            DEBUG(3, App, "returned from releaseObject")
        }

        DEBUG(2, App, "app->releaseObject finished releasing this component")

        for (unsigned int devAssignIndex = 0; devAssignIndex < appComponentDevices->length (); devAssignIndex++) {
            int component_found = 0;
            if (strcmp((*appComponentDevices)[devAssignIndex].componentId, \
                       (*appComponentImplementations)[mem].componentId)==0) {
                CORBA::Object_var _devObj = CORBA::Object::_nil ();
                char assignedDeviceLabel[255];
                strcpy (assignedDeviceLabel, (*appComponentDevices)[devAssignIndex].assignedDeviceId);
                for (unsigned int i=0; i<registeredDevices.size(); i++) {
                    char* _regdevid = NULL;
                    try {
                        _regdevid = registeredDevices[i]->identifier();
                    } catch ( CORBA::SystemException& ) {
                        std::cout << "[Application::releaseObject] While calling \"identifier[" << i << "]\": CORBA::SystemException\n";
                        exit(EXIT_FAILURE);
                    } catch ( ... ) {
                        std::cout << "[Application::releaseObject] While calling \"identifier[" << i << "]\": Unknown Exception\n";
                        exit(EXIT_FAILURE);
                    }
                    if (strcmp(_regdevid, assignedDeviceLabel) == 0) {
                        _devObj= registeredDevices[i];
                    } else {
                        CORBA::string_free( _regdevid );
                        continue;
                    }

                    CORBA::string_free( _regdevid );

                    if (component_found) {
                        break;
                    } else {
                        component_found = 1;
                    }

                    if (strlen(localFileName)>0) {
                        CF::LoadableDevice_var loadDev;
                        try {
                            loadDev = CF::LoadableDevice::_narrow(_devObj);
                        } catch ( ... ) {
                            std::cout << "[Application::releaseObject] \"CF::LoadableDevice::_narrow\" failed with Unknown Exception\n";
                            exit(EXIT_FAILURE);
                        }
                        DEBUG(3, App, "Unloading: " << localFileName)
                        try {
                            loadDev->unload(localFileName);
                        } catch ( CORBA::SystemException& se ) {
                            std::cout << "[Application::releaseObject] \"loadDev->unload\" failed with CORBA::SystemException\n";
                            exit(EXIT_FAILURE);
                        } catch ( ... ) {
                            std::cout << "[Application::releaseObject] \"loadDev->unload\" failed with Unknown Exception\n";
                            exit(EXIT_FAILURE);
                        }
                    }
                }

                // Check if there is any process running for this component
                // if the componentId is contained in the processIdList, then terminate the process
                for (unsigned int i = 0; i<appComponentProcessIds->length(); i++) {
                    if ( strcmp( (*appComponentDevices)[devAssignIndex].componentId,\
                                 (*appComponentProcessIds)[i].componentId) == 0) {
                        DEBUG(3, App, "terminating")
                        CF::ExecutableDevice_var execDev;
                        try {
                            execDev = CF::ExecutableDevice::_narrow (_devObj);
                        } catch ( ... ) {
                            std::cout << "[Application::releaseObject] \"CF::ExecutableDevice::_narrow\" failed with CORBA::SystemException\n";
                            exit(EXIT_FAILURE);
                        }

                        try {
                            execDev ->terminate ((*appComponentProcessIds)[i].processId );
                        } catch ( CORBA::SystemException& se ) {
                            std::cout << "[Application::releaseObject] \"execDev->terminate\" failed with CORBA::SystemException for appComponentProcessIds[" << i << "]." << (*appComponentProcessIds)[i].processId << "\n";
                            exit(EXIT_FAILURE);
                        }
                        break;
                        // NOTE: We are not deleting the node containing the ProcessId
                        // Given that there's only one executable per component, the whole list
                        // will be deleted when the application gets destroyed
                    }
                }

                CF::Device_var dev;
                try {
                    dev = CF::Device::_narrow(_devObj);
                } catch ( ... ) {
                    std::cout << "[Application::releaseObject] \"CF::Device::_narrow\" failed with Unknown Exception\n";
                    exit(EXIT_FAILURE);
                }
                const char *PRFFileName = _spdParser.getPRFFile ();
                CF::File_var _prf;
                try {
                    _prf = fileMgr->open( PRFFileName, true );
                } catch ( CF::InvalidFileName &_ex ) {
                    std::cout << "[Application::releaseObject] While opening PRF: " << _ex.msg << "\n";
                    exit(EXIT_FAILURE);
                } catch ( CF::FileException &_ex ) {
                    std::cout << "[Application::releaseObject] While opening PRF: " << _ex.msg << "\n";
                    exit(EXIT_FAILURE);
                } catch ( ... ) {
                    std::cout << "[Application::releaseObject] While opening PRF: Unknown Exception\n";
                    exit(EXIT_FAILURE);
                }

                PRFParser PRFFile ( _prf );
                try {
                    _prf->close();
                } catch ( CORBA::SystemException& se ) {
                    std::cout << "[Application::releaseObject] \"prf->close\" failed with CORBA::SystemException\n";
                    exit(EXIT_FAILURE);
                } catch ( ... ) {
                    std::cout << "[Application::releaseObject] \"prf->close\" failed with Unknown Exception\n";
                    exit(EXIT_FAILURE);
                }
                std::vector <PRFProperty *> *prfSimpleProp = PRFFile.getMatchingProperties ();
                CF::Properties deallocCapacities (10);
                deallocCapacities.length ( prfSimpleProp->size ());
                for (unsigned int propIndex = 0; propIndex < prfSimpleProp->size (); propIndex++) {
                    CF::DataType *dt = (*prfSimpleProp)[propIndex]->getDataType ();
                    deallocCapacities[propIndex].id = CORBA::string_dup (dt->id);
                    deallocCapacities[propIndex].value = dt->value;
                    DEBUG(3, App, "Property name: " << deallocCapacities[propIndex].id)
                }

                DEBUG(3, App, "Number props: " << prfSimpleProp->size())
                if (prfSimpleProp->size()>0) {
                    DEBUG(3, App, "deallocating")
                    dev->deallocateCapacity (deallocCapacities);
                    DEBUG(3, App, "Finshed deallocating")
                } else {
                    DEBUG(3, App, "No capacity to deallocate")
                }
            }

            DEBUG(3, App, "Next component")
        }

        DEBUG(3, App, "End of component list")
    }

    std::string temp_waveform_context("DomainName1");
    CosNaming::Name_var cosName = orb->string_to_CosName(temp_waveform_context.c_str());
    CORBA::Object_var obj_DNContext;
    try {
        obj_DNContext = orb->inc->resolve(cosName);
    } catch ( CORBA::SystemException& se ) {
        std::cout << "[Application::releaseObject] \"orb->inc->resolve\" failed with CORBA::SystemException\n";
        exit(EXIT_FAILURE);
    } catch ( ... ) {
        std::cout << "[Application::releaseObject] \"orb->inc->resolve\" failed with Unknown Exception\n";
        exit(EXIT_FAILURE);
    }

    CosNaming::NamingContext_var DNContext;
    try {
        DNContext = CosNaming::NamingContext::_narrow(obj_DNContext);
    } catch ( ... ) {
        std::cout << "[Application::releaseObject] \"CosNaming::NamingContext::_narrow\" failed with Unknown Exception\n";
        exit(EXIT_FAILURE);
    }
    CosNaming::Name DNContextname;
    DNContextname.length(1);
    DNContextname[0].id = CORBA::string_dup(application_naming_context.c_str());
    try {
        DNContext->unbind(DNContextname);
    } catch ( CORBA::SystemException& se ) {
        std::cout << "[Application::releaseObject] \"DNContext->unbind\" failed with CORBA::SystemException\n";
        exit(EXIT_FAILURE);
    } catch ( ... ) {
        std::cout << "[Application::releaseObject] \"DNContext->unbind\" failed with CORBA::SystemException\n";
        exit(EXIT_FAILURE);
    }

    delete sadParser;

    sadParser = NULL;
}


char *Application_impl::name ()
throw (CORBA::SystemException)
{
    return CORBA::string_dup(appName.c_str());
}


char *Application_impl::profile ()
throw (CORBA::SystemException)
{
    return CORBA::string_dup(sadProfile.c_str());
}


PROC_ID_SEQ * Application_impl::componentProcessIds ()
throw (CORBA::SystemException)
{
    CF::Application::ComponentProcessIdSequence_var result = \
            new CF::Application::ComponentProcessIdSequence(*appComponentProcessIds);
    return result._retn();
}


ELEM_SEQ * Application_impl::componentNamingContexts ()
throw (CORBA::SystemException)
{
    CF::Application::ComponentElementSequence_var result = new ELEM_SEQ(*appComponentNamingContexts);
    return result._retn();
}


ELEM_SEQ * Application_impl::componentImplementations ()
throw (CORBA::SystemException)
{
    CF::Application::ComponentElementSequence_var result = new ELEM_SEQ(*appComponentImplementations);
    return result._retn();
}


CF::DeviceAssignmentSequence * Application_impl::componentDevices ()
throw (CORBA::SystemException)
{
    CF::DeviceAssignmentSequence_var result = new CF::DeviceAssignmentSequence(*appComponentDevices);
    return result._retn();
}

