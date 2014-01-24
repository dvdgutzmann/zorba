/*
 * Copyright 2006-2012 The FLWOR Foundation.
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
 
// ******************************************
// *                                        *
// * THIS IS A GENERATED FILE. DO NOT EDIT! *
// * SEE .xml FILE WITH SAME NAME           *
// *                                        *
// ******************************************

#include "stdafx.h"
#include "zorbatypes/rchandle.h"
#include "zorbatypes/zstring.h"
#include "runtime/visitors/planiter_visitor.h"
#include "runtime/jsound/jsound.h"
#include "system/globalenv.h"


#include "store/api/iterator.h"

namespace zorba {

// <JSoundAnnotateIterator>
SERIALIZABLE_CLASS_VERSIONS(JSoundAnnotateIterator)

void JSoundAnnotateIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (NaryBaseIterator<JSoundAnnotateIterator, PlanIteratorState>*)this);
}


void JSoundAnnotateIterator::accept(PlanIterVisitor& v) const
{
  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

JSoundAnnotateIterator::~JSoundAnnotateIterator() {}


zstring JSoundAnnotateIterator::getNameAsString() const {
  return "fn-zorba-jsound:annotate";
}
// </JSoundAnnotateIterator>


// <JSoundValidateIterator>
SERIALIZABLE_CLASS_VERSIONS(JSoundValidateIterator)

void JSoundValidateIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (NaryBaseIterator<JSoundValidateIterator, PlanIteratorState>*)this);
}


void JSoundValidateIterator::accept(PlanIterVisitor& v) const
{
  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

JSoundValidateIterator::~JSoundValidateIterator() {}


zstring JSoundValidateIterator::getNameAsString() const {
  return "fn-zorba-jsound:validate";
}
// </JSoundValidateIterator>



}


