/* 
 * decompression.c
 * Authors: Ismail & Alina iqadir01; axie04
 * March 6th 2025
 * CS40 HW4: arith
 * Purpose: Implements the image decompressor. It converts an image in a 
 * compressed format (using DCT coefficients and y, pb, pr color space) to 
 * a viewable rgb image.
*/

#include "decompression.h"

/* CvcToRgb 
* Purpose: converts an image from y, pb, pr color space to rgb color space
* Expects: a valid input image stored in y, pb, pr format and  valid 
* A2Methods_T methods object for array operations
* Parameters:  
* ybrImage: A2Methods_UArray2 2d array representing the y, pb, pr image
* methods: instance of A2Methods_T for accessing and manipulating the 2d array
* Return: a Pnm_ppm image converted from y, pb, pr to rgb color space
*/
extern A2Methods_UArray2 CvcToRgb(A2Methods_UArray2 ybrImage, 
                                  A2Methods_T methods) 
{
        assert(ybrImage && methods);
        int width = methods->width(ybrImage);
        int height = methods->height(ybrImage);
        unsigned denominator = 255;

        /*allocate memory for the new rgb image*/
        Pnm_ppm rgbImage;
        NEW(rgbImage);
        assert(rgbImage);

        /*intialize the rgb image structure*/
        rgbImage->width = width; 
        rgbImage->height = height;
        rgbImage->denominator = denominator;
        rgbImage->methods = methods;
        rgbImage->pixels = methods->new_with_blocksize(width, height, 
                                                    sizeof(struct Pnm_rgb), 2);
        if (!rgbImage->pixels) {
            fprintf(stderr, "Failled to allocate memory for RGB pixels\n");
            Pnm_ppmfree(&rgbImage);
            exit(EXIT_FAILURE);
        }
        
        for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                        /*get YPbPr pixels and corresponding rgb pixel pos*/
                        Pnm_ybr ybrPixel = methods->at(ybrImage, col, row);
                        Pnm_rgb rgbPixel = methods->at(rgbImage->pixels, col, 
                                                       row);

                        /*convert y, pb, pr to rgb*/
                        float r = (ybrPixel->y + 1.402 * ybrPixel->pr) 
                                * denominator;
                        float g = (ybrPixel->y - 0.344136 * ybrPixel->pb - 
                                0.714136 * ybrPixel->pr) * denominator;
                        float b = (ybrPixel->y + 1.772 * ybrPixel->pb) 
                                * denominator;

                        /*ensure rgb values are within valid range of 0-255*/
                        r = roundRange(r, denominator, 0);
                        g = roundRange(g, denominator, 0);
                        b = roundRange(b, denominator, 0);

                        rgbPixel->red = (unsigned)(r);
                        rgbPixel->green = (unsigned)(g);
                        rgbPixel->blue = (unsigned)(b);
                }
        }

        return rgbImage;
}

