import module namespace init = "http://www.zorba-xquery.com/modules/store/static-collections/initialization";
import module namespace manip = "http://www.zorba-xquery.com/modules/store/static-collections/manipulation";

import module namespace ns = "http://www.example.com/example" at "../collection_001.xqdata";

init:create-collection(xs:QName("ns:collection_append_only"));

manip:delete-nodes(xs:QName("ns:collection_append_only"), xs:QName("ns:collection_append_only")[1]);
