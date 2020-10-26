#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main()
{
    char x[1024] = "";
    strcat(x, "This is 1\n");
    strcat(x, "This is 2\n");
    printf("%s\n", x);
}