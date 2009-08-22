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
#include "KeySet.hpp"

#define DA_SIZE (check[0])
#define NUM_KEY (base[0])

using namespace std;

namespace mada
{
template <class IndexType, class KeyType> class DoubleArray
{
private:
    MappedArray<IndexType> base;
    MappedArray<IndexType> check;
    KeyType term; // terminal symbol
    KeyType max; // the maximal value of KeyType

    IndexType e_head;

    int keys;

    KeySet<KeyType> R;

    int keylen(const KeyType *key);
    void W_Base(IndexType index, IndexType val);
    void W_Check(IndexType index, IndexType val);
    IndexType X_Check(KeySet<KeyType> &A);
    int Forward(IndexType s, KeyType a);
    void GetLabel(IndexType index);
    void Modify(IndexType index, KeyType b);
    void Insert(IndexType index, IndexType pos, const KeyType *a);
    void Delete(IndexType index);

    void ConstructUnusedList();
public:
    DoubleArray(const char *basefile,
		const char *checkfile,
		KeyType term,
		KeyType max,
		int initialize);
    ~DoubleArray();

    IndexType Search(const KeyType *a);
    IndexType Add(const KeyType *a);
    IndexType Remove(const KeyType *a);

    int loadWordList(const char *file);
    void dump();
    void printInfo();
};

template <class IndexType, class KeyType>
DoubleArray<IndexType, KeyType>::DoubleArray(const char *basefile,
					     const char *checkfile,
					     KeyType term,
					     KeyType max,
					     int initialize) :
    base(basefile),
    check(checkfile),
    R(max)
{
    if (initialize)
    {
	base.clear();
	NUM_KEY = 0; /* base[0] is used for the number of keys */
	base[1] = 1;

	check.clear();
	DA_SIZE = 1; /* check[0] is used for the size of double array. */
	check[1] = 0;
    }

    if (term <= 0)
	throw 1; /* terminal symbol must be greater than 0. */

    this->term = term;
    this->max = max;
    this->e_head = 0;
}

template <class IndexType, class KeyType>
DoubleArray<IndexType, KeyType>::~DoubleArray()
{
    base.truncate(DA_SIZE+1);
    check.truncate(DA_SIZE+1);
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
    if (e_head == 0) {
	if (index > DA_SIZE) {
	    DA_SIZE = index;
	    base.expand_to(DA_SIZE);
	    check.expand_to(DA_SIZE);
	}

	base[index] = val;
    } else {
	// update unused element list
	if (index > DA_SIZE) {
	    base.expand_to(index);
	    check.expand_to(index);

	    // (W-1)
	    IndexType e_index;

	    if (index == DA_SIZE + 1) {
		e_index = e_head;
		while (-check[e_index] <= DA_SIZE)
		    e_index = -check[e_index];
	    } else {
		for (e_index = DA_SIZE + 1; e_index < index-1; e_index++)
		    check[e_index] = -(e_index+1);
	    }

	    base[index] = val;
	    DA_SIZE = index;
	    check[e_index] = -(DA_SIZE + 1);
	} else {
	    base[index] = val;
	}
    }
}

template <class IndexType, class KeyType>
inline void DoubleArray<IndexType, KeyType>::W_Check(IndexType index, IndexType val)
{
    if (e_head == 0) {
	// unused element list is not in use.
	if (index > DA_SIZE) {
	    DA_SIZE = index;
	    base.expand_to(DA_SIZE);
	    check.expand_to(DA_SIZE);
	}

	check[index] = val;
    } else {
	// update unused element list
	if (index > DA_SIZE) {
	    base.expand_to(index);
	    check.expand_to(index);

	    // (W-1)
	    IndexType e_index;

	    if (index == DA_SIZE + 1) {
		e_index = e_head;
		while (-check[e_index] <= DA_SIZE)
		    e_index = -check[e_index];
	    } else {
		for (e_index = DA_SIZE + 1; e_index < index-1; e_index++)
		    check[e_index] = -(e_index+1);
	    }

	    check[index] = val;
	    DA_SIZE = index;
	    check[e_index] = -(DA_SIZE + 1);
	} else {
	    // (W-2)
	    if (check[index] < 0) {
		// (W-3a)
		if (index != e_head) {
		    // (W-3b)
		    IndexType prev_index = e_head;
		    while (prev_index <= DA_SIZE) {
			if (index == -check[prev_index]) {
			    check[prev_index] = check[index];
			    break;
			}

			prev_index = -check[prev_index];
		    }
		    check[index] = val;
		} else {
		    e_head = -check[index];
		    check[index] = val;
		}
	    } else if (val == 0) {
		// (W-4a)
		if (index >= e_head) {
		    // (W-4b)
		    IndexType prev_index = e_head;
		    while (prev_index <= DA_SIZE) {
			if (prev_index < index &&
			    index < -check[prev_index]) {
			    check[index] = check[prev_index];
			    check[prev_index] = -index;
			    break;
			}

			prev_index = -check[prev_index];
		    }
		} else {
		    check[index] = -e_head;
		    e_head = index;
		}
	    } else {
		check[index] = val;
	    }
	}
    }
}

template <class IndexType, class KeyType>
inline IndexType DoubleArray<IndexType, KeyType>::X_Check(KeySet<KeyType> &A)
{
    if (e_head) {
	// unused element list
	KeyType c1 = A[0];
	for (int i=0; i<A.size(); i++)
	    if (A[i] < c1)
		c1 = A[i];

	// (XX-1)
	IndexType e_index = e_head;

	// (XX-2)
	do {
	    IndexType q = e_index - c1;

	    if (q >= 1) {
		int ok = 1;
		for (size_t i = 0; i<A.size(); i++) {
		    KeyType c = A[i];

		    if (q+c <= DA_SIZE && check[q+c] > 0) {
			ok = 0;
			break;
		    }
		}

		if (ok) {
//		    printf ("q (1) = %d\n", q);
		    return q;
		}
	    }

	    e_index = -check[e_index];
	} while (e_index <= DA_SIZE); // (XX-3)

//	printf ("q (2) = %d, e_index = %d\n", e_index-c1, e_index);
	if (e_index - c1 < 1)
	    return 1;
	else
	    return e_index - c1;
    } else {
	IndexType q;
	KeyType c;

	// X-1
	q = 1;

	// X-2
	do {
	    int ok = 1;

	    for (size_t i = 0; i<A.size(); i++) {
		KeyType c = A[i];

		if (q+c <= DA_SIZE && check[q+c] > 0) {
		    ok = 0;
		    break;
		}
	    }

	    // this q meets the condition that check[q+c]=0 for all c in A.
	    if (ok) {
//		printf ("q (3) = %d\n", q);
		return q;
	    }

	    q++;
	} while (q <= DA_SIZE);

	// (X-3)
//	printf ("q (4) = %d\n", q);
	return q;
    }
}

template <class IndexType, class KeyType>
int DoubleArray<IndexType, KeyType>::Forward(IndexType s, KeyType a)
{
    IndexType t;

    t = base[s] + a;
    if (0 < t && t < DA_SIZE+1 && check[t] == s)
	return t;
    else
	return 0;
}

template <class IndexType, class KeyType>
inline void DoubleArray<IndexType, KeyType>::GetLabel(IndexType index)
{
    IndexType t;

    R.clear ();

    if (index <= 0)
	return;

    for (KeyType a=1; a<=max; a++)
    {
	t = base[index] + a;
	if (0 < t && t < DA_SIZE+1 && check[t] == index)
	    R.push_back (a);

	// For signed value.
	// e.g.
	//    * KeyType is char.
	//    * max is 127.
	//      => This loop would be the infinite loop without the following
	//         break condition.
	if (a == max)
	    break;
    }
}

template <class IndexType, class KeyType>
inline void DoubleArray<IndexType, KeyType>::Modify(IndexType index, KeyType b)
{
    IndexType t, old_t, q;

    // (M-1)
    IndexType oldbase = base[index];

/*
    list<KeyType> tmp = R;
    tmp.push_back (b);
    W_Base (index, X_Check (tmp));
*/
    R.push_back (b);
    W_Base (index, X_Check (R));
    R.pop_back ();

    // (M-2)
    for (size_t i = 0; i<R.size(); i++) {
	KeyType c = R[i];

	t = base[index] + c;
	W_Check (t, index);

	old_t = oldbase + c;
	W_Base (t, base[old_t]);

	if (base[old_t] > 0) {
	    // (M-3)

	  //	    for (q=1; q<=DA_SIZE; q++)
	  for (q=base[old_t]+1; q<=base[old_t]+max; q++)
	    if (check[q] == old_t)
	      W_Check (q, t);
	}

	base[old_t] = 0;
	check[old_t] = 0;
    }
}

template <class IndexType, class KeyType>
void DoubleArray<IndexType, KeyType>::Insert(IndexType index, IndexType pos, const KeyType *a)
{
    int n = keylen (a);
    IndexType t = base[index] + a[pos-1];

    // (I-1)
    if (t <= DA_SIZE && check[t] > 0) {
	GetLabel (index);
	Modify (index, a[pos-1]);

	t = base[index] + a[pos-1];
    }

    // (I-2)
    W_Check (t, index);
    index = t;
    pos++;

    // (I-3)
    while (pos <= n+1) {
	R.clear ();
	R.push_back (a[pos-1]);
	IndexType newbase = X_Check (R);

	W_Base (index, newbase);

	t = base[index] + a[pos-1];
	W_Check (t, index);
	index = t;
	pos++;
    }

    W_Base (t, -1); // ToDo: Change -1 according to the record.
}

template <class IndexType, class KeyType>
void DoubleArray<IndexType, KeyType>::Delete(IndexType index)
{
    W_Base (index, 0);
    W_Check (index, 0);
}

template <class IndexType, class KeyType>
void DoubleArray<IndexType, KeyType>::ConstructUnusedList()
{
    int count;
    vector<IndexType> r;

    for (IndexType index=1; index<=DA_SIZE; index++) {
	if (check[index] <= 0 && base[index] == 0)
	    r.push_back (index);
    }

    if (r.size() < 3)
	return; // don't construct unused list;

    e_head = r[0];
    for (IndexType i=0; i<=r.size()-2; i++)
	check[r[i]] = -r[i+1];
    check[r[r.size()-1]] = -(DA_SIZE + 1);
}

/*
 * This method check if a key is included in this double
 * If the specified key is found in this trie, this method returns
 * the index number of the leaf node (the index of the BASE array).
 * Otherwise, it returns 0.
 *
 * Argument:
 *   a: Key to be searched.
 *      The end of this string must be ended with terminal symbol "term".
 */
template <class IndexType, class KeyType>
IndexType DoubleArray<IndexType, KeyType>::Search(const KeyType *a)
{
    if (!NUM_KEY)
	return 0;

    // (D-1)
    IndexType index = 1;
    IndexType pos = 1;
    IndexType t;

    do {
	// (D-2)
	t = Forward (index, a[pos-1]);
	if (t == 0)
	    return 0;
	else {
	    index = t;
	    pos++;
	}
    } while (base[index] >= 0); // (D-3)

    return index;
}

/*
 * This method inserts a new key to this double array. If it successfully
 * adds the specified key, it returns 1. Otherwise, it returns 0.
 *
 * Argument:
 *   a: Key to be added.
 *      The end of this string must be ended with terminal symbol "term".
 */
template <class IndexType, class KeyType>
IndexType DoubleArray<IndexType, KeyType>::Add(const KeyType *a)
{
    // (D-1)
    IndexType index = 1;
    IndexType pos = 1;
    IndexType t;

    do {
	// (D-2)
	t = Forward (index, a[pos-1]);
	if (t == 0) {
	    Insert (index, pos, a);

	    if (e_head == 0)
		ConstructUnusedList ();

	    NUM_KEY = NUM_KEY + 1;
	    return 1;
	} else {
	    index = t;
	    pos++;
	}
    } while (base[index] >= 0); // (D-3)

    return 0;
}

/*
 * This method remove an existing key from this double array. If the
 * specified key is successfully removed, it returns 1. Otherwise, it
 * returns 0.
 *
 * Argument:
 *   a: Key to be removed.
 *      The end of this string must be ended with terminal symbole "term".
 */
template <class IndexType, class KeyType>
IndexType DoubleArray<IndexType, KeyType>::Remove(const KeyType *a)
{
    if (!NUM_KEY)
	return 0;

    // (D-1)
    IndexType index = 1;
    IndexType pos = 1;
    IndexType t;

    do {
	// (D-2)
	t = Forward (index, a[pos-1]);
	if (t == 0)
	    return 0;
	else {
	    index = t;
	    pos++;
	}
    } while (base[index] >= 0); // (D-3)

    Delete (index);
    NUM_KEY = NUM_KEY - 1;
    return 1;
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
    unsigned char uword[256];
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
	    for (size_t i=0; i<=len; i++)
	      uword[i] = static_cast<unsigned char>(word[i]);

	    count += Add (uword);
	}
    }
    fclose (f);

    return count;
}

