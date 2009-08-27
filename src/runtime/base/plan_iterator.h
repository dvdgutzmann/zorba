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
#ifndef ZORBA_PLAN_ITERATOR_H
#define ZORBA_PLAN_ITERATOR_H

#include <typeinfo>

#include "common/shared_types.h"

#include "zorbaerrors/Assert.h"

#include "compiler/parser/query_loc.h"

#include "zorbaserialization/class_serializer.h"

#include "store/api/item.h"


// Info: Forcing inlining a function in g++:
// store::Item_t next() __attribute__((always_inline)) {...}

/*******************************************************************************

  Consts and macros to automate Duff's Device and separation of code and state. 

  DEFUALT_STACK_INIT : Initializes the state object of the current iterator
                       (which always includes the Duff's Device) and returns
                       a pointer to that state in the state block.
  STACK_PUSH         : Returns the passed item and saves the current position
                       in the file into the duffs line so that when the nextImpl
                       method of the iterator is called again, execution will
                       start right after the STACK_PUSH macro.
  STACK_END          : Ends the execution of the nextImpl method

********************************************************************************/

#define DEFAULT_STACK_INIT(stateType, stateObject, planState )  \
  stateObject = StateTraitsImpl<stateType>::getState(planState, this->stateOffset); \
  switch (stateObject->getDuffsLine())                          \
  {                                                             \
  case PlanIteratorState::DUFFS_ALLOCATE_RESOURCES:

#define STACK_PUSH(status, stateObject)                        \
   do                                                          \
   {                                                           \
     stateObject->setDuffsLine(__LINE__);                      \
     return status;                                            \
     case __LINE__: ;                                          \
   } while (0)

#define STACK_END( stateObject )                               \
  do {                                                         \
    stateObject->setDuffsLine(__LINE__);                       \
    return false;                                              \
  case __LINE__:                                               \
    stateObject->setDuffsLine(__LINE__ + 1);                   \
  case __LINE__ + 1:                                           \
    ZORBA_ASSERT (false && "nextImpl() called past iterator end");   \
    return false;                                              \
  default:                                                     \
    return false;                                              \
  } while (0);                                                 \
  }


namespace zorba
{

class RuntimeCB;
class PlanIterVisitor;

class ZorbaDebuggerCommons;

/*******************************************************************************
  Class to represent state that is shared by all plan iterators. 

  theBlock        : Pointer to the memory block that stores the local state of
                    each individual plan iterator.
  theBlockSize    : Size (in bytes) of the block.
  theRuntimeCB    : A runtime control block which contains all the information
                    required during query processing. Specifically, it contains
                    the static and dynamic context. Additionally, at a later time,
                    it might contain some kind of memory manager or other infrastructure
                    that is needed.
********************************************************************************/
class PlanState
{
public:
  int8_t*      theBlock;

  uint32_t     theBlockSize;

  uint32_t     theStackDepth;
  
  // TODO this guy should become const because nothing can change anymore during runtime
  //      we need to make all accessor in the control block and static context (see also shortcut below)
  //      const before doing that
  CompilerCB*  theCompilerCB; 

  RuntimeCB*   theRuntimeCB;

  ZorbaDebuggerCommons* theDebuggerCommons;

  dynamic_context*
  dctx();

public:
  PlanState(uint32_t blockSize, uint32_t aStackDepth = 0);

  ~PlanState();

  void checkDepth (const QueryLoc &loc);

  const RuntimeCB *getRuntimeCB () const { return theRuntimeCB; }
};


/*******************************************************************************
  Root object of all iterator state objects
********************************************************************************/
class PlanIteratorState
{
public:
  static const uint32_t DUFFS_ALLOCATE_RESOURCES = 0;

private:
  uint32_t theDuffsLine;

public:
#if ZORBA_BATCHING_TYPE == 1
public:
  uint32_t        theCurrItem;
  store::Item_t   theBatch[ZORBA_BATCHING_BATCHSIZE];
#endif
#ifndef NDEBUG
  bool            theIsOpened;
#endif

public:
  PlanIteratorState() 
    : theDuffsLine(DUFFS_ALLOCATE_RESOURCES)
#if ZORBA_BATCHING_TYPE == 1
      , theCurrItem(ZORBA_BATCHING_BATCHSIZE)
#endif
#ifndef NDEBUG
      , theIsOpened(false)
#endif
  {}

  ~PlanIteratorState() {}

  /** 
   * Initialize the current state object.
   * The base class initializes information used for the duffs device.
   *
   * To store information for new iterators, it might be necessary to derive
   * from this class. All initialization of such information should be done in
   * this function. If resources are requested during initialization, they must
   * be released in the destructor.
   *
   * Classes that inherit from PlanIteratorState must call the init function of 
   * their parent class explicitly in order to guarantee proper initialization.
   */
  void init(PlanState&) 
  { 
    theDuffsLine = DUFFS_ALLOCATE_RESOURCES;
#if ZORBA_BATCHING_TYPE == 1
    theCurrItem = ZORBA_BATCHING_BATCHSIZE;
#endif
  }