/* dctCoefficientsToCvc 
* Purpose: converts compressed image stored in dct coefficients to y, pb, pr
* color space
* Expects: valid compressed image in dct format and methods for accessing
* image data
* Parameters:  
* compressedImage: A2Methods_Uarray2 structure containing dct coefficients
* methods: instance of A2Methods_T for accessing and manipulating the 2d array
* Return: 2d array containing the reconstructed y, pb, pr color space values
*/
extern A2Methods_UArray2 dctCoefficientsToCvc(A2Methods_UArray2 
                                              compressedImage,
                                              A2Methods_T methods) 
{
        assert(compressedImage && methods);
        /*the image was compressed in 2x2 blocks, so double width and hiehgt*/
        int width = methods->width(compressedImage) * 2;
        int height = methods->height(compressedImage) * 2;

        /*allocate memory for the output image in yPbPr format*/
        A2Methods_UArray2 cvcImage = methods->new_with_blocksize(width, height, 
                                                    sizeof(struct Pnm_ybr), 2);
        assert(cvcImage);

        /*iterate through each compressed 2x2 block*/
        for (int row = 0; row < methods->height(compressedImage); row++) {
                for (int col = 0; col < methods->width(compressedImage); 
                     col++) {
                        DCTBlock blockElems = methods->at(compressedImage, 
                                                          col, row);

                        /*convert packed pb and pr to float chroma values*/
                        float pbAvg = Arith40_chroma_of_index(blockElems->pb);
                        float prAvg = Arith40_chroma_of_index(blockElems->pr);
                    
                        float aScale = getQuantMultiplier(A_MIN_RANGE, 
                                                          A_MAX_RANGE, 0, 
                                                       (((int)pow(2, 6)) - 1));
                        float scale = getQuantMultiplier(BCD_RANGE_LOW, 
                                                 BCD_RANGE_HIGH, BCD_QUANT_LOW, 
                                                 BCD_QUANT_HIGH);
                        /*convert quantized a, b, c, d to  float values*/
                        float a = (blockElems->a * 1.0) / aScale;
                        float b = (blockElems->b * 1.0) / scale;
                        float c = (blockElems->c * 1.0) / scale;
                        float d = (blockElems->d * 1.0) / scale;

                        Pnm_ybr pixel1 = methods->at(cvcImage, 2 * col, 
                                                     2 * row);
                        Pnm_ybr pixel2 = methods->at(cvcImage, 2 * col + 1, 
                                                     2 * row);
                        Pnm_ybr pixel3 = methods->at(cvcImage, 2 * col, 2 * 
                                                     row + 1);
                        Pnm_ybr pixel4 = methods->at(cvcImage, 2 * col + 1, 2 * 
                                                     row + 1);

                        /*assign pb and pr averages to all 4 pixels*/
                        pixel1->pb = pbAvg;
                        pixel2->pb = pbAvg;
                        pixel3->pb = pbAvg;
                        pixel4->pb = pbAvg;

                        pixel1->pr = prAvg;
                        pixel2->pr = prAvg;
                        pixel3->pr = prAvg;
                        pixel4->pr = prAvg;

                        /*calculate y values for the 4 pixels*/
                        pixel1->y = roundRange(a - b - c + d, A_MAX_RANGE, 
                                               A_MIN_RANGE);
                        pixel2->y = roundRange(a - b + c - d, A_MAX_RANGE,
                                               A_MIN_RANGE);
                        pixel3->y = roundRange(a + b - c - d, A_MAX_RANGE,
                                               A_MIN_RANGE);
                        pixel4->y = roundRange(a + b + c + d, A_MAX_RANGE,
                                               A_MIN_RANGE);
                }
        }

        return cvcImage;
}

/* unpackCodewords 
* Purpose: converts an array of packed codewords into an array of dct 
* coefficient blocks
* Expects: valid A2Methods_UArray2 array containing packed codewords and valid
* methods for accessing image data
* Parameters:  
* codewordArr: 2d array of uint64_t values representing a pack DCTBlock
* methods: instance of A2Methods_T for accessing and manipulating the 2d array
* Return: a new A2Methods_UArray2 containig DCTBlock structures, where each
* block has unpacked values representing the compressed image
*/
extern A2Methods_UArray2 unpackCodewords(A2Methods_UArray2 codewordArr,
                                        A2Methods_T methods) 
{
        int width = methods->width(codewordArr);
        int height = methods->height(codewordArr);

        /*allocate memory for array with DCT coefficient blocks*/
        A2Methods_UArray2 compressedImage = methods->new_with_blocksize(width, 
                                           height, sizeof(struct DCTBlock), 2);
        assert(compressedImage);

        for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                        /*get packed codeword from the array*/
                        uint64_t codeword = 
                        *((uint64_t *)methods->at(codewordArr, col, row));
                        /*get corresponding DCTBlock structure*/
                        DCTBlock wordBlock = methods->at(compressedImage, col,
                                                         row);
                        unpack(codeword, wordBlock);
                }
        }

        return compressedImage;
}

