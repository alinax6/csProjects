/* 
 * compression.c
 * Authors: Ismail & Alina iqadir01; axie04
 * March 6th 2025
 * CS40 HW4: arith
 * Purpose: Implements the image compressor. Reads an RGB image, converts it
 * to y, pb, pr color space, applices dct transformation on 2x2 blocks, 
 * quantizes and packs data into codewords.
*/

#include "compression.h"

/*bit width for pr_index, pb_index, d, c, b, a*/
unsigned quantBitwidthArray[] = {4, 4, 6, 6, 6, 6};

/* readPpmImage 
* Purpose: reads a ppm image from an input file and trimes the dimensions so 
* that they are even
* Expects: a valid input file pointer and a valid A2Methods_T methods structure
* for handling 2d arrays
* Parameters:  
* *input: file pointer to the ppm image file
* methods: methods structure for handling 2d arrays
* Return: a Pnm_ppm structure containing the modified image with even 
* dimensions. If the image is too small (width or height is 0), the function
* exits with an error message.
*/
extern Pnm_ppm readPpmImage(FILE *input, A2Methods_T methods) 
{
        if (!input) {
            fprintf(stderr, "Error: Invalid input file\n");
            exit(EXIT_FAILURE);
        }
        unsigned width;
        unsigned height;
        Pnm_ppm image = Pnm_ppmread(input, methods);

        /*ensures width and height are even by removing the last pixel if odd*/
        width = (image->width) - ((image->width) & 1);
        height = (image->height) - ((image->height) & 1);

        /*checks if image is too small*/
        if (width < 2 || height < 2) {
                fprintf(stderr, "image is too small to compress\n");
                Pnm_ppmfree(&image);
                exit(EXIT_FAILURE);
        }

        A2Methods_UArray2 newImage = methods->new_with_blocksize(width, height,
                                                    sizeof(struct Pnm_rgb), 2);
        if (!newImage) {
            fprintf(stderr, "memory allocation for new image failed.\n");
            Pnm_ppmfree(&image);
            exit(EXIT_FAILURE);
        }

        /*copy pixel data from original image to the new image*/
        for (unsigned row = 0; row < height; row++) {
            for (unsigned col = 0; col < width; col++) {
                    Pnm_rgb newPixel = methods->at(newImage, col, row);
                    Pnm_rgb oldPixel = methods->at(image->pixels, col, row);

                    *newPixel = *oldPixel;
            }
        }
        
        if (image->pixels) {
                methods->free(&(image->pixels));
        }

        image->pixels = newImage;
        image->width  = width;
        image->height = height;
        return image;
}

/* roundRange 
* Purpose: ensures that a float number is within a specified range by clamping
* the values outside of the high and low bounds
* Expects: a valid float number and valid upper and lower bounds
* Parameters: 
* num: the number to be clamped within the range
* high: upper bound of the range
* low: lower bound of the range 
* Return: 
*/
float roundRange(float num, float high, float low) 
{
        if (num < low) {
                num = low;
        } else if (num > high) {
                num = high;
        }

        return num;
}

/* rgbToCvc 
* Purpose: converts a rgb image into y, pb, pr color space respresentation
* Expects: a valid Pnm_ppm rgb image and a valid A2Methods_T object for 
* array operations
* Parameters:  
* rgbImage: input rgb image in Pnm_ppm format
* methods: instance of A2Methods_T for accessing and manipulating the 2d array
* Return: a new A2Methods_UArray2 2d array containing y, pb, pr color space
* representations of the input rgb image
*/
extern A2Methods_UArray2 rgbToCvc(Pnm_ppm rgbImage, A2Methods_T methods) 
{
        assert(rgbImage && methods);

        int width = rgbImage->width;
        int height = rgbImage->height;

        /*create a new y, pb, pr image*/
        A2Methods_UArray2 cvcImage = methods->new_with_blocksize(width, height, 
                                                    sizeof(struct Pnm_ybr), 2);
        assert(cvcImage);

        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                    /*get yPbPr pixel and corresponding rgb pixel*/
                    Pnm_ybr ybrPixel = methods->at(cvcImage, col, row);
                    Pnm_rgb rgbPixel = methods->at(rgbImage->pixels, col, row);

                    /*extract red, gren, blue components from rgb pixel*/
                    float r = rgbPixel->red;
                    float g = rgbPixel->green;
                    float b = rgbPixel->blue;

                    /*convert rgb to y, pb, pr*/
                    ybrPixel->y  = (0.299 * r + 0.587 * g + 0.114 * b);
                    ybrPixel->pb = (-0.168736 * r - 0.331264 * g + 0.5 * b);
                    ybrPixel->pr = (0.5 * r - 0.418688 * g - 0.081312 * b);
            }
        }

        return cvcImage;
}

