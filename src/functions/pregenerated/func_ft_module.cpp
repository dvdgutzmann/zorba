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
 
// ******************************************
// *                                        *
// * THIS IS A GENERATED FILE. DO NOT EDIT! *
// * SEE .xml FILE WITH SAME NAME           *
// *                                        *
// ******************************************


#include "stdafx.h"
#include "runtime/full_text/ft_module.h"
#include "functions/func_ft_module.h"


namespace zorba{



PlanIter_t full_text_current_lang::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new CurrentLangIterator(sctx, loc, argv);
}

PlanIter_t full_text_host_lang::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new HostLangIterator(sctx, loc, argv);
}

PlanIter_t full_text_is_stem_lang_supported::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new IsStemLangSupportedIterator(sctx, loc, argv);
}

PlanIter_t full_text_is_stop_word_lang_supported::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new IsStopWordLangSupportedIterator(sctx, loc, argv);
}

PlanIter_t full_text_is_thesaurus_lang_supported::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new IsThesaurusLangSupportedIterator(sctx, loc, argv);
}

PlanIter_t full_text_is_stop_word::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new IsStopWordIterator(sctx, loc, argv);
}

PlanIter_t full_text_stem::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new StemIterator(sctx, loc, argv);
}

PlanIter_t full_text_strip_diacritics::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new StripDiacriticsIterator(sctx, loc, argv);
}

PlanIter_t full_text_thesaurus_lookup::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new ThesaurusLookupIterator(sctx, loc, argv);
}



PlanIter_t full_text_tokenize_string::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new TokenizeStringIterator(sctx, loc, argv);
}

void populate_context_ft_module(static_context* sctx)
{
  {
    

    DECL_WITH_KIND(sctx, full_text_current_lang,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","current-lang"), 
        GENV_TYPESYSTEM.LANGUAGE_TYPE_ONE),
        FunctionConsts::FULL_TEXT_CURRENT_LANG_0);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_host_lang,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","host-lang"), 
        GENV_TYPESYSTEM.LANGUAGE_TYPE_ONE),
        FunctionConsts::FULL_TEXT_HOST_LANG_0);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_is_stem_lang_supported,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","is-stem-lang-supported"), 
        GENV_TYPESYSTEM.LANGUAGE_TYPE_ONE, 
        GENV_TYPESYSTEM.BOOLEAN_TYPE_ONE),
        FunctionConsts::FULL_TEXT_IS_STEM_LANG_SUPPORTED_1);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_is_stop_word_lang_supported,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","is-stop-word-lang-supported"), 
        GENV_TYPESYSTEM.LANGUAGE_TYPE_ONE, 
        GENV_TYPESYSTEM.BOOLEAN_TYPE_ONE),
        FunctionConsts::FULL_TEXT_IS_STOP_WORD_LANG_SUPPORTED_1);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_is_thesaurus_lang_supported,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","is-thesaurus-lang-supported"), 
        GENV_TYPESYSTEM.LANGUAGE_TYPE_ONE, 
        GENV_TYPESYSTEM.BOOLEAN_TYPE_ONE),
        FunctionConsts::FULL_TEXT_IS_THESAURUS_LANG_SUPPORTED_1);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_is_thesaurus_lang_supported,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","is-thesaurus-lang-supported"), 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.LANGUAGE_TYPE_ONE, 
        GENV_TYPESYSTEM.BOOLEAN_TYPE_ONE),
        FunctionConsts::FULL_TEXT_IS_THESAURUS_LANG_SUPPORTED_2);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_is_stop_word,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","is-stop-word"), 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.BOOLEAN_TYPE_ONE),
        FunctionConsts::FULL_TEXT_IS_STOP_WORD_1);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_is_stop_word,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","is-stop-word"), 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.LANGUAGE_TYPE_ONE, 
        GENV_TYPESYSTEM.BOOLEAN_TYPE_ONE),
        FunctionConsts::FULL_TEXT_IS_STOP_WORD_2);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_stem,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","stem"), 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_ONE),
        FunctionConsts::FULL_TEXT_STEM_1);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_stem,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","stem"), 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.LANGUAGE_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_ONE),
        FunctionConsts::FULL_TEXT_STEM_2);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_strip_diacritics,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","strip-diacritics"), 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_ONE),
        FunctionConsts::FULL_TEXT_STRIP_DIACRITICS_1);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_thesaurus_lookup,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","thesaurus-lookup"), 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_PLUS),
        FunctionConsts::FULL_TEXT_THESAURUS_LOOKUP_1);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_thesaurus_lookup,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","thesaurus-lookup"), 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_PLUS),
        FunctionConsts::FULL_TEXT_THESAURUS_LOOKUP_2);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_thesaurus_lookup,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","thesaurus-lookup"), 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.LANGUAGE_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_PLUS),
        FunctionConsts::FULL_TEXT_THESAURUS_LOOKUP_3);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_thesaurus_lookup,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","thesaurus-lookup"), 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.LANGUAGE_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_PLUS),
        FunctionConsts::FULL_TEXT_THESAURUS_LOOKUP_4);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_thesaurus_lookup,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","thesaurus-lookup"), 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.LANGUAGE_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.INTEGER_TYPE_ONE, 
        GENV_TYPESYSTEM.INTEGER_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_PLUS),
        FunctionConsts::FULL_TEXT_THESAURUS_LOOKUP_6);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_tokenize,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","tokenize"), 
        GENV_TYPESYSTEM.ANY_NODE_TYPE_ONE, 
        GENV_TYPESYSTEM.ANY_NODE_TYPE_STAR),
        FunctionConsts::FULL_TEXT_TOKENIZE_1);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_tokenize,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","tokenize"), 
        GENV_TYPESYSTEM.ANY_NODE_TYPE_ONE, 
        GENV_TYPESYSTEM.LANGUAGE_TYPE_ONE, 
        GENV_TYPESYSTEM.ANY_NODE_TYPE_STAR),
        FunctionConsts::FULL_TEXT_TOKENIZE_2);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_tokenize_string,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","tokenize-string"), 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_STAR),
        FunctionConsts::FULL_TEXT_TOKENIZE_STRING_1);

  }


  {
    

    DECL_WITH_KIND(sctx, full_text_tokenize_string,
        (createQName("Error: could not find "prefix" and "localname" attributes for "zorba:function" element","","tokenize-string"), 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.LANGUAGE_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_STAR),
        FunctionConsts::FULL_TEXT_TOKENIZE_STRING_2);

  }

}


}



