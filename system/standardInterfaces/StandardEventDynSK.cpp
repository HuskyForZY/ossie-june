// This file is generated by omniidl (C++ backend) - omniORB_4_1. Do not edit.

#include "StandardEvent.h"

OMNI_USING_NAMESPACE(omni)

static const char* _0RL_dyn_library_version = omniORB_4_1_dyn;

static ::CORBA::TypeCode::_Tracker _0RL_tcTrack(__FILE__);

static const char* _0RL_enumMember_StandardEvent_mStateChangeCategoryType[] = { "ADMINISTRATIVE_STATE_EVENT", "OPERATIONAL_STATE_EVENT", "USAGE_STATE_EVENT" };
static CORBA::TypeCode_ptr _0RL_tc_StandardEvent_mStateChangeCategoryType = CORBA::TypeCode::PR_enum_tc("IDL:StandardEvent/StateChangeCategoryType:1.0", "StateChangeCategoryType", _0RL_enumMember_StandardEvent_mStateChangeCategoryType, 3, &_0RL_tcTrack);
#if defined(HAS_Cplusplus_Namespace) && defined(_MSC_VER)
// MSVC++ does not give the constant external linkage otherwise.
namespace StandardEvent { 
  const ::CORBA::TypeCode_ptr _tc_StateChangeCategoryType = _0RL_tc_StandardEvent_mStateChangeCategoryType;
} 
#else
const ::CORBA::TypeCode_ptr StandardEvent::_tc_StateChangeCategoryType = _0RL_tc_StandardEvent_mStateChangeCategoryType;
#endif

static const char* _0RL_enumMember_StandardEvent_mStateChangeType[] = { "LOCKED", "UNLOCKED", "SHUTTING_DOWN", "ENABLED", "DISABLED", "IDLE", "ACTIVE", "BUSY" };
static CORBA::TypeCode_ptr _0RL_tc_StandardEvent_mStateChangeType = CORBA::TypeCode::PR_enum_tc("IDL:StandardEvent/StateChangeType:1.0", "StateChangeType", _0RL_enumMember_StandardEvent_mStateChangeType, 8, &_0RL_tcTrack);
#if defined(HAS_Cplusplus_Namespace) && defined(_MSC_VER)
// MSVC++ does not give the constant external linkage otherwise.
namespace StandardEvent { 
  const ::CORBA::TypeCode_ptr _tc_StateChangeType = _0RL_tc_StandardEvent_mStateChangeType;
} 
#else
const ::CORBA::TypeCode_ptr StandardEvent::_tc_StateChangeType = _0RL_tc_StandardEvent_mStateChangeType;
#endif

static CORBA::PR_structMember _0RL_structmember_StandardEvent_mStateChangeEventType[] = {
  {"producerId", CORBA::TypeCode::PR_string_tc(0, &_0RL_tcTrack)},
  {"sourceId", CORBA::TypeCode::PR_string_tc(0, &_0RL_tcTrack)},
  {"stateChangeCategory", _0RL_tc_StandardEvent_mStateChangeCategoryType},
  {"stateChangeFrom", _0RL_tc_StandardEvent_mStateChangeType},
  {"stateChangeTo", _0RL_tc_StandardEvent_mStateChangeType}
};

#ifdef _0RL_tc_StandardEvent_mStateChangeEventType
#  undef _0RL_tc_StandardEvent_mStateChangeEventType
#endif
static CORBA::TypeCode_ptr _0RL_tc_StandardEvent_mStateChangeEventType = CORBA::TypeCode::PR_struct_tc("IDL:StandardEvent/StateChangeEventType:1.0", "StateChangeEventType", _0RL_structmember_StandardEvent_mStateChangeEventType, 5, &_0RL_tcTrack);

#if defined(HAS_Cplusplus_Namespace) && defined(_MSC_VER)
// MSVC++ does not give the constant external linkage otherwise.
namespace StandardEvent { 
  const ::CORBA::TypeCode_ptr _tc_StateChangeEventType = _0RL_tc_StandardEvent_mStateChangeEventType;
} 
#else
const ::CORBA::TypeCode_ptr StandardEvent::_tc_StateChangeEventType = _0RL_tc_StandardEvent_mStateChangeEventType;
#endif


