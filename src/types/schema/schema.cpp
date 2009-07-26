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
#include <string>

#include "schema.h"
#include "StrX.h"
#include "LoadSchemaErrorHandler.h"
#include "PrintSchema.h"
#include "XercesParseUtils.h"

#include <zorbatypes/xerces_xmlcharray.h>

#include "zorbamisc/ns_consts.h"
#include "zorbaerrors/Assert.h"

#include "types/typeimpl.h"
#include "types/root_typemanager.h"
#include "types/typeops.h"
#include "types/casting.h"

#include "system/globalenv.h"

#include "store/api/item_factory.h"

//using namespace std;

namespace zorba
{
SERIALIZABLE_CLASS_VERSIONS(Schema)
END_SERIALIZABLE_CLASS_VERSIONS(Schema)

//#define DO_PRINT_SCHEMA_INFO
//#define DO_TRACE

#ifdef DO_TRACE

#define TRACE(msg)                                                            \
  {                                                                           \
    std::cout << __FUNCTION__ << ": " << msg << std::endl; std::cout.flush(); \
  }

#else
#define TRACE(msg)
#endif


const char* Schema::XSD_NAMESPACE = XML_SCHEMA_NS;

bool Schema::_isInitialized = false;

#ifndef ZORBA_NO_XMLSCHEMA

xqp_string transcode(const XMLCh* const str)
{
    char* trStr = XMLString::transcode(str);
    xqp_string res (trStr);
    XMLString::release(&trStr);
    return res;
}


SchemaLocationEntityResolver::SchemaLocationEntityResolver(
    const XMLCh* logical_uri,
    std::string& location)
  :
  theLocation(location),
  theLogicalURI(logical_uri) 
{
}


InputSource* SchemaLocationEntityResolver::resolveEntity(
    const XMLCh* const publicId,
    const XMLCh* const systemID)
{
  TRACE("pId: " << StrX(publicId) << "  sId: " << StrX(systemID) );;


  if (XMLString::compareString(systemID, theLogicalURI) == 0) 
  {
    XMLChArray xerces_xsdURL (theLocation.c_str());
    XMLURL url (xerces_xsdURL.get ());
    // URLInputSource is a Xerces class that provides for the parser access to
    // data which is referenced via a URL, as apposed to a local file name. The
    // objective of this class is to create an input stream via which the parser
    // can spool in data from the referenced source.
    return new URLInputSource(url);
  }
  else 
  {
    return NULL;
  }
}

#endif //ZORBA_NO_XMLSCHEMA


/*******************************************************************************
  Initialize the Xerces platform. Called from GlobalEnvironment::init().
********************************************************************************/
void Schema::initialize()
{
#ifndef ZORBA_NO_XMLSCHEMA
    if (_isInitialized)
        return;

    try
    {
        XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();
        _isInitialized = true;
    }
    catch (const XERCES_CPP_NAMESPACE::XMLException& toCatch)
    {
        std::cerr   << "Error during Xerces-C initialization! Message:\n"
               << StrX(toCatch.getMessage()) << std::endl;
        return;         
    }
#endif
}


/*******************************************************************************
  Terminate the Xerces platform. Called from GlobalEnvironment::destroy().
********************************************************************************/
void Schema::terminate()
{
#ifndef ZORBA_NO_XMLSCHEMA
    if (_isInitialized)
    {
        XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate();    
        _isInitialized = false;
    }
#endif
}


/*******************************************************************************

********************************************************************************/
Schema::Schema()
{
#ifndef ZORBA_NO_XMLSCHEMA
    _grammarPool = new XMLGrammarPoolImpl(XMLPlatformUtils::fgMemoryManager);
    _udTypesCache = new serializable_hashmap<xqtref_t>;
#endif
}

Schema::Schema(::zorba::serialization::Archiver &ar) 
{
#ifndef ZORBA_NO_XMLSCHEMA
    initialize();
    _grammarPool = new XMLGrammarPoolImpl(XMLPlatformUtils::fgMemoryManager);
#endif
}


/*******************************************************************************

********************************************************************************/
Schema::~Schema()
{
#ifndef ZORBA_NO_XMLSCHEMA
    delete _grammarPool;
    delete _udTypesCache;
#endif
}



/*******************************************************************************

********************************************************************************/
void Schema::printXSDInfo(bool excludeBuiltIn)
{
#ifndef ZORBA_NO_XMLSCHEMA
  PrintSchema::printInfo(excludeBuiltIn, _grammarPool);
#endif
}


#ifndef ZORBA_NO_XMLSCHEMA


/*******************************************************************************

********************************************************************************/
void Schema::registerXSD(const char* xsdURL, std::string& location, const QueryLoc& loc)
{
    std::auto_ptr<SAX2XMLReader> parser;

  TRACE("url=" << xsdURL << " loc=" << location);

    try
  {
    SAX2XMLReader* reader = XMLReaderFactory::createXMLReader(XMLPlatformUtils::fgMemoryManager,
                                                   _grammarPool);

    parser.reset(reader);
    parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);                     // Perform namespace processing
    parser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, false);             // Do not report attributes used for namespace declarations and optionally  do not report original prefixed names
    parser->setFeature(XMLUni::fgXercesSchema, true);                           // Enable parser's schema support
    parser->setFeature(XMLUni::fgXercesSchemaFullChecking, true);               // time and memory intensive to be disabled only if schema files previously checked
    parser->setFeature(XMLUni::fgSAX2CoreValidation, true);                     // report all validation errors
    parser->setFeature(XMLUni::fgXercesDynamic, true);                          // validate only if schema is available
    ////parser->setFeature(XMLUni::fgXercesLoadSchema, true);                     // load the schema in the grammar pool
    parser->setFeature(XMLUni::fgXercesDisableDefaultEntityResolution, true);   // disables network resolver

        parser->setProperty(XMLUni::fgXercesScannerName, (void *)XMLUni::fgSGXMLScanner);

