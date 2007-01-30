/* -*- mode: c++; indent-tabs-mode: nil -*-
 *
 *  $Id: item_iterator.cpp,v 1.1 2006/10/09 07:07:59 Paul Pedersen Exp $
 *
 *	Copyright 2006-2007 FLWOR Foundation.
 *
 *  Author: Paul Pedersen
 *
 */

#include "iterator.h"

#include "../context/context.h"
#include "../util/Assert.h"
#include "../util/xqp_exception.h"
#include "../values/xs_primitive_values.h"

#include <iostream>
#include <sstream>

using namespace std;
namespace xqp {


/*...........................................
	: item_iterator base                      :
	:.........................................:
*/

item_iterator::item_iterator(
	context * _ctx_p)
:
	ctx_p(_ctx_p)
{
}

item_iterator::item_iterator(
	item_iterator const& it)
:
	ctx_p(it.ctx_p)
{
}

item_iterator& item_iterator::operator=(
	item_iterator const& it)
{
	ctx_p = it.ctx_p;
	return *this;
}

item_iterator::~item_iterator()
{
}

void item_iterator::open()
{
}

void item_iterator::close()
{
}

rchandle<item> item_iterator::next()
{
	return NULL;
}

rchandle<item> item_iterator::peek() const 
{
	return NULL;
}

bool item_iterator::done() const
{
	return true;
}

void item_iterator::rewind()
{
}

rchandle<item> item_iterator::operator*() const
{
	return peek();
}

item_iterator& item_iterator::operator++()
{
	next();
	return *this;
}

string item_iterator::string_value()
{
	ostringstream oss;
	while (!done()) {
		rchandle<item> i_h = next();
		item* i_p = &*i_h;
		if (i_p==NULL) continue;
		i_p->put(oss,ctx_p);
	}
	return oss.str();
}



/*...........................................
	: binary iterator                         :
	:.........................................:
*/

void binary_iterator::open()
{
	it1_h->open();
	it2_h->open();
}

void binary_iterator::close()
{
	it1_h->close();
	it2_h->close();
}

rchandle<item> binary_iterator::next()
{
	rchandle<item> i1 = it1_h->next();
	rchandle<item> i2 = it2_h->next();
	return (*op)(ctx_p,i1,i2);
}

rchandle<item> binary_iterator::peek() const 
{
	rchandle<item> i1 = it1_h->peek();
	rchandle<item> i2 = it2_h->peek();
	return (*op)(ctx_p,i1,i2);
}

bool binary_iterator::done() const
{
	return it1_h->done() || it2_h->done();
}

void binary_iterator::rewind()
{
	it1_h->rewind();
	it2_h->rewind();
}

rchandle<item> binary_iterator::operator*() const
{
	return peek();
}

item_iterator& binary_iterator::operator++()
{
	next();
	return *this;
}

binary_iterator::binary_iterator(
	context * ctx_p,
	rchandle<item_iterator> _it1_h,
	rchandle<item_iterator> _it2_h,
	rchandle<item> (*_op)(context *, rchandle<item> const&, rchandle<item> const&))
:
	item_iterator(ctx_p),
	it1_h(_it1_h),
	it2_h(_it2_h),
	op(_op)
{
}

binary_iterator& binary_iterator::operator=(
	binary_iterator const& it)
{
	ctx_p = it.ctx_p;
	it1_h = it.it1_h;
	it2_h = it.it2_h;
	op = it.op;
	return *this;
}

binary_iterator::~binary_iterator()
{
}



/*...........................................
	: singleton iterator                      :
	:.........................................:
*/

void singleton_iterator::open()
{
}

void singleton_iterator::close()
{
	done_b = false;
}

rchandle<item> singleton_iterator::next()
{
	if (!done_b) {
		done_b = true;
		return i_h;
	}
	else {
		return NULL;
	}
}

rchandle<item> singleton_iterator::peek() const 
{
	return (!done_b) ? i_h : NULL;
}

bool singleton_iterator::done() const
{
	return done_b;
}

void singleton_iterator::rewind()
{
	done_b = false;
}

rchandle<item> singleton_iterator::operator*() const
{
	return peek();
}

item_iterator& singleton_iterator::operator++()
{
	next();
	return *this;
}

singleton_iterator::singleton_iterator(
	context * ctx_p,
	rchandle<item> _i_h)
:
	item_iterator(ctx_p),
	i_h(_i_h),
	done_b(false)
{
#ifdef DEBUG
	cout << "singleton_iterator(rchandle<item>)\n";
#endif
}

singleton_iterator::singleton_iterator(
	context * ctx_p,
	item * _i_p)
:
	item_iterator(ctx_p),
	i_h(_i_p),
	done_b(false)
{
#ifdef DEBUG
	cout << "singleton_iterator(item*)\n";
#endif
}

singleton_iterator::singleton_iterator(
	context * ctx_p,
	string const& s)
:
	item_iterator(ctx_p),
	i_h(new xs_string_value(s)),
	done_b(false)
{
#ifdef DEBUG
	cout << "singleton_iterator(string)\n";
#endif
}

singleton_iterator::~singleton_iterator()
{
}

singleton_iterator::singleton_iterator(
	context * ctx_p,
	bool v)
:
	item_iterator(ctx_p),
	i_h(new xs_boolean_value(v)),
	done_b(false)
{
#ifdef DEBUG
	cout << "singleton_iterator(bool)\n";
#endif
}

singleton_iterator::singleton_iterator(
	context * ctx_p,
	double v)
:
	item_iterator(ctx_p),
	i_h(new xs_double_value(v)),
	done_b(false)
{
#ifdef DEBUG
	cout << "singleton_iterator(double)\n";
#endif
}

singleton_iterator::singleton_iterator(
	context * ctx_p,
	float v)
:
	item_iterator(ctx_p),
	i_h(new xs_float_value(v)),
	done_b(false)
{
#ifdef DEBUG
	cout << "singleton_iterator(float)\n";
#endif
}

singleton_iterator::singleton_iterator(
	context * ctx_p,
	int v)
:
	item_iterator(ctx_p),
	i_h(new xs_int_value(v)),
	done_b(false)
{
#ifdef DEBUG
	cout << "singleton_iterator(int)\n";
#endif
}

singleton_iterator::singleton_iterator(
	context * ctx_p,
	long long v)
:
	item_iterator(ctx_p),
	i_h(new xs_long_value(v)),
	done_b(false)
{
#ifdef DEBUG
	cout << "singleton_iterator(long long)\n";
#endif
}

singleton_iterator::singleton_iterator(
	context * ctx_p,
	short v)
:
	item_iterator(ctx_p),
	i_h(new xs_short_value(v)),
	done_b(false)
{
#ifdef DEBUG
	cout << "singleton_iterator(short)\n";
#endif
}

singleton_iterator::singleton_iterator(
	context * ctx_p,
	signed char v)
:
	item_iterator(ctx_p),
	i_h(new xs_byte_value(v)),
	done_b(false)
{
#ifdef DEBUG
	cout << "singleton_iterator(signed char)\n";
#endif
}

singleton_iterator::singleton_iterator(
	context * ctx_p,
	unsigned char v)
:
	item_iterator(ctx_p),
	i_h(new xs_unsignedByte_value(v)),
	done_b(false)
{
#ifdef DEBUG
	cout << "singleton_iterator(unsigned char)\n";
#endif
}

singleton_iterator::singleton_iterator(
	context * ctx_p,
	unsigned int v)
:
	item_iterator(ctx_p),
	i_h(new xs_unsignedInt_value(v)),
	done_b(false)
{
#ifdef DEBUG
	cout << "singleton_iterator(signed int)\n";
#endif
}

singleton_iterator::singleton_iterator(
	context * ctx_p,
	unsigned long long v)
:
	item_iterator(ctx_p),
	i_h(new xs_nonNegativeInteger_value(v)),
	done_b(false)
{
#ifdef DEBUG
	cout << "singleton_iterator(unsigned long long)\n";
#endif
}

singleton_iterator::singleton_iterator(
	context * ctx_P,
	unsigned short v)
:
	item_iterator(ctx_p),
	i_h(new xs_unsignedShort_value(v)),
	done_b(false)
{
#ifdef DEBUG
	cout << "singleton_iterator(unsigned short)\n";
#endif
}

singleton_iterator::singleton_iterator(
	singleton_iterator const& it)
:
	item_iterator(it.ctx_p),
	i_h(it.i_h),
	done_b(it.done_b)
{
}

singleton_iterator& singleton_iterator::operator=(
	singleton_iterator const& it)
{
	ctx_p = it.ctx_p;
	i_h = it.i_h;
	done_b = it.done_b;
	return *this;
}



/*...........................................
	: concat iterator                         :
	:.........................................:
*/

void concat_iterator::open()
{
}

void concat_iterator::close()
{
}

rchandle<item> concat_iterator::next()
{
	while (currit_h->done() && ++walker!=sentinel) {
		currit_h = *walker;
		++it_counter;
	}
	if (done()) return NULL;
	return currit_h->next();
}

rchandle<item> concat_iterator::peek() const
{
	if (done()) return NULL;
	return currit_h->peek();
}

bool concat_iterator::done() const
{
	return (it_counter==it_list.size()
					&& currit_h->done());
}

void concat_iterator::rewind()
{
	walker = it_list.begin();
	for (; walker!=sentinel; ++walker) {
		(*walker)->rewind();
	}
	it_counter = 0;
}

rchandle<item> concat_iterator::operator*() const
{
	return peek();
}

item_iterator& concat_iterator::operator++()
{
	next();
	return *this;
}

concat_iterator::concat_iterator(
	context * ctx_p,
	list<rchandle<item_iterator> > _it_list)
:
	item_iterator(ctx_p),
	it_list(_it_list),
	walker(_it_list.begin()),
	sentinel(_it_list.end()),
	it_counter(1)
{
	currit_h = *walker;
	while (currit_h->done() && ++walker!=sentinel) {
		currit_h = *walker;
		++it_counter;
	}
}

concat_iterator::concat_iterator(
	concat_iterator const& it)
:
	item_iterator(it.ctx_p),
	it_list(it.it_list),
	walker(it.walker),
	sentinel(it.sentinel),
	currit_h(it.currit_h),
	it_counter(it.it_counter)
{
}

concat_iterator& concat_iterator::operator=(
	concat_iterator const& it)
{
	ctx_p = it.ctx_p;
	it_list = it.it_list;
	walker = it.walker;
	sentinel = it.sentinel;
	currit_h = it.currit_h;
	it_counter = it.it_counter;
	return *this;
}

concat_iterator::~concat_iterator()
{
}



}	/* namespace xqp */


