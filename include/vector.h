#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stdlib.h> // ignore warning, it's wrong
#include <stdio.h>

// Layout: [ptr, size, size, float]
// Size: 28(32), 16
#define DECLARE_VECTOR(vector_name, func_prefix, type)\
    typedef struct vector_name {\
        size_t count;\
        size_t size;\
        type* data;\
        float realloc_ratio;\
    } vector_name;\
\
    bool func_prefix##_init(vector_name* arr, size_t size);\
    bool func_prefix##_free(vector_name* arr);\
    bool func_prefix##_add(vector_name* arr, type item);\
    bool func_prefix##_realloc(vector_name* arr);\
    bool func_prefix##_realloc_set(vector_name* arr, size_t newSize);\
    bool func_prefix##_pop_swap(vector_name* arr, size_t index);\


#define DEFINE_VECTOR(vector_name, func_prefix, type)\
    bool func_prefix##_init(vector_name* arr, size_t size) {\
        arr->count = 0;\
        arr->size = size;\
        arr->realloc_ratio = 2.0f;\
        arr->data = (type*) calloc(arr->size, sizeof(type));\
		return arr->data != NULL;\
    }\
\
    bool func_prefix##_free(vector_name* arr) {\
		if (arr->data != NULL) free(arr->data);\
        arr->data = NULL;\
        arr->count = 0;\
        arr->size = 0;\
        arr->realloc_ratio = 0;\
		return arr->data == NULL;\
    }\
\
    bool func_prefix##_realloc(vector_name* arr) {\
        arr->size *= arr->realloc_ratio;\
        type* reallocated = (type*) realloc(arr->data, arr->size * sizeof(type));\
        if (reallocated == NULL) {\
            printf("\n>>> Reallocation Failure <<<\n");\
            return false;\
        }\
        arr->data = reallocated;\
        return true;\
    }\
\
    bool func_prefix##_realloc_set(vector_name* arr, size_t newSize) {\
        arr->size = newSize;\
        type* reallocated = (type*) realloc(arr->data, arr->size * sizeof(type));\
        if (reallocated == NULL) {\
            printf("\n>>> Reallocation Failure <<<\n");\
            return false;\
        }\
        arr->data = reallocated;\
        return true;\
    }\
\
    bool func_prefix##_add(vector_name* arr, type item) {\
        arr->count++;\
        if (arr->count > arr->size) {\
            if(!func_prefix##_realloc(arr)) {\
                printf("\n>>> Add Failure, due to Reallocation Failure. <<<\n");\
                return false;\
            }\
        }\
        arr->data[arr->count - 1] = item;\
        return true;\
    }\
\
    bool func_prefix##_pop_swap(vector_name* arr, size_t index) {\
        if (index >= arr->count) {\
            printf("\n>>> Pop-Swap Failure <<<\n...Index out of Bounds...");\
            return false;\
        }\
\
        size_t lastIndex = arr->count - 1;\
        type swap = arr->data[lastIndex];\
\
        if (index != lastIndex) {\
            arr->data[index] = swap;\
        }\
        arr->count--;\
		return true;\
    }\


// #define DECLARE_VECTOR_PTR(vector_name, func_prefix, type_ptr)\
//     typedef struct vector_name {\
//         type_ptr* data;\
//         size_t count;\
//         size_t size;\
//         float realloc_ratio;\
//     } vector_name;\
// \
//     void func_prefix##_init(vector_name* arr);\
//     void func_prefix##_free(vector_name* arr);\
//     int func_prefix##_add(vector_name* arr, type_ptr item);\
//     int func_prefix##_realloc(vector_name* arr);\
//     int func_prefix##_realloc_set(vector_name* arr, size_t newSize);\
//     type_ptr func_prefix##_pop_swap(vector_name* arr, int index);\
//
//
// #define DEFINE_VECTOR_PTR(vector_name, func_prefix, type_ptr)\
//     void func_prefix##_init(vector_name* arr) {\
//         arr->count = 0;\
//         arr->size = 1;\
//         arr->realloc_ratio = 2.0f;\
//         arr->data = (type_ptr*) calloc(arr->size, sizeof(type_ptr));\
//         if (arr->data == NULL) {\
//             printf("\n>>> Initial Allocation of Vector Failed <<<\n");\
//         }\
//     }\
// \
//     void func_prefix##_free(vector_name* arr) {\
//         for (size_t i = 0; i < arr->count; i++) {\
//             free(arr->data[i]);\
//         }\
//         free(arr->data);\
//         arr->data = NULL;\
//         arr->count = 0;\
//         arr->size = 0;\
//         arr->realloc_ratio = 0;\
//     }\
// \
//     int func_prefix##_realloc(vector_name* arr) {\
//         arr->size *= arr->realloc_ratio;\
//         type_ptr* reallocated = (type_ptr*) realloc(arr->data, arr->size * sizeof(type_ptr));\
//         if (reallocated == NULL) {\
//             printf("\n>>> Reallocation Failure <<<\n");\
//             return 0;\
//         }\
//         arr->data = reallocated;\
//         return 1;\
//     }\
// \
//     int func_prefix##_realloc_set(vector_name* arr, size_t newSize) {\
//         arr->size = newSize;\
//         type_ptr* reallocated = (type_ptr*) realloc(arr->data, arr->size * sizeof(type_ptr));\
//         if (reallocated == NULL) {\
//             printf("\n>>> Reallocation Failure <<<\n");\
//             return 0;\
//         }\
//         arr->data = reallocated;\
//         return 1;\
//     }\
// \
//     int func_prefix##_add(vector_name* arr, type_ptr item) {\
//         arr->count++;\
//         if (arr->count > arr->size) {\
//             if(!func_prefix##_realloc(arr)) {\
//                 printf("\n>>> Add Failure, due to Reallocation Failure. <<<\n");\
//                 return 0;\
//             }\
//         }\
//         arr->data[arr->count - 1] = item;\
//         return 1;\
//     }\
// \
//     type_ptr func_prefix##_pop_swap(vector_name* arr, int index) {\
//         if (index < 0 || index >= arr->count) {\
//             printf("\n>>> Pop-Swap Failure <<<\n...Index out of Bounds...");\
//             return NULL;\
//         }\
//         type_ptr popped = arr->data[index];\
//         int last = arr->count - 1;\
//         arr->data[index] = arr->data[last];\
//         arr->data[last] = NULL;\
//         arr->count--;\
//         return popped;\
//     }\
        
#endif // VECTOR_H