    // this doesn't seem to work
    //XMLChArray extSchemaLocation("namespace /location/file.xsd");
    //parser->setFeature(XMLUni::fgXercesSchemaExternalSchemaLocation, extSchemaLocation.get());
    

        LoadSchemaErrorHandler handler(loc);
        parser->setErrorHandler(&handler);

    XMLChArray xerces_xsdURL(xsdURL);
        SchemaLocationEntityResolver lEntityResolver(xerces_xsdURL.get(), location);
        parser->setEntityResolver(&lEntityResolver);

    //this works but using loadProlog
    //parser->loadGrammar("file:///location/file.xsd",
    //                    Grammar::SchemaGrammarType, true);

        parser->loadGrammar(xsdURL, Grammar::SchemaGrammarType, true);
        
        if (handler.getSawErrors())
        {
            handler.resetErrors();
    }
    }
    catch (const OutOfMemoryException&)
    {
    ZORBA_ERROR_LOC_DESC(XQST0059, loc,
                         std::string("OutOfMemoryException during parsing ") +
                         std::string(xsdURL));
    }
    catch (const XMLException& e)
    {
    ZORBA_ERROR_LOC_DESC_OSS(XQST0059, loc,
                             "Error during parsing: " << xsdURL << " "
                             << StrX(e.getMessage()));
    }
  catch (const error::ZorbaError& e) 
  {
      throw e;
    }
    catch (...)
    {
    ZORBA_ERROR_LOC_DESC(XQST0059, loc,
                         std::string("Unexpected exception during parsing: ") +
                         std::string(xsdURL));
    }
  
#ifdef DO_PRINT_SCHEMA_INFO // enable this to debug registered user defined schema types
      printXSDInfo(true);
#endif
}


/*******************************************************************************

********************************************************************************/
void Schema::getTypeNameFromElementName(
    const store::Item* qname,
    store::Item_t& typeName)
{
  XSTypeDefinition* typeDef = getTypeDefForElement(qname);

  if (!typeDef) 
  {
    ZORBA_ERROR_PARAM(XPST0008, "schema-element", qname->getStringValue()->c_str());
  }

  const XMLCh* typeNameStr = typeDef->getName();
  const XMLCh* typeUri = typeDef->getNamespace();

  GENV_ITEMFACTORY->createQName(typeName,
                                StrX(typeUri).localForm(),
                                "",
                                StrX(typeNameStr).localForm());
}


/*******************************************************************************

********************************************************************************/
void Schema::getTypeNameFromAttributeName(
    const store::Item* qname,
    store::Item_t& typeName)
{
  XSTypeDefinition* typeDef = getTypeDefForAttribute(qname);

  if (!typeDef) 
  {
    ZORBA_ERROR_PARAM(XPST0008, "schema-attribute", qname->getStringValue()->c_str());
  }

  const XMLCh* typeNameStr = typeDef->getName();
  const XMLCh* typeUri = typeDef->getNamespace();

  GENV_ITEMFACTORY->createQName(typeName,
                                StrX(typeUri).localForm(),
                                "",
                                StrX(typeNameStr).localForm());
}


/*******************************************************************************

********************************************************************************/
xqtref_t Schema::createXQTypeFromElementName(
    const TypeManager* typeManager,
    const store::Item* qname,
    const bool riseErrors)
{
  TRACE("qn:" << qname->getLocalName()->str() << " @ " << qname->getNamespace()->str() );
  XSTypeDefinition* typeDef = getTypeDefForElement(qname);

  if (!riseErrors && !typeDef)
      return NULL;

  if(!typeDef)
  {
    ZORBA_ERROR_PARAM(XPST0008, "schema-element", qname->getStringValue()->c_str());
  }
  
  xqtref_t res = createXQTypeFromTypeDefinition(typeManager, typeDef);
  TRACE("res:" << res->get_qname()->getLocalName()->str() << " @ " << res->get_qname()->getNamespace()->str());

  return res;
}


/*******************************************************************************

********************************************************************************/
xqtref_t Schema::createXQTypeFromAttributeName(
    const TypeManager* typeManager,
    const store::Item* qname)
{
  XSTypeDefinition* typeDef = getTypeDefForAttribute(qname);

  if (!typeDef) 
  {
    ZORBA_ERROR_PARAM(XPST0008, "schema-attribute", qname->getStringValue()->c_str());
  }

  return createXQTypeFromTypeDefinition(typeManager, typeDef);
}


/*******************************************************************************
  Checks if the Type with the qname exists in the schema as a user-defined type
  if it does than return an XQType for it, if not return NULL
********************************************************************************/
xqtref_t Schema::createXQTypeFromTypeName(
    const TypeManager* typeManager,
    const store::Item* qname)
{
  XSTypeDefinition* typeDef = NULL;
  xqtref_t res;

  TRACE("typeManager: " << typeManager << " type qname: " 
        << qname->getLocalName()->c_str() << "@"
        << qname->getNamespace()->c_str());

  if (_grammarPool == NULL)
        return NULL;

  const char* nsuri_cstr = qname->getNamespace()->c_str();
  const char* local_cstr = qname->getLocalName()->c_str();

    // check the cache first
  std::string key = qname->getLocalName()->str() + ":" + qname->getNamespace()->str() + " " +
    TypeOps::decode_quantifier(TypeConstants::QUANT_ONE);

    if( _udTypesCache->get(key, res) )
        return res;

    // not found in cache, make a new one
  XMLChArray local(local_cstr);
  XMLChArray uri(nsuri_cstr);
  
    bool modelHasChanged;
    XSModel* xsModel = _grammarPool->getXSModel(modelHasChanged);

  typeDef = xsModel->getTypeDefinition(local, uri);
  
  if ( typeDef == NULL )
  {
        res = NULL;
    TRACE("No type definition for " << local << "@" << uri);
    TRACE("add to TypesCache: key:'" << key << "'  t:"
          << ( res==NULL ? "NULL" : TypeOps::decode_quantifier(res->get_quantifier())) );
    // stick it in the cache even if it's NULL
    _udTypesCache->put(key, res);
  }
    else
  {
    res = createXQTypeFromTypeDefinition(typeManager, typeDef);
  }
  
  return res;
}


