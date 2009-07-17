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
template <class BaseArrayType, class CheckArrayType, class NCType> class DoubleArray
{
private:
    MappedArray<BaseArrayType> base;
    MappedArray<CheckArrayType> check;
    MappedArray<NCType> tail;
    BaseArrayType pos;
public:
    DoubleArray(const char *basefile,
		const char *checkfile,
		const char *tailfile,
		BaseArrayType pos,
		int initialize);
    ~DoubleArray();

    void dump();
};

template <class BaseArrayType, class CheckArrayType, class NCType>
DoubleArray<BaseArrayType, CheckArrayType, NCType>::DoubleArray(const char *basefile,
								const char *checkfile,
								const char *tailfile,
								BaseArrayType pos,
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
	pos = 1;
    }
    else
    {
	this->pos = pos;
    }
}

template <class BaseArrayType, class CheckArrayType, class NCType>
DoubleArray<BaseArrayType, CheckArrayType, NCType>::~DoubleArray()
{

}

#define MIN(a,b) (a < b ? a : b)
#define MAX(a,b) (a > b ? a : b)
template <class BaseArrayType, class CheckArrayType, class NCType>
void DoubleArray<BaseArrayType, CheckArrayType, NCType>::dump()
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

}

#endif // _MADA_DOUBLE_ARRAY_HPP_
