#include <stdio.h>
#include "ht.h"

int main(int argc, char **argv)
{
    ht * t = ht_new(NULL);
    char buffer[] = "GARBAGEGARBAGEGARBAGEGARBAGEGARBAGEGARBAGEGARBAGE";
    if (ht_set(t, "test", "this is a test string", 0) == NULL)
        fprintf(stderr, "SET: table ERROR\n");
    if (!ht_get(t, "test", &buffer, 0))
        fprintf(stderr, "GET: table MISS\n");
    printf("%s: %s\n", "test", buffer);

    if (ht_set(t, "test", "t", 0) != NULL)
        fprintf(stderr, "SET: table ERROR\n");

    if (ht_update(t, "test", "this is another test string", 0) == NULL)
        fprintf(stderr, "SET: ERROR updating\n");
    if (!ht_get(t, "test", &buffer, 0))
        fprintf(stderr, "GET: table MISS\n");
    printf("%s: %s\n", "test", buffer);

    ht_free(t);
    return 0;
}

