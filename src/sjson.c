#include "sjson.h"

#include <stdio.h>

#define S_ISDIGIT(c) ((c) >= 0x30 && (c) <= 0x39)

typedef struct {
    char *ptr;
    char *end;
} S_ctx;

static void S_skip_whitespace(S_ctx *ctx) {
    while (ctx->ptr != ctx->end && (*ctx->ptr == ' ' 
                || *ctx->ptr == '\n'|| *ctx->ptr == '\r' 
                || *ctx->ptr == '\t')) {
        ctx->ptr++;
    }
}

static int S_skip_over_if_possible(S_ctx *ctx) {
    if (ctx->ptr == ctx->end) {
        return 0;
    }
    ctx->ptr++;
    S_skip_whitespace(ctx);
    return 1;
}

/* -------------------- Value -------------------- */

typedef enum e_S_value_type {
    S_VALUE_TYPE_STRING = 0,
    S_VALUE_TYPE_NUMBER,
    S_VALUE_TYPE_OBJECT,
    S_VALUE_TYPE_ARRAY,
    S_VALUE_TYPE_BOOLEAN,
    S_VALUE_TYPE_NULL
} S_value_type_t;

typedef struct s_S_value {
    S_value_type_t type;
} S_value_t;

static S_value_t  *S_parse_value(S_ctx *ctx);
static void       S_value_destroy(S_value_t **value);

/* ----------------------------------------------- */

/* -------------------- String -------------------- */

typedef struct {
    S_value_t this_value;
    char      *data;
    size_t    len;
} S_string_t;

static S_string_t *S_string_create(void) {
    S_string_t *str;

    str = malloc(sizeof *str);
    if (str == NULL) {
        return NULL;
    }
    str->len = 0;
    str->data = NULL;
    str->this_value.type = S_VALUE_TYPE_STRING;
    return str;
}

static void S_string_destroy(S_string_t **str) {
    free((*str)->data);
    free(*str);
    *str = NULL;
}

static S_string_t *S_parse_string(S_ctx *ctx) {
    S_string_t *str;
    char       *start;
   
    if (*ctx->ptr != '"') {
        return NULL;
    }
    str = S_string_create();
    if (str == NULL) {
        return NULL;
    }
    start = ctx->ptr + 1;
    while (++ctx->ptr != ctx->end && *ctx->ptr != '"') {
        if (*ctx->ptr != '\\' || *(++ctx->ptr) != 'u') {
            str->len++;
            continue;
        }
        str->len += 4; // 4 unicode chars
    }
    str->data = calloc(1, str->len + 1);
    memcpy(str->data, start, str->len);
    ctx->ptr++;
    return str;
}

/* ------------------------------------------------ */

/* -------------------- Array -------------------- */

typedef struct {
    S_value_t this_value;
    S_value_t **values;
    size_t    num_values;
    size_t    size;
} S_array_t;

static S_array_t *S_array_create(void) {
    S_array_t *arr;

    arr = malloc(sizeof *arr);
    if (arr == NULL) {
        return NULL;
    }
    arr->this_value.type = S_VALUE_TYPE_ARRAY;
    arr->num_values = 0;
    arr->size = 0;
    arr->values = NULL;
    return arr;
}

static void S_array_destroy(S_array_t **arr) {
    size_t i;

    if ((*arr)->values == NULL) {
        (*arr)->num_values = 0;
        *arr = NULL;
        free(*arr);
        return;
    }
    for (i = 0; i < (*arr)->num_values; i++) {
        S_value_destroy(&(*arr)->values[i]);
    }
    (*arr)->num_values = 0;
    *arr = NULL;
    free(*arr);
}

static int S_array_emplace_value(S_array_t *arr, S_value_t *value) {
    S_value_t **temp;

    if (arr->values == NULL) {
        arr->size = 8;
        arr->values = malloc(sizeof *arr->values * arr->size);
        if (arr->values == NULL) {
            return 0;
        }
    }
    if (arr->num_values - 1 >= arr->size) {
        arr->size = arr->size * 2;
        temp = realloc(arr->values, sizeof *arr->values * arr->size);
        if (temp == NULL) {
            return 0;
        }
        arr->values = temp;
    }
    arr->values[arr->num_values++] = value;
    return 1;
}

