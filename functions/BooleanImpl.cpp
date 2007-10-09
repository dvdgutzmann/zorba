/*
 *	Copyright 2006-2007 FLWOR Foundation.
 *  Authors: Tim Kraska, David Graf
 */

#include "BooleanImpl.h"
#include "../types/casting.h"
#include "../runtime/zorba.h"
#include "../errors/Error.h"
#include "../functions/AccessorsImpl.h"
#include "../store/api/temp_seq.h"

namespace xqp
{

	/*______________________________________________________________________
	|
	|	15.1.1 fn:boolean
	|	fn:boolean($arg as item()*) as xs:boolean
	|
	|	Computes the effective boolean value of the sequence $arg.
	|_______________________________________________________________________*/
	FnBooleanIterator::FnBooleanIterator ( const yy::location& loc, Iterator_t& arg0, bool negate_arg )
	:
		Batcher<FnBooleanIterator> ( loc ), arg0_ ( arg0 ), negate ( negate_arg ) {}
		
	FnBooleanIterator::~FnBooleanIterator() {}
			
	std::ostream&
	FnBooleanIterator::_show ( std::ostream& os ) const
	{
		this->arg0_->show ( os );
		return os;
	}

	Item_t
	FnBooleanIterator::effectiveBooleanValue ( const yy::location& loc, int8_t* stateBlock, Iterator_t& iter, bool negate )
	{
		Item_t item;
		TypeCode type;
		Item_t result;

		// TODO produceNext must be replaced to allow batching
		item = iter->produceNext(stateBlock);

		if ( item == NULL )
		{
			// empty sequence => false
			result = zorba::getZorbaForCurrentThread()->getItemFactory()->createBoolean ( negate ^ false );
		}
		else if ( item->isNode() )
		{
			// node => true
			result = zorba::getZorbaForCurrentThread()->getItemFactory()->createBoolean ( negate ^ true );
		}
		else
		{
			type = item->getType();
			if (
					// TODO produceNext must be replaced to allow batching
			    ( iter->produceNext(stateBlock) == NULL )
			    &&
			    ( type == xs_boolean
			      || sequence_type::derives_from ( type, xs_string )
			      || sequence_type::derives_from ( type, xs_anyURI )
			      || sequence_type::derives_from ( type, xs_untypedAtomicValue )
			      || sequence_type::isNumeric ( type )
			    )
			)
			{
				// atomic type xs_boolean, xs_string, xs_anyURI, xs_untypedAtomic
				// => effective boolean value is defined in the items
				result = item->getEBV();
				if (negate)
					result = zorba::getZorbaForCurrentThread()->getItemFactory()->createBoolean ( negate ^ result->getBooleanValue() );
			}
			else
			{
				// else error => fn:boolean not defined
				ZorbaErrorAlerts::error_alert (
				    error_messages::FORG0006_INVALID_ARGUMENT_TYPE,
				    error_messages::RUNTIME_ERROR,
				    &loc,
				    false,
				    "Wrong arguments in fn:boolean function!"
				);
			}
		}

		return result;
	}

	Item_t
	FnBooleanIterator::nextImpl(int8_t* stateBlock)
	{	
		STACK_INIT();
		STACK_PUSH ( FnBooleanIterator::effectiveBooleanValue ( this->loc, stateBlock, this->arg0_, this->negate ) );
		STACK_END();
	}

	void
	FnBooleanIterator::resetImpl(int8_t* stateBlock)
	{
		this->resetChild ( this->arg0_, stateBlock );
	}

	void
	FnBooleanIterator::releaseResourcesImpl(int8_t* stateBlock)
	{
		this->releaseChildResources ( this->arg0_, stateBlock);
	}
	/* end class FnBooleanIterator */
	
	/* begin class LogicIterator */
	LogicIterator::LogicIterator ( const yy::location& loc, Iterator_t arg0, Iterator_t arg1, LogicType lt)
	:
		Batcher<LogicIterator> ( loc), iterLeft(arg0), iterRight(arg1), logicType(lt) {}
	LogicIterator::~LogicIterator(){}
			
	Item_t 
	LogicIterator::nextImpl(int8_t* stateBlock)
	{
		bool bRes;
		
		STACK_INIT();
		switch(this->logicType)
		{
		case AND:
			bRes = FnBooleanIterator::effectiveBooleanValue(this->loc, stateBlock, this->iterLeft)->getBooleanValue() 
							&& FnBooleanIterator::effectiveBooleanValue(this->loc, stateBlock, this->iterRight)->getBooleanValue();
			break;
		case OR:
			bRes = FnBooleanIterator::effectiveBooleanValue(this->loc, stateBlock, this->iterLeft)->getBooleanValue() 
							|| FnBooleanIterator::effectiveBooleanValue(this->loc, stateBlock, this->iterRight)->getBooleanValue();;
			break;
		}
		STACK_PUSH(zorba::getZorbaForCurrentThread()->getItemFactory()->createBoolean(bRes));
		STACK_END();
	}
	
