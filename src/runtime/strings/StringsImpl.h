/**
 * @copyright
 * ========================================================================
 *	Copyright 2006-2007 FLWOR Foundation
 * ========================================================================
 *
 * @author Sorin Nasoi (sorin.nasoi@ipdevel.ro)
 * @file functions/StringsImpl.h
 *
 */

#ifndef XQP_STRINGS_IMPL_H
#define XQP_STRINGS_IMPL_H

#include "context/common.h"
#include "runtime/base/iterator.h"
#include "runtime/base/unarybase.h"
#include "runtime/base/narybase.h"
// #include "types/sequence_type.h"

#ifdef WIN32
#include "util/win32/compatib_defs.h"
#endif

namespace xqp {

class zorba;
class GenericCast;

/**______________________________________________________________________
 *
 * 7.2 Functions to Assemble and Disassemble Strings
 *_______________________________________________________________________*/

 /**
 * 7.2.1 fn:codepoints-to-string
 * -------------------- */
/* begin class CodepointsToStringIterator */
class CodepointsToStringIterator : public UnaryBaseIterator<CodepointsToStringIterator>{
public:
		CodepointsToStringIterator ( const yy::location& loc, PlanIter_t& arg )
	:
		UnaryBaseIterator<CodepointsToStringIterator>( loc, arg ){};

		~CodepointsToStringIterator(){};
public:
		Item_t nextImpl(PlanState& planState);
};
/* end class CodepointsToStringIterator */


/**
 * 7.2.2 fn:string-to-codepoints
 * --------------------*/
/*begin class StringToCodepointsIterator*/
class StringToCodepointsIterator : public UnaryBaseIterator<StringToCodepointsIterator>
{
public:
		StringToCodepointsIterator ( const yy::location& loc, PlanIter_t& arg )
	:
		UnaryBaseIterator<StringToCodepointsIterator>( loc, arg ){};

	~StringToCodepointsIterator() {};

public:
	Item_t nextImpl(PlanState& planState);
}; /*end class StringToCodepointsIterator*/

/**
 * 7.3.2 fn:compare
 * --------------------*/
/* begin class CompareStrIterator */

class CompareStrIterator: public NaryBaseIterator<CompareStrIterator> {
	public:
		CompareStrIterator ( const yy::location& loc, std::vector<PlanIter_t>& args );
		~CompareStrIterator();

		Item_t nextImpl(PlanState& planState);
};

/* end class CompareStrIterator */

/**
 * 7.3.3 fn:codepoint-equal
 * --------------------*/
class CodepointEqualIterator : public Batcher<CodepointEqualIterator>
{
public:
	CodepointEqualIterator(
		yy::location loc,
		PlanIter_t _argv0,
		PlanIter_t _argv1)
	:
		Batcher<CodepointEqualIterator>(loc),
		argv0(_argv0),
		argv1(_argv1)
	{}

	CodepointEqualIterator(
		const CodepointEqualIterator& cpEqual_it)
	:
		Batcher<CodepointEqualIterator>(cpEqual_it),
		argv0(cpEqual_it.argv0),
		argv1(cpEqual_it.argv1)
	{}

	~CodepointEqualIterator() {}

public:
	Item_t nextImpl(PlanState& planState);
	void resetImpl(PlanState& planState);
	void releaseResourcesImpl(PlanState& planState);
	std::ostream&  _show(std::ostream&) const;

protected:
	PlanIter_t argv0;
	PlanIter_t argv1;

	std::vector<char> v0;
	std::vector<char> v1;
	char * c0;
	char * c1;
	int16_t vLength;
	bool finish;
};/*class CodepointEqualIterator */

/**
 * 7.4.1 fn:concat
 * --------------------*/
class ConcatFnIterator : public Batcher<ConcatFnIterator>
{
public:
	ConcatFnIterator(
		yy::location loc,
		const vector<PlanIter_t>& _argv)
	:
	Batcher<ConcatFnIterator>(loc),
		argv(_argv),
		currit_h(NULL),
		cursor(0),
		res()
	{}

	ConcatFnIterator(
		const ConcatFnIterator& concatFn_it)
	:
	Batcher<ConcatFnIterator>(concatFn_it),
		argv(concatFn_it.argv),
		currit_h(concatFn_it.currit_h),
		cursor(concatFn_it.cursor),
		res(concatFn_it.res)
	{}

		~ConcatFnIterator() {}

public:
	Item_t nextImpl(PlanState& planState);
	void resetImpl(PlanState& planState);
	void releaseResourcesImpl(PlanState& planState);
	std::ostream&  _show(std::ostream&) const;

protected:
	std::vector<PlanIter_t> argv;
	PlanIter_t currit_h;
	uint32_t cursor;
	std::string res;

}; /*class ConcatFnIterator*/


/**
 * 7.4.2 fn:string-join
 * --------------------*/
class StringJoinIterator : public Batcher<StringJoinIterator>
{
public:
	StringJoinIterator(
		yy::location loc,
		const vector<PlanIter_t>& _argv)
	:
		Batcher<StringJoinIterator>(loc),
		argv(_argv),
		currit_h(NULL),
		cursor(0),
		res()
	{}

	StringJoinIterator(
		const StringJoinIterator& stringJoin_it)
	:
		Batcher<StringJoinIterator>(stringJoin_it),
		argv(stringJoin_it.argv),
		currit_h(stringJoin_it.currit_h),
		cursor(stringJoin_it.cursor),
		res(stringJoin_it.res)
	{}

		~StringJoinIterator() {}

public:
	Item_t nextImpl(PlanState& planState);
	void resetImpl(PlanState& planState);
	void releaseResourcesImpl(PlanState& planState);
	std::ostream&  _show(std::ostream&) const;

protected:
	std::vector<PlanIter_t> argv;
	PlanIter_t currit_h;
	uint32_t cursor;
	std::string res;

}; /*class StringJoinIterator*/

}/*namespace xqp*/
#endif /* XQP_STRINGS_IMPL_H */