static S_array_t *S_parse_array(S_ctx *ctx) {
    S_array_t *arr;
    S_value_t *value;
    int       res;

    if (*ctx->ptr != '[') {
        return NULL;
    }
    arr = S_array_create();
    if (arr == NULL) {
        return NULL;
    }
    for (;;) {
        S_skip_whitespace(ctx);
        if (S_skip_over_if_possible(ctx) == 0) {
            S_array_destroy(&arr);
            return NULL;
        }
        value = S_parse_value(ctx);
        if (value == NULL) {
            S_array_destroy(&arr);
            return NULL;
        }
        res = S_array_emplace_value(arr, value);
        if (res == 0) {
            S_array_destroy(&arr);
            return NULL;
        }
        S_skip_whitespace(ctx);
        if (*ctx->ptr == ']') {
            break;
        } else if (*ctx->ptr == ',') {
            continue;
        }
        S_array_destroy(&arr);
        return NULL;
    }
    ctx->ptr++;
    return arr;
}

/* ----------------------------------------------- */

/* -------------------- Number -------------------- */

typedef struct {
    S_value_t this_value;
    double    value;
} S_number_t;

static S_number_t *S_number_create(void) {
    S_number_t *num;

    num = malloc(sizeof *num);
    if (num == NULL) {
        return NULL;
    }
    num->this_value.type = S_VALUE_TYPE_NUMBER;
    num->value = 0.0;
    return num;
}

static void S_number_destroy(S_number_t **num) {
    free(*num);
}

static int S_number_check_if_possible(char c) {
    return S_ISDIGIT(c) || c == '-' || c == 'e' 
        || c == 'E' || c == '.';
}

static S_number_t *S_parse_number(S_ctx *ctx) {
    S_number_t *num;
    char       *start;

    if (!S_number_check_if_possible(*ctx->ptr)) {
        return NULL;
    }
    num = S_number_create();
    if (num == NULL) {
        return NULL;
    }
    start = ctx->ptr;
    while (S_number_check_if_possible(*ctx->ptr)) {
        ctx->ptr++;
    }
    num->value = strtod(start, &ctx->ptr);
    return num;
}

/* ------------------------------------------------ */

/* -------------------- Boolean -------------------- */

typedef struct {
    S_value_t     this_value;
    unsigned char value;
} S_boolean_t;

static S_boolean_t *S_boolean_create(void) {
    S_boolean_t *b;

    b = malloc(sizeof *b);
    if (b == NULL) {
        return NULL;
    }
    b->this_value.type = S_VALUE_TYPE_BOOLEAN;
    b->value = 0;
    return b;
}

static void S_boolean_destroy(S_boolean_t **b) {
    free(*b);
    *b = NULL;
}

static S_boolean_t *S_parse_boolean(S_ctx *ctx) {
    S_boolean_t *b;

    b = S_boolean_create();
    if (b == NULL) {
        return NULL;
    }
    if (*ctx->ptr == 't') {
        if (ctx->ptr + 4 >= ctx->end) {
            S_boolean_destroy(&b);
            return NULL;
        }
        if (strncmp(ctx->ptr, "true", 4) != 0) {
            S_boolean_destroy(&b);
            return NULL;
        }
        ctx->ptr += 4;
        b->value = 1;
    } else if (*ctx->ptr == 'f') {
        if (ctx->ptr + 5 >= ctx->end) {
            S_boolean_destroy(&b);
            return NULL;
        }
        if (strncmp(ctx->ptr, "false", 5) != 0) {
            S_boolean_destroy(&b);
            return NULL;
        }
        ctx->ptr += 5;
        b->value = 0;
    } else {
        S_boolean_destroy(&b);
        return NULL;
    }
    return b;
}

/* ------------------------------------------------- */

/* -------------------- Null -------------------- */

typedef struct {
    S_value_t this_value;
} S_null_t;

