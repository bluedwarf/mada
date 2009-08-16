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
 *
 *
 * Warning:
 *  IndexType must be signed value.
 *  KeyType must be unsigned value.
 */

#ifndef _MADA_DOUBLE_ARRAY_HPP_
#define _MADA_DOUBLE_ARRAY_HPP_

#include <vector>
#include <list>
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
    IndexType tail_pos;
    KeyType term; // terminal symbol
    KeyType max; // the maximal value of KeyType

/*
    KeyType *fetch_str(IndexType p);
    int str_cmp(const KeyType *str1, const KeyType *str2);
    void ins_str(IndexType r, const KeyType *e, IndexType d_pos);
    IndexType set_str(IndexType p, const KeyType *y, IndexType y_len);
    vector<KeyType> set_list(IndexType r);
    IndexType modify(IndexType current, IndexType r,
		     KeyType a, vector<KeyType> c_list);
*/

    IndexType da_size;

    int keylen(const KeyType *key);
    void W_Base(IndexType index, IndexType val);
    void W_Check(IndexType index, IndexType val);
    IndexType X_Check(list<KeyType> &A);
    int Forward(IndexType s, KeyType a);
    list<KeyType> GetLabel(IndexType index);
    void Modify(IndexType index, list<KeyType> &R, KeyType b);
    void Insert(IndexType index, IndexType pos, const KeyType *a);
public:
    DoubleArray(const char *basefile,
		const char *checkfile,
		const char *tailfile,
		IndexType pos,
		KeyType term,
		KeyType max,
		int initialize);
    ~DoubleArray();

