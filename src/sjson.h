#ifndef SJSON_H
#define SJSON_H

#include <stdlib.h>
#include <string.h>

typedef enum e_S_value_type     S_value_type_t;
typedef struct s_S_value        S_value_t;
typedef struct s_S_array        S_array_t;
typedef struct s_S_object_entry S_object_entry_t;
typedef S_object_entry_t        *S_object_t;

typedef enum {
    S_ERROR_CODE_OK = 0,
    S_ERROR_CODE_OBJECT_NOT_FOUND,
    S_ERROR_CODE_INVALID_TYPE,
    S_ERROR_CODE_MALLOC_ERR,
    S_ERROR_CODE_OUT_OF_BOUNDS
} S_error_code_t;

typedef unsigned char S_bool_t;

S_object_t S_parse(char *data, size_t sz);

S_value_t  *S_object_get(S_object_t obj, const char *name, S_error_code_t *err);
S_bool_t   S_object_get_bool(S_object_t obj, const char *name, S_error_code_t *err);
double     S_object_get_number(S_object_t obj, const char *name, S_error_code_t *err);
S_object_t S_object_get_object(S_object_t obj, const char *name, S_error_code_t *err);
char       *S_object_get_string(S_object_t obj, const char *name, S_error_code_t *err);
S_array_t  *S_object_get_array(S_object_t obj, const char *name, S_error_code_t *err);

S_value_t  *S_array_get(S_array_t *arr, size_t i, S_error_code_t *err);
S_bool_t   S_array_get_bool(S_array_t *arr, size_t i, S_error_code_t *err);
double     S_array_get_number(S_array_t *arr, size_t i, S_error_code_t *err);
S_object_t S_array_get_object(S_array_t *arr, size_t i, S_error_code_t *err);
char       *S_array_get_string(S_array_t *arr, size_t i, S_error_code_t *err);
S_array_t  *S_array_get_array(S_array_t *arr, size_t i, S_error_code_t *err);

void S_object_destroy_recursively(S_object_t *obj);

#endif
