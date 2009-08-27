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
#ifndef ZORBA_RUNTIME_REST_H
#define ZORBA_RUNTIME_REST_H

#include <istream>
#include <memory>
#include <streambuf>

#include "common/shared_types.h"

#include "runtime/base/narybase.h"

namespace zorba {

  
typedef void CURL;
typedef void CURLM;


class CurlStreamBuffer;
typedef rchandle<CurlStreamBuffer> CurlStreamBuffer_t;


class CurlStreamBuffer : public std::streambuf, public SimpleRCObject
{
public:
  CurlStreamBuffer(CURLM* aMultiHandle, CURL* aEasyHandle);
  virtual ~CurlStreamBuffer();

  virtual int overflow(int c);
  virtual int underflow();
  int multi_perform();

  const char* getErrorBuffer() const;

protected:
  char* CurlErrorBuffer;
  CURLM* MultiHandle;
  CURL* EasyHandle;

  // callback called by curl
  static size_t
  write_callback(char *buffer, size_t size, size_t nitems, void *userp);
  
  static const int INITIAL_BUFFER_SIZE = 1024;
};


/****************************************************************************
 *
 * rest-get Iterator state
 *
 ****************************************************************************/

class ZorbaRestGetIteratorState : public PlanIteratorState
{
public:
  ZorbaRestGetIteratorState();
  ~ZorbaRestGetIteratorState();

  void init(PlanState&);
  void reset(PlanState&);

  CURLM* MultiHandle;
  CURL* EasyHandle;
  CurlStreamBuffer_t theStreamBuffer;
  std::auto_ptr<std::vector<std::string> > headers;
};

/****************************************************************************
 *
 * rest-get Iterator 
 *
 ****************************************************************************/

class ZorbaRestGetIterator : public NaryBaseIterator<ZorbaRestGetIterator,
                                                     ZorbaRestGetIteratorState > 
{
public:                                                                  
  ZorbaRestGetIterator(
        short sctx,
        const QueryLoc& loc,
        std::vector<PlanIter_t>& aChildren,
        bool tidy = false)
    :
    NaryBaseIterator<ZorbaRestGetIterator, ZorbaRestGetIteratorState >(sctx, loc, aChildren),
    isGetTidy(tidy)
  { } 

  void openImpl(PlanState& planState, uint32_t& offset);

  bool nextImpl(store::Item_t& result, PlanState& aPlanState) const;
                                                                         
  virtual void 
  accept(PlanIterVisitor& v) const
  {
    v.beginVisit(*this);
    std::vector<PlanIter_t>::const_iterator iter =  theChildren.begin();
    std::vector<PlanIter_t>::const_iterator lEnd =  theChildren.end();
    for ( ; iter != lEnd; ++iter ) {
      ( *iter )->accept ( v );
    }
    v.endVisit(*this);
  }
private:
  bool isGetTidy;
public:
  SERIALIZABLE_CLASS(ZorbaRestGetIterator)
  SERIALIZABLE_CLASS_CONSTRUCTOR2T(ZorbaRestGetIterator, NaryBaseIterator<ZorbaRestGetIterator, ZorbaRestGetIteratorState >)
  void serialize(::zorba::serialization::Archiver &ar)
  {
    serialize_baseclass(ar, (NaryBaseIterator<ZorbaRestGetIterator, ZorbaRestGetIteratorState >*)this);
    ar & isGetTidy;
  }
};

/****************************************************************************
 *
 * zorba-rest post iterator
 *
 ****************************************************************************/

class ZorbaRestPostIterator : public NaryBaseIterator<ZorbaRestPostIterator, ZorbaRestGetIteratorState >
{
public:
  ZorbaRestPostIterator( short sctx, const QueryLoc& loc, std::vector<PlanIter_t>& aChildren, bool tidy = false)
    : NaryBaseIterator<ZorbaRestPostIterator, ZorbaRestGetIteratorState >(sctx, loc, aChildren),
      isPostTidy(tidy)
  { }

  void openImpl(PlanState& planState, uint32_t& offset);

  bool nextImpl(store::Item_t& result, PlanState& aPlanState) const;

  virtual void
      accept(PlanIterVisitor& v) const
  {
    v.beginVisit(*this);
    std::vector<PlanIter_t>::const_iterator iter =  theChildren.begin();
    std::vector<PlanIter_t>::const_iterator lEnd =  theChildren.end();
    for ( ; iter != lEnd; ++iter ) {
      ( *iter )->accept ( v );
    }
    v.endVisit(*this);
  }
private:
  bool isPostTidy;
public:
  SERIALIZABLE_CLASS(ZorbaRestPostIterator)
  SERIALIZABLE_CLASS_CONSTRUCTOR2T(ZorbaRestPostIterator, NaryBaseIterator<ZorbaRestPostIterator, ZorbaRestGetIteratorState >)
  void serialize(::zorba::serialization::Archiver &ar)
  {
    serialize_baseclass(ar, (NaryBaseIterator<ZorbaRestPostIterator, ZorbaRestGetIteratorState >*)this);
    ar & isPostTidy;	
  }
};

/****************************************************************************
 *
 * zorba-rest put iterator
 *
 ****************************************************************************/

class ZorbaRestPutIterator : public NaryBaseIterator<ZorbaRestPutIterator, ZorbaRestGetIteratorState >
{
public:
  ZorbaRestPutIterator( short sctx,const QueryLoc& loc, std::vector<PlanIter_t>& aChildren)
    : NaryBaseIterator<ZorbaRestPutIterator, ZorbaRestGetIteratorState >(sctx, loc, aChildren)
  { }

