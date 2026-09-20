/**************************************************************
 *
 *                     MemController.c
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
/********** seg_address **********
 * 
 * Purpose:
 *      retrieves a pointer to the first data word in the segment
 *     
 * Parameters:
 *      Um_info *info: pointer to Um_info
 *      uint32_t id: segment id to retrieve
 *
 * Return:
 *      uint32_t pointer to the segment data not including the size header
 *
 * Expects:
 *      info is not null
 *
 * Effects:
 *      allowed failure if id is invalid, or pointing to unmapped segment
 *      
 *
************************/
static uint32_t *seg_address(Um_info *info, uint32_t id)
{

}

/********** expand_segs **********
 * 
 * Purpose:
 *      expand the segment pointer array when capacity reached
 *      and no segment id is available
 *     
 * Parameters:
 *      Um_info *info: pointer to Um_info struct
 *
 * Return:
 *      None
 *
 * Expects:
 *      info is not NULL; info->seg_idx reached info->seg_arrsize;
 *      info->idle_segs is exhausted
 *
 * Effects:
 *      original *info->seg_mems is freed, now it points to a new array
 *      which doubles the old array's size; seg_arrsize doubled;
 *
************************/
static void expand_segs(Um_info *info)
{

}

/********** map_segment **********
 * 
 * Purpose:
 *      Maps a new segment of the given size into segmented memory
 *
 * Parameters:
 *      Um_info *info: pointer to the Um_info struct
 *
 * Return:
 *      None
 *
 * Expects:
 *      info is not NULL; there is enough resource in the machine to
 *      create the new segment; info->rc should hold new segment size
 *
 * Effects:
 *      allocates a block of memory and stores its pointer in the next 
 *      available index in info->seg_mems[], use idle segment id first,
 *      if no idle id, then map new segment with info->seg_idx and increase
 *      it, call expand_segs when reached capacity;
 *      newly mapped segment id is put in info->rb;
 *      CRE if machine resource not enough to map given size;
 *      
************************/
void map_segment(Um_info *info)
{

}




/********** seg_unmap **********
 * 
 * Purpose:
 *      unmaps and frees a segment with given id in $r[C], 
 *      then save it's id for reuse
 *
 * Parameters: 
 *      Um_info *info: pointer to Um_info struct
 *
 * Return:
 *      None
 *
 * Expects:
 *      info is not NULL; info->rc holds target segment id
 *
 * Effects:
 *      frees allocated memory and marks the segment id as available;
 *      if id is out of range or points to unmapped segment, allowed fail
 *
************************/
void seg_unmap(Um_info *info)
{

}

/********** seg_length **********
 * Purpose: returns the number of words stored in a segment
 *     
 *
 * Parameters:
 *     Um_info *um: pointer to Um_info
 *     uint32_t id: segment id
 *
 * Return: uint32_t representing the number of words in the segment
 *
 * Expects: um is not null, id < 1024, and um->seq_mem[id] is not null
 *
 * Effects: none
 *      
 *
************************/
static uint32_t seg_length(Um_info *um, uint32_t id)
{

}

/********** load_segment **********
 * 
 * Purpose:
 *      duplicate and load segment with id $r[B] to index 0;
 *      set the program counter to $r[C].
 *     
 * Parameters:
 *     Um_info *um: pointer to Um_info struct
 *
 * Return:
 *      None
 *
 * Expects:
 *      info is not null; info->rb holds segment id;
 *      info->rc should hold program counter
 *
 * Effects:
 *      if id is out of range, or pointing to invalid segment
 *      block, allowed fail;
 *      CRE if duplication exhausts machine resource
 *
************************/
void load_segment(Um_info *info)
{

}


