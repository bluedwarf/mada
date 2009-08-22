#include <stdio.h>
#include <time.h>
#include <string.h>

#include <glib.h>

int main(int argc, char *argv[])
{
    char key[256];
    FILE *f;
    GHashTable *hash;

//    g_mem_set_vtable (glib_mem_profiler_table);

    if (argc < 2) {
	fprintf (stderr, "Specify the word list file.\n");
	return 0;
    }

    // open a word list file
    f = fopen (argv[1], "r");
    if (!f) {
	fprintf (stderr, "Couldn't open %s.\n", argv[1]);
	return 0;
    }

    // construct a hash table from the word list
    hash = g_hash_table_new (g_str_hash, g_str_equal);

    clock_t start = clock();
    while (fgets (key, 256, f))
    {
	size_t len = strlen (key);
	key[len-1] = '\0';

	if (len >= 1)
	    g_hash_table_insert (hash, g_strdup(key), GINT_TO_POINTER(1));
    }
    clock_t end = clock();

    printf ("Inserting time: %f sec\n", (float)(end-start)/(float)CLOCKS_PER_SEC);
    fclose (f);

//    g_mem_profile();

    // look up

    // open a word list file again
    f = fopen (argv[1], "r");
    if (!f) {
	fprintf (stderr, "Couldn't open %s.\n", argv[1]);
	return 0;
    }

    start = clock();
    while (fgets (key, 256, f))
    {
	size_t len = strlen (key);
	gint res;
	key[len-1] = '#';

	if (len >= 1) {
	    res = GPOINTER_TO_INT (g_hash_table_lookup (hash, key));
	    if (res)
		printf ("Failed to find %s\n", key);
	}
    }
    end = clock();

    printf ("Search time: %f sec\n", (float)(end-start)/(float)CLOCKS_PER_SEC);
    fclose (f);

    g_hash_table_destroy (hash);
}