  void openImpl(PlanState& planState, uint32_t& offset);

  bool nextImpl(store::Item_t& result, PlanState& aPlanState) const;

  virtual void accept(PlanIterVisitor& v) const
  {
    v.beginVisit(*this);
    std::vector<PlanIter_t>::const_iterator iter =  theChildren.begin();
    std::vector<PlanIter_t>::const_iterator lEnd =  theChildren.end();
    for ( ; iter != lEnd; ++iter ) {
      ( *iter )->accept ( v );
    }
    v.endVisit(*this);
  }

public:
  SERIALIZABLE_CLASS(ZorbaRestPutIterator)
  SERIALIZABLE_CLASS_CONSTRUCTOR2T(ZorbaRestPutIterator, NaryBaseIterator<ZorbaRestPutIterator, ZorbaRestGetIteratorState >)
  void serialize(::zorba::serialization::Archiver &ar)
  {
    serialize_baseclass(ar, (NaryBaseIterator<ZorbaRestPutIterator, ZorbaRestGetIteratorState >*)this);
  }
};

/****************************************************************************
 *
 * zorba-rest put iterator
 *
 ****************************************************************************/

class ZorbaRestDeleteIterator : public NaryBaseIterator<ZorbaRestDeleteIterator, ZorbaRestGetIteratorState >
{
public:
  ZorbaRestDeleteIterator( short sctx,const QueryLoc& loc, std::vector<PlanIter_t>& aChildren)
    : NaryBaseIterator<ZorbaRestDeleteIterator, ZorbaRestGetIteratorState >(sctx, loc, aChildren)
  { }

  void openImpl(PlanState& planState, uint32_t& offset);

  bool nextImpl(store::Item_t& result, PlanState& aPlanState) const;

  virtual void accept(PlanIterVisitor& v) const
  {
    v.beginVisit(*this);
    std::vector<PlanIter_t>::const_iterator iter =  theChildren.begin();
    std::vector<PlanIter_t>::const_iterator lEnd =  theChildren.end();
    for ( ; iter != lEnd; ++iter ) {
      ( *iter )->accept ( v );
    }
    v.endVisit(*this);
  }
public:
  SERIALIZABLE_CLASS(ZorbaRestDeleteIterator)
  SERIALIZABLE_CLASS_CONSTRUCTOR2T(ZorbaRestDeleteIterator, NaryBaseIterator<ZorbaRestDeleteIterator, ZorbaRestGetIteratorState >)
  void serialize(::zorba::serialization::Archiver &ar)
  {
    serialize_baseclass(ar, (NaryBaseIterator<ZorbaRestDeleteIterator, ZorbaRestGetIteratorState >*)this);
  }
};

/****************************************************************************
 *
 * zorba-rest head iterator
 *
 ****************************************************************************/

class ZorbaRestHeadIterator : public NaryBaseIterator<ZorbaRestHeadIterator, ZorbaRestGetIteratorState >
{
public:
  ZorbaRestHeadIterator( short sctx,const QueryLoc& loc, std::vector<PlanIter_t>& aChildren)
    : NaryBaseIterator<ZorbaRestHeadIterator, ZorbaRestGetIteratorState >(sctx, loc, aChildren)
  { }

  void openImpl(PlanState& planState, uint32_t& offset);

  bool nextImpl(store::Item_t& result, PlanState& aPlanState) const;

  virtual void accept(PlanIterVisitor& v) const
  {
    v.beginVisit(*this);
    std::vector<PlanIter_t>::const_iterator iter =  theChildren.begin();
    std::vector<PlanIter_t>::const_iterator lEnd =  theChildren.end();
    for ( ; iter != lEnd; ++iter ) {
      ( *iter )->accept ( v );
    }
    v.endVisit(*this);
  }
public:
  SERIALIZABLE_CLASS(ZorbaRestHeadIterator)
  SERIALIZABLE_CLASS_CONSTRUCTOR2T(ZorbaRestHeadIterator, NaryBaseIterator<ZorbaRestHeadIterator, ZorbaRestGetIteratorState >)
  void serialize(::zorba::serialization::Archiver &ar)
  {
    serialize_baseclass(ar, (NaryBaseIterator<ZorbaRestHeadIterator, ZorbaRestGetIteratorState >*)this);
  }
};


} /* namespace zorba */

#endif
