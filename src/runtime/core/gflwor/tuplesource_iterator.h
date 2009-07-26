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
#ifndef ZORBA_RUNTIME_FLWOR_TUPLESOURCE_ITERATOR
#define ZORBA_RUNTIME_FLWOR_TUPLESOURCE_ITERATOR

#include "runtime/base/noarybase.h"

namespace zorba 
{
namespace flwor 
{
    /**
     * The TupleSourceIterator initializes a TupleStream. This iterator is always the first iterator. 
     * 
     */
    class TupleSourceIterator : public NoaryBaseIterator<TupleSourceIterator, PlanIteratorState> {
      public:
        TupleSourceIterator ( short sctx, const QueryLoc& loc);
        ~TupleSourceIterator();

      public:
        bool nextImpl ( store::Item_t& result, PlanState& planState ) const;
        virtual void accept ( PlanIterVisitor& ) const;
      public:
        SERIALIZABLE_CLASS(TupleSourceIterator)
        SERIALIZABLE_CLASS_CONSTRUCTOR2T(TupleSourceIterator, NoaryBaseIterator<TupleSourceIterator, PlanIteratorState>)
        void serialize(::zorba::serialization::Archiver &ar)
        {
          serialize_baseclass(ar, (NoaryBaseIterator<TupleSourceIterator, PlanIteratorState>*)this);
        }
    };
  }
}


#endif  /* ZORBA_RUNTIME_FLWOR_TUPLESTREAM_ITERATOR */

/*
 * Local variables:
 * mode: c++
 * End:
 */