static const char* _0RL_enumMember_StandardEvent_mSourceCategoryType[] = { "DEVICE_MANAGER", "DEVICE", "APPLICATION_FACTORY", "APPLICATION", "SERVICE" };
static CORBA::TypeCode_ptr _0RL_tc_StandardEvent_mSourceCategoryType = CORBA::TypeCode::PR_enum_tc("IDL:StandardEvent/SourceCategoryType:1.0", "SourceCategoryType", _0RL_enumMember_StandardEvent_mSourceCategoryType, 5, &_0RL_tcTrack);
#if defined(HAS_Cplusplus_Namespace) && defined(_MSC_VER)
// MSVC++ does not give the constant external linkage otherwise.
namespace StandardEvent { 
  const ::CORBA::TypeCode_ptr _tc_SourceCategoryType = _0RL_tc_StandardEvent_mSourceCategoryType;
} 
#else
const ::CORBA::TypeCode_ptr StandardEvent::_tc_SourceCategoryType = _0RL_tc_StandardEvent_mSourceCategoryType;
#endif

static CORBA::PR_structMember _0RL_structmember_StandardEvent_mDomainManagementObjectRemovedEventType[] = {
  {"producerId", CORBA::TypeCode::PR_string_tc(0, &_0RL_tcTrack)},
  {"sourceId", CORBA::TypeCode::PR_string_tc(0, &_0RL_tcTrack)},
  {"sourceName", CORBA::TypeCode::PR_string_tc(0, &_0RL_tcTrack)},
  {"sourceCategory", _0RL_tc_StandardEvent_mSourceCategoryType}
};

#ifdef _0RL_tc_StandardEvent_mDomainManagementObjectRemovedEventType
#  undef _0RL_tc_StandardEvent_mDomainManagementObjectRemovedEventType
#endif
static CORBA::TypeCode_ptr _0RL_tc_StandardEvent_mDomainManagementObjectRemovedEventType = CORBA::TypeCode::PR_struct_tc("IDL:StandardEvent/DomainManagementObjectRemovedEventType:1.0", "DomainManagementObjectRemovedEventType", _0RL_structmember_StandardEvent_mDomainManagementObjectRemovedEventType, 4, &_0RL_tcTrack);

#if defined(HAS_Cplusplus_Namespace) && defined(_MSC_VER)
// MSVC++ does not give the constant external linkage otherwise.
namespace StandardEvent { 
  const ::CORBA::TypeCode_ptr _tc_DomainManagementObjectRemovedEventType = _0RL_tc_StandardEvent_mDomainManagementObjectRemovedEventType;
} 
#else
const ::CORBA::TypeCode_ptr StandardEvent::_tc_DomainManagementObjectRemovedEventType = _0RL_tc_StandardEvent_mDomainManagementObjectRemovedEventType;
#endif


static CORBA::PR_structMember _0RL_structmember_StandardEvent_mDomainManagementObjectAddedEventType[] = {
  {"producerId", CORBA::TypeCode::PR_string_tc(0, &_0RL_tcTrack)},
  {"sourceId", CORBA::TypeCode::PR_string_tc(0, &_0RL_tcTrack)},
  {"sourceName", CORBA::TypeCode::PR_string_tc(0, &_0RL_tcTrack)},
  {"sourceCategory", _0RL_tc_StandardEvent_mSourceCategoryType},
  {"sourceIOR", CORBA::TypeCode::PR_Object_tc()}
};

#ifdef _0RL_tc_StandardEvent_mDomainManagementObjectAddedEventType
#  undef _0RL_tc_StandardEvent_mDomainManagementObjectAddedEventType
#endif
static CORBA::TypeCode_ptr _0RL_tc_StandardEvent_mDomainManagementObjectAddedEventType = CORBA::TypeCode::PR_struct_tc("IDL:StandardEvent/DomainManagementObjectAddedEventType:1.0", "DomainManagementObjectAddedEventType", _0RL_structmember_StandardEvent_mDomainManagementObjectAddedEventType, 5, &_0RL_tcTrack);

