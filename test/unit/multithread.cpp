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

#include <vector>
#include <iostream>
#include <sstream>

#include <zorba/zorba.h>
#include <inmemorystore/inmemorystore.h>

using namespace zorba;

void* query_thread_1(void *param);
void* query_thread_2(void *param);
void* query_thread_3(void *param);

#define NR_THREADS  20

struct data {
  Zorba*  lZorba;
  Item    lItem;
} dataObj;

static  XQuery_t    lQuery;

#ifdef ZORBA_HAVE_PTHREAD_H

/*
Simple cloning test
*/
bool
multithread_example_1(Zorba* aZorba)
{
  unsigned int  i;
  pthread_t     pt[NR_THREADS];
  
  try {
    lQuery = aZorba->compileQuery("1+1");

    for(i=0; i<NR_THREADS; i++)
    {
      pthread_create(&pt[i], NULL, query_thread_1, (void*)i);
    }

    //wait for threads to finish
    for(i=0;i<NR_THREADS;i++)
    {
      void  *thread_result;
      pthread_join(pt[i], &thread_result);
    }

    lQuery = NULL;
    
    return true;
  }
  catch (ZorbaException &e) {
    std::cerr << "some exception " << e << std::endl;
    return false;
  }
}


/*
Create multiple threads that compile and execute a query
*/
bool
multithread_example_2(Zorba* aZorba)
{
  unsigned int  i;
  pthread_t     pt[NR_THREADS];
  
  try {
    for(i=0; i<NR_THREADS; i++)
    {
      pthread_create(&pt[i], NULL, query_thread_2, (void*)aZorba);
    }

    //wait for threads to finish
    for(i=0;i<NR_THREADS;i++)
    {
      void  *thread_result;
      pthread_join(pt[i], &thread_result);
    }
    
    return true;
  }
  catch (ZorbaException &e) {
    std::cerr << "some exception " << e << std::endl;
    return false;
  }
}


/*
Create separate queries that are working with the same document loaded in store.
*/
bool
multithread_example_3(Zorba* aZorba)
{
  unsigned int  i;
  pthread_t     pt[NR_THREADS];

  try {
    std::stringstream lInStream("<books><book>Book 1</book><book>Book 2</book><book>Book 3</book></books>");

    Item aItem = aZorba->getXmlDataManager()->loadDocument("books.xml", lInStream);

    dataObj.lZorba = aZorba;
    dataObj.lItem = aItem;
    
    for(i=0; i<NR_THREADS; i++)
    {
      pthread_create(&pt[i], NULL, query_thread_3, (void*)(&dataObj));
    }

    for(i=0;i<NR_THREADS;i++)
    {
      void  *thread_result;
      pthread_join(pt[i], &thread_result);
    }

    return true;
  }
  catch (ZorbaException &e) {
    std::cerr << "some exception " << e << std::endl;
    return false;
  }
}

int
multithread(int argc, char* argv[])
{
  store::SimpleStore* lStore = inmemorystore::InMemoryStore::getInstance();
  Zorba*              lZorba = Zorba::getInstance(lStore);
  bool                res = false;

  std::cout << std::endl  << "executing multithread test 1 : ";
  res = multithread_example_1(lZorba);
  if (!res) {
    std::cout << "Failed" << std::endl;
    lZorba->shutdown();
    inmemorystore::InMemoryStore::shutdown(lStore);
    return 1;
  }
  else std::cout << "Passed" << std::endl;

  std::cout << std::endl  << "executing multithread test 2 : ";
  res = multithread_example_2(lZorba);
  if (!res) {
    std::cout << "Failed" << std::endl;
    lZorba->shutdown();
    inmemorystore::InMemoryStore::shutdown(lStore);
    return 1;
  }
  else std::cout << "Passed" << std::endl;

  std::cout << std::endl  << "executing multithread test 3 : ";
  res = multithread_example_3(lZorba);
  if (!res) {
    std::cout << "Failed" << std::endl;
    lZorba->shutdown();
    inmemorystore::InMemoryStore::shutdown(lStore);
    return 1;
  }
  else std::cout << "Passed" << std::endl;
  
  lZorba->shutdown();
  inmemorystore::InMemoryStore::shutdown(lStore);
  return 0;
}


void* query_thread_1(void *param)
{
  XQuery_t        xquery_clone;

  xquery_clone = lQuery->clone();
  if(xquery_clone == NULL)
  {
    std::cout << "cannot clone xquery object" << std::endl;
    return (void*)1;
  }

  std::ostringstream os;
  os << xquery_clone << std::endl;

  xquery_clone->close();

  return (void*)0;
}


void* query_thread_2(void *param)
{
  XQuery_t    query;
  
  query = ((Zorba*)param)->compileQuery("2+2");

  std::ostringstream os;
  os << query << std::endl;

  query->close();

  return (void*)0;
}


void* query_thread_3(void *param)
{
  data* var = (data*)param;

  XQuery_t lQuery = var->lZorba->compileQuery("declare variable $var external; doc('books.xml')//book");

  DynamicContext* lCtx = lQuery->getDynamicContext();

  lCtx->setVariable("var", var->lItem);

  std::ostringstream os;
  os << lQuery << std::endl;

  return (void*)0;
}
#endif
