/* 
 * compression.h
 * Authors: Ismail & Alina iqadir01; axie04
 * March 6th 2025
 * CS40 HW4: arith
 * Purpose: Defines the interface for image compression, including conversion
 * between color spaces, Discrete Cosine Transform (DCT), quantization,
 * and bit-packing of compressed data into codewords
*/

#ifndef COMPRESSION_INCLUDED
#define COMPRESSION_INCLUDED

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "arith40.h"
#include "pnm.h"
#include "a2blocked.h"
#include "a2methods.h"
#include "uarray2b.h"
#include "bitpack.h"
#include "mem.h"
#include "assert.h"

/* Defines range values for DCT coefficients and quantization */
#define A_MIN_RANGE 0
#define A_MAX_RANGE 1
#define BCD_RANGE_LOW -0.3
#define BCD_RANGE_HIGH 0.3
#define BCD_QUANT_LOW -15
#define BCD_QUANT_HIGH 15

/* Structure to hold information for array manipulation */
struct closure {
    A2Methods_UArray2 *array;
    A2Methods_T methods;
    unsigned denominator;
};

/* Structure representing a pixel in component video format */
typedef struct Pnm_ybr {
    float y;
    float pb;
    float pr;
} *Pnm_ybr;

/* Structure to store DCT coefficients for a 2x2 pixel block */
typedef struct DCTBlock {
    uint64_t a;
    int64_t b;
    int64_t c;
    int64_t d;
    uint64_t pb;
    uint64_t pr;
} *DCTBlock;

/* Array for quantization bitwidth values */
extern unsigned quantBitwidthArray[];

/* Reads a PPM image from a file and converts it to a Pnm_ppm structure */
extern Pnm_ppm readPpmImage(FILE *input, A2Methods_T methods);

/* Rounds a value within a given range */
float roundRange(float num, float high, float low);

/* Converts an RGB image to component video color space (YPbPr) */
extern A2Methods_UArray2 rgbToCvc(Pnm_ppm rgbImage, A2Methods_T methods);

/* Converts component video representation to DCT coefficients */
extern A2Methods_UArray2 cvcToDCTCoefficients(A2Methods_UArray2 cvcImage, 
                                              A2Methods_T methods);

/* Packs DCT coefficients */
extern A2Methods_UArray2 packCodewords(A2Methods_UArray2 compressedImage, 
                                      A2Methods_T methods);

/* Packs a DCTBlock structure into a single 64-bit codeword */
extern uint64_t pack(DCTBlock block);

/* Outputs the compressed codewords */
extern void outputCodewords(A2Methods_UArray2 codewordArr,
                            A2Methods_T methods);

/* Computes the quantization multiplier for value scaling */
extern float getQuantMultiplier(float min, float max, int qMin, int qMax);

#endif