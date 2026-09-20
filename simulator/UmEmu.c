/**************************************************************
 *
 *                     UmEmu.c
 *
 *     Assignment: um
 *     Authors:    Alina Xie, Qihang Yang
 *     Date:       Apr. 01, 2025
 *
 *     
 *
**************************************************************/
#include <stdio.h>
#include "Loader.h"
#include "UmInsSet.h"
#include "InsParser.h"
#include "UmInfo.h"
#include <assert.h>


/**********   **********
 *
 *     
 *
 * Parameters:
 *     
 *
 * Return:
 *
 * Expects:
 *
 * Effects:
 *      
 *
************************/
static void flush_vals(Um_info info)
{
        info.ra = 0;
        info.rb = 0;
        info.rc = 0;
        info.value = 0;
}



/********** run_um_emu  **********
 *
 * Purpose:
 *      Initialize main program loop, translate binary data to um instruction
 *      and execute all
 *
 * Parameters:
 *      FILE *fp: a file pointer for reading binary um instructions
 *     
 * Return:
 *      none
 *
 * Expects:
 *      The file should contains um instruction binary data, first 4 bit as 
 *      opcode, last 9 bit as 3 registers; or 5-7 as one register with 25 bit
 *      value.
 *
 * Effects:
 *      Terminate when program counter reach the end of seg 0;
 *      allow failure modes, specified in the um specs;
 *      CRE if computer resources exhausted, or input binary violate
 *      32 bit format
 *      
 *
************************/
void run_um_emu(FILE *fp)
{
        /* initialize registers & segmented memory */
        uint32_t regs[8];
        Seq_T idle = Seq_new(512);
        Um_info info = { 
                regs,
                NULL,
                0, /* program counter */
                0, /* zero segment's length */
                idle, 
                0, 1024, /* default array to 1024 size */
                0, 0, 0, 0 /* possible values for instructions */
        };
        /* make a default size array for segmented memory pointers */
        info.seg_mems = malloc(info.seg_arrsize, sizeof(uint32_t*));
        
        /* load, 0 index should hold instructions after this;
        and size of instructions is tracked by ins_length */
        info.zero_length = load(fp, info.seg_mems);

        /* instruction set */
        um_func *instructions = um_instructions;

        /* the main loop begin */
        while (info.seg_mems[0] < info.zero_length) {
                int counter = info.prog_counter;
                /* parse instructions */
                int opcode = parse_instruction(&info);
                /* increase program counter */
                info.prog_counter++;
                /* check if binary violate format, then do instruction */
                assert(opcode >= 0 && opcode < 14);
                instructions[opcode](&info);
        }



}




/**********   **********
 *
 *     
 *
 * Parameters:
 *     
 *
 * Return:
 *
 * Expects:
 *
 * Effects:
 *
************************/






/**********   **********
 *
 *     
 *
 * Parameters:
 *     
 *
 * Return:
 *
 * Expects:
 *
 * Effects:
 *
************************/