(:
 : Copyright 2006-2009 The FLWOR Foundation.
 :
 : Licensed under the Apache License, Version 2.0 (the "License");
 : you may not use this file except in compliance with the License.
 : You may obtain a copy of the License at
 :
 : http://www.apache.org/licenses/LICENSE-2.0
 :
 : Unless required by applicable law or agreed to in writing, software
 : distributed under the License is distributed on an "AS IS" BASIS,
 : WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 : See the License for the specific language governing permissions and
 : limitations under the License.
:)

(:~
 : Module that defines the errors raised different Zorba modules.
 :
 : @author Stephanie Russell
 : @version 1.0
 :)
module namespace  err = "http://www.zorba-xquery.com/modules/errors" ;

(:~
 : Errors namespace URI.
:)
declare variable $err:errNS as xs:string := "http://www.zorba-xquery.com/modules/errors";

(:~
 : xs:QName with namespace URI="http://www.zorba-xquery.com/modules/excel/errors" and local name 'err:YS001". No alternative spelling found.
:)
declare variable $err:YS001 as xs:QName := fn:QName($err:errNS, "err:YS001");

(:~
 : xs:QName with namespace URI="http://www.zorba-xquery.com/modules/excel/errors" and local name 'err:YS002". Location not found.
:)
declare variable $err:YS002 as xs:QName := fn:QName($err:errNS, "err:YS002");

(:~
 : xs:QName with namespace URI="http://www.zorba-xquery.com/modules/excel/errors" and local name 'err:GS001". Request is incorrect.
:)
declare variable $err:GS001 as xs:QName := fn:QName($err:errNS, "err:GS001");

(:~
 : xs:QName with namespace URI="http://www.zorba-xquery.com/modules/excel/errors" and local name 'err:GS002". Book not found.
:)
declare variable $err:GS002 as xs:QName := fn:QName($err:errNS, "err:GS002");

(:~
 : xs:QName with namespace URI="http://www.zorba-xquery.com/modules/excel/errors" and local name 'err:OC001". This signing method is not implemented yet.
:)
declare variable $err:OC001 as xs:QName := fn:QName($err:errNS, "err:OC001");

(:~
 : xs:QName with namespace URI="http://www.zorba-xquery.com/modules/excel/errors" and local name 'err:OC002". This signing method is not supported.
:)
declare variable $err:OC002 as xs:QName := fn:QName($err:errNS, "err:OC002");

(:~
 : xs:QName with namespace URI="http://www.zorba-xquery.com/modules/excel/errors" and local name 'err:OC003". Http 401 error.
:)
declare variable $err:OC003 as xs:QName := fn:QName($err:errNS, "err:OC003");

(:~
 : xs:QName with namespace URI="http://www.zorba-xquery.com/modules/excel/errors" and local name 'err:OC004". Http 500 error.
:)
declare variable $err:OC004 as xs:QName := fn:QName($err:errNS, "err:OC004");