  /** 
   * Reset the current state object.
   *
   * Reset() is invoked by the resetImpl() method of the associated iterator.
   * Reset() sets the duffs line so that when the nextImpl() method of the
   * iterator is called again after a resetImpl(), no resources will be
   * allocated again (i.e. the init() method of the state obj will not be
   * called).
   *
   * Classes that inherit from PlanIteratorState must call the reset function 
   * of their parent class explicitly in order to guarantee a proper reset.
   */
  void reset(PlanState&) 
  { 
    theDuffsLine = DUFFS_ALLOCATE_RESOURCES; 
#if ZORBA_BATCHING_TYPE == 1
    theCurrItem = ZORBA_BATCHING_BATCHSIZE;
    // seeting the first item to NULL only is sufficient so
    // that produceNext knows that it's not finished yet
    theBatch[0] = NULL; 
#endif
  }
  
  void setDuffsLine(uint32_t aVal) { theDuffsLine = aVal; }
  uint32_t getDuffsLine() const    { return theDuffsLine; }
};


template <class T>
class StateTraitsImpl
{
private:
  StateTraitsImpl() {} // prevent instantiation

public:
  static uint32_t getStateSize() 
  { 
    return sizeof(T); 
  }

  static T* getState(PlanState& planState, uint32_t stateOffset)
  {
    return reinterpret_cast<T*>(planState.theBlock + stateOffset);
  }

  static void createState(PlanState& planState, uint32_t& stateOffset, uint32_t& offset)
  {
    stateOffset = offset;
    offset += StateTraitsImpl<T>::getStateSize();
    new (planState.theBlock + stateOffset)T();
  }

  static void initState(PlanState& planState, uint32_t& stateOffset)
  {
    getState(planState, stateOffset)->init(planState); 
  }

  static void reset(PlanState& planState, uint32_t stateOffset)
  {
    (reinterpret_cast<T*>(planState.theBlock+ stateOffset))->reset(planState);
  }

  static void destroyState(PlanState& planState, uint32_t stateOffset) 
  { 
    (reinterpret_cast<T*>(planState.theBlock + stateOffset))->~T(); 
  }
};

#if ZORBA_BATCHING_TYPE == 1

#error "Batching not implemented with the new iterator contract."

#endif


/*******************************************************************************
  Base class of all plan iterators.
********************************************************************************/
class PlanIterator : public SimpleRCObject
{
  friend class PlanIterWrapper;

protected:
  uint32_t                   stateOffset;

public:
  QueryLoc                   loc;
  short                      sctx;
  mutable static_context   * theSctx; // no need to serialize this

public:
  SERIALIZABLE_ABSTRACT_CLASS(PlanIterator);

  PlanIterator(zorba::serialization::Archiver& ar)
    :
    SimpleRCObject(ar),
    theSctx(NULL)
  {}

  void serialize(::zorba::serialization::Archiver &ar)
  {
    //serialize_baseclass(ar, (SimpleRCObject*)this);
    ar & stateOffset;
    ar & loc;
    ar & sctx;
  }

public:
  PlanIterator(short aContext, const QueryLoc& aLoc) 
    :
    stateOffset(0),
    loc(aLoc),
    sctx(aContext),
    theSctx(NULL)
  {}
  
  PlanIterator(const PlanIterator& it) 
    : 
    SimpleRCObject(it), 
    stateOffset(0),
    loc(it.loc),
    sctx(it.sctx),
    theSctx(it.theSctx)
  {}

  virtual ~PlanIterator() {}

public:

  void setLocation (const QueryLoc& loc_) { loc = loc_; }

  uint32_t getStateOffset() const { return stateOffset; }

  /**
   * get the static context for this iterator
   */
  static_context* getStaticContext(PlanState& planState) const;

  CollationCache* collationCache(PlanState& planState);

  /**
   * Return true if "this" may return a pending update list.
   */
  virtual bool isUpdating() const { return false; }

  /**
   * Accept method for the PlanIterator-Tree-Visitor
   * 
   * @param PlanIterVisitor
   */
  virtual void accept(PlanIterVisitor&) const = 0;

  /**
   * Begin the execution of the iterator
   * Initializes information required for the plan state
   * and constructs the state object.
   */
  virtual void open(PlanState& planState, uint32_t& offset) = 0;
  
  /** 
   * Produces an output item of the iterator. Implicitly, the first call 
   * of 'producNext' initializes the iterator and allocates resources 
   * (main memory, file descriptors, etc.). 
   *
   * @param stateBLock
   */
#if ZORBA_BATCHING_TYPE == 1
  virtual void produceNext(PlanState& planState) const = 0;
#else
  virtual bool produceNext(store::Item_t& handle, PlanState& planState) const = 0;
#endif


  /** 
   * Restarts the iterator so that the next 'produceNext' call will start 
   * again from the beginning (should not release any resources).  
   *
   * @param planState
   */
  virtual void reset(PlanState& planState) const = 0;

  /** 
   * Finish the execution of the iterator.
   * Releases all resources and destroy the according plan state
   * objects
   * Make sure that no exception is throw when destroying the states.
   * Otherwise we will have a lot of memory leaks.
   *
   * @param planState
   */
  virtual void close(PlanState& planState) throw() = 0;

