// This file is generated by omniidl (C++ backend)- omniORB_4_1. Do not edit.
#ifndef __Radio__Control_hh__
#define __Radio__Control_hh__

#ifndef __CORBA_H_EXTERNAL_GUARD__
#include <omniORB4/CORBA.h>
#endif

#ifndef  USE_stub_in_nt_dll
# define USE_stub_in_nt_dll_NOT_DEFINED_Radio__Control
#endif
#ifndef  USE_core_stub_in_nt_dll
# define USE_core_stub_in_nt_dll_NOT_DEFINED_Radio__Control
#endif
#ifndef  USE_dyn_stub_in_nt_dll
# define USE_dyn_stub_in_nt_dll_NOT_DEFINED_Radio__Control
#endif



#ifndef __cf_hh_EXTERNAL_GUARD__
#define __cf_hh_EXTERNAL_GUARD__
#include "ossie/cf.h"
#endif



#ifdef USE_stub_in_nt_dll
# ifndef USE_core_stub_in_nt_dll
#  define USE_core_stub_in_nt_dll
# endif
# ifndef USE_dyn_stub_in_nt_dll
#  define USE_dyn_stub_in_nt_dll
# endif
#endif

#ifdef _core_attr
# error "A local CPP macro _core_attr has already been defined."
#else
# ifdef  USE_core_stub_in_nt_dll
#  define _core_attr _OMNIORB_NTDLL_IMPORT
# else
#  define _core_attr
# endif
#endif

#ifdef _dyn_attr
# error "A local CPP macro _dyn_attr has already been defined."
#else
# ifdef  USE_dyn_stub_in_nt_dll
#  define _dyn_attr _OMNIORB_NTDLL_IMPORT
# else
#  define _dyn_attr
# endif
#endif





_CORBA_MODULE standardInterfaces

_CORBA_MODULE_BEG

#ifndef __standardInterfaces_mRadio__Control__
#define __standardInterfaces_mRadio__Control__

  class Radio_Control;
  class _objref_Radio_Control;
  class _impl_Radio_Control;
  
  typedef _objref_Radio_Control* Radio_Control_ptr;
  typedef Radio_Control_ptr Radio_ControlRef;

  class Radio_Control_Helper {
  public:
    typedef Radio_Control_ptr _ptr_type;

    static _ptr_type _nil();
    static _CORBA_Boolean is_nil(_ptr_type);
    static void release(_ptr_type);
    static void duplicate(_ptr_type);
    static void marshalObjRef(_ptr_type, cdrStream&);
    static _ptr_type unmarshalObjRef(cdrStream&);
  };

  typedef _CORBA_ObjRef_Var<_objref_Radio_Control, Radio_Control_Helper> Radio_Control_var;
  typedef _CORBA_ObjRef_OUT_arg<_objref_Radio_Control,Radio_Control_Helper > Radio_Control_out;

