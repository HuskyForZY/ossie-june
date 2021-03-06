// This file is generated by omniidl (C++ backend) - omniORB_4_1. Do not edit.

#include "soundControl.h"

OMNI_USING_NAMESPACE(omni)

static const char* _0RL_dyn_library_version = omniORB_4_1_dyn;

static ::CORBA::TypeCode::_Tracker _0RL_tcTrack(__FILE__);

static const char* _0RL_enumMember_standardInterfaces_maudioInControl_mInType[] = { "itMicrophone", "itNet", "itFile" };
static CORBA::TypeCode_ptr _0RL_tc_standardInterfaces_maudioInControl_mInType = CORBA::TypeCode::PR_enum_tc("IDL:standardInterfaces/audioInControl/InType:1.0", "InType", _0RL_enumMember_standardInterfaces_maudioInControl_mInType, 3, &_0RL_tcTrack);
const CORBA::TypeCode_ptr standardInterfaces::audioInControl::_tc_InType = _0RL_tc_standardInterfaces_maudioInControl_mInType;

#if defined(HAS_Cplusplus_Namespace) && defined(_MSC_VER)
// MSVC++ does not give the constant external linkage otherwise.
namespace standardInterfaces { 
  const ::CORBA::TypeCode_ptr _tc_audioInControl = CORBA::TypeCode::PR_interface_tc("IDL:standardInterfaces/audioInControl:1.0", "audioInControl", &_0RL_tcTrack);
} 
#else
const ::CORBA::TypeCode_ptr standardInterfaces::_tc_audioInControl = CORBA::TypeCode::PR_interface_tc("IDL:standardInterfaces/audioInControl:1.0", "audioInControl", &_0RL_tcTrack);
#endif

static const char* _0RL_enumMember_standardInterfaces_maudioOutControl_mOutType[] = { "otSpeaker", "otNet", "otFile" };
static CORBA::TypeCode_ptr _0RL_tc_standardInterfaces_maudioOutControl_mOutType = CORBA::TypeCode::PR_enum_tc("IDL:standardInterfaces/audioOutControl/OutType:1.0", "OutType", _0RL_enumMember_standardInterfaces_maudioOutControl_mOutType, 3, &_0RL_tcTrack);
const CORBA::TypeCode_ptr standardInterfaces::audioOutControl::_tc_OutType = _0RL_tc_standardInterfaces_maudioOutControl_mOutType;

#if defined(HAS_Cplusplus_Namespace) && defined(_MSC_VER)
// MSVC++ does not give the constant external linkage otherwise.
namespace standardInterfaces { 
  const ::CORBA::TypeCode_ptr _tc_audioOutControl = CORBA::TypeCode::PR_interface_tc("IDL:standardInterfaces/audioOutControl:1.0", "audioOutControl", &_0RL_tcTrack);
} 
#else
const ::CORBA::TypeCode_ptr standardInterfaces::_tc_audioOutControl = CORBA::TypeCode::PR_interface_tc("IDL:standardInterfaces/audioOutControl:1.0", "audioOutControl", &_0RL_tcTrack);
#endif

static void _0RL_standardInterfaces_maudioInControl_mInType_marshal_fn(cdrStream& _s, void* _v)
{
  standardInterfaces::audioInControl::InType* _p = (standardInterfaces::audioInControl::InType*)_v;
  *_p >>= _s;
}
static void _0RL_standardInterfaces_maudioInControl_mInType_unmarshal_fn(cdrStream& _s, void*& _v)
{
  standardInterfaces::audioInControl::InType* _p = (standardInterfaces::audioInControl::InType*)_v;
  *_p <<= _s;
}

void operator<<=(::CORBA::Any& _a, standardInterfaces::audioInControl::InType _s)
{
  _a.PR_insert(_0RL_tc_standardInterfaces_maudioInControl_mInType,
               _0RL_standardInterfaces_maudioInControl_mInType_marshal_fn,
               &_s);
}

::CORBA::Boolean operator>>=(const ::CORBA::Any& _a, standardInterfaces::audioInControl::InType& _s)
{
  return _a.PR_extract(_0RL_tc_standardInterfaces_maudioInControl_mInType,
                       _0RL_standardInterfaces_maudioInControl_mInType_unmarshal_fn,
                       &_s);
}

static void _0RL_standardInterfaces_maudioInControl_marshal_fn(cdrStream& _s, void* _v)
{
  omniObjRef* _o = (omniObjRef*)_v;
  omniObjRef::_marshal(_o, _s);
}
static void _0RL_standardInterfaces_maudioInControl_unmarshal_fn(cdrStream& _s, void*& _v)
{
  omniObjRef* _o = omniObjRef::_unMarshal(standardInterfaces::audioInControl::_PD_repoId, _s);
  _v = _o;
}
static void _0RL_standardInterfaces_maudioInControl_destructor_fn(void* _v)
{
  omniObjRef* _o = (omniObjRef*)_v;
  if (_o)
    omni::releaseObjRef(_o);
}