#if defined(HAS_Cplusplus_Namespace) && defined(_MSC_VER)
// MSVC++ does not give the constant external linkage otherwise.
namespace StandardEvent { 
  const ::CORBA::TypeCode_ptr _tc_DomainManagementObjectAddedEventType = _0RL_tc_StandardEvent_mDomainManagementObjectAddedEventType;
} 
#else
const ::CORBA::TypeCode_ptr StandardEvent::_tc_DomainManagementObjectAddedEventType = _0RL_tc_StandardEvent_mDomainManagementObjectAddedEventType;
#endif


static void _0RL_StandardEvent_mStateChangeCategoryType_marshal_fn(cdrStream& _s, void* _v)
{
  StandardEvent::StateChangeCategoryType* _p = (StandardEvent::StateChangeCategoryType*)_v;
  *_p >>= _s;
}
static void _0RL_StandardEvent_mStateChangeCategoryType_unmarshal_fn(cdrStream& _s, void*& _v)
{
  StandardEvent::StateChangeCategoryType* _p = (StandardEvent::StateChangeCategoryType*)_v;
  *_p <<= _s;
}

void operator<<=(::CORBA::Any& _a, StandardEvent::StateChangeCategoryType _s)
{
  _a.PR_insert(_0RL_tc_StandardEvent_mStateChangeCategoryType,
               _0RL_StandardEvent_mStateChangeCategoryType_marshal_fn,
               &_s);
}

::CORBA::Boolean operator>>=(const ::CORBA::Any& _a, StandardEvent::StateChangeCategoryType& _s)
{
  return _a.PR_extract(_0RL_tc_StandardEvent_mStateChangeCategoryType,
                       _0RL_StandardEvent_mStateChangeCategoryType_unmarshal_fn,
                       &_s);
}

static void _0RL_StandardEvent_mStateChangeType_marshal_fn(cdrStream& _s, void* _v)
{
  StandardEvent::StateChangeType* _p = (StandardEvent::StateChangeType*)_v;
  *_p >>= _s;
}
static void _0RL_StandardEvent_mStateChangeType_unmarshal_fn(cdrStream& _s, void*& _v)
{
  StandardEvent::StateChangeType* _p = (StandardEvent::StateChangeType*)_v;
  *_p <<= _s;
}

void operator<<=(::CORBA::Any& _a, StandardEvent::StateChangeType _s)
{
  _a.PR_insert(_0RL_tc_StandardEvent_mStateChangeType,
               _0RL_StandardEvent_mStateChangeType_marshal_fn,
               &_s);
}

::CORBA::Boolean operator>>=(const ::CORBA::Any& _a, StandardEvent::StateChangeType& _s)
{
  return _a.PR_extract(_0RL_tc_StandardEvent_mStateChangeType,
                       _0RL_StandardEvent_mStateChangeType_unmarshal_fn,
                       &_s);
}

static void _0RL_StandardEvent_mStateChangeEventType_marshal_fn(cdrStream& _s, void* _v)
{
  StandardEvent::StateChangeEventType* _p = (StandardEvent::StateChangeEventType*)_v;
  *_p >>= _s;
}
static void _0RL_StandardEvent_mStateChangeEventType_unmarshal_fn(cdrStream& _s, void*& _v)
{
  StandardEvent::StateChangeEventType* _p = new StandardEvent::StateChangeEventType;
  *_p <<= _s;
  _v = _p;
}
static void _0RL_StandardEvent_mStateChangeEventType_destructor_fn(void* _v)
{
  StandardEvent::StateChangeEventType* _p = (StandardEvent::StateChangeEventType*)_v;
  delete _p;
}

void operator<<=(::CORBA::Any& _a, const StandardEvent::StateChangeEventType& _s)
{
  StandardEvent::StateChangeEventType* _p = new StandardEvent::StateChangeEventType(_s);
  _a.PR_insert(_0RL_tc_StandardEvent_mStateChangeEventType,
               _0RL_StandardEvent_mStateChangeEventType_marshal_fn,
               _0RL_StandardEvent_mStateChangeEventType_destructor_fn,
               _p);
}
void operator<<=(::CORBA::Any& _a, StandardEvent::StateChangeEventType* _sp)
{
  _a.PR_insert(_0RL_tc_StandardEvent_mStateChangeEventType,
               _0RL_StandardEvent_mStateChangeEventType_marshal_fn,
               _0RL_StandardEvent_mStateChangeEventType_destructor_fn,
               _sp);
}