#endif

  // interface Radio_Control
  class Radio_Control {
  public:
    // Declarations for this interface type.
    typedef Radio_Control_ptr _ptr_type;
    typedef Radio_Control_var _var_type;

    static _ptr_type _duplicate(_ptr_type);
    static _ptr_type _narrow(::CORBA::Object_ptr);
    static _ptr_type _unchecked_narrow(::CORBA::Object_ptr);
    
    static _ptr_type _nil();

    static inline void _marshalObjRef(_ptr_type, cdrStream&);

    static inline _ptr_type _unmarshalObjRef(cdrStream& s) {
      omniObjRef* o = omniObjRef::_unMarshal(_PD_repoId,s);
      if (o)
        return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
      else
        return _nil();
    }

    static _core_attr const char* _PD_repoId;

    // Other IDL defined within this scope.
    
  };

  class _objref_Radio_Control :
    public virtual ::CORBA::Object,
    public virtual omniObjRef
  {
  public:
    void set_number_of_channels(::CORBA::ULong num);
    void get_number_of_channels(::CORBA::ULong& num);
    void get_gain_range(::CORBA::ULong channel, ::CORBA::Float& gmin, ::CORBA::Float& gmax, ::CORBA::Float& gstep);
    void set_gain(::CORBA::ULong channel, ::CORBA::Float gain);
    void get_gain(::CORBA::ULong channel, ::CORBA::Float& gain);
    void get_frequency_range(::CORBA::ULong channel, ::CORBA::Float& fmin, ::CORBA::Float& fmax, ::CORBA::Float& fstep);
    void set_frequency(::CORBA::ULong channel, ::CORBA::Float f);
    void get_frequency(::CORBA::ULong channel, ::CORBA::Float& f);
    void start(::CORBA::ULong channel);
    void stop(::CORBA::ULong channel);
    void set_values(const CF::Properties& values);

    inline _objref_Radio_Control()  { _PR_setobj(0); }  // nil
    _objref_Radio_Control(omniIOR*, omniIdentity*);

  protected:
    virtual ~_objref_Radio_Control();

    
  private:
    virtual void* _ptrToObjRef(const char*);

    _objref_Radio_Control(const _objref_Radio_Control&);
    _objref_Radio_Control& operator = (const _objref_Radio_Control&);
    // not implemented

    friend class Radio_Control;
  };

  class _pof_Radio_Control : public _OMNI_NS(proxyObjectFactory) {
  public:
    inline _pof_Radio_Control() : _OMNI_NS(proxyObjectFactory)(Radio_Control::_PD_repoId) {}
    virtual ~_pof_Radio_Control();

    virtual omniObjRef* newObjRef(omniIOR*,omniIdentity*);
    virtual _CORBA_Boolean is_a(const char*) const;
  };

  class _impl_Radio_Control :
    public virtual omniServant
  {
  public:
    virtual ~_impl_Radio_Control();

    virtual void set_number_of_channels(::CORBA::ULong num) = 0;
    virtual void get_number_of_channels(::CORBA::ULong& num) = 0;
    virtual void get_gain_range(::CORBA::ULong channel, ::CORBA::Float& gmin, ::CORBA::Float& gmax, ::CORBA::Float& gstep) = 0;
    virtual void set_gain(::CORBA::ULong channel, ::CORBA::Float gain) = 0;
    virtual void get_gain(::CORBA::ULong channel, ::CORBA::Float& gain) = 0;
    virtual void get_frequency_range(::CORBA::ULong channel, ::CORBA::Float& fmin, ::CORBA::Float& fmax, ::CORBA::Float& fstep) = 0;
    virtual void set_frequency(::CORBA::ULong channel, ::CORBA::Float f) = 0;
    virtual void get_frequency(::CORBA::ULong channel, ::CORBA::Float& f) = 0;
    virtual void start(::CORBA::ULong channel) = 0;
    virtual void stop(::CORBA::ULong channel) = 0;
    virtual void set_values(const CF::Properties& values) = 0;
    
  public:  // Really protected, workaround for xlC
    virtual _CORBA_Boolean _dispatch(omniCallHandle&);

  private:
    virtual void* _ptrToInterface(const char*);
    virtual const char* _mostDerivedRepoId();
    
  };


  _CORBA_MODULE_VAR _dyn_attr const ::CORBA::TypeCode_ptr _tc_Radio_Control;

#ifndef __standardInterfaces_mRX__Control__
#define __standardInterfaces_mRX__Control__

  class RX_Control;
  class _objref_RX_Control;
  class _impl_RX_Control;
  
  typedef _objref_RX_Control* RX_Control_ptr;
  typedef RX_Control_ptr RX_ControlRef;

  class RX_Control_Helper {
  public:
    typedef RX_Control_ptr _ptr_type;

    static _ptr_type _nil();
    static _CORBA_Boolean is_nil(_ptr_type);
    static void release(_ptr_type);
    static void duplicate(_ptr_type);
    static void marshalObjRef(_ptr_type, cdrStream&);
    static _ptr_type unmarshalObjRef(cdrStream&);
  };

  typedef _CORBA_ObjRef_Var<_objref_RX_Control, RX_Control_Helper> RX_Control_var;
  typedef _CORBA_ObjRef_OUT_arg<_objref_RX_Control,RX_Control_Helper > RX_Control_out;

