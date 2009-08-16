/*
 * DoubleArray.hpp
 * Copyright (C) 2009 Takashi Nakamoto <bluedwarf@bpost.plala.or.jp>.
 *
 * This program is part of MaDa Double Array library.
 *
 * MaDa Double Array library is free software: you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * MaDa Double Array library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with MaDa Double Array library. If not, see
 * <http://www.gnu.org/licenses/>.
 */

/*
 * Double Array with C++.
 */

#ifndef _MADA_DOUBLE_ARRAY_HPP_
#define _MADA_DOUBLE_ARRAY_HPP_

#include <vector>
#include "MappedArray.hpp"

using namespace std;

namespace mada
{
template <class IndexType, class KeyType> class DoubleArray
{
private:
    MappedArray<IndexType> base;
    MappedArray<IndexType> check;
    MappedArray<KeyType> tail;
    IndexType pos;
    KeyType term; // terminal symbol
    KeyType max; // the maximal value of KeyType

    int keylen(const KeyType *key);
    KeyType *fetch_str(IndexType p);
    int str_cmp(const KeyType *str1, const KeyType *str2);
    void a_insert(IndexType r, const KeyType *b);
    void ins_str(IndexType r, const KeyType *e, IndexType d_pos);
    IndexType set_str(IndexType p, const KeyType *y, IndexType y_len);
    void b_insert(IndexType r, const KeyType *b, const KeyType *c, const KeyType *d);
    IndexType x_check(vector<KeyType> &c_list);
    vector<KeyType> set_list(IndexType r);
    IndexType modify(IndexType current, IndexType r,
		     KeyType a, vector<KeyType> c_list);

public:
    DoubleArray(const char *basefile,
		const char *checkfile,
		const char *tailfile,
		IndexType pos,
		KeyType term,
		KeyType max,
		int initialize);
    ~DoubleArray();

    void dump();
    int find(const KeyType *a); // Check key "a" whether it is registered.
    int insert(const KeyType *a); // Register key "a".
    int remove(const KeyType *a); // Remove key "a".

    int loadWordList(const char *file);
};

template <class IndexType, class KeyType>
DoubleArray<IndexType, KeyType>::DoubleArray(const char *basefile,
					     const char *checkfile,
					     const char *tailfile,
					     IndexType pos,
					     KeyType term,
					     KeyType max,
					     int initialize) :
    base(basefile),
    check(checkfile),
    tail(tailfile)
{
    if (initialize)
    {
	base.clear();
	base[0] = 0; /* undefiend */
	base[1] = 1;

	check.clear();
	check[0] = 0; /* undefined */
	check[1] = 1;

	tail.clear();
	tail[0] = 0; /* undefined */
	this->pos = 1;
    }
    else
    {
	this->pos = pos;
    }

    if (term <= 0)
	throw 1; /* terminal symbol must be greater than 0. */

    this->term = term;
    this->max = max;
}

template <class IndexType, class KeyType>
DoubleArray<IndexType, KeyType>::~DoubleArray()
{

}

#define MIN(a,b) (a < b ? a : b)
#define MAX(a,b) (a > b ? a : b)
template <class IndexType, class KeyType>
void DoubleArray<IndexType, KeyType>::dump()
{
    int i, j;

    for (i = 1; i <= check[1]; i += 15)
    {
	/* print indices */
	printf ("       ");
	for (j = i; j <= MIN(i+14, check[1]); j++)
	    printf ("%4d", j);
	printf ("\n");

	/* print the BASE array */
	printf ("  BASE ");
	for (j = i; j <= MIN(i+14, check[1]); j++)
	    printf ("%4d", base[j]);
	printf ("\n");

	/* print the CHECK array */
	printf (" CEHCK ");
	for (j = i; j <= MIN(i+14, check[1]); j++)
	    printf ("%4d", check[j]);
	printf ("\n");

	printf ("\n");
    }

    for (i = 1; i <= pos-1; i += 15)
    {
	/* print indices */
	printf ("       ");
	for (j = i; j <= MIN(i+14, pos-1); j++)
	    printf ("%4d", j);
	printf ("\n");

	/* print the TAIL array */
	printf ("  TAIL ");
	for (j = i; j <= MIN(i+14, pos-1); j++)
	    printf ("%4d", tail[j]);
	printf ("\n");
	printf ("\n");
    }
}
#undef MIN
#undef MAX

/*
 * This method check if the specified key is included in this double
 * array. If so this method returns 1, otherwise 0.
 *
 * Argument:
 *   a: Key to be added.
 *      The end of this string must be ended with terminal symbole "term".
 */
template <class IndexType, class KeyType>
int DoubleArray<IndexType, KeyType>::find(const KeyType *a)
{
    IndexType r, h, t, n, p;
    KeyType *s_temp;

    r = 1;
    h = 0;
    n = keylen(a);

    do
    {
	h += 1;
	t = base[r] + a[h-1];

	if (t > check[1] || check[t] != r)
	    return 0;
	else
	    r = t;
    } while (!(base[r] < 0));

    if (h == n + 1)
	return 1;
    else
	s_temp = fetch_str (-base[r]);

    p = str_cmp (a + h, s_temp);
    delete (s_temp);
    if (p == -1)
	return 1;
    else
	return 0;
}

template <class IndexType, class KeyType>
int DoubleArray<IndexType, KeyType>::keylen(const KeyType *a)
{
    int i = 0;

    while (a[i] != term)
	i++;

    return i;
}

/*
 * Note that the returned value must be freed by invoker.
 */
template <class IndexType, class KeyType>
KeyType *DoubleArray<IndexType, KeyType>::fetch_str(IndexType p)
{
    int k;
    KeyType *ret;

    k = 0;
    while (tail[p + k] != term)
	k++;

    ret = new KeyType [k+2];
    for (int i=0; i<k+1; i++)
	ret[i] = tail[p+i];
    ret[k + 1] = 0;

    return ret;
}

template <class IndexType, class KeyType>
int DoubleArray<IndexType, KeyType>::str_cmp(const KeyType *str1,
					     const KeyType *str2)
{
    int i;

    i = 0;
    while (str1[i]  == str2[i])
    {
	if (str1[i] == term)
	    return -1;

	i++;
    }

    return i;
}

/*
 * This method inserts a new key to this double array. If it successfully
 * added the specified key, it returns 0. Otherwise, it returns 0.
 *
 * Argument:
 *   a: Key to be added.
 *      The end of this string must be ended with terminal symbole "term".
 */
template <class IndexType, class KeyType>
int DoubleArray<IndexType, KeyType>::insert(const KeyType *a)
{
    IndexType r, h, t, n, p;
    KeyType *s_temp;

    r = 1;
    h = 0;
    n = keylen(a);

    do
    {
	h += 1;
	t = base[r] + a[h-1];

	if (t > check[1] || check[t] != r)
	{
	    KeyType *b = new KeyType [n + 2 - h];
	    for (int i=0; i<n+2-h; i++)
		b[i] = a[i + h - 1];

	    a_insert (r, b);
	    delete b;

	    return 1;
	}
	else
	    r = t;
    } while (!(base[r] < 0));

    if (h == n + 1)
	return 0;
    else
	s_temp = fetch_str (-base[r]);

    p = str_cmp (a + h, s_temp);
    if (p == -1)
    {
	delete s_temp;
	return 0;
    }
    else
    {
	KeyType *x = new KeyType [p+1];
	for (int i=0; i<p; i++)
	    x[i] = a[h+i];
	x[p] = term;

	b_insert (r, x, a + h + p, s_temp + p);

	delete s_temp;
	delete x;

	return 1;
    }
}

template <class IndexType, class KeyType>
void DoubleArray<IndexType, KeyType>::a_insert(IndexType r, const KeyType *b)
{
    IndexType t;

    t = base[r] + b[0];
    if (check[t] != 0)
    {
	vector<KeyType> list1 = set_list (r);
	vector<KeyType> list2 = set_list (check[t]);

	if (list1.size() + 1 < list2.size())
	    modify (r, r, b[0], list1);
	else
	    modify (r, check[t], 0, list2);
    }

    ins_str (r, b, pos);
}

template <class IndexType, class KeyType>
void DoubleArray<IndexType, KeyType>::ins_str(IndexType r, const KeyType *e, IndexType d_pos)
{
    IndexType t, e_len;
    KeyType *e_with_record;

    t = base[r] + e[0];
    check[t] = r;
    base[t] = -d_pos;
    if (t > check[1])
	check[1] = t;

    e_len = keylen (e);
    e_with_record = new KeyType [e_len + 1];
    for (int i=0; i<e_len; i++)
	e_with_record[i] = e[i+1];
    e_with_record[e_len] = '$'; // record (dummy)

    pos = set_str (d_pos, e_with_record, e_len + 1);

    delete e_with_record;
}

template <class IndexType, class KeyType>
IndexType DoubleArray<IndexType, KeyType>::set_str(IndexType p, const KeyType *y, IndexType y_len)
{
    IndexType i;

    for (i=0; i<y_len; i++)
	tail[p + i] = y[i];

    return (p == pos ? pos + y_len : pos);
}

template <class IndexType, class KeyType>
void DoubleArray<IndexType, KeyType>::b_insert(IndexType r,
					       const KeyType *b,
					       const KeyType *c,
					       const KeyType *d)
{
    IndexType old_pos = -base[r];

    for (int i=0; i<keylen(b); i++)
    {
	vector<KeyType> c_list;
	c_list.push_back (b[i]);
	base[r] = x_check (c_list);

	if (base[r] + b[i] > check[1])
	    check[1] = base[r] + b[i];

	check[base[r] + b[i]] = r;
	r = base[r] + b[i];
    }

    vector<KeyType> c2_list;
    c2_list.push_back (c[0]);
    c2_list.push_back (d[0]);
    base[r] = x_check (c2_list);
    if (r > check[1])
	check[1] = r;
    ins_str (r, d, old_pos);
    ins_str (r, c, pos);
}

template <class IndexType, class KeyType>
IndexType DoubleArray<IndexType, KeyType>::x_check(vector<KeyType> &c_list)
{
    int i;
    IndexType q;
    KeyType c;

    q = 1;
    while (1)
    {
	i = 0;
	while (1)
	{
	    if (i == c_list.size())
		return q;

	    c = c_list[i];

	    if (q + c > check[1])
	    {
		i++;
		continue;
	    }

	    if (check[q + c] != 0)
		break;

	    i++;
	}

	q++;
    }
}

template <class IndexType, class KeyType>
vector<KeyType> DoubleArray<IndexType, KeyType>::set_list(IndexType r)
{
    IndexType t;
    KeyType a;
    vector<KeyType> ret;

    if (r <= 0)
	return ret;

    for (a=1; a<=max; a++)
    {
	t = base[r] + a;
	if (t >= 2 && check[t] == r)
	    ret.push_back (a);

	if (a == max)
	    break;
    }

    return ret;
}

template <class IndexType, class KeyType>
IndexType DoubleArray<IndexType, KeyType>::modify(IndexType current,
						  IndexType r,
						  KeyType a,
						  vector<KeyType> c_list)
{
    IndexType old_base = base[r];

    if (a == 0)
	base[r] = x_check (c_list);
    else
    {
	vector<KeyType> tmp_list;
	int found = 0;
	for (int i=0; i<c_list.size(); i++)
	{
	    if (c_list[i] == a)
		found = 1;

	    tmp_list.push_back (c_list[i]);
	}
	if (!found)
	    tmp_list.push_back (a);

	base[r] = x_check (tmp_list);
    }

    KeyType c;
    IndexType t, t_dash;

    for (int i=0; i<c_list.size(); i++)
    {
	c = c_list[i];

	t = old_base + c;
	t_dash = base[r] + c;

	if (t_dash > check[1])
	    check[1] = t_dash;
	check[t_dash] = r;
	base[t_dash] = base[t];

	if (base[t] > 0)
	{
	    for (int q=2; q<=check[1]; q++)
	    {
		if (check[q] == t)
		    check[q] = t_dash;
	    }

	    if (t == current)
		current = t_dash;
	}

	base[t] = 0;
	check[t] = 0;
    }

    return current;
}

template <class IndexType, class KeyType>
int DoubleArray<IndexType, KeyType>::remove(const KeyType *a)
{
    IndexType r, h, t, n, p;
    KeyType *s_temp;

    r = 1;
    h = 0;
    n = keylen(a);

    do
    {
	h += 1;
	t = base[r] + a[h-1];

	if (t > check[1] || check[t] != r)
	    return 0;
	else
	    r = t;
    } while (!(base[r] < 0));

    if (h == n + 1)
    {
	base[r] = 0;
	check[r] = 0;
	return 1;
    }
    else
	s_temp = fetch_str (-base[r]);

    p = str_cmp (a + h, s_temp);
    delete (s_temp);
    if (p == -1)
    {
	base[r] = 0;
	check[r] = 0;
	return 1;
    }
    else
	return 0;
}

/*
 * Read keys from text file and add those keys to this double array.
 *
 * == RETURN ==
 *  -1: Failed to open the specified file.
 *  0:  The number of newly added keys.
 */
template <class IndexType, class KeyType>
int DoubleArray<IndexType, KeyType>::loadWordList(const char *file)
{
    int len;
    int count = 0;
    char word[256];
    FILE *f;

    f = fopen (file, "r");
    if (!f)
	return -1;

    while (fgets (word, 255, f))
    {
	len = strlen (word);

	if (len >= 1)
	{
	    word[len-1] = term; /* replace '\n' with terminal symbol */
	    count += insert (word);
	}
    }
    fclose (f);

    return count;
}

}

#endif // _MADA_DOUBLE_ARRAY_HPP_
