#ifndef SJSON_H
#define SJSON_H

#include <stdlib.h>
#include <string.h>
#include <math.h>

#define S_WRITE_NUMBER_NUM_DECIMAL_POINT 10

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

/***
 * Parses a JSON string into an object representation.
 * @param const char * data The string data to parse
 * @param size_t sz Size of the string being parsed
 * @return Object representation of the JSON string
 ***/
S_object_t S_parse(const char *data, size_t sz);

/***
 * Writes the JSON object into a string.
 * @param S_object_t obj The JSON object to print
 * @return String serialized JSON object (heap allocated)
 ***/
char *S_write(S_object_t obj);

/***
 * Helper functions for a JSON object.
 * These functions take the object, and the name of the
 * JSON 'object' to return.
 * 
 * Example:
 * To get the value of the bool from o = {"x" : true}
 * S_object_get_bool(o, "x", NULL);
 *
 * The error pointer will be set with the error code 
 * if any error occurs during the execution of the function
 ***/
S_value_t  *S_object_get(S_object_t obj, const char *name, S_error_code_t *err);
S_bool_t   S_object_get_bool(S_object_t obj, const char *name, S_error_code_t *err);
double     S_object_get_number(S_object_t obj, const char *name, S_error_code_t *err);
S_object_t S_object_get_object(S_object_t obj, const char *name, S_error_code_t *err);
char       *S_object_get_string(S_object_t obj, const char *name, S_error_code_t *err);
S_array_t  *S_object_get_array(S_object_t obj, const char *name, S_error_code_t *err);
S_bool_t   S_object_is_null(S_object_t obj, const char *name, S_error_code_t *err);

/***
 * Helper functions for a JSON array.
 * These functions take the array and the index of
 * the element to be returned.
 *
 * Example:
 * To get the nth element from the array 'a' as a
 * bool
 *
 * S_array_get_bool(a, n, NULL);
 *
 * The error pointer will be set with the error code
 * if any error occurs during the execution of the function
 ***/
S_value_t  *S_array_get(S_array_t *arr, size_t i, S_error_code_t *err);
S_bool_t   S_array_get_bool(S_array_t *arr, size_t i, S_error_code_t *err);
double     S_array_get_number(S_array_t *arr, size_t i, S_error_code_t *err);
S_object_t S_array_get_object(S_array_t *arr, size_t i, S_error_code_t *err);
char       *S_array_get_string(S_array_t *arr, size_t i, S_error_code_t *err);
S_array_t  *S_array_get_array(S_array_t *arr, size_t i, S_error_code_t *err);
S_bool_t   S_array_is_null(S_array_t *arr, size_t i, S_error_code_t *err);

/***
 * Frees a JSON object and its value (recursively).
 * @param S_object_t * obj The object to destry
 ***/
void S_destroy(S_object_t *obj);

#endif