#define MIN(a,b) (a < b ? a : b)
#define MAX(a,b) (a > b ? a : b)
template <class IndexType, class KeyType>
void DoubleArray<IndexType, KeyType>::dump()
{
    int i, j;

    for (i = 1; i <= DA_SIZE; i += 15)
    {
	/* print indices */
	printf ("       ");
	for (j = i; j <= MIN(i+14, DA_SIZE); j++)
	    printf ("%4d", j);
	printf ("\n");

	/* print the BASE array */
	printf ("  BASE ");
	for (j = i; j <= MIN(i+14, DA_SIZE); j++)
	    printf ("%4d", base[j]);
	printf ("\n");

	/* print the CHECK array */
	printf (" CEHCK ");
	for (j = i; j <= MIN(i+14, DA_SIZE); j++)
	    printf ("%4d", check[j]);
	printf ("\n");

	printf ("\n");
    }
}
#undef MIN
#undef MAX

template <class IndexType, class KeyType>
void DoubleArray<IndexType, KeyType>::printInfo()
{
    printf ("Size of index: %d bytes\n", sizeof(IndexType));
    printf ("Size of array: %d (%d bytes)\n",
	    DA_SIZE, DA_SIZE*sizeof(IndexType)*2);
    printf ("The number of keys: %d\n", NUM_KEY);
}

}

#endif // _MADA_DOUBLE_ARRAY_HPP_
