/* 
 * compress40.c
 * Authors: Ismail & Alina
 * March 6th 2025
 * Purpose: Defines the image compression and decompression functions for ppm
 * images. The functions are called in 40image.c use -c for compression and -d
 * for decompression. 
 * compress40 reads a ppm images, converts rgb to yPbPr, applies dct to 
 * converts the pixel values to coefficients, packs the dct coefficients into
 * bit fields, and outputs the compressed data in binary codewords.
 * decompress40 reads the compressed codewords from the input, unpacks the bit
 * fields to retrieve dct coefficents, applies inverse dct, converts yPbPr to 
 * rgb and writes the decompressed ppm image to stdout.
*/

#include "assert.h"
#include "compress40.h"
#include "arith40.h"
#include "a2blocked.h"
#include "uarray2b.h"
#include "compression.h"
#include "decompression.h"
#include "bitpack.h"
#include "pnm.h"
#include <stdlib.h>
#include <stdio.h>

/* compress40 
* Purpose: reads PPM image from input, compressed it using dct, and write the
* compressed image as binary codewords
* Expects: a valid input file
* Parameters:  
* *input: pointer to the input ppm image file
* Return: none
*/
void compress40(FILE *input) 
{
        /*get blocked array methods for handling 2d arrays*/
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods);

        /*read the ppm image from the input file*/
        Pnm_ppm rgbImage = readPpmImage(input, methods);

        /*convert the image from rgb to yPbPr color space*/
        A2Methods_UArray2 cvcImage = rgbToCvc(rgbImage, methods);

        /*apply dct and quantization*/
        A2Methods_UArray2 dctBlockImage = cvcToDCTCoefficients(cvcImage, 
                                                               methods);

        /*pack dct coefficients into 64-bit codewords*/
        A2Methods_UArray2 codewordArr = packCodewords(dctBlockImage, methods);

        /*output compressed codewords in binary format*/
        outputCodewords(codewordArr, methods);

        /*free allocated memory*/
        methods->free(&cvcImage);
        methods->free(&dctBlockImage);
        methods->free(&codewordArr);
        Pnm_ppmfree(&rgbImage);
}

/* decompress40 
* Purpose: reads compressed image from input decompresses it using inverse dct,
* and writes the reconstructed ppm image to stdout
* Expects: a valid input file
* Parameters: 
* *input: pointer to the input file containing the compressed image data
* Return: none
*/
void decompress40(FILE *input) 
{
    /*get blocked array methods for handling 2d arrays*/
    A2Methods_T methods = uarray2_methods_blocked;
    assert(methods);

    /*read compressed codewords from input file*/
    A2Methods_UArray2 codewordArr = readCodewords(input, methods);

    /*unpack the codewords to reconstruct dct coefficients blocks*/
    A2Methods_UArray2 compressedImage = unpackCodewords(codewordArr, methods);

    /*convert dct coefficients to yPbPr color space*/
    A2Methods_UArray2 cvcImage = dctCoefficientsToCvc(compressedImage, 
                                                      methods);

    /*convert yPbPr color space to rgb*/
    Pnm_ppm rgbImage = CvcToRgb(cvcImage, methods);

    /*free allocated memory and output decompressed image*/
    methods->free(&codewordArr);
    methods->free(&compressedImage);
    methods->free(&cvcImage);

    Pnm_ppmwrite(stdout, rgbImage);
    Pnm_ppmfree(&rgbImage);
}