/* cvcToDCTCoefficients 
* Purpose: converts an image in y, pb, pr color spaceto DCT coefficients
* Expects: a valid A2Methods_UArray2 2d array of y, pb, pr pixel values and 
* a valid A2Methods_T object for array operations
* Parameters:  
* cvcImage: 2d array containing y, pb, pr pixel data
* methods: instance of A2Methods_T for accessing and manipulating the 2d array
* Return: a compressed A2Methods_UArray2 2d array where each element is a 
* DCTBlock containing DCT-transformed coefficients
*/
extern A2Methods_UArray2 cvcToDCTCoefficients(A2Methods_UArray2 cvcImage, 
                                              A2Methods_T methods) 
{
        int width = methods->width(cvcImage) / 2;
        int height = methods->height(cvcImage) / 2;
        int denominator = 255;

        /*create a new 2d array for storing compressed DCT coefficient blocks*/
        A2Methods_UArray2 compressedImage = methods->new_with_blocksize(width, 
                                           height, sizeof(struct DCTBlock), 2);
        assert(compressedImage); 

        /*iterate over each 2x2 block*/
        for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                        int colIndex = 2 * col;
                        int rowIndex = 2 * row;

                        /*retrieve  4 y, pb, pr pixels to form a 2x2 block*/
                        Pnm_ybr pixel1 = methods->at(cvcImage, colIndex, 
                                                     rowIndex);
                        Pnm_ybr pixel2 = methods->at(cvcImage, colIndex + 1, 
                                                     rowIndex);
                        Pnm_ybr pixel3 = methods->at(cvcImage, colIndex, 
                                                     rowIndex + 1);
                        Pnm_ybr pixel4 = methods->at(cvcImage, colIndex + 1, 
                                                    rowIndex + 1);

                        DCTBlock blockElems = methods->at(compressedImage, col,
                                                          row);

                        /*compute average chroma values*/
                        float pbAvg = (pixel4->pb + pixel3->pb + pixel2->pb + 
                                    pixel1->pb) / 4.0;
                        float prAvg = (pixel4->pr + pixel3->pr + pixel2->pr + 
                                    pixel1->pr) / 4.0;
                        /*clamp chroma values to -0.5-0.5*/
                        pbAvg = roundRange(pbAvg/denominator, 0.5, -0.5);
                        prAvg = roundRange(prAvg/denominator, 0.5, -0.5);

                        /*quantize chroma values*/
                        blockElems->pb = 
                        (uint64_t)(Arith40_index_of_chroma(pbAvg));
                        blockElems->pr = 
                        (uint64_t)(Arith40_index_of_chroma(prAvg));

                        /*compute dct coefficients*/
                        float a = (pixel4->y + pixel3->y + pixel2->y + 
                                pixel1->y) / 4.0;
                        float b = (pixel4->y + pixel3->y - pixel2->y - 
                                pixel1->y) / 4.0;
                        float c = (pixel4->y - pixel3->y + pixel2->y - 
                                pixel1->y) / 4.0;
                        float d = (pixel4->y - pixel3->y - pixel2->y + 
                                pixel1->y) / 4.0;
                    
                        /*clamp and scale the a coefficient*/
                        a = roundRange(a/denominator, 1.0, 0);                     

                        float aScale = getQuantMultiplier(A_MIN_RANGE, 
                                                          A_MAX_RANGE, 0, 
                                                       (((int)pow(2, 6)) - 1));
                        blockElems->a = (uint64_t)(round(a * aScale));
                        
                        /*clamp and scale b, c, d to fit 5-bit range*/
                        b = roundRange(b/denominator, BCD_RANGE_HIGH, 
                                       BCD_RANGE_LOW);
                        c = roundRange(c/denominator, BCD_RANGE_HIGH, 
                                       BCD_RANGE_LOW);
                        d = roundRange(d/denominator, BCD_RANGE_HIGH, 
                                       BCD_RANGE_LOW);

                        float scale = getQuantMultiplier(BCD_RANGE_LOW, 
                                                        BCD_RANGE_HIGH, 
                                                        BCD_QUANT_LOW, 
                                                        BCD_QUANT_HIGH);
                        blockElems->b = (int64_t)(round(b * scale));
                        blockElems->c = (int64_t)(round(c * scale));
                        blockElems->d = (int64_t)(round(d * scale));           
                }
        }

        return compressedImage;
}

/* getQuantMultiplier 
* Purpose: computes scaling factor for mapping a float range to a quantized
* integer range
* Expects: a valid floating point range and a valid quantized range
* Parameters:  
* min: lower bound of the floating point range
* max: upper bound of the floating point range
* qMin: lower bound of qunatized range
* qMax: upper bound of qunatized range
* Return: float multiplier that is used map a value proportionally to the range
*/
extern float getQuantMultiplier(float min, float max, int qMin, int qMax) 
{
        return (float)(qMax - qMin) / (max - min);
}

