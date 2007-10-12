/*
 *	Copyright 2006-2007 FLWOR Foundation.
 *  Author: David Graf (david.graf@28msec.com)
 *
 */

#include "util/rchandle.h"

#include "store/naive/simple_store.h"
#include "store/naive/simple_temp_seq.h"
#include "store/naive/simple_collection.h"

namespace xqp
{
	typedef rchandle<TempSeq> TempSeq_t;

	/* begin class SimpleStore */
	SimpleStore::SimpleStore()
	{

	}
	SimpleStore::~SimpleStore()
	{

	}

	TempSeq_t SimpleStore::createTempSeq ( PlanIter_t& iterator, bool lazy )
	{
		TempSeq_t tempSeq = new SimpleTempSeq ( iterator );
		return tempSeq;
	}
	void SimpleStore::setGarbageCollectionStrategy ( const xqp_string& garbageCollectionStrategy )
	{

	}
	void SimpleStore::apply ( PUL_t pendingUpdateList )
	{

	}
	void SimpleStore::apply ( PUL_t pendingUpdateList, Requester requester )
	{

	}
	Item_t SimpleStore::getReference ( Item_t )
	{
		return rchandle<Item> ( NULL );
	}
	Item_t SimpleStore::getFixedReference ( Item_t, Requester requester, Timetravel timetravel )
	{
		return rchandle<Item> ( NULL );
	}
	Item_t SimpleStore::getNodeByReference ( Item_t )
	{
		return rchandle<Item> ( NULL );
	}
	Item_t SimpleStore::getNodeByReference ( Item_t, Requester requester, Timetravel timetravel )
	{
		return rchandle<Item> ( NULL );
	}
	int32_t SimpleStore::compare ( Item_t item1, Item_t item2 )
	{
		return 2;
	}
	PlanIter_t SimpleStore::sort ( PlanIter_t iterator, bool ascendent, bool duplicateElemination )
	{
		return rchandle<PlanIterator> ( NULL );
	}
	PlanIter_t SimpleStore::distinctNodeStable ( PlanIter_t )
	{
		return rchandle<PlanIterator> ( NULL );
	}
	Collection_t SimpleStore::getCollection ( Item_t uri )
	{
		return rchandle<Collection> ( NULL );
	}
	Collection_t SimpleStore::createCollection ( Item_t uri )
	{
		return rchandle<Collection> ( NULL );
	}
	Collection_t SimpleStore::createCollection()
	{
		return rchandle<Collection> ( NULL );
	}
	void SimpleStore::deleteCollection ( Item_t uri )
	{

	}
	Item_t SimpleStore::createURI() {
		return Item_t( NULL );
	}
	/* end class SimpleStore */

} /* namespace xqp */