/*******************************************************************************
  Get the type definition for a globally declared element
********************************************************************************/
XSTypeDefinition* Schema::getTypeDefForElement(const store::Item* qname)
{
  XSTypeDefinition* typeDef = NULL;

  TRACE(" element qname: " << qname->getLocalName()->c_str() << "@" << qname->getNamespace()->c_str());

  if (_grammarPool == NULL)
    return NULL;

  XMLChArray local(qname->getLocalName()->c_str());
  XMLChArray uri(qname->getNamespace()->c_str());

  bool xsModelWasChanged;
  XSModel* model = _grammarPool->getXSModel(xsModelWasChanged);

  XSElementDeclaration* decl = model->getElementDeclaration(local, uri);
  
  if (decl)
    typeDef = decl->getTypeDefinition();

  return typeDef;
}


/*******************************************************************************
  Get the type definition for a globally declared attribute
********************************************************************************/
XSTypeDefinition* Schema::getTypeDefForAttribute(const store::Item* qname)
{
  XSTypeDefinition* typeDef = NULL;

  TRACE(" attribute qname: " << qname->getLocalName()->c_str() << "@" << qname->getNamespace()->c_str());

  if (_grammarPool == NULL)
    return NULL;

  XMLChArray local(qname->getLocalName()->c_str());
  XMLChArray uri(qname->getNamespace()->c_str());

  bool xsModelWasChanged;
  XSModel* model = _grammarPool->getXSModel(xsModelWasChanged);

  XSAttributeDeclaration* decl = model->getAttributeDeclaration(local, uri);
  
  if (decl)
    typeDef = decl->getTypeDefinition();

  return typeDef;
}


