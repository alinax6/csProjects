/* 
 * bitpack.c
 * Authors: Ismail & Alina
 * March 6th 2025
 * Purpose: Provides the bit-packing and bit-unpacking functionality. 
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "bitpack.h"
#include "assert.h"

#define MAX_WIDTH 64
#define MIN_WIDTH 0

Except_T Bitpack_Overflow = {"Bitpack Overflow when packing bits\n"};

/* Bitpack_fitsu 
* Purpose: Check if an unsigned integer n can be stored in width number of bits
* Expects: width should be between 0 and 64
* Parameters:  
* n: unsigned integer to check
* width: the number of bits available for representation
* Return: returns true if n fits within width number of bits, otherwise false
*/
bool Bitpack_fitsu(uint64_t n, unsigned width) 
{
        /*check that width is within a valid range*/
        assert(width <= MAX_WIDTH);
        uint64_t maxVal;

        /*if width is 64, any 64-bit value can fit*/
        if (width == MAX_WIDTH) {
                return true;
        }

        /*if width is 0, no positive number can be stored*/
        if (width == MIN_WIDTH) {
                maxVal = 0;
        } else {
                maxVal = ((uint64_t)1 << width); /*compute 2^width*/
        }

        /*check if n greater than maxVal*/
        if (n >= maxVal || (width == MIN_WIDTH  && n != MIN_WIDTH)) {
                return false;
        } else {
                return true;
        }
}

/* Bitpack_fitss 
* Purpose: Check if an signed integer n can be stored in width number of bits
* Expects: width should be between 0 and 64
* Parameters:  
* n: signed integer to check
* width: the number of bits available for representation
* Return: returns true if n fits within width number of bits, otherwise false
*/
bool Bitpack_fitss( int64_t n, unsigned width) 
{
        assert(width <= MAX_WIDTH);

        if (width == MAX_WIDTH) {
                return true;
        }
        if (width == MIN_WIDTH) {
                return false;
        }

        /*compute max positive value that can be stored in width bits*/
        int64_t maxVal = ((int64_t)1 << (width - 1));

        /*check if n fits within signed range*/
        if (n >= maxVal || n < (maxVal * -1)) {
                return false;
        } else {
                return true;
        }
}

/* Bitpack_getu 
* Purpose: gets an unsigned integer of width bits from a word starting at the
* least significant bit
* Expects: width should be between 0 and 64 and the width added to the least
* significant bit should not be more than 64
* Parameters:  
* word: 64-bit word containing the packed data
* width: the number of bits to extract
* lsb: the starting position of the field
* Return: the extracted unsigned integer, returns 0 if width is 0
*/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb) 
{
        assert(width <= MAX_WIDTH);
        assert((width + lsb) <= MAX_WIDTH);

        /*if width is 0, no bits can be extracted*/
        if (width == MIN_WIDTH) {
                return 0;
        }


        uint64_t mask = ((uint64_t)1 << width) - 1;
        mask = mask << lsb; /*shift maks to start of lsb position*/

        /*apply mask*/
        uint64_t field = (word & mask);

        /*shift the extracted field to the lsb*/
        field = field >> lsb;

        return field;
}

/* Bitpack_gets 
* Purpose: gets an signed integer of width bits from a word starting at the
* least significant bit
* Expects: width should be between 0 and 64 and the width added to the least
* significant bit should not be more than 64
* Parameters:  
* word: 64-bit word containing the packed data
* width: the number of bits to extract
* lsb: the starting position of the field
* Return: the extracted signed integer, returns 0 if width is 0
*/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb) 
{
        assert(width <= MAX_WIDTH);
        assert((width + lsb) <= MAX_WIDTH);

        if (width == MIN_WIDTH) {
                return 0;
        }

        int64_t field;
        /*extract bit field as an unsigned int*/
        uint64_t uField = Bitpack_getu(word, width, lsb);
        /*get the max positive value for a signed int 2^(width-1)-1*/
        uint64_t maxVal = ((uint64_t)1 << (width -1)) - 1;

        /*if extracted value is within max positive range, cast it to int64_t*/
        if (uField <= maxVal) {
                field = (int64_t)uField;
        } else { /*if uField exceeds max, it's negative.*/
                /*convert to signed representation*/
                field = (int64_t)(uField - ((uint64_t)1 << width));
        }

        return field;
}

/* Bitpack_newu 
* Purpose: packs an unsigned integer value into the bit field within a 64-bit
* word
* Expects: width should be between 0 and 64 and the width added to the least
* significant bit should not be more than 64
* Parameters:  
* words: the 64-bit word that will be modified
* width: the number of bits to be allocated
* lsb: starting position of value
* value: the unsigned integer that gets inserted
* Return: a new 64-bit word with value packed into the bit range
*/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                      uint64_t value) 
{
        assert(width <= MAX_WIDTH);
        assert((width + lsb) <= MAX_WIDTH);

        /*check if value fits within the width*/
        if (Bitpack_fitsu(value, width) != true) {
                RAISE(Bitpack_Overflow);
                assert(0);
        }

        /*create a mask to preserve bits outside the target field*/
        uint64_t leftField = ((uint64_t)1 << (64 - width - lsb)) - 1;
        leftField = leftField << (width + lsb);
        uint64_t rightField = ((uint64_t)1 << lsb) - 1;

        /*combine both masks*/ 
        uint64_t mask = leftField + rightField;
        uint64_t field = (word & mask) | (value << lsb);

        return field;
}

/* Bitpack_news 
* Purpose: packs a signed integer value into the bit field within a 64-bit
* word
* Expects: width should be between 0 and 64 and the width added to the least
* significant bit should not be more than 64
* Parameters:  
* words: the 64-bit word that will be modified
* width: the number of bits to be allocated
* lsb: starting position of value
* value: the unsigned integer that gets inserted
* Return: a new 64-bit word with value packed into the bit range
*/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
                      int64_t value) 
{
        assert(width <= MAX_WIDTH);
        assert((width + lsb) <= MAX_WIDTH);

        if (Bitpack_fitss(value, width) != true) {
                printf("\nOverflow: value=%ld, width=%d\n", value, width);
                RAISE(Bitpack_Overflow);
                assert(0);
        }

        /*mask left most bits, shift it into position, mask right most bits*/
        uint64_t leftField = ((uint64_t)1 << (64 - width - lsb)) - 1;
        leftField = leftField << (width + lsb);
        uint64_t rightField = ((uint64_t)1 << lsb) - 1;
        uint64_t mask = leftField + rightField;

        uint64_t shiftedVal = value;
        /*shift left to align with msb and shift back into position*/
        shiftedVal = shiftedVal << ((uint64_t)64 - width);
        shiftedVal = shiftedVal >> ((uint64_t)64 - width - lsb);

        /*preserve bits outside target region and insert value in target pos*/
        uint64_t field = (word & mask);
        field = (field | shiftedVal);

        return field;
}