	void 
	LogicIterator::resetImpl(int8_t* stateBlock)
	{
		this->resetChild( this->iterLeft, stateBlock );
		this->resetChild( this->iterRight, stateBlock );
	}
	
	void 
	LogicIterator::releaseResourcesImpl(int8_t* stateBlock)
	{
		this->releaseChildResources( this->iterLeft, stateBlock );
		this->releaseChildResources( this->iterRight, stateBlock );
	}
	/* end class LogicIterator */

	/* begin class ComparisonIterator */
	CompareIterator::CompareIterator ( const yy::location& loc, Iterator_t arg0, Iterator_t arg1, CompareType argCompType )
	:
		Batcher<CompareIterator> ( loc ), compareType(argCompType) 
	{
		this->genericCast = new GenericCast();
		this->iter0 = new FnDataIterator ( loc, arg0 );
		this->iter1 = new FnDataIterator ( loc, arg1 );
	}
	
	CompareIterator::~CompareIterator()
	{
		delete this->genericCast;
	}
	
	Item_t
	CompareIterator::nextImpl(int8_t* stateBlock)
	{
		Item_t item0;
		Item_t item1;
		TypeCode type0;
		TypeCode type1;
		TypeCode resultType;
		TempSeq_t temp0;
		TempSeq_t temp1;
		int32_t i0;
		int32_t i1;
		bool found;
		bool empty;
		int32_t compareResult;

		STACK_INIT();

		if (this->isGeneralComparison())
		{
			// TODO Optimizations for >, >=, < and <=
			temp0 = zorba::getZorbaForCurrentThread()->getStore()->createTempSeq(this->iter0);
			temp1 = zorba::getZorbaForCurrentThread()->getStore()->createTempSeq(this->iter1);
			i0 = 1;
			found = false;
			while (!found && temp0->containsItem(i0))
			{
				i1 = 1;
				while (!found && temp1->containsItem(i1))
				{
					if (CompareIterator::generalComparison(temp0->getItem( i0 ), temp1->getItem( i1 )))
						found = true;
					i1++;
				}
				i0++;
			}

			STACK_PUSH( zorba::getZorbaForCurrentThread()->getItemFactory()->createBoolean( found ) );
		} /* if general comparison */
		else if (this->isValueComparison())
		{
			if (( (item0 = this->consumeNext(this->iter0, stateBlock)) != NULL ) 
						&& ((item1 = this->consumeNext(this->iter1, stateBlock))!=NULL))
			{
				STACK_PUSH( zorba::getZorbaForCurrentThread()->getItemFactory()->createBoolean( CompareIterator::valueComparison(item0, item1) ) );
				if (this->consumeNext(this->iter0, stateBlock) != NULL || this->consumeNext(this->iter1, stateBlock) != NULL)
				{
					ZorbaErrorAlerts::error_alert (
						error_messages::FOCH0004_Collation_does_not_support_collation_units,
						error_messages::RUNTIME_ERROR,
						&loc,
						false,
						"Value comparions must not be made with sequences with length greater 1!"
					);
				}
			}
		} /* if value comparison */
		else if (this->isNodeComparison())
		{
			ZorbaErrorAlerts::error_alert (
				error_messages::XQP0015_SYSTEM_NOT_YET_IMPLEMENTED,
				error_messages::RUNTIME_ERROR,
				&loc,
				false,
				"Node comparison is not yet implemented!"
			);
		} /* if node comparison */
		
		STACK_END();
	}

	void
	CompareIterator::resetImpl(int8_t* stateBlock)
	{
		this->resetChild ( this->iter0, stateBlock );
		this->resetChild ( this->iter1, stateBlock );
	}

	void
	CompareIterator::releaseResourcesImpl(int8_t* stateBlock)
	{
		this->releaseChildResources ( this->iter0, stateBlock );
		this->releaseChildResources ( this->iter1, stateBlock );
	}
	
	bool 
	CompareIterator::isValueComparison()
	{
		bool retVal = false;
		switch(this->compareType)
		{
		case VALUE_EQUAL:
		case VALUE_NOT_EQUAL:
		case VALUE_LESS:
		case VALUE_LESS_EQUAL:
		case VALUE_GREATER:
		case VALUE_GREATER_EQUAL:
			retVal = true;
			break;
		default:
			retVal = false;
			break;
		}
		return retVal;
	}
	
