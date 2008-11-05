/*
 * Copyright 2006-2008 The FLWOR Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef ZORBA_TYPEMANAGERIMPL_H
#define ZORBA_TYPEMANAGERIMPL_H

#include "types/typemanager.h"
#include "store/api/item.h"

namespace zorba {

class NodeTest;

struct qname_hash_equals {
  static uint32_t hash(const store::Item *qn)
  {
    return qn->hash();
  }

  static bool equal(const store::Item *qn1, const store::Item *qn2)
  {
    return qn1->equals(qn2);
  }
};


/*
 * Interface used by other parts of zorba to ask questions about types.
 */
class TypeManagerImpl : public TypeManager 
{
public:
  TypeManagerImpl(int level, TypeManager *parent)
    :
    TypeManager(level),
    m_parent(parent) 
  {
  }

  virtual ~TypeManagerImpl() { }

  TypeManager *get_parent_type_manager() const { return m_parent; }

  /* Factory Methods */
  xqtref_t create_type_x_quant(
        const XQType& type,
        TypeConstants::quantifier_t quantifier) const;

  xqtref_t create_type(
        const XQType& type,
        TypeConstants::quantifier_t quantifier) const;

  xqtref_t create_type(const TypeIdentifier& ident) const;

  xqtref_t create_value_type(const store::Item* item) const;

  virtual xqtref_t create_named_type(
        store::Item* qname,
        TypeConstants::quantifier_t quantifier = TypeConstants::QUANT_ONE) const;

  virtual xqtref_t create_named_atomic_type(
        store::Item* qname,
        TypeConstants::quantifier_t quantifier) const;

  xqtref_t create_atomic_type(
        TypeConstants::atomic_type_code_t type_code,
        TypeConstants::quantifier_t quantifier) const;

  xqtref_t create_node_type(
        rchandle<NodeTest> nodetest,
        xqtref_t content_type,
        TypeConstants::quantifier_t quantifier,
        bool nillable) const;

  xqtref_t create_any_type() const;

  xqtref_t create_any_item_type(TypeConstants::quantifier_t quantifier) const;

  xqtref_t create_any_simple_type() const;

  xqtref_t create_untyped_type() const;

  xqtref_t create_empty_type() const;

  xqtref_t create_none_type() const;

protected:
  TypeManager *m_parent;
};

}

#endif /* ZORBA_TYPEMANAGER_H */
/* vim:set ts=2 sw=2: */