/* unpack 
* Purpose: extracts dct coefficients from a packed codework and stores them
* in a DCTBlock structure
* Expects: a valid 64-bit codeword containing encoded dct coefficients
* a valid pointer to a DCTBlock structure
* Parameters:  
* codeword: a 64-bit unsigned int containing packed image data
* block: pointer to a DCTBlock structure where the unpacked coefficients are
* stored
* Return: none
*/
extern void unpack(uint64_t codeword, DCTBlock block) 
{
        unsigned index = 0;
        unsigned offset = 0;
        /*extract pr*/
        block->pr = Bitpack_getu(codeword, quantBitwidthArray[index], offset);
        offset += quantBitwidthArray[index];
        index++;

        /*extract pb*/
        block->pb = Bitpack_getu(codeword, quantBitwidthArray[index], offset);
        offset += quantBitwidthArray[index];
        index++;

        /*extract d coefficient*/
        block->d = Bitpack_gets(codeword, quantBitwidthArray[index], offset);
        offset += quantBitwidthArray[index];
        index++;

        /*extract c coefficient*/
        block->c = Bitpack_gets(codeword, quantBitwidthArray[index], offset);
        offset += quantBitwidthArray[index];
        index++;

        /*extract b coefficient*/
        block->b = Bitpack_gets(codeword, quantBitwidthArray[index], offset);
        offset += quantBitwidthArray[index];
        index++;

        /*extract a coefficient*/
        block->a = Bitpack_getu(codeword, quantBitwidthArray[index], offset);
}

/* readCodewords 
* Purpose: read compressed image data from a file and stores it in a 2d array
* of codewords representing compressed pixel blocks
* Expects: valid file pointer to the input file and valid methods for 
* accessing image data
* Parameters:  
* *input: file pointer to the read in compressed data
* methods: instance of A2Methods_T for accessing and manipulating the 2d array
* Return: an A2Methods_UArray2 2d array containing the read in codewords
*/
extern A2Methods_UArray2 readCodewords(FILE *input, A2Methods_T methods) 
{
        assert(input);
        assert(methods);
        unsigned width, height;

        /*read the image header*/
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u", 
                          &width, &height);
        assert(read == 2);

        /*checks that a newline is present*/
        int c = getc(input);
        assert(c == '\n');

        /*allocate memory for storing the compressed codewords*/
        A2Methods_UArray2 compressedImage = methods->new_with_blocksize(width, 
                                                  height, sizeof(uint64_t), 2);
        assert(compressedImage);

        for (unsigned row = 0; row < height; row++) {
                for (unsigned col = 0; col < width; col++) {
                        uint32_t word32 = 0;
                        /*read 4 bytes from file to reconstruct 32-bit word*/
                        int byte1 = getc(input);
                        int byte2 = getc(input);
                        int byte3 = getc(input);
                        int byte4 = getc(input);

                        /*checks if bytes are EOF*/
                        if (byte1 == EOF || byte2 == EOF || byte3 == EOF || 
                            byte4 == EOF) {
                            fprintf(stderr, "EOF when reading codewords\n");
                            methods->free(&compressedImage);
                            exit(EXIT_FAILURE);
                        }

                        /*shifts byte1 to place it in the msb and so on*/
                        word32 |= ((uint32_t)(unsigned char)byte1 << 24);
                        word32 |= ((uint32_t)(unsigned char)byte2 << 16);
                        word32 |= ((uint32_t)(unsigned char)byte3 << 8);
                        word32 |= ((uint32_t)(unsigned char)byte4);

                        /*store 32-bit word as 64-bit value in the array*/
                        *((uint64_t *)methods->at(compressedImage, col, row)) = 
                        (uint64_t)word32;
                }
        }

        return compressedImage;
}