/* packCodewords 
* Purpose: converts a 2d array of DCTBlock structures into a 2d array of 64-bit
* packed codewords, storing the quantized dct coeeficies and chroma values
* Expects: a valid 2d array of DCTBlock elements and a valid A2Methods_T 
* object for array operations
* Parameters:  
* compressedImage: 2d array containing DCTBlock structures where each block
* represents the transformed image data
* methods: instance of A2Methods_T for accessing and manipulating the 2d array
* Return: an A2Methods_UArray2 2d arraycontaining 64-bit code words
*/
extern A2Methods_UArray2 packCodewords(A2Methods_UArray2 compressedImage, 
                                      A2Methods_T methods) 
{
        assert(compressedImage && methods);
        int width = methods->width(compressedImage);
        int height = methods->height(compressedImage);

        /*create new 2d array for storing packed codewords*/
        A2Methods_UArray2 codeword = methods->new_with_blocksize(width, height, 
                                                          sizeof(uint64_t), 2);
        assert(codeword);

        /*iterate over the entire compressed image*/
        for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                        /*retrieve DCTBlock and get corresponding codeword*/
                        DCTBlock wordBlock = methods->at(compressedImage, col, 
                                                         row);
                        uint64_t *newWordBlock = methods->at(codeword, col, 
                                                             row);
                        /*pack the codeword block*/
                        *newWordBlock = pack(wordBlock);
                }
        }

        return codeword;
}

/* outputCodewords 
* Purpose: outputs the packed 64-bit codewords in binary format
* Expects: a valid 2d array containing 64-bit packed codewords and a valid 
* A2Methods_T object for array operations
* Parameters:  
* codewordArr: 2d array containg 64-bitcodewords that were packed from the 
* image data
* methods: instance of A2Methods_T for accessing and manipulating the 2d array
* Return: none
*/
extern void outputCodewords(A2Methods_UArray2 codewordArr,
                            A2Methods_T methods) 
{
        assert(codewordArr && methods);
        int width = (methods->width(codewordArr));
        int height = (methods->height(codewordArr));

        printf("COMP40 Compressed image format 2\n%u %u", width, height);
        printf("\n");

        /*iterate through the compressed image*/
        for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                        uint64_t codeword = 
                        *((uint64_t *)methods->at(codewordArr, col, row));

                        /*convert 64-bit to 32-bit*/
                        uint32_t word32 = (uint32_t)codeword;
                        /*write the codeword in 4 bytes in big-endian order*/   
                        putchar((unsigned char)(word32 >> 24));    
                        putchar((unsigned char)((word32 << 8) >> 24)); 
                        putchar((unsigned char)((word32 << 16) >> 24)); 
                        putchar((unsigned char)((word32 << 24) >> 24)); 
                }       
        }
}

/* pack 
* Purpose: packs a DCTBlock structure into a single 64-bit word
* Expects: a valid pointer to a DCTBlock structure
* Parameters:  
* block: a pointer to a DCTBlock structure conaining quantized coefficients 
* for an image block
* Return: an uint64_t packed word where each field is stored using its 
* corresponding bit width
*/
extern uint64_t pack(DCTBlock block) 
{
        uint64_t packedword = 0;
        unsigned index = 0;
        unsigned offset = 0;

        packedword = Bitpack_newu(packedword, quantBitwidthArray[index], 
                                  offset, block->pr); /*pack pr*/
        /*updates offset to move to next bit position and increments index to 
        point to next element in quantBitwidthArray*/
        offset += quantBitwidthArray[index];
        index++;

        packedword = Bitpack_newu(packedword, quantBitwidthArray[index],
                                  offset, block->pb); /*pack pb*/
        offset += quantBitwidthArray[index];
        index++;

        packedword = Bitpack_news(packedword, quantBitwidthArray[index], 
                                  offset, block->d); /*pack d coefficient*/
        offset += quantBitwidthArray[index];
        index++;

        packedword = Bitpack_news(packedword, quantBitwidthArray[index], 
                                  offset, block->c); /*pack c coefficient*/
        offset += quantBitwidthArray[index];
        index++;

        packedword = Bitpack_news(packedword, quantBitwidthArray[index], 
                                  offset, block->b); /*pack b coefficient*/
        offset += quantBitwidthArray[index];
        index++;

        packedword = Bitpack_newu(packedword, quantBitwidthArray[index], 
                                  offset, block->a); /*pack a coefficient*/
        return packedword;
}
