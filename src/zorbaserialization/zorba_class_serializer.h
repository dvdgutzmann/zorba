
#ifndef ZORBA_SPECIFIC_CLASS_SERIALIZATION
#define ZORBA_SPECIFIC_CLASS_SERIALIZATION

#include "zorbaserialization/class_serializer.h"
#include "zorbaserialization/archiver.h"
//#include "zorbaerrors/error_manager.h"

#include "zorbatypes/rchandle.h"
#include "zorbautils/checked_vector.h"
#include "store/api/shared_types.h"
#include "zorbatypes/m_apm.h"
//#include "context/context.h"
#include <map>

namespace zorba{
  class XQType;
  class function;

  namespace serialization{
//void operator&(Archiver &ar, XQType *&obj);
void operator&(Archiver &ar, const XQType *&obj);
#ifndef ZORBA_NO_BIGNUMBERS
void operator&(Archiver &ar, MAPM &obj);
#endif

void operator&(Archiver &ar, XQPCollator *&obj);
void operator&(Archiver &ar, store::Item* &obj);
void operator&(Archiver &ar, std::map<int, rchandle<function> >* &obj);//ArityFMap

//void serialize_qname(Archiver &ar, store::Item_t &qname);

#define SERIALIZE_TYPEMANAGER(type_mgr_type, type_mgr)                             \
  bool is_root_type_mgr = (!GENV.isRootStaticContextInitialized() || ((TypeManager*)type_mgr == (TypeManager*)&GENV_TYPESYSTEM));            \
  ar.set_is_temp_field(true);                                       \
  ar & is_root_type_mgr;                                            \
  ar.set_is_temp_field(false);                                      \
  if(is_root_type_mgr)                                              \
  {                                                                 \
    if(!ar.is_serializing_out())                                    \
    {                                                               \
      type_mgr = (type_mgr_type*)&GENV_TYPESYSTEM;                  \
      /*RCHelper::addReference(type_mgr);*/                         \
    }                                                               \
  }                                                                 \
  else                                                              \
  {                                                                 \
    ar & type_mgr;                                                  \
  }

#define SERIALIZE_TYPEMANAGER_RCHANDLE(type_mgr_type, type_mgr)                             \
  bool is_root_type_mgr = (!GENV.isRootStaticContextInitialized() || ((TypeManager*)type_mgr.getp() == (TypeManager*)&GENV_TYPESYSTEM));            \
  ar.set_is_temp_field(true);                                       \
  ar & is_root_type_mgr;                                            \
  ar.set_is_temp_field(false);                                      \
  if(is_root_type_mgr)                                              \
  {                                                                 \
    if(!ar.is_serializing_out())                                    \
     type_mgr = (type_mgr_type*)&GENV_TYPESYSTEM;                   \
  }                                                                 \
  else                                                              \
  {                                                                 \
    ar & type_mgr;                                                  \
  }

template<class T>
void operator&(Archiver &ar, checked_vector<T> &obj)
{
  if(ar.is_serializing_out())
  {
    char  strtemp[20];
    sprintf(strtemp, "%d", (int)obj.size());
    bool is_ref;
    is_ref = ar.add_compound_field("checked_vector<T>", 0, !FIELD_IS_CLASS, strtemp, &obj, ARCHIVE_FIELD_NORMAL);
    if(!is_ref)
    {
      typename checked_vector<T>::iterator  it;
      for(it=obj.begin(); it != obj.end(); it++)
      {
        ar & (*it);
      }
      ar.add_end_compound_field();
    }
  }
  else
  {
    char  *type;
    std::string value;
    int   id;
    int   version;
    bool  is_simple;
    bool  is_class;
    enum  ArchiveFieldTreat field_treat;
    int   referencing;
    bool  retval;
    retval = ar.read_next_field(&type, &value, &id, &version, &is_simple, &is_class, &field_treat, &referencing);
    if(!retval && ar.get_read_optional_field())
      return;
    ar.check_nonclass_field(retval, type, "checked_vector<T>", is_simple, is_class, field_treat, ARCHIVE_FIELD_NORMAL, id);
    ar.register_reference(id, field_treat, &obj);

    int size;
    sscanf(value.c_str(), "%d", &size);
    obj.resize(size);
    typename checked_vector<T>::iterator  it;
    for(it=obj.begin(); it != obj.end(); it++)
    {
      ar & (*it);
    }
    ar.read_end_current_level();

  }
}

template<class T>
void operator&(Archiver &ar, zorba::rchandle<T> &obj)
{
  if(ar.is_serializing_out())
  {
    bool is_ref;
    is_ref = ar.add_compound_field("rchandle<T>", 0, !FIELD_IS_CLASS, "", &obj, ARCHIVE_FIELD_NORMAL);
    if(!is_ref)
    {
      T *p = obj.getp();
      ar & p;
      ar.add_end_compound_field();
    }
  }
  else
  {
    char  *type;
    std::string value;
    int   id;
    int   version;
    bool  is_simple;
    bool  is_class;
    enum  ArchiveFieldTreat field_treat;
    int   referencing;
    bool  retval;
    retval = ar.read_next_field(&type, &value, &id, &version, &is_simple, &is_class, &field_treat, &referencing);
    if(!retval && ar.get_read_optional_field())
      return;
    ar.check_nonclass_field(retval, type, "rchandle<T>", is_simple, is_class, field_treat, ARCHIVE_FIELD_NORMAL, id);
    ar.register_reference(id, field_treat, &obj);

    T *p;
    ar & p;
    obj = p;
    if(p == NULL)
    {
      ar.reconf_last_delayed_rcobject((void**)&p, (void**)obj.getp_ref());
    }
    ar.read_end_current_level();

  }
}

template<class T>
void operator&(Archiver &ar, zorba::const_rchandle<T> &obj)
{
  if(ar.is_serializing_out())
  {
    bool is_ref;
    is_ref = ar.add_compound_field("const_rchandle<T>", 0, !FIELD_IS_CLASS, "", &obj, ARCHIVE_FIELD_NORMAL);
    if(!is_ref)
    {
      T *p = (T*)obj.getp();
      ar & p;
      ar.add_end_compound_field();
    }
  }
  else
  {
    char  *type;
    std::string value;
    int   id;
    int   version;
    bool  is_simple;
    bool  is_class;
    enum  ArchiveFieldTreat field_treat;
    int   referencing;
    bool  retval;
    retval = ar.read_next_field(&type, &value, &id, &version, &is_simple, &is_class, &field_treat, &referencing);
    if(!retval && ar.get_read_optional_field())
      return;
    ar.check_nonclass_field(retval, type, "const_rchandle<T>", is_simple, is_class, field_treat, ARCHIVE_FIELD_NORMAL, id);
    ar.register_reference(id, field_treat, &obj);

    T *p;
    ar & p;
    obj = p;
    ar.read_end_current_level();

  }
}


}}
#endif
