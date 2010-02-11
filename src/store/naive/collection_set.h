/*
 * Copyright 2006-2010 The FLWOR Foundation.
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
#ifndef ZORBA_SIMPLE_STORE_COLLECTION_SET_H
#define ZORBA_SIMPLE_STORE_COLLECTION_SET_H

#include "store/api/shared_types.h"
#include "store/naive/shared_types.h"
#include "zorbautils/hashmap_itemp.h"
#include "store/api/collection.h"

namespace zorba {
  
  namespace simplestore {

    class CollectionIterator;
    
    /*******************************************************************************
      Collections container to ease the implementation of stores which contain 
      a different kind of memory management. For the simplestore, the Collections
      is only a wrapper class around an ItemPointerHashMap.
    ********************************************************************************/
    class CollectionSet
    {
    public:
      typedef ItemPointerHashMap<store::Collection_t> Set;
      static const ulong DEFAULT_COLLECTION_MAP_SIZE;
    
    protected:
      Set  theCollections;
    
    public:
      CollectionSet();
    
      void clear();
    
      bool
      insert(const store::Item* aName, store::Collection_t& aCollection);
    
      bool
      get(const store::Item* aName, store::Collection_t& aCollection);
    
      bool
      remove(const store::Item* aName);
    
      store::Iterator_t
      names();
    
      CollectionIterator_t
      collections();
    
    }; /* class CollectionSet */
    
    /*******************************************************************************
      Collection iterator
      Returned by the CollectionSet::collections function
    ********************************************************************************/
    class CollectionIterator : public SimpleRCObject
    {
    protected:
      CollectionSet::Set*          theCollections;
      CollectionSet::Set::iterator theIterator;
      bool                         theOpened;
    
    public:
      CollectionIterator(CollectionSet::Set* aCollections);
    
      virtual ~CollectionIterator();
    
      virtual void
      open();
    
      virtual bool
      next(store::Collection_t&);
    
      virtual void
      reset();
    
      virtual void
      close();
    };

  } /* namespace simplestore */
} /* namespace zorba */

#endif

/*
 * Local variables:
 * mode: c++
 * End:
 */
