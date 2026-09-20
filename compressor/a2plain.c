/* a2plain.c
 * Ismail & Alina iqadir01; axie04
 * February 20th 2025
 * Purpose:
 * Implement basic 2D array operations using row-major/column-major mappings 
 * (no blocking)functionality for creating/freeing/accessing arrays 
 * (+iteration)
 *
 * Expects:
 * Valid dimensions and array parameters when creating arrays
 * Properly initialized array structures
 */

 #include <string.h>

#include <a2plain.h>
#include "uarray2.h"
#include "a2methods.h"

typedef A2Methods_UArray2 T;

/* create new 2D array
 * initializes array with given width, height, and element size
 *
 * Parameters:
 * int width: number of columns
 * int height: number of rows
 * int size: size of each element in bytes
 *
 * Return: new 2D array instance
 *
 * Expects:
 * valid positive width, height, and size
 */
static A2Methods_UArray2 new(int width, int height, int size)
{
        return UArray2_new(width, height, size);
}

/* create new 2D array ignoring blocksize
 * initializes array as regular 2D array since blocksize isn't used
 *
 * Parameters:
 * int width: number of columns
 * int height: number of rows
 * int size: size of each element in bytes
 * int blocksize: unused parameter
 *
 * Return: new 2D array instance
 *
 * Expects:
 * valid positive width, height, and size
 */
static A2Methods_UArray2 new_with_blocksize(int width, int height, int size,
                                            int blocksize)
{
        (void)blocksize;
        return UArray2_new(width, height, size);
}

/* blocksize for plain arrays
 * always returns 1 since no blocking is used
 *
 * Parameters:
 * T array2: 2D array instance
 *
 * Return: 1
 *
 * Expects:
 * valid array instance
 */
static int blocksize(T array2)
{
        (void)array2;
        return 1;
}

/* free 2D array
 * deallocates memory for array
 *
 * Parameters:
 * T *array2: pointer to 2D array instance
 *
 * Expects:
 * valid pointer to a valid array
 */
static void a2free(T *array2) 
{
        UArray2_free((UArray2_T *) array2);
}

/* get width of 2D array
 * returns number of columns
 *
 * Parameters:
 * T array2: 2D array instance
 *
 * Return: width of the array
 *
 * Expects:
 * valid array instance
 */
static int width(T array2) 
{
        return UArray2_width(array2);
}

/* get height of 2D array
 * returns number of rows
 *
 * Parameters:
 * T array2: 2D array instance
 *
 * Return: height of the array
 *
 * Expects:
 * valid array instance
 */
static int height(T array2)
{
        return UArray2_height(array2);
}

/* get element size
 * returns size of each element in bytes
 *
 * Parameters:
 * T array2: 2D array instance
 *
 * Return: size of an element
 *
 * Expects:
 * valid array instance
 */
static int size(T array2)
{
        return UArray2_size(array2);
}

/* access element in array
 * returns pointer to element at (i, j)
 *
 * Parameters:
 * T array2: 2D array instance
 * int i: column index
 * int j: row index
 *
 * Expects:
 * valid array instance and indices within bounds
 */
static void *at(T array2, int i, int j)
{
        return UArray2_at(array2, i, j);
}

/* map over elements row by row
 * applies a function to each element in row-major order
 *
 * Parameters:
 * A2Methods_UArray2 uarray2: array instance
 * A2Methods_applyfun apply: function to apply
 * void *cl: closure/context for apply function
 *
 * Expects:
 * valid array and apply function
 */
static void map_row_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}

/* map over elements column by column
 * applies a function to each element in column-major order
 *
 * Parameters:
 * A2Methods_UArray2 uarray2: array instance
 * A2Methods_applyfun apply: function to apply
 * void *cl: closure/context for apply function
 *
 * Expects:
 * valid array and apply function
 */
static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}

struct small_closure {
        A2Methods_smallapplyfun *apply; 
        void                    *cl;
};

/* wrapper for small apply function
 * calls apply function for individual elements
 *
 * Parameters:
 * int i, int j: element indices
 * UArray2_T uarray2: array instance
 * void *elem: element pointer
 * void *vcl: closure/context
 *
 * Expects:
 * valid parameters
 */
static void apply_small(int i, int j, UArray2_T uarray2,
                        void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)uarray2;
        cl->apply(elem, cl->cl);
}

/* small map row-major
 * applies small function row by row
 *
 * Parameters:
 * A2Methods_UArray2 a2: array instance
 * A2Methods_smallapplyfun apply: small apply function
 * void *cl: closure/context
 *
 * Expects:
 * valid array and apply function
 */
static void small_map_row_major(A2Methods_UArray2 a2,
                                A2Methods_smallapplyfun apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}

/* small map column-major
 * applies small function column by column
 *
 * Parameters:
 * A2Methods_UArray2 a2: array instance
 * A2Methods_smallapplyfun apply: small apply function
 * void *cl: closure/context
 *
 * Expects:
 * valid array and apply function
 */
static void small_map_col_major(A2Methods_UArray2 a2,
                                A2Methods_smallapplyfun apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}

static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize,
        a2free,
        width,
        height,
        size,
        blocksize,
        at,
        map_row_major,
        map_col_major,
        NULL,
        map_row_major,
        small_map_row_major,
        small_map_col_major,
        NULL,
        small_map_row_major
};

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;