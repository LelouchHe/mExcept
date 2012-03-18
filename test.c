#include <stdio.h>

#include "except.h"

const except_T e = {"exception"};

void fun2()
{

    THROW(e);
}

void fun1()
{

    fun2();
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

    return 0;
}
