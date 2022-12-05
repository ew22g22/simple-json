#ifndef SJSON_H
#define SJSON_H

#include <stdlib.h>
#include <string.h>

typedef enum e_S_value_type     S_value_type_t;
typedef struct s_S_value        S_value_t;
typedef struct s_S_object_entry S_object_entry_t;
typedef S_object_entry_t        *S_object_t;

S_object_t S_parse(char *data, size_t sz);

void S_object_destroy_recursively(S_object_t *obj);

#endif
