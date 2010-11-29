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

#ifndef ZORBA_FULL_TEXT_WORDNET_THESAURUS_H
#define ZORBA_FULL_TEXT_WORDNET_THESAURUS_H

#include <deque>

#include "../ft_thesaurus.h"
#include "wn_types.h"

namespace zorba {
namespace wordnet {

///////////////////////////////////////////////////////////////////////////////

/**
 * A %wordnet::thesaurus is an ft_thesaurus for Wordnet.
 * See: http://wordnet.princeton.edu/
 */
class thesaurus : public ft_thesaurus {
public:
  thesaurus( zstring const &phrase, zstring const &relationship,
             ft_int at_least, ft_int at_most );
  ~thesaurus();

  bool next( zstring *synonym );

private:
  pointer::type const ptr_type_;
  ft_int const at_least_, at_most_;

  ft_int level_;

  typedef std::deque<synset_id_t> synset_id_queue;
  synset_id_queue synset_id_queue_;

  typedef std::deque<lemma_id_t> synonym_queue;
  synonym_queue synonym_queue_;

  // forbid these
  thesaurus( thesaurus const& );
  thesaurus& operator=( thesaurus const& );
};

///////////////////////////////////////////////////////////////////////////////

} // namespace wordnet
} // namespace zorba

#endif  /* ZORBA_FULL_TEXT_WORDNET_THESAURUS_H */
/* vim:set et sw=2 ts=2: */