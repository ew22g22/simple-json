#include <stdio.h>

#include "../src/sjson.h"

int main(void)
{
    S_error_code_t err;
    char *test = "{\"test\" : [{\"test2\" : \"cool\"}, [{\"test3\" : 2}]], \"test4\" : [null, true]}";
    
    S_object_t o = S_parse(test, strlen(test));
    S_array_t *a = S_object_get_array(o, "test", &err);
    S_array_t *b = S_array_get_array(a, 1, &err);
    S_object_t c = S_array_get_object(b, 0, &err);
    double i = S_object_get_number(c, "test3", &err);
    char *x = S_write(o);
    printf("%s\n", x);
    printf("%d %d\n", (int) i, (int) err);
    free(x);
    S_destroy(&o);
    return 0;
}
