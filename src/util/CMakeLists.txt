# Copyright 2006-2016 zorba.io
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

SET(UTIL_SRCS
  ascii_util.cpp
  csv_parser.cpp
  dynamic_bitset.cpp
  error_util.cpp
  fs_util.cpp
  indent.cpp
  json_parser.cpp
  json_util.cpp
  locale.cpp
  stream_util.cpp
  string_util.cpp
  time_util.cpp
  time_parse.cpp
  unicode_util.cpp
  unicode_categories.cpp
  uri_util.cpp
  utf8_util.cpp
  utf8_streambuf.cpp
  xml_util.cpp
  fx/fxcharheap.cpp
  string/empty_rep_base.cpp
  http_util.h
  http_util.cpp)

IF (NOT ZORBA_CXX_NULLPTR)
  LIST(APPEND UTIL_SRCS cxx_util.cpp)
ENDIF (NOT ZORBA_CXX_NULLPTR)

IF (ZORBA_WITH_FILE_ACCESS)
  LIST(APPEND UTIL_SRCS mmap_file.cpp)
ENDIF (ZORBA_WITH_FILE_ACCESS)

IF (ZORBA_NO_ICU)
  LIST(APPEND UTIL_SRCS
    passthru_streambuf.cpp
    zorba_regex.cpp
    zorba_regex_engine.cpp)
ELSE (ZORBA_NO_ICU)
  LIST(APPEND UTIL_SRCS
    icu_regex.cpp
    icu_streambuf.cpp)
ENDIF (ZORBA_NO_ICU)

HEADER_GROUP_SUBFOLDER(UTIL_SRCS fx)
HEADER_GROUP_SUBFOLDER(UTIL_SRCS win32)

ADD_SRC_SUBFOLDER(UTIL_SRCS hash UTIL_HASH_SRCS)
