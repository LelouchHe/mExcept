#include <stdio.h>

#include "except.h"

const except_T e = {"exception"};

void fun2()
{

    THROW(e);
    printf("some");
}

void fun1()
{

    fun2();

    printf("some");
}

int main()
{
    TRY
    {
        THROW(e);
    }
    CACHE(e)
    {
        printf("cache %s\n", e.msg);
    }
    END_TRY;

    fun1();

    printf("some");

    return 0;
}
