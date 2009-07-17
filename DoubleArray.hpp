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
  ダブル配列(Double Array)の実装

  ダブル配列をC++で実装したものである。

== 配列のインデックスについて ==

  参考文献[1]、[2]では配列や文字列のインデックスが1ではじまる。C言語では一般に配列の
  インデックスが0からはじめるのが基本だが、本実装ではBASE配列、CEHCK配列、TAIL配列と
  もにインデックスが1からはじまるようにしてある。このため、これらの配列の0番目の要素
  の値は不定である。しかし、文字列処理はできるだけC言語の慣習に従うためにインデックス
  を0からはじまるようにしてあるため、参考文献[1]、[2]と一部のインデックスが異なる。

[参考文献]
  [1] 吉村賢治，「自然言語処理の基礎」，サイエンス社，2000，pp.174-177．
      (ISBN 4-7819-0956-6)
  [2] 青江順一，ダブル配列による高速ディジタル検索アルゴリズム，電子情報
      通信学会論文誌，J71-D，9，pp.1592-1600，1987．
 */

#ifndef _MADA_DOUBLE_ARRAY_HPP_
#define _MADA_DOUBLE_ARRAY_HPP_

#include "MappedArray.hpp"

namespace mada
{
template <class IndexType, class KeyType> class DoubleArray
{
private:
    MappedArray<IndexType> base;
    MappedArray<IndexType> check;
    MappedArray<KeyType> tail;
    IndexType pos;
    KeyType term; // 見出し語の終端文字

    int key_len(const KeyType *key);
    KeyType *fetch_str(IndexType p);
    int str_cmp(const KeyType *str1, const KeyType *str2);
public:
    DoubleArray(const char *basefile,
		const char *checkfile,
		const char *tailfile,
		IndexType pos,
		KeyType term,
		int initialize);
    ~DoubleArray();

    void dump();
    int find(const KeyType *a); // Check key "a" whether it is registered.
    int insert(const KeyType *a); // Register key "a".
};

template <class IndexType, class KeyType>
DoubleArray<IndexType, KeyType>::DoubleArray(const char *basefile,
								 const char *checkfile,
								 const char *tailfile,
								 IndexType pos,
								 KeyType term,
								 int initialize) :
    base(basefile),
    check(checkfile),
    tail(tailfile)
{
    if (initialize)
    {
	base[0] = 0; /* undefiend */
	base[1] = 1;

	check[0] = 0; /* undefined */
	check[1] = 1;

	tail[0] = 0; /* undefined */
	this->pos = 1;
    }
    else
    {
	this->pos = pos;
    }

    if (term <= 0)
	throw 1; /* 終端記号の内部表現値も1以上でなければならない */

    this->term = term;
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
 * このメソッドはダブル配列に指定された見出し語が存在するのかどうかを調べ
 * ます。もし見出し語が存在するのであれば1を、そうでなければ0を返します。
 * 引数:
 *  a: 見出し語。ただし、文字列の末尾は終端記号 term で終わっていなければ
 *     ならない。
 *
 * この関数で使用される変数と参考文献[2]の図2は以下のように対応している。
 *  r => r
 *  h => h
 *  n => n - 1
 *  a_1 => a[1]
 *  a_h => a[h - 1]
 *  S_TEMP => s_temp
 *  FETCH_STR => fetch_str
 *  STR_CMP => str_cmp
 *  return(TRUE) => return 1;
 *  return(FALSE) => return 0;
 */
template <class IndexType, class KeyType>
int DoubleArray<IndexType, KeyType>::find(const KeyType *a)
{
    int r, h, t, n, p;
    KeyType *s_temp;

    r = 1;
    h = 0;
    n = key_len(a);

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
int DoubleArray<IndexType, KeyType>::key_len(const KeyType *a)
{
    int i = 0;

    while (a[i] != term)
	i++;

    return i;
}

/*
 * 参考文献[2]の図2におけるFETCH_STRに対応する関数
 *
 * 返り値の文字列は呼び出し元によってメモリが開放されなければいけない。
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

/*
 * 参考文献[2]の図2におけるSTR_CMPに対応する関数
 */
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
 * このメソッドはダブル配列に指定された見出し語を挿入します。
 * 見出し語の挿入に成功すれば1を返し、すでに見出し語がダブル配列に存在して
 * いた場合には0を返します。
 * 引数:
 *  a: 見出し語。ただし、文字列の末尾は終端記号 term で終わっていなければ
 *     ならない。
 *
 * この関数で使用される変数と参考文献[2]の図2は以下のように対応している。
 *  r => r
 *  h => h
 *  n => n - 1
 *  a_1 => a[1]
 *  a_h => a[h - 1]
 *  S_TEMP => s_temp
 *  FETCH_STR => double_array_fetch_str
 *  STR_CMP => double_array_str_cmp
 *  return(TRUE) => return 0;
 *  return(FALSE) => return 1;
 *
 * ただし、参考文献[2]の「4.1 キーの追加」に記述されている変更を加えてある。
 * また、C言語の慣習に合わせるため、返り値がdouble_array_find関数とは逆になっ
 * ている。
 *
 * 参考文献[2]において、「4.1 キーの追加」の章の「(b) 行(1-3)のreturn(FALSE)
 * の変更」におけるrは紛らわしい。B_INSERT関数の第1引数に用いられるrは図2の
 * rと同一である。しかし、その他のrは図2におけるSTR_CMP関数の戻り値とすると
 * 矛盾が無くなる。よって、その他のrは本メソッドでは変数pとしてある。
 */
template <class IndexType, class KeyType>
int DoubleArray<IndexType, KeyType>::insert(const KeyType *a)
{
    int r, h, t, n, p;
    KeyType *s_temp;

    r = 1;
    h = 0;
    n = key_len(a);

    do
    {
	h += 1;
	t = base[r] + a[h-1];

	if (t > check[1] || check[t] != r)
	{
	    KeyType *b = new KeyType [n + 2 - h];
	    for (int i=0; i<n+2-h; i++)
		b[i] = a[i + h - 1];

//	    double_array_a_insert (r, b);
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
	printf("Not implemented yet.\n");
	return 0;
/*
	char *x;
	x = (char *) malloc (sizeof(char) * (p + 1));
	strncpy (x, a + h, p);
	x[p] = 0;

	double_array_b_insert (da, r, x, a + h + p, s_temp + p);

	delete s_temp;
	free (x);
*/
	return 1;
    }
}

}

#endif // _MADA_DOUBLE_ARRAY_HPP_
