#include<stdio.h>
#include<string.h>
#include "types.h"
#include "decode.h"
#include "common.h"

Status read_and_validate_decode_args(char* argv[],DecodeInfo *decInfo){
    //Checking if stego image has neccesary Extension
    if(strstr(argv[2],".bmp")){
        char *ptr=strstr(argv[2],".bmp");
        if(strcmp(argv[2] +strlen(argv[2]) - 4 , ptr)){
            printf("[ERROR]-The Stego Image should have .bmp extension\n");
            return e_failure;
        }else{
            //If valid, Store the name in structure 
            decInfo->stego_image_fname=argv[2];
        }
    }else{
        printf("[ERROR]-The Stego Image should have .bmp extension\n");
        return e_failure;
    }
    //Cehcking if user has given the output file name or not
    if(argv[3]!=NULL){
        //if given, validating the name
        if(strstr(argv[3],".txt")){
        char *ptr=strstr(argv[3],".txt");
            if(strcmp(argv[3] +strlen(argv[3]) - 4 , ptr)){
                printf("[ERROR]-The Output File should have .txt extension\n");
                return e_failure;
            }else{
                ptr=strtok(argv[3],".");
                strcpy(decInfo->secret_data_fname,ptr);
            }
        }else{
                printf("[ERROR]-The Output File should have .txt extension\n");
                return e_failure;
        }
    }else{
        //giving Default Name 
        strcpy(decInfo->secret_data_fname,"Output");
    }
    printf("[SUCCESS]-Arguments are Valid\n");
    return e_success;
}
Status do_decoding(DecodeInfo *decInfo){
    printf("\n-------------------STARTED DECODING------------------\n\n");

    //Calling all neccesary Function for Decoding, if Failure, return to the main Function
    if(open_decode_files(decInfo)==e_failure){
        return e_failure;
    }
    if(decode_magic_string(decInfo)==e_failure){
        return e_failure;
    }
    if(decode_extn_size(decInfo)==e_failure){
        return e_failure;
    }
    if(decode_extn(decInfo)==e_failure){
        return e_failure;
    }
    if(decode_secret_data_size(decInfo)==e_failure){
        return e_failure;
    }
    if(decode_secret_data(decInfo)==e_failure){
        return e_failure;
    }
    printf("\n--------------------[SUCCESS]------------------------\n");
    printf("----------------DECODING SUCCESSFULL-----------------\n\n");
    return e_success;
}
Status open_decode_files(DecodeInfo *decInfo){
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

    	return e_failure;
    }
    printf("[SUCCESS]-Opened Image File Successfully\n");
    return e_success;
}
//Function Defintion to Encode Magic String
Status decode_magic_string(DecodeInfo *decInfo){
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);
    int i;
    for(i=0;i<strlen(MAGIC_STRING);i++){
        char ch=0;
        if(fread(decInfo->image_data,MAX_IMAGE_BUFFFER,1,decInfo->fptr_stego_image)!=1){
            return e_failure;
        }
        decode_byte_from_lsb(&ch,decInfo->image_data);
        decInfo->magic_string[i]=ch;
    }
    decInfo->magic_string[i]='\0';
    if(strcmp(decInfo->magic_string,MAGIC_STRING)){
        printf("[FAIL]-Magic Strings Dont Match. Cannot Decode\n");
        return e_failure;
    }
    printf("[SUCCESS]-Decoded Magic String and Verified Magic String Successfully\n");
    return e_success;
}
//Decoding Extension size 
Status decode_extn_size(DecodeInfo *decInfo){
    char arr[32];
    if(fread(arr,32,1,decInfo->fptr_stego_image)!=1){
        return e_failure;
    }
    if(decode_size_from_lsb(arr,&decInfo->extn_size)==e_failure){
        return e_failure;
    }
    printf("[SUCCESS]-File Extension Size Decoded Successfully\n");
    return e_success;
}
//Decoding Extension and opening the output file
Status decode_extn(DecodeInfo *decInfo){
    int i;
    for(i=0;i<decInfo->extn_size;i++){
        char ch=0;
        if(fread(decInfo->image_data,8,1,decInfo->fptr_stego_image)!=1){
            return e_failure;
        }
        decode_byte_from_lsb(&ch,decInfo->image_data);
        decInfo->file_extn[i]=ch;
    }
    decInfo->file_extn[i]='\0';
    printf("[SUCCESS]-File Extension Decoded Successfully\n");
    strcat(decInfo->secret_data_fname,decInfo->file_extn);
    //Opening the file in the given Extension
    decInfo->fptr_secret_data=fopen(decInfo->secret_data_fname,"w");
    if(decInfo->fptr_secret_data==NULL){
        perror("fopen");
        fprintf(stderr,"Unable to open %s",decInfo->secret_data_fname);
        return e_failure;
    }
    printf("[SUCCESS]-Output File Opened and Saved as [%s] as per the extension\n",decInfo->secret_data_fname);
    return e_success;
}
//Reading the secret File size 
Status decode_secret_data_size(DecodeInfo *decInfo){
    char arr[32];
    if(fread(arr,32,1,decInfo->fptr_stego_image)!=1){
        return e_failure;
    }
    decode_size_from_lsb(arr,&decInfo->file_size);
    printf("[SUCCESS]-Secret File Size Decoded Successfully\n");
    return e_success;
}
//Decode File Data and stroing in the Output File
Status decode_secret_data(DecodeInfo *decInfo){
    for(int i=0;i<decInfo->file_size;i++){
        char ch=0;
        if(fread(decInfo->image_data,8,1,decInfo->fptr_stego_image)!=1){
            return e_failure;
        }
        decode_byte_from_lsb(&ch,decInfo->image_data);
        fwrite(&ch,1,1,decInfo->fptr_secret_data);
    }
    printf("\n[SUCCESS]-Secret File Decoded Successfully\n");
    printf("         -STORED IN FILE NAMED %s\n",decInfo->secret_data_fname);
    return e_success;
}
//Decoding a character from 8 bytes
Status decode_byte_from_lsb(char *data,char *buffer){
    for(int i=0;i<8;i++){
        *data=*data|(buffer[i]&0x01)<<(7-i);
    }
    return e_success;
}
//Decoing the size's of 4 bytes from 32 bytes
Status decode_size_from_lsb(char *buffer,int *size){
    *size=0;
    for(int i=0;i<32;i++){
        *size = (buffer[i] & 1)<<(31-i) | *size;
    }
    return e_success;
}
