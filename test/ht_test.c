#include <stdio.h>
#include <string.h>
#include "ht.h"


/* Awesomely horrible hash function, but easy to test. */
hashkey hasher(char * key){
    printf("Hashing %s\n", key);
    hashkey code = 0;
    for (; *key != 0; key++)
        code += *key;
    printf("Code is %d\n", (int)code);
    return code;
}

int eachf(size_t size, const char * k, const void * v, void * d){
    printf("\t'%s' : %*s\n", k, (int) size, (const char *) v);
    return 0;
}

int main(int argc, char **argv)
{
    ht * t = ht_new(NULL);
    size_t bsize;
    char buffer[] = "GARBAGEGARBAGEGARBAGEGARBAGEGARBAGEGARBAGEGARBAGE";

    bsize = 1 + strlen(buffer);
    if (ht_set(t, "test", "this is a test string", 0) == NULL)
        fprintf(stderr, "SET: table ERROR\n");
    if (!ht_get(t, "test", &buffer, bsize))
        fprintf(stderr, "GET: table MISS\n");
    printf("%s: %s\n", "test", buffer);

    if (ht_set(t, "test", "t", 0) != NULL)
        fprintf(stderr, "SET: table ERROR\n");

    if (ht_update(t, "test", "this is another test string", 0) == NULL)
        fprintf(stderr, "SET: ERROR updating\n");
    if (!ht_get(t, "test", &buffer, bsize))
        fprintf(stderr, "GET: table MISS\n");
    printf("%s: %s\n", "test", buffer);
    ht_free(t);
    printf("Freeing the table\n");


    t = ht_new(hasher);
    if (ht_set(t, "test", "this is a test string", 0) == NULL)
        fprintf(stderr, "SET: table ERROR\n");
    if (!ht_get(t, "test", &buffer, 0))
        fprintf(stderr, "GET: table MISS\n");
    printf("%s: %s\n", "test", buffer);
    ht_each(t, eachf, NULL);

    ht_free(t);
    return 0;
}