  /** Returns the size of the state which must be saved for the current iterator
    * on the state block
    */
  virtual uint32_t getStateSize() const = 0;
  
  /** Returns the size of the state for the current iterator 
    * and all its sub-iterators.
    */
  virtual uint32_t getStateSizeOfSubtree() const = 0;

#if ZORBA_BATCHING_TYPE == 1  

  static store::Item_t consumeNext(
        store::Item_t& result,
        const PlanIterator* subIter,
        PlanState& planState)
  {
    try
    {
      // use the producer's (subIter) planstate to access it's batch
      PlanIteratorState* lState = 
      StateTraitsImpl<PlanIteratorState>::getState(planState, subIter->getStateOffset());

      if ( lState->theCurrItem == ZORBA_BATCHING_BATCHSIZE )
      {
        subIter->produceNext(planState);
        lState->theCurrItem = 0;
      }
    }
    catch(error::ZorbaError& e)
    {
      if(loc != NULL)
      {
        e->theQueryLine = loc.getLineBegin();
        e->theQueryColumn = loc.getColumnBegin();
        throw(e);
      }
    }
    return lState->theBatch[lState->theCurrItem++];
  }

#else // ZORBA_BATCHING_TYPE

#ifndef NDEBUG
  static bool consumeNext(
        store::Item_t& result, 
        const PlanIterator* subIter,
        PlanState& planState);
#else
  static bool consumeNext(
        store::Item_t& result, 
        const PlanIterator* subIter,
        PlanState& planState)
  {
    return subIter->produceNext(result, planState);
  }
#endif

#endif // ZORBA_BATCHING_TYPE
};


extern const ::zorba::serialization::ClassVersion g_Batcher_class_versions[];
extern const int g_Batcher_class_versions_count;

/*******************************************************************************
  Class to implement batching
********************************************************************************/
template <class IterType>
class Batcher: public PlanIterator
{
public:
  SERIALIZABLE_CLASS_NO_FACTORY(Batcher)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(Batcher, PlanIterator)
  void serialize(::zorba::serialization::Archiver &ar)
  {
    serialize_baseclass(ar, (PlanIterator*)this);
  }

public:
  Batcher(const Batcher<IterType>& b)  : PlanIterator(b) {}

  Batcher(short sctx, const QueryLoc& loc) : PlanIterator(sctx, loc) {}

  ~Batcher() {}


#if ZORBA_BATCHING_TYPE == 1  
  void produceNext(PlanState& planState) const 
  {
    PlanIteratorState* lState = StateTraitsImpl<PlanIteratorState>::getState(planState, stateOffset);
#ifndef NDEBUG
    ZORBA_ASSERT(lState->theIsOpened); // open must hve been called before
#endif
    uint32_t i = 0;
    do
    {
      static_cast<const IterType*>(this)->nextImpl(lState->theBatch[i], planState);
    } while ( lState->theBatch[i] != NULL && ++i < ZORBA_BATCHING_BATCHSIZE ); 
    // note the pre-increment in the second operand above
  }
#else
  bool produceNext(store::Item_t& result, PlanState& planState) const
  {
#ifndef NDEBUG
    PlanIteratorState* lState = StateTraitsImpl<PlanIteratorState>::getState(planState, stateOffset);
    ZORBA_ASSERT(lState->theIsOpened); // open must have been called before
#endif
    return static_cast<const IterType*>(this)->nextImpl(result, planState);
  }
#endif

  void open(PlanState& planState, uint32_t& offset)
  {
    static_cast<IterType*>(this)->openImpl(planState, offset);
#ifndef NDEBUG
    // do it after openImpl because the state is created there
    PlanIteratorState* lState = StateTraitsImpl<PlanIteratorState>::getState(planState, stateOffset);
    ZORBA_ASSERT( ! lState->theIsOpened ); // don't call open twice
    lState->theIsOpened = true;
#endif
  }

  void reset(PlanState& planState) const
  {
#ifndef NDEBUG
    PlanIteratorState* lState = StateTraitsImpl<PlanIteratorState>::getState(planState, stateOffset);
    ZORBA_ASSERT( lState->theIsOpened ); // must be open
#endif
    static_cast<const IterType*>(this)->resetImpl(planState);
  }

  void close(PlanState& planState) throw()
  {
#ifndef NDEBUG
    PlanIteratorState* lState = StateTraitsImpl<PlanIteratorState>::getState(planState, stateOffset);
#endif
    static_cast<IterType*>(this)->closeImpl(planState);
#ifndef NDEBUG
    lState->theIsOpened = false;
#endif
  }

  inline void nextImpl(store::Item_t& result, PlanState& planState) const;

  inline void openImpl(PlanState& planState, uint32_t& offset) const;

  inline void resetImpl(PlanState& planState) const;

  inline void closeImpl(PlanState& planState);
};

} /* namespace zorba */

#endif  /* ZORBA_ITERATOR_H */

/*
 * Local variables:
 * mode: c++
 * End:
 */
