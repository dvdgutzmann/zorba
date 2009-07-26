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
#ifndef ZORBA_VALIDATE_IMPL_H
#define ZORBA_VALIDATE_IMPL_H


#include "common/common.h"
#ifndef ZORBA_NO_XMLSCHEMA

#include "compiler/parser/parse_constants.h"
#include "common/shared_types.h"
#include "runtime/base/unarybase.h"
#include "runtime/booleans/compare_types.h"
#include "types/schema/EventSchemaValidator.h"
#include "zorbatypes/xqpstring.h"

namespace zorba
{

class RuntimeCB; // TODO we should have a shared_runtime_types.h
class GenericCast;
class TypeManager;
class EventSchemaValidator;


/*______________________________________________________________________
  | 
  | XQuery 1.1
  | 
  | 3.14 Validate Expressions
  | [86] ValidateExpr   ::= "validate" (ValidationMode | ("as" TypeName))? "{" Expr "}"
  | [87] ValidationMode ::= "lax" | "strict"
  | 
  | A validate expression can be used to validate a document node or an element node 
  | with respect to the in-scope schema definitions, using the schema validation process 
  | defined in [XML Schema]. If the operand of a validate expression does not evaluate 
  | to exactly one document or element node, a type error is raised [err:XQTY0030]. 
  | In this specification, the node that is the operand of a validate expression is 
  | called the operand node.
  | 
  | A validate expression returns a new node with its own identity and with no parent. 
  | The new node and its descendants are given type annotations that are generated by 
  | applying a validation process to the operand node. In some cases, default values 
  | may also be generated by the validation process.
  | 
  | A validate expression may optionally specify a validation mode. The default 
  | validation mode is strict.
  | 
  | A validate expression may optionally specify a TypeName. This type name must be 
  | found in the in-scope schema definitions; if it is not, a static error is raised 
  | [err:XQST0104]. If the type name is unprefixed, it is interpreted as a name in the 
  | default namespace for elements and types.
  |_______________________________________________________________________*/
class ValidateIterator : public UnaryBaseIterator<ValidateIterator, PlanIteratorState>
{
private:
  enum ParseConstants::validation_mode_t _validationMode;
  rchandle<TypeManager> _typemgr;
  store::Item_t _typeName;

public:
  SERIALIZABLE_CLASS(ValidateIterator)
  SERIALIZABLE_CLASS_CONSTRUCTOR2T(ValidateIterator, UnaryBaseIterator<ValidateIterator, PlanIteratorState>)
  void serialize(::zorba::serialization::Archiver &ar)
  {
    serialize_baseclass(ar, (UnaryBaseIterator<ValidateIterator, PlanIteratorState>*)this);
    SERIALIZE_ENUM(enum ParseConstants::validation_mode_t, _validationMode);
    SERIALIZE_TYPEMANAGER_RCHANDLE(TypeManager, _typemgr);
    ar & _typeName;
  }
public:
  ValidateIterator (
        short sctx,
        const QueryLoc& loc,
        PlanIter_t& aIter,
        TypeManager *,
        store::Item_t a_typeName,
        ParseConstants::validation_mode_t a_validationMode);

  bool nextImpl(store::Item_t& result, PlanState& planState) const;

  virtual void accept(PlanIterVisitor&) const;

  static bool effectiveValidationValue (
        store::Item_t& result,
        const store::Item_t& sourceNode,
        const store::Item_t& typeName,
        TypeManager* typeManager,
        ParseConstants::validation_mode_t validationMode,
        static_context* sctx,
        const QueryLoc& loc); 

  static store::Item_t processElement (
        static_context* sctx,
        TypeManager* typeManager, 
        EventSchemaValidator& schemaValidator,
        store::Item* parent,
        const store::Item_t& element);
  
  static void validateAttributes (
        EventSchemaValidator& schemaValidator,
        store::Iterator_t attributes);

  static void processAttributes(
        static_context* sctx,
        TypeManager* typeManager,
        EventSchemaValidator& schemaValidator,
        store::Item* parent,
        store::Iterator_t attributes);

  static void processChildren (
        static_context* sctx,
        TypeManager* typeManager,
        EventSchemaValidator& schemaValidator,
        store::Item* parent,
        store::Iterator_t children);

  static void processNamespaces (
        EventSchemaValidator& schemaValidator,
        const store::Item_t& item);
        
  static void processTextValue (
        static_context* sctx,
        TypeManager* typeManager, 
        store::NsBindings& bindings,
        const store::Item_t& typeQName,
        xqpStringStore_t& textValue, 
        std::vector<store::Item_t> &resultList);

  ~ValidateIterator () {};
};

}

#endif // ZORBA_NO_XMLSCHEMA    
#endif

/*
 * Local variables:
 * mode: c++
 * End:
 */