/*
    int remove(const KeyType *a); // Remove key "a".
*/

    IndexType Search(const KeyType *a);
    IndexType Add(const KeyType *a);

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
	da_size = 1;

	tail.clear();
	tail[0] = 0; /* undefined */
	this->tail_pos = 1;
    }
    else
    {
	this->tail_pos = pos;
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

template <class IndexType, class KeyType>
int DoubleArray<IndexType, KeyType>::keylen(const KeyType *a)
{
    int i = 0;

    while (a[i] != term)
	i++;

    return i;
}

template <class IndexType, class KeyType>
void DoubleArray<IndexType, KeyType>::W_Base(IndexType index, IndexType val)
{
    base[index] = val;

    if (index > da_size)
	da_size = index;
}

template <class IndexType, class KeyType>
void DoubleArray<IndexType, KeyType>::W_Check(IndexType index, IndexType val)
{
    check[index] = val;

    if (index > da_size)
	da_size = index;
}

template <class IndexType, class KeyType>
IndexType DoubleArray<IndexType, KeyType>::X_Check(list<KeyType> &A)
{
    IndexType q;
    KeyType c;

    // X-1
    q = 1;

    // X-2
    do {
	int ok = 1;

	typename list<KeyType>::iterator c = A.begin();
	while (c != A.end()) {
	    if (check[q+(*c)] != 0) {
		ok = 0;
		break;
	    }
	    c++;
	}

        // this q meets the condition that check[q+c]=0 for all c in A.
	if (ok) 
	    return q;

	q++;
    } while (q <= da_size);

    // (X-3)
    return q;
}

template <class IndexType, class KeyType>
int DoubleArray<IndexType, KeyType>::Forward(IndexType s, KeyType a)
{
    IndexType t;

    t = base[s] + a;
    if (0 < t && t < da_size+1 && check[t] == s)
	return t;
    else
	return 0;
}

template <class IndexType, class KeyType>
list<KeyType> DoubleArray<IndexType, KeyType>::GetLabel(IndexType index)
{
    IndexType t;
    list<KeyType> ret;

    if (index <= 0)
	return ret;

    for (KeyType a=1; a<=max; a++)
    {
	t = base[index] + a;
	if (0 < t && t < da_size+1 && check[t] == index)
	    ret.push_back (a);

	// For signed value.
	// e.g.
	//    * KeyType is char.
	//    * max is 127.
	//      => This loop would be the infinite loop without the following
	//         break condition.
	if (a == max)
	    break;
    }

    return ret;
}

template <class IndexType, class KeyType>
void DoubleArray<IndexType, KeyType>::Modify(IndexType index, list<KeyType> &R, KeyType b)
{
    // (M-1)
    IndexType oldbase = base[index];

    list<KeyType> tmp = R;
    tmp.push_back (b);
    W_Base (index, X_Check (tmp));

    // (M-2)
    typename list<KeyType>::iterator c = R.begin();
    while (c != R.end()) {
	IndexType t = base[index] + (*c);
	W_Check (t, index);

	IndexType old_t = oldbase + (*c);
	W_Base (t, base[old_t]);

	if (base[old_t] > 0) {
	    // (M-3)
	    for (IndexType q=1; q<=da_size; q++)
		if (check[q] == old_t)
		    W_Check (q, t);
	}

	base[old_t] = 0;
	check[old_t] = 0;

	c++;
    }
}

template <class IndexType, class KeyType>
void DoubleArray<IndexType, KeyType>::Insert(IndexType index, IndexType pos, const KeyType *a)
{
    int n = keylen (a);

    // (I-1)
    if (check[base[index] + a[pos-1]] > 0) {
	list<KeyType> R = GetLabel (index);
	Modify (index, R, a[pos-1]);
    }

    // (I-2)
    IndexType t = base[index] + a[pos-1];
    W_Check (t, index);
    index = t;
    pos++;

    // (I-3)
    while (pos <= n+1) {
	list<KeyType> A;
	A.push_back (a[pos-1]);
	IndexType newbase = X_Check (A);

	W_Base (index, newbase);

	t = base[index] + a[pos-1];
	W_Check (t, index);
	index = t;
	pos++;
    }

    W_Base (t, -1); // ToDo: Change -1 according to the record.
}

/*
 * Note that the returned value must be freed by invoker.
 */
/*
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

template <class IndexType, class KeyType>
void DoubleArray<IndexType, KeyType>::ins_str(IndexType r, const KeyType *e, IndexType d_pos)
{
    IndexType t, e_len;
    KeyType *e_with_record;

    t = base[r] + e[0];
    check[t] = r;
    base[t] = -d_pos;
    if (t > da_size)
	da_size = t;

    e_len = keylen (e);
    e_with_record = new KeyType [e_len + 1];
    for (int i=0; i<e_len; i++)
	e_with_record[i] = e[i+1];
    e_with_record[e_len] = '$'; // record (dummy)

    tail_pos = set_str (d_pos, e_with_record, e_len + 1);

    delete e_with_record;
}

template <class IndexType, class KeyType>
IndexType DoubleArray<IndexType, KeyType>::set_str(IndexType p, const KeyType *y, IndexType y_len)
{
    IndexType i;

    for (i=0; i<y_len; i++)
	tail[p + i] = y[i];

    return (p == tail_pos ? tail_pos + y_len : tail_pos);
}
*/
/*
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

	if (t_dash > da_size)
	    da_size = t_dash;
	check[t_dash] = r;
	base[t_dash] = base[t];

	if (base[t] > 0)
	{
	    for (int q=2; q<=da_size; q++)
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

	if (t > da_size || check[t] != r)
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
*/

/*
 * This method check if a key is included in this double
 * If the specified key is found in this trie, this method returns
 * the index number of the leaf node (the index of the BASE array).
 * Otherwise, it returns 0.
 *
 * Argument:
 *   a: Key to be added.
 *      The end of this string must be ended with terminal symbole "term".
 */
template <class IndexType, class KeyType>
IndexType DoubleArray<IndexType, KeyType>::Search(const KeyType *a)
{
    // D-1
    IndexType index = 1;
    IndexType pos = 1;
    IndexType t;

    do {
	// D-2
	t = Forward (index, a[pos-1]);
	if (t == 0)
	    return 0;
	else {
	    index = t;
	    pos++;
	}
    } while (base[index] >= 0); // D-3

    return index;
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
IndexType DoubleArray<IndexType, KeyType>::Add(const KeyType *a)
{
    IndexType index = 1;
    IndexType pos = 1;
    IndexType t;

    do {
	t = Forward (index, a[pos-1]);
	if (t == 0) {
	    Insert (index, pos, a);
	    return 1;
	} else {
	    index = t;
	    pos++;
	}
    } while (base[index] >= 0);

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
	    count += Add (word);
	}
    }
    fclose (f);

    return count;
}

}

#endif // _MADA_DOUBLE_ARRAY_HPP_
