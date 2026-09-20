/**************************************************************
 *
 *                     InsParser.c
 *
 *     um
 *     Authors:    Alina Xie, Qihang Yang
 *     Date:       Apr. 01, 2025
 *
 *     
 *
**************************************************************/
#include "UmInfo.h"

/********** parse_instruction **********
 *
 * Purpose:
 *      parse one 32 bit word instruction, store parsed values
 *      that will be used for this instruction in Um_info struct;
 *      return ops code for caller.
 *
 * Parameters:
 *      Um_info info: pointer to a Um_info struct
 *
 * Return:
 *      uint32_t representing the extracted opcode
 *
 * Expects:
 *      info should be correctly initialized, with program counter within
 *      length of 0th segmented memory, and segmented memory can't be null;
 *          
 * Effects:
 *      info's ra, rb, rc, value fields might be changed based on instruction
 *      read
 *
************************/
uint32_t parse_instruction(Um_info *info)
{

}