/*******************************************************************************

********************************************************************************/
xqtref_t Schema::createXQTypeFromTypeDefinition(
    const TypeManager* typeManager,
    XSTypeDefinition* xsTypeDef)
{
    if (!xsTypeDef)
    {
        ZORBA_ASSERT(false);
        return NULL;
    }

    xqtref_t result;

    if (xsTypeDef->getTypeCategory() == XSTypeDefinition::SIMPLE_TYPE)
    {
        // first check if it is a built-in type
        const XMLCh* uri = xsTypeDef->getNamespace();
        xqp_string strUri = transcode(uri);

    if ( XMLString::equals(strUri.c_str(), Schema::XSD_NAMESPACE) )
        {
            const XMLCh* local = xsTypeDef->getName();
            // maybe there is a better way than comparing strings 
            // but it seems Xerces doesn't have a code for built-in types
            if ( XMLString::equals(SchemaSymbols::fgDT_STRING, local) )
            {
                result = GENV_TYPESYSTEM.STRING_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_INT, local) )
            {
                result = GENV_TYPESYSTEM.INT_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_BOOLEAN, local) )
            {
                result = GENV_TYPESYSTEM.BOOLEAN_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_INTEGER, local) )
            {
                result = GENV_TYPESYSTEM.INTEGER_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_POSITIVEINTEGER, local) )
            {
                result = GENV_TYPESYSTEM.POSITIVE_INTEGER_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_NONPOSITIVEINTEGER, local) )
            {
                result = GENV_TYPESYSTEM.NON_POSITIVE_INTEGER_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_NEGATIVEINTEGER, local) )
            {
                result = GENV_TYPESYSTEM.NEGATIVE_INTEGER_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_NONNEGATIVEINTEGER, local) )
            {
                result = GENV_TYPESYSTEM.NON_NEGATIVE_INTEGER_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_UBYTE, local) )
            {
                result = GENV_TYPESYSTEM.UNSIGNED_BYTE_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_USHORT, local) )
            {
                result = GENV_TYPESYSTEM.UNSIGNED_SHORT_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_UINT, local) )
            {
                result = GENV_TYPESYSTEM.UNSIGNED_INT_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_ULONG, local) )
            {
                result = GENV_TYPESYSTEM.UNSIGNED_LONG_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_BYTE, local) )
            {
                result = GENV_TYPESYSTEM.BYTE_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_SHORT, local) )
            {
                result = GENV_TYPESYSTEM.SHORT_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_LONG, local) )
            {
                result = GENV_TYPESYSTEM.LONG_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_FLOAT, local) )
            {
                result = GENV_TYPESYSTEM.FLOAT_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_DOUBLE, local) )
            {
                result = GENV_TYPESYSTEM.DOUBLE_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_DECIMAL, local) )
            {
                result = GENV_TYPESYSTEM.DECIMAL_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_NORMALIZEDSTRING, local) )
            {
                result = GENV_TYPESYSTEM.NORMALIZED_STRING_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_TOKEN, local) )
            {
                result = GENV_TYPESYSTEM.TOKEN_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_NAME, local) )
            {
                result = GENV_TYPESYSTEM.NAME_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_NCNAME, local) )
            {
                result = GENV_TYPESYSTEM.NCNAME_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_DATETIME, local) )
            {
                result = GENV_TYPESYSTEM.DATETIME_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_DATE, local) )
            {
                result = GENV_TYPESYSTEM.DATE_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_TIME, local) )
            {
                result = GENV_TYPESYSTEM.TIME_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_YEAR, local) )
            {
                result = GENV_TYPESYSTEM.GYEAR_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_YEARMONTH, local) )
            {
                result = GENV_TYPESYSTEM.GYEAR_MONTH_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_MONTHDAY, local) )
            {
                result = GENV_TYPESYSTEM.GMONTH_DAY_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_DAY, local) )
            {
                result = GENV_TYPESYSTEM.GDAY_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_MONTH, local) )
            {
                result = GENV_TYPESYSTEM.GMONTH_DAY_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_DURATION, local) )
            {
                result = GENV_TYPESYSTEM.DURATION_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_ANYSIMPLETYPE, local) )
            {
                result = GENV_TYPESYSTEM.ANY_SIMPLE_TYPE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_BASE64BINARY, local) )
            {
                result = GENV_TYPESYSTEM.BASE64BINARY_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_HEXBINARY, local) )
            {
                result = GENV_TYPESYSTEM.HEXBINARY_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_ANYURI, local) )
            {
                result = GENV_TYPESYSTEM.ANY_URI_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgDT_QNAME, local) )
            {
                result = GENV_TYPESYSTEM.QNAME_TYPE_ONE;
            }
            else if ( XMLString::equals(SchemaSymbols::fgATTVAL_ID, local) )
            {
                result = GENV_TYPESYSTEM.ID_TYPE_ONE;
            }
            // SchemaSymbols::fgDT_NMTOKEN doesn't exist in Xerces
            else if ( XMLString::equals(XMLChArray("NMTOKEN").get (), local) )
            {
                result = GENV_TYPESYSTEM.NMTOKEN_TYPE_ONE;
            }
            else if ( XMLString::equals(XMLChArray("IDREF").get (), local) )
            {
                result = GENV_TYPESYSTEM.IDREF_TYPE_ONE;
            }
      else if ( XMLString::equals(XMLChArray("IDREFS").get (), local) )
      {
        store::Item_t qname;
        GENV_ITEMFACTORY->createQName(qname,
                                      XML_SCHEMA_NS,
                                      "XS",
                                      "IDREFS");

        result = new UserDefinedXQType(typeManager,
                                       qname,
                                       NULL,
                                       TypeConstants::QUANT_ONE,
                                       GENV_TYPESYSTEM.IDREF_TYPE_ONE.getp());
      }
            else if ( XMLString::equals(XMLChArray("ENTITY").get (), local) )
            {
        store::Item_t qname;
        GENV_ITEMFACTORY->createQName(qname,
                                      XML_SCHEMA_NS,
                                      "XS",
                                      "ENTITIES");

        result = new UserDefinedXQType(typeManager,
                                       qname,
                                       NULL,
                                       TypeConstants::QUANT_ONE,
                                       GENV_TYPESYSTEM.ENTITY_TYPE_ONE.getp());
            }
      else if ( XMLString::equals(XMLChArray("ENTITIES").get (), local) )
      {
        result = GENV_TYPESYSTEM.ENTITY_TYPE_PLUS;
      }
            else if ( XMLString::equals(XMLChArray("NOTATION").get (), local) )
            {
                result = GENV_TYPESYSTEM.NOTATION_TYPE_ONE;
            }
            // YearMonthDuration and DayTimeDuration are not in schema spec
            else
            {
                // type not covered  
                //cout << "Assertion Error: Type unknown " << StrX(local).localForm() << "@" << Schema::XSD_NAMESPACE; cout.flush(); 
                ZORBA_ASSERT(false);
                result = NULL;
            }
        }
        else
        {
            // must be a user defined simple type
            XSSimpleTypeDefinition * xsSimpleTypeDef = (XSSimpleTypeDefinition *)xsTypeDef;
      
            xqp_string lNamespace (strUri);
            xqp_string lPrefix;
            xqp_string lLocal = transcode(xsTypeDef->getName());
      
            store::Item_t qname;
      GENV_ITEMFACTORY->createQName(qname,
                                    lNamespace.getStore(),
                                    lPrefix.getStore(),
                                    lLocal.getStore());
      
      
            switch ( xsSimpleTypeDef->getVariety() )
            {
            case XSSimpleTypeDefinition::VARIETY_ATOMIC:
                {
                    XSTypeDefinition* baseTypeDef = xsTypeDef->getBaseType();
                    if ( !baseTypeDef )
                    {
                        //error allway must have a baseType
                        ZORBA_ASSERT(false);             
                        result = NULL;
                    }
        
        xqtref_t baseXQType = createXQTypeFromTypeDefinition(typeManager, baseTypeDef);
        
        xqtref_t xqType = xqtref_t(new UserDefinedXQType(typeManager,
                                                         qname,
                                                         baseXQType,
                                                         TypeConstants::QUANT_ONE,
                                                         XQType::SIMPLE_CONTENT_KIND));
          
                    result = xqType;
                }
                break;
      
            case XSSimpleTypeDefinition::VARIETY_LIST:
                {
                    XSSimpleTypeDefinition * itemTypeDef = xsSimpleTypeDef->getItemType();
                    if ( !itemTypeDef )
                    {
                        //error since VARIETY is LIST must have an itemType
                        ZORBA_ASSERT(false);             
                        result = NULL;
                    }
        xqtref_t itemXQType = createXQTypeFromTypeDefinition(typeManager, itemTypeDef);
        
        if ( itemXQType->type_kind() == XQType::USER_DEFINED_KIND )
        {   // if UDT add it to the cache, otherwise it will get lost
          addTypeToCache(itemXQType);    
        }
        
        //cout << " creating UDT Simple List Type: " << qname->getLocalName()->c_str() <<  "@" << qname->getNamespace()->c_str() << " of " << itemXQType->toString() << endl; cout.flush();
        
        xqtref_t xqType = xqtref_t(new UserDefinedXQType(typeManager,
                                                         qname,
                                                         NULL /*GENV_TYPESYSTEM.ANY_SIMPLE_TYPE*/,
                                                         TypeConstants::QUANT_ONE,
                                                         itemXQType.getp()));
        
        //cout << "   created UDT Simple List Type: " << xqType->toString() << endl; cout.flush();
        addTypeToCache(xqType);

        result = xqType;
      }
      break;
      
            case XSSimpleTypeDefinition::VARIETY_UNION:
                {
                    XSSimpleTypeDefinitionList * memberTypesDefList = xsSimpleTypeDef->getMemberTypes();
                    if ( !memberTypesDefList )
                    {
                        //error since VARIETY is UNION must have a memberTypesDefList
                        ZORBA_ASSERT(false);             
                        result = NULL;
                    }
        
        //cout << " creating UDT Simple Union Type: " << qname->getLocalName()->c_str() <<  "@" << qname->getNamespace()->c_str() << " of: ";
        std::vector<xqtref_t> unionItemTypes;
        
        for ( unsigned int i = 0; i < memberTypesDefList->size(); i++)
        {
          XSSimpleTypeDefinition* itemTypeDef = memberTypesDefList->elementAt(i);
          xqtref_t itemXQType = createXQTypeFromTypeDefinition(typeManager, itemTypeDef);
          unionItemTypes.push_back(itemXQType);                        
          //cout << " " << itemXQType->toString();
          
          if ( itemXQType->type_kind() == XQType::USER_DEFINED_KIND )
          {   // if UDT add it to the cache, otherwise it will get lost
            addTypeToCache(itemXQType);    
          }
        }
        //cout << endl; cout.flush();
        
        xqtref_t xqType = xqtref_t(new UserDefinedXQType(typeManager,
                                                         qname,
                                                         NULL /*GENV_TYPESYSTEM.ANY_SIMPLE_TYPE*/,
                                                         TypeConstants::QUANT_ONE,
                                                         unionItemTypes));
        
        //cout << "   created UDT Union Type: " << xqType->toString() << endl; cout.flush();
        addTypeToCache(xqType);

        result = xqType;
      }
      break;
      
            case XSSimpleTypeDefinition::VARIETY_ABSENT:
                // Xerces source says it must be anySimpleType
                result = GENV_TYPESYSTEM.ANY_SIMPLE_TYPE;
                break;
        
            default:
                ZORBA_ASSERT(false);            
            }

      checkForAnonymousTypesInType(typeManager, xsTypeDef);

        } // end user defined simple types
    }     // end simple types
    else
    {  
        // is not a simple type 
        // first check if it is a built-in type
        const XMLCh* uri = xsTypeDef->getNamespace();
        xqp_string strUri = transcode(uri);
    
        if ( XMLString::equals(strUri.c_str (), Schema::XSD_NAMESPACE) )
        {
            const XMLCh* local = xsTypeDef->getName();
            // maybe there is a better way than comparing strings 
            // but it seems Xerces doesn't have a code for built-in types
            if ( XMLString::equals(XMLChArray("anyType").get(), local) )
            {
                result = GENV_TYPESYSTEM.ANY_TYPE;
            }
            else if ( XMLString::equals(XMLChArray("untyped").get(), local) )
            {
                result = GENV_TYPESYSTEM.UNTYPED_TYPE;
            }            
            else
            {
                // there are no other non simple known types in xsd namespace
                ZORBA_ASSERT(false);
            }
        }
        else
        {
            // user defined complex Type
            XSTypeDefinition* baseTypeDef = xsTypeDef->getBaseType();
            if ( !baseTypeDef )
            {
        //error allways must have a baseType
                ZORBA_ASSERT(false);             
                result = NULL;
            }
      
      xqtref_t baseXQType = createXQTypeFromTypeDefinition(typeManager, baseTypeDef);
      
            xqp_string lNamespace (strUri);
            xqp_string lPrefix;
            xqp_string lLocal = transcode(xsTypeDef->getName());
      
            store::Item_t qname;
      GENV_ITEMFACTORY->createQName(qname,
                                    lNamespace.getStore(),
                                    lPrefix.getStore(),
                                    lLocal.getStore());
      
      // get content type
      XSComplexTypeDefinition* xsComplexTypeDef =
        static_cast<XSComplexTypeDefinition*>(xsTypeDef);

            XQType::content_kind_t contentType;
      
            switch(xsComplexTypeDef->getContentType())
            {
            case XSComplexTypeDefinition::CONTENTTYPE_MIXED:
                contentType = XQType::MIXED_CONTENT_KIND;
                break;
            case XSComplexTypeDefinition::CONTENTTYPE_ELEMENT:
                contentType = XQType::ELEMENT_ONLY_CONTENT_KIND;
                break;
            case XSComplexTypeDefinition::CONTENTTYPE_SIMPLE:
                contentType = XQType::SIMPLE_CONTENT_KIND;
                break;
            case XSComplexTypeDefinition::CONTENTTYPE_EMPTY:
                contentType = XQType::EMPTY_CONTENT_KIND;
                break;
            default:
                ZORBA_ASSERT(false);
            }
      
      xqtref_t xqType = xqtref_t(new UserDefinedXQType(typeManager,
                                                       qname,
                                                       baseXQType,
                                                       TypeConstants::QUANT_ONE,
                                                       contentType));
            result = xqType;

      addTypeToCache(xqType);

      //check if it contains anonymous types (they are not available through xsModel API)
      // add them to the cache
      checkForAnonymousTypesInType(typeManager, xsTypeDef);
        }
    } 
  
    return result;
}