::CORBA::Boolean operator>>=(const ::CORBA::Any& _a, StandardEvent::StateChangeEventType*& _sp)
{
  return _a >>= (const StandardEvent::StateChangeEventType*&) _sp;
}
::CORBA::Boolean operator>>=(const ::CORBA::Any& _a, const StandardEvent::StateChangeEventType*& _sp)
{
  void* _v;
  if (_a.PR_extract(_0RL_tc_StandardEvent_mStateChangeEventType,
                    _0RL_StandardEvent_mStateChangeEventType_unmarshal_fn,
                    _0RL_StandardEvent_mStateChangeEventType_marshal_fn,
                    _0RL_StandardEvent_mStateChangeEventType_destructor_fn,
                    _v)) {
    _sp = (const StandardEvent::StateChangeEventType*)_v;
    return 1;
  }
  return 0;
}

static void _0RL_StandardEvent_mSourceCategoryType_marshal_fn(cdrStream& _s, void* _v)
{
  StandardEvent::SourceCategoryType* _p = (StandardEvent::SourceCategoryType*)_v;
  *_p >>= _s;
}
static void _0RL_StandardEvent_mSourceCategoryType_unmarshal_fn(cdrStream& _s, void*& _v)
{
  StandardEvent::SourceCategoryType* _p = (StandardEvent::SourceCategoryType*)_v;
  *_p <<= _s;
}

void operator<<=(::CORBA::Any& _a, StandardEvent::SourceCategoryType _s)
{
  _a.PR_insert(_0RL_tc_StandardEvent_mSourceCategoryType,
               _0RL_StandardEvent_mSourceCategoryType_marshal_fn,
               &_s);
}

::CORBA::Boolean operator>>=(const ::CORBA::Any& _a, StandardEvent::SourceCategoryType& _s)
{
  return _a.PR_extract(_0RL_tc_StandardEvent_mSourceCategoryType,
                       _0RL_StandardEvent_mSourceCategoryType_unmarshal_fn,
                       &_s);
}

static void _0RL_StandardEvent_mDomainManagementObjectRemovedEventType_marshal_fn(cdrStream& _s, void* _v)
{
  StandardEvent::DomainManagementObjectRemovedEventType* _p = (StandardEvent::DomainManagementObjectRemovedEventType*)_v;
  *_p >>= _s;
}
static void _0RL_StandardEvent_mDomainManagementObjectRemovedEventType_unmarshal_fn(cdrStream& _s, void*& _v)
{
  StandardEvent::DomainManagementObjectRemovedEventType* _p = new StandardEvent::DomainManagementObjectRemovedEventType;
  *_p <<= _s;
  _v = _p;
}
static void _0RL_StandardEvent_mDomainManagementObjectRemovedEventType_destructor_fn(void* _v)
{
  StandardEvent::DomainManagementObjectRemovedEventType* _p = (StandardEvent::DomainManagementObjectRemovedEventType*)_v;
  delete _p;
}

void operator<<=(::CORBA::Any& _a, const StandardEvent::DomainManagementObjectRemovedEventType& _s)
{
  StandardEvent::DomainManagementObjectRemovedEventType* _p = new StandardEvent::DomainManagementObjectRemovedEventType(_s);
  _a.PR_insert(_0RL_tc_StandardEvent_mDomainManagementObjectRemovedEventType,
               _0RL_StandardEvent_mDomainManagementObjectRemovedEventType_marshal_fn,
               _0RL_StandardEvent_mDomainManagementObjectRemovedEventType_destructor_fn,
               _p);
}
void operator<<=(::CORBA::Any& _a, StandardEvent::DomainManagementObjectRemovedEventType* _sp)
{
  _a.PR_insert(_0RL_tc_StandardEvent_mDomainManagementObjectRemovedEventType,
               _0RL_StandardEvent_mDomainManagementObjectRemovedEventType_marshal_fn,
               _0RL_StandardEvent_mDomainManagementObjectRemovedEventType_destructor_fn,
               _sp);
}

