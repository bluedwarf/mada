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

#define MAPPED_ARRAY_TESTFILE "abc"
void testMappedArray()
{
    FILE *f;

    // Note that the test file "abc" will be removed.
    remove(MAPPED_ARRAY_TESTFILE);

    f = fopen(MAPPED_ARRAY_TESTFILE, "r");
    if (f) {
        fclose(f);
        assert (f != NULL); // Failed to remove the test file.
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

    printf ("Unit test of mapped array was successfully finished.\n");
}

int main(int argc, char* argv)
{
    testMappedArray();
}
