/**************************************************************
 *
 *                     Loader.c
 *
 *     um
 *     Authors:    Alina Xie, Qihang Yang
 *     Date:       Apr. 01, 2025
 *
 *     
 *
**************************************************************/
#include <stdlib.h>
#include <stdio.h>
/********** load **********
 *
 * Purpose:     
 *      load binary instructions into 0th segmented memory;
 *      return length of instructions
 *
 * Parameters:
 *      FILE *fp: the file pointer to read binary instructions
 *      uint32_t **seg_mem: pointer to an array holding segment memory pointers
 *
 * Return:
 *      uint32_t as the length of instructions
 * 
 * Expects:
 *      fp can't be null, seg_mem should be initialized;
 *      fp stores um binary instruction, size does not exceed machine limit
 *
 * Effects:
 *      CRE if any input null or read data exceeds machine limit;
 *      seg_mem's 0th element will be replaced by a pointer to read
 *      um instruction 32 bit words;
 *      caller assumes ownership of 0th segment memory
 *
************************/

uint32_t load(FILE *fp, uint32_t **seg_mem)
{

}