::CORBA::Boolean operator>>=(const ::CORBA::Any& _a, StandardEvent::DomainManagementObjectRemovedEventType*& _sp)
{
  return _a >>= (const StandardEvent::DomainManagementObjectRemovedEventType*&) _sp;
}
::CORBA::Boolean operator>>=(const ::CORBA::Any& _a, const StandardEvent::DomainManagementObjectRemovedEventType*& _sp)
{
  void* _v;
  if (_a.PR_extract(_0RL_tc_StandardEvent_mDomainManagementObjectRemovedEventType,
                    _0RL_StandardEvent_mDomainManagementObjectRemovedEventType_unmarshal_fn,
                    _0RL_StandardEvent_mDomainManagementObjectRemovedEventType_marshal_fn,
                    _0RL_StandardEvent_mDomainManagementObjectRemovedEventType_destructor_fn,
                    _v)) {
    _sp = (const StandardEvent::DomainManagementObjectRemovedEventType*)_v;
    return 1;
  }
  return 0;
}

static void _0RL_StandardEvent_mDomainManagementObjectAddedEventType_marshal_fn(cdrStream& _s, void* _v)
{
  StandardEvent::DomainManagementObjectAddedEventType* _p = (StandardEvent::DomainManagementObjectAddedEventType*)_v;
  *_p >>= _s;
}
static void _0RL_StandardEvent_mDomainManagementObjectAddedEventType_unmarshal_fn(cdrStream& _s, void*& _v)
{
  StandardEvent::DomainManagementObjectAddedEventType* _p = new StandardEvent::DomainManagementObjectAddedEventType;
  *_p <<= _s;
  _v = _p;
}
static void _0RL_StandardEvent_mDomainManagementObjectAddedEventType_destructor_fn(void* _v)
{
  StandardEvent::DomainManagementObjectAddedEventType* _p = (StandardEvent::DomainManagementObjectAddedEventType*)_v;
  delete _p;
}

void operator<<=(::CORBA::Any& _a, const StandardEvent::DomainManagementObjectAddedEventType& _s)
{
  StandardEvent::DomainManagementObjectAddedEventType* _p = new StandardEvent::DomainManagementObjectAddedEventType(_s);
  _a.PR_insert(_0RL_tc_StandardEvent_mDomainManagementObjectAddedEventType,
               _0RL_StandardEvent_mDomainManagementObjectAddedEventType_marshal_fn,
               _0RL_StandardEvent_mDomainManagementObjectAddedEventType_destructor_fn,
               _p);
}
void operator<<=(::CORBA::Any& _a, StandardEvent::DomainManagementObjectAddedEventType* _sp)
{
  _a.PR_insert(_0RL_tc_StandardEvent_mDomainManagementObjectAddedEventType,
               _0RL_StandardEvent_mDomainManagementObjectAddedEventType_marshal_fn,
               _0RL_StandardEvent_mDomainManagementObjectAddedEventType_destructor_fn,
               _sp);
}

::CORBA::Boolean operator>>=(const ::CORBA::Any& _a, StandardEvent::DomainManagementObjectAddedEventType*& _sp)
{
  return _a >>= (const StandardEvent::DomainManagementObjectAddedEventType*&) _sp;
}
::CORBA::Boolean operator>>=(const ::CORBA::Any& _a, const StandardEvent::DomainManagementObjectAddedEventType*& _sp)
{
  void* _v;
  if (_a.PR_extract(_0RL_tc_StandardEvent_mDomainManagementObjectAddedEventType,
                    _0RL_StandardEvent_mDomainManagementObjectAddedEventType_unmarshal_fn,
                    _0RL_StandardEvent_mDomainManagementObjectAddedEventType_marshal_fn,
                    _0RL_StandardEvent_mDomainManagementObjectAddedEventType_destructor_fn,
                    _v)) {
    _sp = (const StandardEvent::DomainManagementObjectAddedEventType*)_v;
    return 1;
  }
  return 0;
}