void Schema::checkForAnonymousTypesInType(const TypeManager* typeManager, XSTypeDefinition* xsTypeDef)
{
    TRACE(" type: " << StrX(xsTypeDef->getName()) << "@" << StrX(xsTypeDef->getNamespace()));

    XSTypeDefinition *xsBaseTypeDef = xsTypeDef->getBaseType();
    if (xsBaseTypeDef)
    {
        addAnonymousTypeToCache(typeManager, xsBaseTypeDef);
    }

    if (xsTypeDef->getTypeCategory() == XSTypeDefinition::SIMPLE_TYPE)
    {
        // Simple
        // to do list, union ??
    }
    else
    {
        // Complex
        XSComplexTypeDefinition* xsComplexTypeDef = static_cast<XSComplexTypeDefinition*>(xsTypeDef);
        XSComplexTypeDefinition::CONTENT_TYPE contentType = xsComplexTypeDef->getContentType();

        if (contentType == XSComplexTypeDefinition::CONTENTTYPE_ELEMENT
          || contentType == XSComplexTypeDefinition::CONTENTTYPE_MIXED)
        {
            XSParticle *xsParticle = xsComplexTypeDef->getParticle();
            checkForAnonymousTypesInParticle(typeManager, xsParticle);
        }
    }
}

void Schema::checkForAnonymousTypesInParticle(const TypeManager* typeManager, XSParticle *xsParticle)
{
    if (!xsParticle)
        return;

    TRACE(" particle: " << StrX(xsParticle->getName()) << "@" << StrX(xsParticle->getNamespace()));

    XSParticle::TERM_TYPE termType = xsParticle->getTermType();
    if (termType == XSParticle::TERM_ELEMENT)
    {
        XSElementDeclaration *xsElement = xsParticle->getElementTerm();
        XSTypeDefinition* xsParticleTypeDef = xsElement->getTypeDefinition();
        addAnonymousTypeToCache(typeManager, xsParticleTypeDef);
    }
    else if (termType == XSParticle::TERM_MODELGROUP)
    {
        XSModelGroup *xsModelGroup = xsParticle->getModelGroupTerm();

        XSParticleList *xsParticleList = xsModelGroup->getParticles();
        for (unsigned i = 0; i < xsParticleList->size(); i++)
        {
            checkForAnonymousTypesInParticle(typeManager, xsParticleList->elementAt(i));
        }
    }
}

