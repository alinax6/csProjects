/**************************************************************
 *
 *                     UmInsSet.c
 *
 *     Assignment: um
 *     Authors:    Alina Xie, Qihang Yang
 *     Date:       Apr. 01, 2025
 *
 *     
 *
 **************************************************************/
#include "UmInsSet.c"

/* the instruction set for the um. 
 * 
 * Contains 14 instructions, 13 three register instruction and 1 one register
 * instruction.
 * 
 * Instructions take same argument, an Um_info struct. Then utilize info of it
 * to execute acutal operations.
 * 
 */
um_func *um_instructions;