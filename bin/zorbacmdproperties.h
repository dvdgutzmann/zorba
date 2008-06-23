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
#ifndef ZORBA_CMD_PROPERTIES_H
#define ZORBA_CMD_PROPERTIES_H

#include <boost/program_options/options_description.hpp>

#include <zorba/config.h>

class ZorbaCMDProperties 
{
private:
  // generic options
  bool        thePrintHelp;
  bool        thePrintVersion;

  // output options
  std::string theOutputFile;
  bool        thePrintQuery, theSerializeAsHTML, 
              theIndent, theByteOrderMark, theOmitXMLDeclaration;

  // static context options
  std::string theBaseUri, theBoundarySpace, theDefaultCollation, 
              theConstructionMode, theOrderingMode;

  // optimization level (i.e. O0, O1)
  std::string theOptLevel;

#ifdef ZORBA_DEBUGGER
  // debug mode
  bool theDebugServer;
  bool theDebugClient;
  std::string thePorts;
  unsigned int theRequestPort;
  unsigned int theEventPort;
#endif

public:
  struct ExternalVariable {
    std::string var_name;
    std::string var_value;
    bool        inline_file;
  };
  // execution options
  typedef std::vector<ExternalVariable> ExternalVars_t;
  typedef std::vector<std::string>      QueriesOrFiles_t;
  
private:
  bool                     theTiming, theInlineQuery;
  int                      theMultipleExecutions;
  QueriesOrFiles_t         theQueriesOrFiles;
  std::string              theContextItem;

  std::vector<std::string> theExternalVarsString;
  ExternalVars_t           theExternalVars;
  
  // all the options that are visible to the user, i.e. printed in the help message
  boost::program_options::options_description theVisibleOptions;

public:
  // load the properties
  bool
  loadProperties(int argc, char* argv[]);

  // general options
  bool 
  printVersion()        { return thePrintVersion; }
  
  bool 
  printHelp()           { return thePrintHelp; }
  
  bool 
  printQuery()          { return thePrintQuery; }

  bool 
  useTiming()           { return theTiming; }

  int 
  getNoOfExecutions()   { return theMultipleExecutions>=0
                                 ?theMultipleExecutions:1; }

  bool 
  inlineQuery()         { return theInlineQuery; }

  const std::string&
  getBoundarySpace()    { return theBoundarySpace; }

  const std::string& 
  getDefaultCollation() { return theDefaultCollation; }

  const std::string& 
  getBaseUri()          { return theBaseUri; }

  const std::string& 
  getConstructionMode() { return theConstructionMode; }

  const std::string& 
  getOrderingMode()     { return theOrderingMode; }

  const std::string& 
  getOutputFile()       { return theOutputFile; }

  bool 
  serializeHTML()       { return theSerializeAsHTML; }

  bool 
  indent()              { return theIndent; }

  bool 
  byteOrderMark()       { return theByteOrderMark; }

  bool
  omitXMLDeclaration()  { return theOmitXMLDeclaration; }

  const std::string&
  getContextItem()      { return theContextItem; }

  const std::string&
  getOptLevel()         { return theOptLevel; } 

  ExternalVars_t::const_iterator
  externalVarsBegin()   { return theExternalVars.begin(); }

  ExternalVars_t::const_iterator
  externalVarsEnd()     { return theExternalVars.end(); }

  QueriesOrFiles_t::const_iterator
  queriesOrFilesBegin() { return theQueriesOrFiles.begin(); }

  QueriesOrFiles_t::const_iterator
  queriesOrFilesEnd()   { return theQueriesOrFiles.end(); }
  
#ifdef ZORBA_DEBUGGER
  bool
  debugServer() { return theDebugServer; }
  
  bool
  debugClient() { return theDebugClient; }

  unsigned int
  requestPort() { return theRequestPort; }

  unsigned int
  eventPort() { return theEventPort; }
#endif

  void 
  printHelp(std::ostream& os) 
  {
    os << "Zorba XQuery Engine" << std::endl;
    os << "Usage: zorba [options]  [-i Query ...] | [-f QueryFile ... ]" << std::endl;
    os << theVisibleOptions << std::endl; 
  }
};
#endif