void Schema::addAnonymousTypeToCache(const TypeManager* typeManager, XSTypeDefinition* xsTypeDef)
{
    if (!xsTypeDef->getAnonymous())
        return;

    XSTypeDefinition* baseTypeDef = xsTypeDef->getBaseType();
    if (!baseTypeDef) {
        //error allways must have a baseType
        ZORBA_ASSERT(false);
        return;
    }

    xqtref_t baseXQType = createXQTypeFromTypeDefinition(typeManager, baseTypeDef);

    const XMLCh* uri = xsTypeDef->getNamespace();
    xqp_string strUri = transcode(uri);
    xqp_string lNamespace(strUri);
    xqp_string lPrefix;
    xqp_string lLocal = transcode(xsTypeDef->getName());

    store::Item_t qname;
    GENV_ITEMFACTORY->createQName(qname,
            lNamespace.getStore(),
            lPrefix.getStore(),
            lLocal.getStore());

    if (xsTypeDef->getTypeCategory()!=XSTypeDefinition::COMPLEX_TYPE)
        return;

    // get content type
    XSComplexTypeDefinition* xsComplexTypeDef =
            static_cast<XSComplexTypeDefinition*> (xsTypeDef);

    XQType::content_kind_t contentType;

    switch (xsComplexTypeDef->getContentType())
    {
        case XSComplexTypeDefinition::CONTENTTYPE_MIXED :
            contentType = XQType::MIXED_CONTENT_KIND;
            break;
        case XSComplexTypeDefinition::CONTENTTYPE_ELEMENT :
            contentType = XQType::ELEMENT_ONLY_CONTENT_KIND;
            break;
        case XSComplexTypeDefinition::CONTENTTYPE_SIMPLE :
            contentType = XQType::SIMPLE_CONTENT_KIND;
            break;
        case XSComplexTypeDefinition::CONTENTTYPE_EMPTY :
            contentType = XQType::EMPTY_CONTENT_KIND;
            break;
        default:
            ZORBA_ASSERT(false);
    }

    xqtref_t xqType = xqtref_t(new UserDefinedXQType(typeManager,
            qname,
            baseXQType,
            TypeConstants::QUANT_ONE,
            contentType));

    addTypeToCache(xqType);

    // check f0r it's containing annonymous types
    checkForAnonymousTypesInType(typeManager, xsComplexTypeDef);
}

void Schema::addTypeToCache(xqtref_t itemXQType)
{
    ZORBA_ASSERT( itemXQType->type_kind() == XQType::USER_DEFINED_KIND );

    const UserDefinedXQType* itemUDType = static_cast<const UserDefinedXQType*>(itemXQType.getp());
    const store::Item* qname = itemUDType->get_qname();
    std::string key = qname->getLocalName()->str() + ":" + qname->getNamespace()->str() + " " + 
        TypeOps::decode_quantifier (itemXQType->get_quantifier());

    xqtref_t res;
    if( !_udTypesCache->get(key, res) )
    {
        TRACE("key: '" << key << "'");
        _udTypesCache->put(key, itemXQType);
    }
}

#endif //ZORBA_NO_XMLSCHEMA


/*******************************************************************************
  Parse a given string to create (if possible) an XDM value of a given type.
  The given type is assumed to be a simple type, i.e. Atomic, List or Union.
********************************************************************************/
bool Schema::parseUserSimpleTypes(
    const xqp_string textValue,
    const xqtref_t& aTargetType, 
    std::vector<store::Item_t> &resultList)
{
    //cout << "parseUserSimpleTypes: '" << textValue << "' to " <<  aTargetType->toString() << endl; cout.flush();

    if ( aTargetType->type_kind() != XQType::USER_DEFINED_KIND )
    {
        // must be a built in type
        store::Item_t atomicResult;
        xqpStringStore_t textval = textValue.getStore();
    //todo add nsCtx
    bool res = GenericCast::instance()->castToAtomic(atomicResult, textval, aTargetType);
        
    if ( res == false )
    {
            return false;
    }
        else
        {
            resultList.push_back(atomicResult);
            return true;
        }
    }

    const UserDefinedXQType* udXQType = static_cast<const UserDefinedXQType*>(aTargetType.getp());

    ZORBA_ASSERT( udXQType->isAtomic() || udXQType->isList() || udXQType->isUnion() );

    bool hasResult = false;
    
    switch ( udXQType->getTypeCategory() )
    {
    case UserDefinedXQType::ATOMIC_TYPE:
        {
            store::Item_t atomicResult;
    hasResult = parseUserAtomicTypes(textValue, aTargetType, atomicResult);
            
            if ( !hasResult )
                return false;
            else
            {
                resultList.push_back(atomicResult);
                return true;
            }
        }
        break;
  
    case UserDefinedXQType::LIST_TYPE:
  
        return parseUserListTypes( textValue, aTargetType, resultList);
        break;

    case UserDefinedXQType::UNION_TYPE:
  
        return parseUserUnionTypes( textValue, aTargetType, resultList);
        break;

    case UserDefinedXQType::COMPLEX_TYPE:
    default:
        ZORBA_ASSERT( false);
        break;
    }
  
    return false;
}


