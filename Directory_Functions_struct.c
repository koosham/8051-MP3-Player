#include "AT89C51RC2.h"
#include <stdio.h>
#include "main.h"
#include "port.h"
#include "uart.h"
#include "spi.h"
#include "sdcard.h"
//#include "print_bytes.h"
#include "read_sector.h"
#include "Directory_Functions_struct.h"

static FS_values_t idata Drive_Values;

/***********************************************************************
DESC: Returns a pointer to the global structure Drive_Values to export to other files
INPUT: void
RETURNS: Pointer to the structure Drive_Values
CAUTION: 
************************************************************************/
FS_values_t * Export_Drive_Values(void)
{
   return &Drive_Values;
}

/***********************************************************************
DESC: Returns an 8-bit value from given array at specified offset
INPUT: offset (in byte) from the first byte of the array and a pointer to the array to read from
RETURNS: 8-bit value
CAUTION: 
************************************************************************/
uint8_t Read8(uint16_t offset, uint8_t xdata * array)
{
	uint8_t return_value = 0;
	
	offset %= 0x200; // if greater than 511, replace remainder of the division of offset and 512
	return_value = *(array + offset);
	return return_value;
}

/***********************************************************************
DESC: Returns an 16-bit value from given array at specified offset
INPUT: offset (in byte) from the first byte of the array and a pointer to the array to read from
RETURNS: 16-bit value
CAUTION: 
************************************************************************/
uint16_t Read16(uint16_t offset, uint8_t xdata * array)
{
	uint16_t return_value = 0;
	uint8_t temp, index;
	
	offset %= 0x200; // if greater than 511, replace remainder of the division of offset and 512
	for (index = 0 ; index < 2 ; index++)
	{
		temp = *(array + offset + (1 - index));
		return_value = return_value << 8;
		return_value |= temp;
	}
	return return_value;
}

/***********************************************************************
DESC: Returns an 32-bit value from given array at specified offset
INPUT: offset (in byte) from the first byte of the array and a pointer to the array to read from
RETURNS: 32-bit value
CAUTION: 
************************************************************************/
uint32_t Read32(uint16_t offset, uint8_t xdata * array)
{
	uint32_t return_value = 0;
	uint8_t temp, index;
	
	offset %= 0x200; // if greater than 511, replace remainder of the division of offset and 512
	for (index = 0 ; index < 4 ; index++)
	{
		temp = *(array + offset + (3 - index));
		return_value = return_value << 8;
		return_value |= temp;
	}
	return return_value;
}

/***********************************************************************
DESC: Prints all short file name entries for a given directory 
INPUT: Starting Sector of the directory and the pointer to a 
block of memory in xdata that can be used to read blocks from the SD card
RETURNS: uint16_t number of entries found in the directory
CAUTION: Supports FAT16, SD_shift must be set before using this function
************************************************************************/
uint16_t Print_Directory(uint32_t Sector_num, uint8_t xdata * array_in)
{ 
	uint32_t Sector, max_sectors;
  uint16_t i, entries;
  uint8_t temp8, j, attr, out_val, error_flag;
  uint8_t * values;
	
  values=array_in;
  entries=0;
  i=0;
  if (Drive_Values.FATtype==FAT16)  // included for FAT16 compatibility
  { 
		max_sectors=Drive_Values.RootDirSecs;   // maximum sectors in a FAT16 root directory
  }
  else
  {
    max_sectors=Drive_Values.SecPerClus;
  }
  Sector=Sector_num;
  error_flag=Read_Sector(Sector,Drive_Values.BytesPerSec,values);
  if(error_flag==NO_ERROR)
  {
		do
    {
      temp8=Read8(0+i,values);  // read first byte to see if empty
      if((temp8!=0xE5)&&(temp8!=0x00))
	    {  
				attr=Read8(0x0b+i,values);
				if((attr&0x0E)==0)   // if hidden, system or Vol_ID bit is set do not print
				{
		      entries++;
					printf("%5d. ",entries);  // print entry number with a fixed width specifier
		      for(j=0;j<8;j++)
					{
						out_val=Read8(i+j,values);   // print the 8 byte name
						putchar(out_val);
					}
          if((attr&0x10)==0x10)  // indicates directory
					{
						for(j=8;j<11;j++)
						{
			        out_val=Read8(i+j,values);
			        putchar(out_val);
						}
						printf("[DIR]\n");
					}
					else       // print a period and the three byte extension for a file
					{
						putchar(0x2E);       
						for(j=8;j<11;j++)
						{
			        out_val=Read8(i+j,values);
			        putchar(out_val);
						}
						putchar(0x0d);
						putchar(0x0a);
					}
		    }
			}
			i=i+32;  // next entry
			if(i>510)
			{
				Sector++;
				if((Sector-Sector_num)<max_sectors)
				{
					error_flag=Read_Sector(Sector,Drive_Values.BytesPerSec,values);
					if(error_flag!=NO_ERROR)
					{
						entries=0;   // no entries found indicates disk read error
						temp8=0;     // forces a function exit
					}
					i=0;
				}
				else
				{
					entries=entries|more_entries;  // set msb to indicate more entries in another cluster
					temp8=0;                       // forces a function exit
				}
			}
		} while(temp8!=0);
	}
	else
	{
		entries=0;    // no entries found indicates disk read error
	}
	return entries;
}

