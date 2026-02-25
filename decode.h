#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>
#include "types.h"       //Contains User Defined Data types
#include "common.h"      //Contains Magic String

#define MAX_FILE_SUFFIX 5
#define MAX_IMAGE_BUFFFER 8
/*Struture that contains File Informataion 
* and few Intermediate Information to collect 
* from the stego image to validate 
*/
typedef struct{
    char *stego_image_fname;  //Stego image
    FILE *fptr_stego_image;
    
    uint extn_size;
    uint file_size;
    char file_extn[MAX_FILE_SUFFIX];
    char image_data[MAX_IMAGE_BUFFFER];
    char magic_string[10];

    FILE *fptr_secret_data;   //Secret text File
    char secret_data_fname[50];
} DecodeInfo;

//Decoding Function Prototype

//Read and validate Arguments
Status read_and_validate_decode_args(char* argv[],DecodeInfo *decInfo);

//Perform Decoding
Status do_decoding(DecodeInfo *decInfo);

//Get File Pointers for i/p and o/p Files
Status open_decode_files(DecodeInfo *decInfo);

//Decode magic string and validate
Status decode_magic_string(DecodeInfo *decInfo);

//Decode Extension Size
Status decode_extn_size(DecodeInfo *decInfo);

//Decode Extension to create the Secret file with specific format
Status decode_extn(DecodeInfo *decInfo);

//Decode Secret Data Size 
Status decode_secret_data_size(DecodeInfo *decInfo);

//Decode Secret Data and write in the outptu file
Status decode_secret_data(DecodeInfo *decInfo);

//Decoding the Strings char by char
Status decode_byte_from_lsb(char *data,char *buffer);

//Decding size of 4 bytes at once
Status decode_size_from_lsb(char *buffer,int *size);

#endif