void operator<<=(::CORBA::Any& _a, standardInterfaces::audioInControl_ptr _o)
{
  standardInterfaces::audioInControl_ptr _no = standardInterfaces::audioInControl::_duplicate(_o);
  _a.PR_insert(standardInterfaces::_tc_audioInControl,
               _0RL_standardInterfaces_maudioInControl_marshal_fn,
               _0RL_standardInterfaces_maudioInControl_destructor_fn,
               _no->_PR_getobj());
}
void operator<<=(::CORBA::Any& _a, standardInterfaces::audioInControl_ptr* _op)
{
  _a.PR_insert(standardInterfaces::_tc_audioInControl,
               _0RL_standardInterfaces_maudioInControl_marshal_fn,
               _0RL_standardInterfaces_maudioInControl_destructor_fn,
               (*_op)->_PR_getobj());
  *_op = standardInterfaces::audioInControl::_nil();
}

::CORBA::Boolean operator>>=(const ::CORBA::Any& _a, standardInterfaces::audioInControl_ptr& _o)
{
  void* _v;
  if (_a.PR_extract(standardInterfaces::_tc_audioInControl,
                    _0RL_standardInterfaces_maudioInControl_unmarshal_fn,
                    _0RL_standardInterfaces_maudioInControl_marshal_fn,
                    _0RL_standardInterfaces_maudioInControl_destructor_fn,
                    _v)) {
    omniObjRef* _r = (omniObjRef*)_v;
    if (_r)
      _o = (standardInterfaces::audioInControl_ptr)_r->_ptrToObjRef(standardInterfaces::audioInControl::_PD_repoId);
    else
      _o = standardInterfaces::audioInControl::_nil();
    return 1;
  }
  return 0;
}

static void _0RL_standardInterfaces_maudioOutControl_mOutType_marshal_fn(cdrStream& _s, void* _v)
{
  standardInterfaces::audioOutControl::OutType* _p = (standardInterfaces::audioOutControl::OutType*)_v;
  *_p >>= _s;
}
static void _0RL_standardInterfaces_maudioOutControl_mOutType_unmarshal_fn(cdrStream& _s, void*& _v)
{
  standardInterfaces::audioOutControl::OutType* _p = (standardInterfaces::audioOutControl::OutType*)_v;
  *_p <<= _s;
}

void operator<<=(::CORBA::Any& _a, standardInterfaces::audioOutControl::OutType _s)
{
  _a.PR_insert(_0RL_tc_standardInterfaces_maudioOutControl_mOutType,
               _0RL_standardInterfaces_maudioOutControl_mOutType_marshal_fn,
               &_s);
}

::CORBA::Boolean operator>>=(const ::CORBA::Any& _a, standardInterfaces::audioOutControl::OutType& _s)
{
  return _a.PR_extract(_0RL_tc_standardInterfaces_maudioOutControl_mOutType,
                       _0RL_standardInterfaces_maudioOutControl_mOutType_unmarshal_fn,
                       &_s);
}

static void _0RL_standardInterfaces_maudioOutControl_marshal_fn(cdrStream& _s, void* _v)
{
  omniObjRef* _o = (omniObjRef*)_v;
  omniObjRef::_marshal(_o, _s);
}
static void _0RL_standardInterfaces_maudioOutControl_unmarshal_fn(cdrStream& _s, void*& _v)
{
  omniObjRef* _o = omniObjRef::_unMarshal(standardInterfaces::audioOutControl::_PD_repoId, _s);
  _v = _o;
}
static void _0RL_standardInterfaces_maudioOutControl_destructor_fn(void* _v)
{
  omniObjRef* _o = (omniObjRef*)_v;
  if (_o)
    omni::releaseObjRef(_o);
}

void operator<<=(::CORBA::Any& _a, standardInterfaces::audioOutControl_ptr _o)
{
  standardInterfaces::audioOutControl_ptr _no = standardInterfaces::audioOutControl::_duplicate(_o);
  _a.PR_insert(standardInterfaces::_tc_audioOutControl,
               _0RL_standardInterfaces_maudioOutControl_marshal_fn,
               _0RL_standardInterfaces_maudioOutControl_destructor_fn,
               _no->_PR_getobj());
}
void operator<<=(::CORBA::Any& _a, standardInterfaces::audioOutControl_ptr* _op)
{
  _a.PR_insert(standardInterfaces::_tc_audioOutControl,
               _0RL_standardInterfaces_maudioOutControl_marshal_fn,
               _0RL_standardInterfaces_maudioOutControl_destructor_fn,
               (*_op)->_PR_getobj());
  *_op = standardInterfaces::audioOutControl::_nil();
}

::CORBA::Boolean operator>>=(const ::CORBA::Any& _a, standardInterfaces::audioOutControl_ptr& _o)
{
  void* _v;
  if (_a.PR_extract(standardInterfaces::_tc_audioOutControl,
                    _0RL_standardInterfaces_maudioOutControl_unmarshal_fn,
                    _0RL_standardInterfaces_maudioOutControl_marshal_fn,
                    _0RL_standardInterfaces_maudioOutControl_destructor_fn,
                    _v)) {
    omniObjRef* _r = (omniObjRef*)_v;
    if (_r)
      _o = (standardInterfaces::audioOutControl_ptr)_r->_ptrToObjRef(standardInterfaces::audioOutControl::_PD_repoId);
    else
      _o = standardInterfaces::audioOutControl::_nil();
    return 1;
  }
  return 0;
}

