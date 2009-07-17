/*
 * main.cpp
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

#include <stdio.h>
#include <assert.h>

#include "MappedArray.hpp"
#include "DoubleArray.hpp"

#define MAPPED_ARRAY_TESTFILE "abc"
void testMappedArray()
{
    FILE *f;

    // Note that the test file "abc" will be removed.
    remove(MAPPED_ARRAY_TESTFILE);

    f = fopen(MAPPED_ARRAY_TESTFILE, "r");
    if (f) {
        fclose(f);
        assert(f != NULL); // Failed to remove the test file.
    }

    {
        // Test of new array.
        mada::MappedArray<int> array(MAPPED_ARRAY_TESTFILE);

        assert(array[0] == 0);
        array[0] = 1;
        assert(array[0] == 1);

        assert(array[INITIAL_MAPPED_SIZE + 10] == 0);
        array[INITIAL_MAPPED_SIZE + 10] = 10;
        assert(array[INITIAL_MAPPED_SIZE + 10] == 10);
    }

    {
        // Test of existing array.
        mada::MappedArray<int> array(MAPPED_ARRAY_TESTFILE);

        assert(array[0] == 1);
        assert(array[INITIAL_MAPPED_SIZE + 10] == 10);
    }

    assert(remove(MAPPED_ARRAY_TESTFILE) == 0);

    printf("Unit test of mapped array was successfully finished.\n");
}

/*
  本テストでは入力記号の有限集合Iをアルファベットの小文字(a-z)とし、これらの
  内部表現値は参考文献[1]の表6.1を用いる。ASCIIコードと内部表現値は以下の関
  数で相互に変換する。末尾記号は"#"である。
    - testEncode関数: ASCIIコードを内部表現値に変換
*/
unsigned char testEncode(char ch)
{
    if (ch >= 'a' && ch <= 'z')
	return (ch - 'a' + 1);
    else if (ch == '#')
	return 27;
    else if (ch == 0)
	return 0;
    else if (ch == '$')
	return 28;
    else if (ch == '?')
	return 29;
    else
    {
	fprintf (stderr, "This implementation of Double-Array doesn't support the character %x.\n", ch);
	exit (EXIT_FAILURE);
    }
}

char testDecode(unsigned char nm)
{
    if (nm >= 1 && nm <= 26)
	return (nm + 'a' - 1);
    else if (nm == 27)
	return '#';
    else if (nm == 0)
	return '\0';
    else if (nm == 28)
	return '$';
    else if (nm == 29)
	return '?';
    else
    {
	fprintf (stderr, "This implementation of Double-Array doesn't support the numerical number %x.\n", nm);
	exit (EXIT_FAILURE);
    }
}

/*
 * 参考文献[1]の図6.12にある見出し集合Kのダブル配列
 * K = {ai#, aitu#, aituide#, aite#}
 */
static int test1_base[30] = {0, 1, 1, 0, 0, 0, -11, 0, 0, 0, 1, -7, 0, 0, 0, 0,
			 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, -1, -4};
static int test1_check[30] = {0, 29, 1, 0, 0, 0, 21, 0, 0, 0, 2, 22, 0, 0, 0,
			    0, 0, 0, 0, 0, 0, 10, 21, 0, 0, 0, 0, 0, 10, 22};
static char test1_tail[13] = {0, '$', '?', '?', '$', '?', '?', 'd', 'e', '#',
			  '$', '#', '$'};
#define TEST1_BASE_SIZE (29)
#define TEST1_TAIL_SIZE (12)
void testDoubleArray1()
{
    {
	mada::MappedArray<int> base("test1_base");
	mada::MappedArray<int> check("test1_check");
	mada::MappedArray<unsigned char> tail("test1_tail");

	for (int i = 0; i <= TEST1_BASE_SIZE; i++)
	{
	    base[i] = test1_base[i];
	    check[i] = test1_check[i];
	    if (i <= TEST1_TAIL_SIZE)
		tail[i] = testEncode(test1_tail[i]);
	}
    }

    mada::DoubleArray<int, int, unsigned char> da("test1_base",
						  "test1_check",
						  "test1_tail",
						  TEST1_TAIL_SIZE + 1,
						  0);
    da.dump();
}

int main(int argc, char* argv)
{
//    testMappedArray();

    testDoubleArray1();
}
