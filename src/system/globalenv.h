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
#ifndef ZORBA_GLOBALENV_H
#define ZORBA_GLOBALENV_H

#include <memory>
#include <zorba/config.h>
#include "common/common.h"
#ifndef ZORBA_NO_BIGNUMBERS
#include "zorbatypes/m_apm.h"
#endif
#include "common/shared_types.h"

namespace zorba {

class RootTypeManager;
class root_static_context;
class XQueryXConvertor;

namespace store {
class Store;
}


// exported for unit testing only
class ZORBA_DLL_PUBLIC GlobalEnvironment 
{
 public:
   ~GlobalEnvironment();
 private:
  void
  init_icu();

  void
  cleanup_icu();

 public:
  static void init(store::Store* store);
  static void destroy();
  static GlobalEnvironment& getInstance()
  {
    assert(m_globalEnv);
    return *m_globalEnv;
  }


  RootTypeManager& getRootTypeManager();
  static_context& getRootStaticContext();
  bool isRootStaticContextInitialized();
  XQueryCompilerSubsystem& getCompilerSubsystem();
  store::Store& getStore();
  store::ItemFactory* getItemFactory();
  store::IteratorFactory* getIteratorFactory();
#ifdef ZORBA_XQUERYX
  XQueryXConvertor    *getXQueryXConvertor();
#endif
private:
  GlobalEnvironment();

  store::Store*                          m_store;
  root_static_context*                   m_rootStaticContext;
#ifndef ZORBA_NO_BIGNUMBERS
  M_APM                                  m_mapm; // this is a pointer type
#endif
  XQueryCompilerSubsystem*               m_compilerSubSys;

  static GlobalEnvironment             * m_globalEnv;

#ifdef ZORBA_XQUERYX
  XQueryXConvertor                      *xqueryx_convertor;
#endif

public:
#if defined ZORBA_WITH_REST && defined ZORBA_WITH_SSL && defined ZORBA_VERIFY_PEER_SSL_CERTIFICATE
#if defined WIN32
  //path where root CA certificates for SSL are kept (filename is "cacert.pem")
  //certificates are used by curl/ssleay (or openssl) when connecting to https.
  char    g_curl_root_CA_certificates_path[1024];
#endif
#endif
};

#define GENV GlobalEnvironment::getInstance()

#define GENV_TYPESYSTEM GlobalEnvironment::getInstance().getRootTypeManager()

#define GENV_COMPILERSUBSYS GlobalEnvironment::getInstance().getCompilerSubsystem()

#define GENV_STORE GlobalEnvironment::getInstance().getStore()

#define GENV_ITEMFACTORY GlobalEnvironment::getInstance().getItemFactory()

#define GENV_ITERATOR_FACTORY GlobalEnvironment::getInstance().getIteratorFactory()

#define GENV_ROOT_STATIC_CONTEXT GlobalEnvironment::getInstance().getRootStaticContext()

}

#endif /* ZORBA_GLOBALENV_H */
/* vim:set ts=2 sw=2: */
