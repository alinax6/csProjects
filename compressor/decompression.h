/* 
 * decompression.h
 * Authors: Ismail & Alina iqadir01; axie04
 * March 6th 2025
 * CS40 HW4: arith
 * Purpose: Defines the interface for image decompression, including unpacking 
 * bit-packed codewords, inverse DCT transformation, conversion to RGB, 
 * and outputting a decompressed PPM image
*/

#ifndef DECOMPRESSION_INCLUDED
#define DECOMPRESSION_INCLUDED

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "arith40.h"
#include "pnm.h"
#include "a2blocked.h"
#include "a2methods.h"
#include "compression.h"
#include "uarray2b.h"
#include "bitpack.h"
#include "mem.h"
#include "assert.h"

/* Defines range values for DCT coef and quantization */
#define A_MIN_RANGE 0
#define A_MAX_RANGE 1
#define BCD_RANGE_LOW -0.3
#define BCD_RANGE_HIGH 0.3
#define BCD_QUANT_LOW -15
#define BCD_QUANT_HIGH 15

/* Structure to hold information for array manipulation */
struct closure1 {
    A2Methods_UArray2 *array;
    A2Methods_T methods;
    unsigned denominator;
};

/* Converts YPbPr component video data back to RGB format */
extern A2Methods_UArray2 CvcToRgb(A2Methods_UArray2 ybrImage, 
                                  A2Methods_T methods);

/* Converts DCT coefficients back to component video representation */
extern A2Methods_UArray2 dctCoefficientsToCvc(A2Methods_UArray2 
                                              compressedImage, 
                                              A2Methods_T methods);

/* Unpacks bit packed codewords into DCT coef values */
extern A2Methods_UArray2 unpackCodewords(A2Methods_UArray2 codewordArr,
                                         A2Methods_T methods);

/* Unpacks a 64-bit codeword into a DCTBlock structure */
extern void unpack(uint64_t codeword, DCTBlock block);

/* Reads bit-packed codewords from a file */
extern A2Methods_UArray2 readCodewords(FILE *input, A2Methods_T methods);

#endif