/***********************************************************************
DESC: Uses the same method as Print_Directory to locate short file names,
      but locates a specified entry and returns and cluster  
INPUT: Starting Sector of the directory, an entry number and a pointer to a 
block of memory in xdata that can be used to read blocks from the SD card
RETURNS: uint32_t with cluster in lower 28 bits.  Bit 28 set if this is 
         a directory entry, clear for a file.  Bit 31 set for error.
CAUTION: 
************************************************************************/
uint32_t Read_Dir_Entry(uint32_t Sector_num, uint16_t Entry, uint8_t xdata * array_in)
{ 
   uint32_t Sector, max_sectors, return_clus;
   uint16_t i, entries;
   uint8_t temp8, attr, error_flag;
   uint8_t * values;

   values=array_in;
   entries=0;
   i=0;
   return_clus=0;
   if (Drive_Values.FATtype==FAT16)  // included for FAT16 compatibility
   { 
      max_sectors=Drive_Values.RootDirSecs;   // maximum sectors in a FAT16 root directory
   }
   else
   {
      max_sectors=Drive_Values.SecPerClus;
   }
   Sector=Sector_num;
   error_flag=Read_Sector(Sector,Drive_Values.BytesPerSec,values);
   if(error_flag==NO_ERROR)
   {
     do
     {
        temp8=Read8(0+i,values);  // read first byte to see if empty
        if((temp8!=0xE5)&&(temp8!=0x00))
	    {  
	       attr=Read8(0x0b+i,values);
		   if((attr&0x0E)==0)    // if hidden do not print
		   {
		      entries++;
              if(entries==Entry)
              {
			    if(Drive_Values.FATtype==FAT32)
                {
                   return_clus=Read8(21+i,values);
				   return_clus=return_clus<<8;
                   return_clus|=Read8(20+i,values);
                   return_clus=return_clus<<8;
                }
                return_clus|=Read8(27+i,values);
			    return_clus=return_clus<<8;
                return_clus|=Read8(26+i,values);
			    attr=Read8(0x0b+i,values);
			    if(attr&0x10) return_clus|=directory_bit;
                temp8=0;    // forces a function exit
              }
              
		    }
		}
		    i=i+32;  // next entry
		    if(i>510)
		    {
			  Sector++;
			  if((Sector-Sector_num)<max_sectors)
			  {
                 error_flag=Read_Sector(Sector,Drive_Values.BytesPerSec,values);
			     if(error_flag!=NO_ERROR)
			     {
			         return_clus=no_entry_found;
                     temp8=0; 
			     }
			     i=0;
			  }
			  else
			  {
			     temp8=0;                       // forces a function exit
			  }
		    }
         
	  }while(temp8!=0);
	}
	else
	{
	   return_clus=no_entry_found;
	}
	if(return_clus==0) return_clus=no_entry_found;
   return return_clus;
 }