/*******************************************************************************
  Parse a given string to create (if possible) an XDM value of a given type.
  The given type is assumed to be a user-defined atomic type.
********************************************************************************/
bool Schema::parseUserAtomicTypes(
    const xqp_string textValue,
    const xqtref_t& aTargetType, 
    store::Item_t &result)
{
  TRACE("parsing '" << textValue << "' to " << aTargetType->toString());

    ZORBA_ASSERT( aTargetType->type_kind() == XQType::USER_DEFINED_KIND );

    const UserDefinedXQType* udXQType = static_cast<const UserDefinedXQType*>(aTargetType.getp());
    ZORBA_ASSERT( udXQType->isAtomic() );

  store::Item* typeQName = udXQType->get_qname();

#ifndef ZORBA_NO_XMLSCHEMA
    XMLChArray localPart (typeQName->getLocalName());
    XMLChArray uriStr (typeQName->getNamespace());
    
    bool wasError = false;

    try 
    {
        // Create grammar resolver and string pool that we pass to the scanner
    std::auto_ptr<GrammarResolver> fGrammarResolver(new GrammarResolver(_grammarPool));
        fGrammarResolver->useCachedGrammarInParse(true);

        // retrieve Grammar for the uri
        SchemaGrammar* sGrammar = (SchemaGrammar*) fGrammarResolver->getGrammar(uriStr);
        if (sGrammar) 
        {
            DatatypeValidator* xsiTypeDV = fGrammarResolver->getDatatypeValidator(uriStr, localPart);
      
            if (!xsiTypeDV) 
            {
                // when complex simple content type use the base type
                // find the first basetype that has a DataTypeValidator
                const UserDefinedXQType* tmpXQType = udXQType;
                
                while (xsiTypeDV==NULL && tmpXQType!=NULL)
                {
                    xqtref_t baseXQType = tmpXQType->getBaseType();
                    if (baseXQType.getp())
                    {
                        store::Item_t baseTypeQName = baseXQType->get_qname();
                        XMLChArray baseLocalPart (baseTypeQName->getLocalName());
                        XMLChArray baseUriStr (baseTypeQName->getNamespace());
                        
                        xsiTypeDV = fGrammarResolver->getDatatypeValidator(baseUriStr, baseLocalPart);

                        tmpXQType = NULL;
                        if (baseXQType->type_kind() == XQType::USER_DEFINED_KIND)
                        {
                            tmpXQType = static_cast<const UserDefinedXQType*>(baseXQType.getp());
                        }
                    }
                }
            }

            if (!xsiTypeDV) 
            {
        ZORBA_ERROR_DESC_OSS( FORG0001, "Type '" << TypeOps::toString (*aTargetType) << "' not found in current context.");
                wasError = true;
            }
      
            XMLChArray xchTextValue (textValue.getStore ());
            xsiTypeDV->validate(xchTextValue.get ());
        }
        else
        {
      ZORBA_ERROR_DESC_OSS( FORG0001, "Uri '" << typeQName->getNamespace()->str() << "' not found in current schema context.");
          wasError = true;
      
        }
    
        if (wasError)
          return false;
    }
    catch (XMLException& idve)
    {
        ZORBA_ERROR_DESC_OSS( FORG0001, 
            "String '" + textValue.trim(" \n\r\t",4) + "' cannot be cast to '" << 
            TypeOps::toString( *aTargetType ) << "' : " << transcode(idve.getMessage()));
        return false;
    }
    catch(const OutOfMemoryException&) 
    {
        throw;
    }
    catch (const error::ZorbaError& e) {
      throw e;
    }
    catch (...)
    {
        throw;
    }
#endif //ZORBA_NO_XMLSCHEMA
  
    const XQType* baseType_ptr = udXQType->getBaseType().getp();
  
    while ( baseType_ptr->type_kind() == XQType::USER_DEFINED_KIND )
    {        
        const UserDefinedXQType* udBaseType_ptr = static_cast<const UserDefinedXQType*>(baseType_ptr);
        baseType_ptr = udBaseType_ptr->getBaseType().getp();
    }
  
    ZORBA_ASSERT( baseType_ptr->type_kind() == XQType::ATOMIC_TYPE_KIND );
  
    xqpStringStore_t textval = textValue.getStore();
    return GenericCast::instance()->castToAtomic(result, textval, baseType_ptr);
}


void splitToAtomicTextValues(
    const xqp_string& textValue,
    std::vector<xqp_string>& atomicTextValues)
{   
    xqp_string normalizedTextValue = textValue.normalizeSpace();
    checked_vector<uint32_t> codes = normalizedTextValue.getCodepoints();

    xqpString::size_type start = 0;
    xqpString::size_type i = 0;

  while (i < codes.size())
    {
        if ( xqpStringStore::is_whitespace(codes[i]) )
        {
      atomicTextValues.push_back(normalizedTextValue.substr(start, i - start));
      start = i+1;
        }
        i++;
    }
  
    if ( start < (i-1) )
    atomicTextValues.push_back(normalizedTextValue.substr(start, i-start));    
}