	bool 
	CompareIterator::isGeneralComparison()
	{
		bool retVal = false;
		switch(this->compareType)
		{
		case GENERAL_EQUAL:
		case GENERAL_NOT_EQUAL:
		case GENERAL_LESS:
		case GENERAL_LESS_EQUAL:
		case GENERAL_GREATER:
		case GENERAL_GREATER_EQUAL:
			retVal = true;
			break;
		default:
			retVal = false;
			break;
		}
		return retVal;
	}
	
	bool
	CompareIterator::isNodeComparison()
	{
		bool retVal = false;
		switch(this->compareType)
		{
		case NODE_EQUAL:
		case NODE_NOT_EQUAL:
			retVal = true;
			break;
		default:
			retVal = false;
			break;
		}
		return retVal;
	}
	
	bool
	CompareIterator::generalComparison(Item_t item0, Item_t item1)
	{
		
		if (sequence_type::derives_from(item0->getType(), xs_untypedAtomicValue))
		{
			if (sequence_type::isNumeric(item1->getType()))
			{
				this->genericCast->setTarget(xs_double);
				item0 = this->genericCast->cast(item0);
			}
			else if (sequence_type::derives_from(item1->getType(), xs_untypedAtomicValue)
								|| sequence_type::derives_from(item1->getType(), xs_string))
			{
				this->genericCast->setTarget(xs_string);
				item0 = this->genericCast->cast(item0);
			}
			else
			{
				this->genericCast->setTarget(item1->getType());
				item0 = this->genericCast->cast(item0);
			}
		}
		
		if (sequence_type::derives_from(item1->getType(), xs_untypedAtomicValue))
		{
			if (sequence_type::isNumeric(item0->getType()))
			{
				this->genericCast->setTarget(xs_double);
				item1 = this->genericCast->cast(item1);
			}
			else if (sequence_type::derives_from(item0->getType(), xs_untypedAtomicValue)
								|| sequence_type::derives_from(item0->getType(), xs_string))
			{
				this->genericCast->setTarget(xs_string);
				item1 = this->genericCast->cast(item1);
			}
			else
			{
				this->genericCast->setTarget(item0->getType());
				item1 = this->genericCast->cast(item1);
			}
		}
		
		return CompareIterator::valueComparison(item0, item1);
	} /* end CompareIterator::generalComparison (...) */