static S_null_t *S_null_create(void) {
    S_null_t *n;

    n = malloc(sizeof *n);
    if (n == NULL) {
        return NULL;
    }
    n->this_value.type = S_VALUE_TYPE_NULL;
    return n;
}

static void S_null_destroy(S_null_t **n) {
    free(*n);
    *n = NULL;
}

static S_null_t *S_parse_null(S_ctx *ctx) {
    S_null_t *n;

    if (*ctx->ptr != 'n') {
        return NULL;
    }
    n = S_null_create();
    if (n == NULL) {
        return NULL;
    }
    if (strncmp(ctx->ptr, "null", 4) != 0) {
        S_null_destroy(&n);
        return NULL;
    }
    ctx->ptr += 4;
    return n;
}

/* -------------------- Object -------------------- */

typedef struct s_S_object_entry {
    S_value_t               this_value;
    S_string_t              *name;
    S_value_t               *value;
    struct s_S_object_entry *next;
} S_object_entry_t;

static S_object_t S_object_create(void) {
    S_object_t obj;

    obj = malloc(sizeof *obj);
    if (obj == NULL) {
        return NULL;
    }
    obj->name = NULL;
    obj->value = NULL;
    obj->next = NULL;
    obj->this_value.type = S_VALUE_TYPE_OBJECT;
    return obj;
}

static void S_object_destroy(S_object_t *obj) {
    if ((*obj)->name != NULL) {
        S_string_destroy(&(*obj)->name);
    }
    free(*obj);
    *obj = NULL;
}

void S_object_destroy_recursively(S_object_t *obj) {
    // TODO: implement
}

static S_object_t S_parse_object(S_ctx *ctx) {
    S_object_t obj;

    obj = S_object_create();
    if (obj == NULL) {
        return NULL;
    }
    if (S_skip_over_if_possible(ctx) == 0) {
        S_object_destroy(&obj);
        return NULL;
    }
    obj->name = S_parse_string(ctx);
    if (obj->name == NULL) {
        S_object_destroy(&obj);
        return NULL;
    }
    S_skip_whitespace(ctx);
    if (*ctx->ptr != ':') {
        S_object_destroy(&obj);
        return NULL;
    }
    S_skip_whitespace(ctx); // Not sure if necessary?
    if (S_skip_over_if_possible(ctx) == 0) {
        S_object_destroy(&obj);
        return NULL;
    }
    obj->value = S_parse_value(ctx);
    if (obj->value == NULL) {
        S_object_destroy(&obj);
        return NULL;
    }
    S_skip_whitespace(ctx);
    if (*ctx->ptr == ',') {
        obj->next = S_parse_object(ctx);
    } else if (*ctx->ptr != '}') {
        S_object_destroy(&obj);
        return NULL;
    }
    return obj;
}

/* ------------------------------------------------ */

static S_value_t *S_parse_value(S_ctx *ctx) {
    if (*ctx->ptr == '"') {
        return S_parse_string(ctx);
    } else if (*ctx->ptr == '{') {
        return S_parse_object(ctx);
    } else if (*ctx->ptr == '[') {
        return S_parse_array(ctx);
    } else if (S_number_check_if_possible(*ctx->ptr)) {
        return S_parse_number(ctx);
    } else if (*ctx->ptr == 't' || *ctx->ptr == 'f') {
        return S_parse_boolean(ctx);
    } else if (*ctx->ptr == 'n') {
        return S_parse_null(ctx);
    } else {
        return NULL;
    }
}

static void S_value_destroy(S_value_t **value) {
    switch ((*value)->type) {
        case S_VALUE_TYPE_STRING:
            S_string_destroy(value);
            break;
        case S_VALUE_TYPE_OBJECT:
            S_object_destroy(value);
            break;
        case S_VALUE_TYPE_ARRAY:
            S_array_destroy(value);
            break;
        case S_VALUE_TYPE_NUMBER:
            S_number_destroy(value);
            break;
        default:
            free(*value);
            break;
    }
}

S_object_t S_parse(char *data, size_t sz) {
    S_ctx ctx;

    ctx.ptr = data;
    ctx.end = data + sz;
    return S_parse_object(&ctx);
}