#endif

  // interface RX_Control
  class RX_Control {
  public:
    // Declarations for this interface type.
    typedef RX_Control_ptr _ptr_type;
    typedef RX_Control_var _var_type;

    static _ptr_type _duplicate(_ptr_type);
    static _ptr_type _narrow(::CORBA::Object_ptr);
    static _ptr_type _unchecked_narrow(::CORBA::Object_ptr);
    
    static _ptr_type _nil();

    static inline void _marshalObjRef(_ptr_type, cdrStream&);

    static inline _ptr_type _unmarshalObjRef(cdrStream& s) {
      omniObjRef* o = omniObjRef::_unMarshal(_PD_repoId,s);
      if (o)
        return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
      else
        return _nil();
    }

    static _core_attr const char* _PD_repoId;

    // Other IDL defined within this scope.
    
  };

  class _objref_RX_Control :
    public virtual _objref_Radio_Control
  {
  public:
    void set_decimation_rate(::CORBA::ULong channel, ::CORBA::ULong M);
    void get_decimation_range(::CORBA::ULong channel, ::CORBA::ULong& dmin, ::CORBA::ULong& dmax, ::CORBA::ULong& dstep);
    void set_data_packet_size(::CORBA::ULong channel, ::CORBA::ULong N);

    inline _objref_RX_Control()  { _PR_setobj(0); }  // nil
    _objref_RX_Control(omniIOR*, omniIdentity*);

  protected:
    virtual ~_objref_RX_Control();

    
  private:
    virtual void* _ptrToObjRef(const char*);

    _objref_RX_Control(const _objref_RX_Control&);
    _objref_RX_Control& operator = (const _objref_RX_Control&);
    // not implemented

    friend class RX_Control;
  };

  class _pof_RX_Control : public _OMNI_NS(proxyObjectFactory) {
  public:
    inline _pof_RX_Control() : _OMNI_NS(proxyObjectFactory)(RX_Control::_PD_repoId) {}
    virtual ~_pof_RX_Control();

    virtual omniObjRef* newObjRef(omniIOR*,omniIdentity*);
    virtual _CORBA_Boolean is_a(const char*) const;
  };

  class _impl_RX_Control :
    public virtual _impl_Radio_Control
  {
  public:
    virtual ~_impl_RX_Control();

    virtual void set_decimation_rate(::CORBA::ULong channel, ::CORBA::ULong M) = 0;
    virtual void get_decimation_range(::CORBA::ULong channel, ::CORBA::ULong& dmin, ::CORBA::ULong& dmax, ::CORBA::ULong& dstep) = 0;
    virtual void set_data_packet_size(::CORBA::ULong channel, ::CORBA::ULong N) = 0;
    
  public:  // Really protected, workaround for xlC
    virtual _CORBA_Boolean _dispatch(omniCallHandle&);

  private:
    virtual void* _ptrToInterface(const char*);
    virtual const char* _mostDerivedRepoId();
    
  };


  _CORBA_MODULE_VAR _dyn_attr const ::CORBA::TypeCode_ptr _tc_RX_Control;

#ifndef __standardInterfaces_mTX__Control__
#define __standardInterfaces_mTX__Control__

  class TX_Control;
  class _objref_TX_Control;
  class _impl_TX_Control;
  
  typedef _objref_TX_Control* TX_Control_ptr;
  typedef TX_Control_ptr TX_ControlRef;

  class TX_Control_Helper {
  public:
    typedef TX_Control_ptr _ptr_type;

    static _ptr_type _nil();
    static _CORBA_Boolean is_nil(_ptr_type);
    static void release(_ptr_type);
    static void duplicate(_ptr_type);
    static void marshalObjRef(_ptr_type, cdrStream&);
    static _ptr_type unmarshalObjRef(cdrStream&);
  };

  typedef _CORBA_ObjRef_Var<_objref_TX_Control, TX_Control_Helper> TX_Control_var;
  typedef _CORBA_ObjRef_OUT_arg<_objref_TX_Control,TX_Control_Helper > TX_Control_out;

#endif

  // interface TX_Control
  class TX_Control {
  public:
    // Declarations for this interface type.
    typedef TX_Control_ptr _ptr_type;
    typedef TX_Control_var _var_type;

    static _ptr_type _duplicate(_ptr_type);
    static _ptr_type _narrow(::CORBA::Object_ptr);
    static _ptr_type _unchecked_narrow(::CORBA::Object_ptr);
    
    static _ptr_type _nil();

    static inline void _marshalObjRef(_ptr_type, cdrStream&);

    static inline _ptr_type _unmarshalObjRef(cdrStream& s) {
      omniObjRef* o = omniObjRef::_unMarshal(_PD_repoId,s);
      if (o)
        return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
      else
        return _nil();
    }

    static _core_attr const char* _PD_repoId;

    // Other IDL defined within this scope.
    
  };

  class _objref_TX_Control :
    public virtual _objref_Radio_Control
  {
  public:
    void set_interpolation_rate(::CORBA::ULong channel, ::CORBA::ULong M);
    void get_interpolation_range(::CORBA::ULong channel, ::CORBA::ULong& dmin, ::CORBA::ULong& dmax, ::CORBA::ULong& dstep);

    inline _objref_TX_Control()  { _PR_setobj(0); }  // nil
    _objref_TX_Control(omniIOR*, omniIdentity*);

  protected:
    virtual ~_objref_TX_Control();

    
  private:
    virtual void* _ptrToObjRef(const char*);

    _objref_TX_Control(const _objref_TX_Control&);
    _objref_TX_Control& operator = (const _objref_TX_Control&);
    // not implemented

    friend class TX_Control;
  };

  class _pof_TX_Control : public _OMNI_NS(proxyObjectFactory) {
  public:
    inline _pof_TX_Control() : _OMNI_NS(proxyObjectFactory)(TX_Control::_PD_repoId) {}
    virtual ~_pof_TX_Control();

    virtual omniObjRef* newObjRef(omniIOR*,omniIdentity*);
    virtual _CORBA_Boolean is_a(const char*) const;
  };

  class _impl_TX_Control :
    public virtual _impl_Radio_Control
  {
  public:
    virtual ~_impl_TX_Control();

    virtual void set_interpolation_rate(::CORBA::ULong channel, ::CORBA::ULong M) = 0;
    virtual void get_interpolation_range(::CORBA::ULong channel, ::CORBA::ULong& dmin, ::CORBA::ULong& dmax, ::CORBA::ULong& dstep) = 0;
    
  public:  // Really protected, workaround for xlC
    virtual _CORBA_Boolean _dispatch(omniCallHandle&);

  private:
    virtual void* _ptrToInterface(const char*);
    virtual const char* _mostDerivedRepoId();
    
  };


  _CORBA_MODULE_VAR _dyn_attr const ::CORBA::TypeCode_ptr _tc_TX_Control;