	bool
	CompareIterator::valueComparison(Item_t item0, Item_t item1)
	{
		// all untyped Atomics to String
		if (sequence_type::derives_from(item0->getType(), xs_untypedAtomicValue))
		{
			this->genericCast->setTarget(xs_string);
			item0 = this->genericCast->cast(item0);
		}
		if  (sequence_type::derives_from(item1->getType(), xs_untypedAtomicValue))
		{
			this->genericCast->setTarget(xs_string);
			item1 = this->genericCast->cast(item1);
		}
		
		// TYPE PROMOTION
		// numeric promotion
		if (sequence_type::derives_from(item0->getType(), xs_float)) {
			if (sequence_type::derives_from(item1->getType(), xs_decimal))
			{
				this->genericCast->setTarget(xs_float);
				item1 = this->genericCast->cast(item1);
			}
		}
		else if (sequence_type::derives_from(item0->getType(), xs_double))
		{
			if (sequence_type::derives_from(item1->getType(), xs_decimal))
			{
				this->genericCast->setTarget(xs_double);
				item1 = this->genericCast->cast(item1);
			}
			else if (sequence_type::derives_from(item0->getType(), xs_float))
			{
				this->genericCast->setTarget(xs_double);
				item1 = this->genericCast->cast(item1);
			}
		}
		
		if (sequence_type::derives_from(item1->getType(), xs_float)) {
			if (sequence_type::derives_from(item0->getType(), xs_decimal))
			{
				this->genericCast->setTarget(xs_float);
				item0 = this->genericCast->cast(item0);
			}
		}
		else if (sequence_type::derives_from(item1->getType(), xs_double))
		{
			if (sequence_type::derives_from(item0->getType(), xs_decimal))
			{
				this->genericCast->setTarget(xs_double);
				item0 = this->genericCast->cast(item0);
			}
			else if (sequence_type::derives_from(item0->getType(), xs_float))
			{
				this->genericCast->setTarget(xs_double);
				item0 = this->genericCast->cast(item0);
			}
		}
		// uri promotion
		if (sequence_type::derives_from(item0->getType(), xs_string)
				&& sequence_type::derives_from(item1->getType(), xs_anyURI)) 
		{
			this->genericCast->setTarget(xs_string);
			item1 = this->genericCast->cast(item1);
		}
		if (sequence_type::derives_from(item1->getType(), xs_string)
				&& sequence_type::derives_from(item0->getType(), xs_anyURI)) 
		{
			this->genericCast->setTarget(xs_string);
			item0 = this->genericCast->cast(item0);
		}

		// computation of result
		int32_t compValue = -2;
		switch(this->compareType)
		{
			case VALUE_EQUAL:
			case GENERAL_EQUAL:
			case VALUE_NOT_EQUAL:
			case GENERAL_NOT_EQUAL:
				compValue = CompareIterator::equal(item0, item1);
				break;
			case VALUE_GREATER:
			case GENERAL_GREATER:
			case VALUE_GREATER_EQUAL:
			case GENERAL_GREATER_EQUAL:
			case VALUE_LESS:
			case GENERAL_LESS:
			case VALUE_LESS_EQUAL:
			case GENERAL_LESS_EQUAL:
				compValue = CompareIterator::compare(item0, item1);
			default:
				break;
		}
		
		if (compValue > -2)
			switch(this->compareType)
			{
				case VALUE_EQUAL:
				case GENERAL_EQUAL:
					return compValue == 0;
					break;
				case VALUE_NOT_EQUAL:
				case GENERAL_NOT_EQUAL:
					return compValue != 0;
					break;
				case VALUE_GREATER:
				case GENERAL_GREATER:
					return compValue > 0;
					break;
				case VALUE_GREATER_EQUAL:
				case GENERAL_GREATER_EQUAL:
					return compValue >= 0;
					break;
				case VALUE_LESS:
				case GENERAL_LESS:
					return compValue < 0;
					break;
				case VALUE_LESS_EQUAL:
				case GENERAL_LESS_EQUAL:
					return compValue <= 0;
					break;
				default:
					break;
			}
			
		ZorbaErrorAlerts::error_alert (
			error_messages::FOCH0004_Collation_does_not_support_collation_units,
			error_messages::RUNTIME_ERROR,
			&loc,
			false,
			"Compare of declared collation operator and operators is not possible!"
		);
		return false;
	} /* end CompareIterator::valueComparison (...) */
	
	int32_t
	CompareIterator::equal(const Item_t& item0, const Item_t& item1)
	{
		// tries first normal compare
		int32_t compareRes = CompareIterator::compare(item0, item1);
		if (compareRes == 0)
			return 0;
		else if (compareRes == -1 || compareRes == 1)
			return 1;
			
		TypeCode type0 = item0->getType();
		TypeCode type1 = item1->getType();
		bool equal;
		if (sequence_type::derives_from(type0, xs_boolean) && sequence_type::derives_from(type1, xs_boolean))
				equal = item0->equals(item1);
		// TODO the rest
		else
			return -2;
		
		if (equal)
			return 0;
		else
			return 1;
	}
	
	int32_t 
	CompareIterator::compare(const Item_t& item0, const Item_t& item1)
	{
		TypeCode type0 = item0->getType();
		TypeCode type1 = item1->getType();
		int32_t ret = -2;
		if (sequence_type::derives_from(type0, xs_float) && sequence_type::derives_from(type1, xs_float))
			if ( item0->getFloatValue() < item1->getFloatValue())
				ret = -1;
			else if ( item0->getFloatValue() == item1->getFloatValue())
				ret = 0;
			else
				ret = 1;
		else if (sequence_type::derives_from(type0, xs_double) && sequence_type::derives_from(type1, xs_double))
			if ( item0->getDoubleValue() < item1->getDoubleValue())
				ret = -1;
			else if ( item0->getDoubleValue() == item1->getDoubleValue())
				ret = 0;
			else
				ret = 1;
		else if (sequence_type::derives_from(type0, xs_decimal) && sequence_type::derives_from(type1, xs_decimal))
			if ( item0->getDecimalValue() < item1->getDecimalValue())
				ret = -1;
			else if ( item0->getDecimalValue() == item1->getDecimalValue())
				ret = 0;
			else
				ret = 1;
		else if (sequence_type::derives_from(type0, xs_string) && sequence_type::derives_from(type1, xs_string))
			if ( item0->getStringValue() < item1->getStringValue())
				ret = -1;
			else if ( item0->getStringValue() == item1->getStringValue())
				ret = 0;
			else
				ret = 1;
		// TODO comparisons for all types

		return ret;
	}
	/* end class ComparisonIterator */

}
