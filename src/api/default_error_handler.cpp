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

#include <zorba/default_error_handler.h>
#include <zorba/zorba_exception.h>

namespace zorba {

DefaultErrorHandler::~DefaultErrorHandler() {
  // out-of-line since it's virtual
}

void DefaultErrorHandler::error( ZorbaException const& ) {
  throw; // MUST NOT THROW 'e' OR THE OBJECT WILL BE SLICED!
}

} // namespace zorba
/* vim:set et sw=2 ts=2: */