_CORBA_MODULE_END



_CORBA_MODULE POA_standardInterfaces
_CORBA_MODULE_BEG

  class Radio_Control :
    public virtual standardInterfaces::_impl_Radio_Control,
    public virtual ::PortableServer::ServantBase
  {
  public:
    virtual ~Radio_Control();

    inline ::standardInterfaces::Radio_Control_ptr _this() {
      return (::standardInterfaces::Radio_Control_ptr) _do_this(::standardInterfaces::Radio_Control::_PD_repoId);
    }
  };

  class RX_Control :
    public virtual standardInterfaces::_impl_RX_Control,
    public virtual Radio_Control
  {
  public:
    virtual ~RX_Control();

    inline ::standardInterfaces::RX_Control_ptr _this() {
      return (::standardInterfaces::RX_Control_ptr) _do_this(::standardInterfaces::RX_Control::_PD_repoId);
    }
  };

  class TX_Control :
    public virtual standardInterfaces::_impl_TX_Control,
    public virtual Radio_Control
  {
  public:
    virtual ~TX_Control();

    inline ::standardInterfaces::TX_Control_ptr _this() {
      return (::standardInterfaces::TX_Control_ptr) _do_this(::standardInterfaces::TX_Control::_PD_repoId);
    }
  };

_CORBA_MODULE_END



_CORBA_MODULE OBV_standardInterfaces
_CORBA_MODULE_BEG

_CORBA_MODULE_END





#undef _core_attr
#undef _dyn_attr

void operator<<=(::CORBA::Any& _a, standardInterfaces::Radio_Control_ptr _s);
void operator<<=(::CORBA::Any& _a, standardInterfaces::Radio_Control_ptr* _s);
_CORBA_Boolean operator>>=(const ::CORBA::Any& _a, standardInterfaces::Radio_Control_ptr& _s);

void operator<<=(::CORBA::Any& _a, standardInterfaces::RX_Control_ptr _s);
void operator<<=(::CORBA::Any& _a, standardInterfaces::RX_Control_ptr* _s);
_CORBA_Boolean operator>>=(const ::CORBA::Any& _a, standardInterfaces::RX_Control_ptr& _s);

void operator<<=(::CORBA::Any& _a, standardInterfaces::TX_Control_ptr _s);
void operator<<=(::CORBA::Any& _a, standardInterfaces::TX_Control_ptr* _s);
_CORBA_Boolean operator>>=(const ::CORBA::Any& _a, standardInterfaces::TX_Control_ptr& _s);



inline void
standardInterfaces::Radio_Control::_marshalObjRef(::standardInterfaces::Radio_Control_ptr obj, cdrStream& s) {
  omniObjRef::_marshal(obj->_PR_getobj(),s);
}


inline void
standardInterfaces::RX_Control::_marshalObjRef(::standardInterfaces::RX_Control_ptr obj, cdrStream& s) {
  omniObjRef::_marshal(obj->_PR_getobj(),s);
}


inline void
standardInterfaces::TX_Control::_marshalObjRef(::standardInterfaces::TX_Control_ptr obj, cdrStream& s) {
  omniObjRef::_marshal(obj->_PR_getobj(),s);
}




#ifdef   USE_stub_in_nt_dll_NOT_DEFINED_Radio__Control
# undef  USE_stub_in_nt_dll
# undef  USE_stub_in_nt_dll_NOT_DEFINED_Radio__Control
#endif
#ifdef   USE_core_stub_in_nt_dll_NOT_DEFINED_Radio__Control
# undef  USE_core_stub_in_nt_dll
# undef  USE_core_stub_in_nt_dll_NOT_DEFINED_Radio__Control
#endif
#ifdef   USE_dyn_stub_in_nt_dll_NOT_DEFINED_Radio__Control
# undef  USE_dyn_stub_in_nt_dll
# undef  USE_dyn_stub_in_nt_dll_NOT_DEFINED_Radio__Control
#endif

#endif  // __Radio__Control_hh__
