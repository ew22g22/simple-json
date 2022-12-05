#include <stdio.h>

#include "../src/sjson.h"

int main(void)
{
    char *test2 = "{\"test\":[]}";
    char *test_json = "{\"test\" : [1, 2, 3, \"Test\"], \"test2\" : null, \"cool\" : { \"other_cool\" : \"dope\" } }";
    S_object_t o = S_parse(test2, strlen(test2));
    printf("%x\n", o);
    return 0;
}
