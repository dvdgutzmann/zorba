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

#include "system/globalenv.h"
#include "compiler/expression/expr.h"
#include "compiler/codegen/plan_visitor.h"
#include "compiler/semantic_annotations/tsv_annotation.h"
#include "compiler/semantic_annotations/annotation_keys.h"

#include "runtime/core/fncall_iterator.h"
#include "runtime/core/var_iterators.h"

#include "functions/function.h"

#include "util/hashmap32.h"

#include "types/typeops.h"

using namespace std;

namespace zorba {

SERIALIZABLE_CLASS_VERSIONS(function)
END_SERIALIZABLE_CLASS_VERSIONS(function)

SERIALIZABLE_CLASS_VERSIONS(user_function)
END_SERIALIZABLE_CLASS_VERSIONS(user_function)

/*******************************************************************************

********************************************************************************/
xqtref_t function::return_type (const std::vector<xqtref_t> &) const 
{
  return sig.return_type ();
}


bool function::validate_args(std::vector<PlanIter_t>& argv) const 
{
  uint32_t n = sig.arg_count ();
  return n == VARIADIC_SIG_SIZE || argv.size() == sig.arg_count();
}


void function::compute_annotation (
    AnnotationHolder *,
    std::vector<AnnotationHolder *> &kids,
    Annotation::key_t k) const 
{
  switch (k) {
  case AnnotationKey::IGNORES_SORTED_NODES:
  case AnnotationKey::IGNORES_DUP_NODES:
    for (unsigned src = 0; src < kids.size (); src++)
      if (kids [src] != NULL)
        TSVAnnotationValue::update_annotation (kids [src], k, TSVAnnotationValue::MAYBE_VAL);
    break;
  default: break;
  }  
}


function::AnnotationProperty_t function::producesDuplicates() const 
{
  xqtref_t rt = sig.return_type ();
  if (TypeOps::type_max_cnt (*rt) <= 1 || TypeOps::is_builtin_simple (*rt))
    return NO;
  return YES;
}


function::AnnotationProperty_t function::producesNodeIdSorted() const 
{
  xqtref_t rt = sig.return_type ();
  if (TypeOps::type_max_cnt (*rt) <= 1 || TypeOps::is_builtin_simple (*rt))
    return YES;
  return NO;
}


/*******************************************************************************

********************************************************************************/
user_function::user_function(
    const QueryLoc& loc, 
                             const signature& _sig, 
                             expr_t expr_body, 
                             enum ParseConstants::function_type_t ftype,
                             bool deterministic_)
  :
  function(_sig), 
  m_loc(loc), 
  m_expr_body(expr_body), 
  theUpdateType((ftype == ParseConstants::fn_update) ? UPDATE_EXPR : SIMPLE_EXPR),
  sequential (ftype == ParseConstants::fn_sequential),
  deterministic (deterministic_),
  leaf (true)
{
  m_state_size = 0;
}


user_function::~user_function()
{
}

const QueryLoc& user_function::get_location() const
{
  return m_loc;
}


void user_function::set_body(expr_t body)
{
  m_expr_body = body;
}


expr_t user_function::get_body() const
{
  return m_expr_body;
}


void user_function::set_args(std::vector<var_expr_t>& args)
{
  m_args = args;
}


const std::vector<var_expr_t>& user_function::get_args() const
{
  return m_args;
}
  

bool user_function::requires_dyn_ctx () const
{
  // All undeclared functions unfoldable. TODO: better analysis
  return (m_expr_body == NULL ||
          m_expr_body->get_annotation (AnnotationKey::UNFOLDABLE_OP).getp() == TSVAnnotationValue::TRUE_VAL.getp());
}


std::vector<LetVarIter_t>& user_function::get_param_iters() const
{
  return m_param_iters;
}


PlanIter_t user_function::get_plan(CompilerCB *ccb) const
{
  if (m_plan == NULL) 
  {
    std::vector<std::vector<LetVarIter_t> > param_iter_vec(m_args.size());
    hash64map<std::vector<LetVarIter_t> *> param_map;

    for(uint32_t i = 0; i < m_args.size(); ++i)
    {
      param_map.put((uint64_t)&*m_args[i], &param_iter_vec[i]);
    }

    m_plan = zorba::codegen(get_fname()->getStringValue()->c_str (),
                            &*m_expr_body,
                            ccb,
                            &param_map);

    for(uint32_t i = 0; i < param_iter_vec.size(); ++i) 
    {
      std::vector<LetVarIter_t>& vec = param_iter_vec[i];
      switch(vec.size()) {
        case 0:
          m_param_iters.push_back(NULL);
          break;

        case 1:
          m_param_iters.push_back(vec[0]);
          break;

        default:
          ZORBA_ASSERT (false);
          break;
      }
    }
  }
  return m_plan;
}


PlanIter_t user_function::codegen (
    short sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    AnnotationHolder &ann) const
{
  return new UDFunctionCallIterator(sctx, loc, argv, this);
}

}

/* vim:set ts=2 sw=2: */
