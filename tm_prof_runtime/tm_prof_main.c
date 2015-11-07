#include <stdio.h>

void tm_prof_begin()
{
    printf("BEGIN\n");
}

void tm_prof_operation(int size)
{
    printf("operation sizeof(mem) = %d\n", size);
}

void tm_prof_commit()
{
    printf("commit\n");
}

