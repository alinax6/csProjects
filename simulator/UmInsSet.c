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

#include <stdlib.h>
#include "UmInfo.h"

/*********************************************/
/*        Define all 14 instructions         */
/*********************************************/

/* generalize um instruction as um_func */
typedef void (*um_func)(Um_info *info);

/* each of these function correspond to an um instruction, Um_info provide
 * the context they need */
static void con_move(Um_info *info)
{
        /* */
}
static void seg_load(Um_info *info)
{

}
static void seg_store(Um_info *info)
{

}
static void addition(Um_info *info)
{

}
static void multip(Um_info *info)
{

}
static void division(Um_info *info)
{

}
static void bit_nand(Um_info *info)
{

}
static void halt_prog(Um_info *info)
{

}
static void map_seg(Um_info *info)
{

}
static void unmap_seg(Um_info *info)
{

}
static void output_val(Um_info *info)
{

}
static void input_val(Um_info *info)
{

}
static void load_prog(Um_info *info)
{

}

static void load_val(Um_info *info)
{

}

/* the array holding all functions simulating um instructions */
static um_func funcs[14] = {
        con_move, seg_load, seg_store, addition, multip, division,
        bit_nand, halt_prog, map_seg, unmap_seg, output_val, input_val,
        load_prog, load_val
};
/* exported for other modules */
um_func *um_instructions = funcs;

