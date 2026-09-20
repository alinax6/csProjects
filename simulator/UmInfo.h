/**************************************************************
 *
 *                     UmInfo.h
 *
 *     Assignment: um
 *     Authors:    Alina Xie, Qihang Yang
 *     Date:       Apr. 01, 2025
 *
 *     
 *
**************************************************************/

#include <stdlib.h>
#include <seq.h>

/* struct Um_info
 *      
 * This struct holds state of the um machine
 * 
 * uint32_t regs[8]:
 *      represent 8 registers of the um
 * uint32_t **seg_mems:
 *      represent segmented memory of the um. Memory data are malloced, this
 *      array holds pointer to pointers to malloced data.
 * uint32_t prog_counter:
 *      index of current word to read at segment 0
 * uint32_t zero_length:
 *      the length of segment 0 memory, used for ending program loop
 * Seq_T idle_segs:
 *      Unused segment id that was mapped before
 * uint32_t seg_idx:
 *      The current maximum index of mapped segmented memory. Increase when 
 *      all segments are in use, then map new segment
 * uint32_t seg_arrsize:
 *      The current capacity of the array holding address to segmented memory;
 *      when seg_idx is reaching this, expand the seg_mems array
 * uint32_t ra, rb, rc, value:
 *      temporary data to be used by instructions. Only some of these
 *      will be used by an instruction
 *      
*/
typedef struct Um_info {
        uint32_t regs[8];
        uint32_t **seg_mems;
        uint32_t prog_counter;
        uint32_t zero_length;
        Seq_T idle_segs;
        uint32_t seg_idx, seg_arrsize;
        uint32_t ra, rb, rc, value;
} Um_info;