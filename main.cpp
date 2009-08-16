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

void printConsoleHelp()
{
    printf ("===== COMMAND LIST =====\n\n");
    printf (" exit: Exit from this console.\n");
    printf (" quit: Exit from this console.\n");
    printf (" add words: Add a word to this double array.\n");
    printf (" delete words: Delete a word from this double array.\n");
    printf (" search words: Search a word in this double array.\n");
    printf (" load file: Add words in file.\n\n");
}

void launchConsole()
{
    char command[256];
    char key[256];
    char term = '#';

    // initialize double array
    mada::DoubleArray<int, char> da("base",
				    "check",
				    "tail",
				    1, term, 127, 1);

    while (1) {
	printf("> ");
	fgets(command, 256, stdin);

	if (strncmp (command, "quit\n", 5) == 0 ||
	    strncmp (command, "exit\n", 5) == 0) {
	    printf ("QUIT\n");
	    break;
	} else if (strncmp (command, "add ", 4) == 0 && 
		   command[4] != '\0') {
	    strcpy (key, command + 4);

	    int len = strlen (key);
	    key[len-1] = term; // replace '\n' with the terminal symbol.

	    if (da.Add (key))
		printf("ADDED \"%s\".\n", key);
	    else
		printf("Failed to add \"%s\".\n", key);
/*
	} else if (strncmp (command, "delete ", 7) == 0 &&
		   command[7] != '\0') {
	    strcpy (key, command + 7);

	    int len = strlen (key);
	    key[len-1] = term; // replace '\n' with the terminal symbol.

	    if (da.remove (key))
		printf("DELETED \"%s\".\n", key);
	    else
		printf("Failed to delete \"%s\".\n", key);
*/
	} else if (strncmp (command, "search ", 7) == 0 &&
		   command[7] != '\0') {
 	    strcpy (key, command + 7);

	    int len = strlen (key);
	    key[len-1] = term; // replace '\n' with the terminal symbol.

	    if (da.Search (key))
		printf("FOUND \"%s\".\n", key);
	    else
		printf("Failed to find \"%s\".\n", key);
	} else if (strncmp (command, "load ", 5) == 0 &&
		   command[5] != '\0') {
	    strcpy (key, command + 5);

	    int len = strlen (key);
	    key[len-1] = '\0'; // replace '\n' with '\0'

	    int count = da.loadWordList (key);
	    if (count == -1)
		printf ("Failed to open %s\n", key);
	    else
		printf ("Added %d keys\n", count);
	} else {
	    printConsoleHelp ();
	}
    }
}

int main(int argc, char* argv)
{
    launchConsole();
}
