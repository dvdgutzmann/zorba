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

#include "util/stl_util.h"
#include "zorbaerrors/error_manager.h"

#ifndef NDEBUG
ZORBA_DLL_PUBLIC bool g_abort_on_error = false;
#define ERROR_ABORT()   do { if (g_abort_on_error) abort(); } while(0)
#else
#define ERROR_ABORT()   ((void)0)
#endif /* NDEBUG */

namespace zorba {
namespace error {

ErrorManager::ErrorManager() {
}

ErrorManager::~ErrorManager() {
  ztd::delete_ptr_seq( errors_ );
}

} // namespace error
} // namespace zorba
/* vim:set et sw=2 ts=2: */
