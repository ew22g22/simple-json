#include <stdio.h>

#include "../src/sjson.h"

int main(void)
{
    char *test_json = "{\"test\" : [1, 2, 3, \"Test\"], \"test2\" : 3}";
    S_object_t o = S_parse(test_json, strlen(test_json));
    printf("%x\n", o);
    return 0;
}
