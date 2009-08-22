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
#include <time.h>

#include "MappedArray.hpp"
#include "DoubleArray.hpp"

void s2us(unsigned char *dest, const char *src)
{
    size_t len = strlen(src);
    for (size_t i=0; i<=len; i++)
	dest[i] = static_cast<unsigned char>(src[i]);
}

void printConsoleHelp()
{
    printf ("===== COMMAND LIST =====\n\n");
    printf (" exit: Exit from this console.\n");
    printf (" quit: Exit from this console.\n");
    printf (" add words: Add a word to this double array.\n");
    printf (" remove words: Delete a word from this double array.\n");
    printf (" search words: Search a word in this double array.\n");
    printf (" load file: Add words in file.\n");
    printf (" search_file file: Search all words in file.\n");
    printf (" dump: Dump double array.\n");
    printf (" info: Show the information of current double array.\n\n");
}

void launchConsole(int init)
{
    char command[256];
    char key[256];
    unsigned char ukey[256];
    char term = '#';

    // initialize double array
    mada::DoubleArray<int, unsigned char> da("mem/base",
					     "mem/check",
					     1, term, 255, init);

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
	    s2us (ukey, key);

	    clock_t start = clock();
	    int res = da.Add (ukey);
	    clock_t end = clock();

	    if (res) {
		printf("ADDED \"%s\".\n", key);
		printf ("%f msec\n", (float)(end-start)/(float)CLOCKS_PER_SEC*1000.0);
	    } else
		printf("Failed to add \"%s\".\n", key);
	} else if (strncmp (command, "remove ", 7) == 0 &&
		   command[7] != '\0') {
	    strcpy (key, command + 7);

	    int len = strlen (key);
	    key[len-1] = term; // replace '\n' with the terminal symbol.
	    s2us (ukey, key);

	    if (da.Remove (ukey))
		printf("DELETED \"%s\".\n", key);
	    else
		printf("Failed to delete \"%s\".\n", key);
	} else if (strncmp (command, "search ", 7) == 0 &&
		   command[7] != '\0') {
 	    strcpy (key, command + 7);

	    int len = strlen (key);
	    ukey[len-1] = term; // replace '\n' with the terminal symbol.
	    s2us (ukey, key);

	    if (da.Search (ukey))
		printf("FOUND \"%s\".\n", key);
	    else
		printf("Failed to find \"%s\".\n", key);
	} else if (strncmp (command, "load ", 5) == 0 &&
		   command[5] != '\0') {
	    strcpy (key, command + 5);
	    key[strlen(key)-1] = '\0';

	    FILE *f = fopen (key, "r");
	    if (!f) {
		printf ("Failed to open %s\n", key);
		return;
	    }

	    int count = 0;
	    clock_t start = clock();

	    while (fgets (key, 255, f))
	    {
		size_t len = strlen (key);

		if (len >= 1)
		{
		    key[len-1] = term; /* replace '\n' with terminal symbol */
		    s2us (ukey, key);
		    count += da.Add (ukey);
		}
	    }
	    fclose (f);

	    clock_t end = clock();

	    printf ("Added %d keys\n", count);
	    printf ("%f sec\n", (float)(end-start)/(float)CLOCKS_PER_SEC);
	} else if (strncmp (command, "search_file ", 12) == 0 &&
		   command[12] != '\0') {
	    strcpy (key, command + 12);
	    key[strlen(key)-1] = '\0';

	    FILE *f = fopen (key, "r");
	    if (!f) {
		printf ("Failed to open %s\n", key);
		return;
	    }

	    while (fgets (key, 255, f)) {
		size_t len = strlen (key);
		
		if (len >= 1)
		{
		    key[len-1] = term; /* replace '\n' with terminal symbol */
		    s2us (ukey, key);
		    if (da.Search (ukey)) {
		      //			printf("FOUND \"%s\".\n", key);
		    } else {
			printf("Failed to find \"%s\".\n", key);
		    }
		}
	    }

	    fclose (f);
	} else if (strncmp (command, "dump\n", 5) == 0) {
	    da.dump();
	} else if (strncmp (command, "info\n", 5) == 0) {
	    da.printInfo();
	} else {
	    printConsoleHelp ();
	}
    }
}

int main(int argc, char* argv[])
{
    if (argc >= 2 && strcmp (argv[1], "init") == 0) {
	printf ("Initializing ...\n");
	launchConsole(1);
    } else
	launchConsole(0);
}