/***********************************************************************
DESC: Does all the initialization required for working with the FAT system. It also reads the values associated with FAT system and loads them to Drive_Values.
INPUT: A pointer to an array with 512 bytes available
RETURNS: Error flag that is equal to NO_ERROR if no error occurs
CAUTION: 
************************************************************************/
uint16_t Mount_Drive(uint8_t xdata * array)
{
	uint8_t number_of_FATs;
	uint16_t error_flag = NO_ERROR;
	uint16_t reserved_sector_count;
	uint16_t root_entry_count;
	uint32_t MBR_relative_sector;
	uint32_t FAT_size;
	uint32_t total_sectors;
	uint32_t root_cluster;
	uint32_t data_sectors;
	uint32_t clusters_count;
 
	error_flag = Read_Sector(0, 512, array);
	if (error_flag == NO_ERROR)
	{
		if (Read8(0, array) == 0xEB || Read8(0, array) == 0xE9)
		{
			MBR_relative_sector = 0;
		}
		else
		{
			MBR_relative_sector = Read32(0x01C6, array);
		}
		error_flag = Read_Sector(MBR_relative_sector, 512, array);
	}
	if (error_flag == NO_ERROR)
	{
		if (Read8(0, array) == 0xEB || Read8(0, array) == 0xE9)
		{
			Drive_Values.BytesPerSec = Read16(0x0B, array);
			Drive_Values.SecPerClus = Read8(0x0D, array);
			reserved_sector_count = Read16(0x0E, array);
			number_of_FATs = Read8(0x10, array);
			root_entry_count = Read16(0x11, array);
			total_sectors = Read16(0x13, array);
			FAT_size = Read16(0x16, array);
			if (total_sectors == 0)
			{
				total_sectors = Read32(0x20, array);
			}
			if (FAT_size == 0)
			{
				FAT_size = Read32(0x24, array);
				root_cluster = Read32(0x2C, array);
			}
			Drive_Values.RootDirSecs = ((root_entry_count * 32) + (Drive_Values.BytesPerSec - 1)) /
																	Drive_Values.BytesPerSec;
			data_sectors = total_sectors - (reserved_sector_count + (number_of_FATs * FAT_size) +
										 Drive_Values.RootDirSecs);
			clusters_count = data_sectors / Drive_Values.SecPerClus;
			if (clusters_count < 65525)
			{
				Drive_Values.FATtype = FAT16;
			}
			else
			{
				Drive_Values.FATtype = FAT32;
			}
			Drive_Values.StartofFAT = reserved_sector_count + MBR_relative_sector;
			Drive_Values.FirstDataSec = reserved_sector_count + (number_of_FATs * FAT_size) +
																	 Drive_Values.RootDirSecs + MBR_relative_sector;
			if (Drive_Values.FATtype == FAT16)
			{
				Drive_Values.FirstRootDirSec = reserved_sector_count + (number_of_FATs * FAT_size) +
																				MBR_relative_sector;
			}
			else if (Drive_Values.FATtype == FAT32)
			{
				Drive_Values.FirstRootDirSec = (root_cluster - 2) * Drive_Values.SecPerClus + Drive_Values.FirstDataSec;
			}
		}
		else
		{
			error_flag = FAT_BPB_NOT_FOUND_ERROR;
		}
	}
	return error_flag;
}

/***********************************************************************
DESC: Finds and returns number of first sector of a given cluster
INPUT: Number of the cluster
RETURNS: Number of the first sector
CAUTION: 
************************************************************************/
uint32_t First_Sector(uint32_t cluster_num)
{
	uint32_t first_sector;
	
	if (cluster_num == 0)
	{
		first_sector = Drive_Values.FirstRootDirSec;
	}
	else if (cluster_num > 1)
	{
		first_sector = (cluster_num - 2) * Drive_Values.SecPerClus + Drive_Values.FirstDataSec;
	}
	return first_sector;
}

/***********************************************************************
DESC: Finds number of next cluster where the rest of a file is stored by looking at the FAT table
INPUT: Number of the current cluster and a pointer to an array with 512 bytes available
RETURNS: Number of the next cluster
CAUTION: 
************************************************************************/
uint32_t Find_Next_Clus(uint32_t cluster_num, uint8_t xdata * array)
{
	uint16_t this_FAT_entry_offset;
	uint32_t this_FAT_sector_number;
	uint32_t next_cluster;
	
	this_FAT_sector_number = Drive_Values.StartofFAT + (cluster_num * Drive_Values.FATtype) / Drive_Values.BytesPerSec;
	Read_Sector(this_FAT_sector_number, 512, array);
	this_FAT_entry_offset = (cluster_num * Drive_Values.FATtype) % Drive_Values.BytesPerSec;
	if (Drive_Values.FATtype == FAT16)
	{
		next_cluster = (uint32_t) Read16(this_FAT_entry_offset,array);
	}
	else if (Drive_Values.FATtype == FAT32)
	{
		next_cluster = Read32(this_FAT_entry_offset,array);
		next_cluster &= 0x0FFFFFFF;
	}
	return next_cluster;
}

/***********************************************************************
DESC: Prints all the sectors of a file (one at a time) specified by the number of the cluster where it is located.
INPUT: Number of the first cluster and a pointer to an array with 512 bytes available
RETURNS: Error flag that is equal to NO_ERROR if no error occurs
CAUTION: 
************************************************************************/
/*uint16_t Open_File(uint32_t cluster, uint8_t xdata * array)
{
	uint16_t error_flag = NO_ERROR;
	uint8_t index = 0;
	uint32_t first_sector;
	first_sector = First_Sector(cluster);
	
	do
	{
		printf("\nCluster Number: %lu - Sector Number: %lu\n", cluster, first_sector + index);
		error_flag = Read_Sector(first_sector + index, 512, array);
		print_memory(array, 512);
		index++;
		if (index == Drive_Values.SecPerClus) //Last sector of the cluster
		{
			cluster = Find_Next_Clus(cluster, array);
			first_sector = First_Sector(cluster);
			index = 0;
			if (cluster == 0x0FFFFFFF) //Last cluster of the file
			{
				printf("\nEnd of File\n");
				break;
			}
		}
	} while(getchar() != 'x' && error_flag == NO_ERROR);
	putchar(BS);
	printf("\n\r");
	return error_flag;
}
*/