// user list types
bool Schema::parseUserListTypes(
    const xqp_string textValue, 
    const xqtref_t& aTargetType,
    std::vector<store::Item_t> &resultList)
{
    ZORBA_ASSERT( aTargetType->type_kind() == XQType::USER_DEFINED_KIND );

    //cout << "parseUserListTypes: '" << textValue << "' to " << aTargetType->toString() << endl; cout.flush();

    const UserDefinedXQType* udXQType = static_cast<const UserDefinedXQType*>(aTargetType.getp());
    ZORBA_ASSERT( udXQType->isList() );

    bool hasResult = true;
    const XQType* listItemType = udXQType->getListItemType();
    ZORBA_ASSERT( listItemType );

    //split text into atoms
    std::vector<xqp_string> atomicTextValues;
    splitToAtomicTextValues(textValue, atomicTextValues);
    
    for ( unsigned int i = 0; i< atomicTextValues.size() ; i++)
    {
        TRACE("trying parsing '" << textValue << "' to " << listItemType->toString());
        bool res = parseUserSimpleTypes(atomicTextValues[i], xqtref_t(listItemType), resultList);
        hasResult = hasResult && res;
    }

    return hasResult;
}

// user union types
bool Schema::parseUserUnionTypes(
    const xqp_string textValue,
    const xqtref_t& aTargetType, 
    std::vector<store::Item_t> &resultList)
{
    //cout << "parseUserUnionTypes: '" << textValue << "'" <<  " to " << aTargetType->toString() << endl; cout.flush();

    ZORBA_ASSERT( aTargetType->type_kind() == XQType::USER_DEFINED_KIND );

    const UserDefinedXQType* udXQType = static_cast<const UserDefinedXQType*>(aTargetType.getp());
    ZORBA_ASSERT( udXQType->isUnion() );


    std::vector<xqtref_t> unionItemTypes = udXQType->getUnionItemTypes();

    for ( unsigned int i = 0; i < unionItemTypes.size(); i++)
    {        
      if (isCastableUserSimpleTypes(textValue, unionItemTypes[i]))
      {                
        return parseUserSimpleTypes(textValue, unionItemTypes[i], resultList);
      }
    }

    return false;
}


// user defined simple types, i.e. Atomic, List or Union Types
bool Schema::isCastableUserSimpleTypes(
    const xqp_string textValue,
    const xqtref_t& aTargetType)
{
    //cout << "isCastableUserSimpleTypes: '" << textValue << "' to " << aTargetType->toString() << endl; cout.flush();

    if ( aTargetType->type_kind() != XQType::USER_DEFINED_KIND )
    {
        // must be a built in type
        store::Item_t atomicResult;
        xqpStringStore_t textval = textValue.getStore();
        return GenericCast::instance()->isCastable(textval, aTargetType); //todo add nsCtx
    }

    ZORBA_ASSERT( aTargetType->type_kind() == XQType::USER_DEFINED_KIND );

    const UserDefinedXQType* udXQType = static_cast<const UserDefinedXQType*>(aTargetType.getp());

    ZORBA_ASSERT( udXQType->isAtomic() || udXQType->isList() || udXQType->isUnion() );


    switch ( udXQType->getTypeCategory() )
    {
    case UserDefinedXQType::ATOMIC_TYPE:
        return isCastableUserAtomicTypes( textValue, aTargetType );
        break;

    case UserDefinedXQType::LIST_TYPE:  
        return isCastableUserListTypes( textValue, aTargetType );
        break;

    case UserDefinedXQType::UNION_TYPE:
        return isCastableUserUnionTypes( textValue, aTargetType );
        break;

    case UserDefinedXQType::COMPLEX_TYPE:
    default:
        ZORBA_ASSERT( false);
        break;
    }

    return false;;
}        

// user defined atomic types
bool Schema::isCastableUserAtomicTypes(
    const xqp_string textValue,
    const xqtref_t& aTargetType)
{
    //cout << "isCastableUserAtomicTypes: '" << textValue << "' to " << aTargetType->toString() << endl; cout.flush();
    xqpStringStore_t text = xqpStringStore_t(textValue.getStore());
    return GenericCast::instance()->isCastable( text, aTargetType.getp());
}        

// user defined list types
bool Schema::isCastableUserListTypes(
    const xqp_string textValue,
    const xqtref_t& aTargetType)
{
    //cout << "isCastableUserListTypes: '" << textValue << "' to " << aTargetType->toString() << endl; cout.flush();
    ZORBA_ASSERT( aTargetType->type_kind() == XQType::USER_DEFINED_KIND );

    const UserDefinedXQType* udXQType = static_cast<const UserDefinedXQType*>(aTargetType.getp());
    ZORBA_ASSERT( udXQType->isList() );

    bool hasResult = true;
    const XQType* listItemType = udXQType->getListItemType();
    
    //split text into atoms
    std::vector<xqp_string> atomicTextValues;
    splitToAtomicTextValues(textValue, atomicTextValues);
    
    for ( unsigned int i = 0; i<atomicTextValues.size() ; i++ )
    {
        bool res = isCastableUserSimpleTypes(atomicTextValues[i], xqtref_t(listItemType));
        hasResult = hasResult && res;
    }

    return hasResult;
}        

// user defined union types
bool Schema::isCastableUserUnionTypes(
    const xqp_string textValue,
    const xqtref_t& aTargetType)
{
    //cout << "isCastableUserUnionTypes: '" << textValue << "' to " << aTargetType->toString() << endl; cout.flush();

    ZORBA_ASSERT( aTargetType->type_kind() == XQType::USER_DEFINED_KIND );

    const UserDefinedXQType* udXQType = static_cast<const UserDefinedXQType*>(aTargetType.getp());
    ZORBA_ASSERT( udXQType->isUnion() );


    std::vector<xqtref_t> unionItemTypes = udXQType->getUnionItemTypes();

    for ( unsigned int i = 0; i<unionItemTypes.size(); i++)
    {        
      if ( isCastableUserSimpleTypes(textValue, unionItemTypes[i]))
        return true;
    }

    return false;
}        
} // namespace zorba

