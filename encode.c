#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("->width of Image = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("->height of Image= %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr_secret ){
    fseek(fptr_secret,0,SEEK_END);
    printf("->File size %ld\n",ftell(fptr_secret));
    return ftell(fptr_secret);
}
/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    printf("[SUCCESS]-Opened Files successfully\n");
    return e_success;
}
/*Checking the operation type from Command Line Arguments*/
OperationType check_operation_type(char* argv[])
{
    if(!strcmp(argv[1],"-e")){
        printf("\n::::Encoding Selected::::\n");
        return e_encode;
    }else if(!strcmp(argv[1],"-d")){
        printf("\n::::Decoding Selected::::\n");
        return e_decode;
    }else{
        printf("\n::::Unsupported command::::\n");
        return e_unsupported;
    }
}
Status read_and_validate_encode_args(char* argv[],EncodeInfo *encInfo){
    if(strstr(argv[2],".bmp")){
        //checking for .bmp in file name using strstr function
        char* ptr=strstr(argv[2],".bmp");
        if(strlen(argv[2])<=4 || strcmp( ptr, argv[2]+strlen(argv[2])- 4 ) != 0){
            printf("[ERROR]-Source image should have .bmp extension at last\n");
            return e_failure;
        }else{
            //storing file name in the struture
            encInfo->src_image_fname=argv[2];
        }
    }else{
        //Error Handling
        printf("[ERROR]-Source image should have .bmp extension\n");
        return e_failure;
    }

    if(strstr(argv[3],".txt")){
        //Checking for .txt extension in the given file name
        char* ptr=strstr(argv[3],".txt");
        //checking if .txt is present at last
        if(strlen(argv[3])<=4 || strcmp( ptr, argv[3]+strlen(argv[3])-4 ) != 0){ 
            printf("[ERROR]-Secret Data should have .txt extension at last\n");
            return e_failure;
        }else{
            //Storing Extension and File name in the structure
            encInfo->secret_fname=argv[3];
            strcpy(encInfo->extn_secret_file,ptr);
        }
    }else{
        printf("[ERROR]-Secret Data should have .txt extension\n");
        return e_failure;
    }

    if(argv[4]!=NULL){              
        //Checking if user has given Output file name
        if(strstr(argv[4],".bmp")){
        char* ptr=strstr(argv[4],".bmp");
            if(strlen(argv[4])<=4 || strcmp( ptr, argv[4]+strlen(argv[4])- 4 ) != 0){
                printf("[ERROR]-Source image should have .bmp extension at last::::\n");
                return e_failure;
            }else{
                encInfo->stego_image_fname=argv[4];
            }
        }else{
            printf("[ERROR]-Source image should have .bmp extension::::\n");
            return e_failure;
        }
    }else{
        encInfo->stego_image_fname="default.stego.bmp";
    }
    printf("[SUCCESS]-Encoding Arguments are Valid\n");
    return e_success;  
}
Status do_encoding(EncodeInfo *encInfo){
    printf("\n--------------------STARTED ENCODING-----------------------\n\n");
    //Calling all the functions and Returning the Status of the Function to the Main Function
    if(open_files(encInfo)==e_failure){
        return e_failure;
    }
    if(check_capacity(encInfo)==e_failure){
        return e_failure;
    }
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure){
        return e_failure;
    }
    if(encode_magic_string(MAGIC_STRING,encInfo)==e_failure){
        return e_failure;
    }
    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)==e_failure){
        return e_failure;
    }
    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo)==e_failure){
        return e_failure;
    }
    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_failure){
        return e_failure;
    }
    if(encode_secret_file_data(encInfo)==e_failure){
        return e_failure;   
    }
    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure){
        return e_failure;
    }
    //If No Function Fails, we return Success Status to the Main Function
    printf("\n-----------------------[SUCCESS]---------------------------\n");
    printf("---------------ENCODING DONE SUCCESSFULY-------------------\n\n");
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo){
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image); //get image capacity
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);           //get File Size
    //Validating if Image Capacity is Enough to Store Secret Data
    if(encInfo->image_capacity < (encInfo->size_secret_file*8)+16+32+32+32){ 
        printf("[ERROR]-Image is too small to store the file\n");
        return e_failure;
    }
    printf("[SUCCESS]-Image capacity is sufficient\n");
    return e_success;
}
Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_stego_image){
    char str[54];
     //Set the File Pointer to Start of the File
    fseek(fptr_src_image,0,SEEK_SET);                   
    if(fread(str,54,1,fptr_src_image)!=1){               //read first 54 bytes
        printf("[ERROR]-Could not read BMP Header\n");
        return e_failure;
    }
    if(fwrite(str,54,1,fptr_stego_image)!=1){           //copy to stego image
        printf("[ERROR]-Could not write BMP Header to image\n");
        return e_failure;
    }
    printf("[SUCCESS]-Header Copied successfully\n");
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo){
    //call function with size, data and structure
    if(encode_data_to_image(magic_string,strlen(magic_string),encInfo)==e_failure){ 
        printf("[ERROR]-Couldn't Encode Magic String\n");
        return e_failure;
    }
    printf("[SUCCESS]-Encoded Magic String Successfully\n");
    return e_success;
}

Status encode_secret_file_extn_size(uint extn_size, EncodeInfo *encInfo){
    char arr[32];
    //Read 32 and bytes and Encode 4 bytes of size
    if(fread(arr,32,1,encInfo->fptr_src_image)!=1){    
        return e_failure;
    }
    for(int i=0;i<32;i++){
        arr[i]= (arr[i] & 0xFE) | (extn_size>>(31-i)) & 1;
    }
    if(fwrite(arr,32,1,encInfo->fptr_stego_image)!=1){
        return e_failure;
    }
    printf("[SUCCESS]-Encoded Secret File Extension Size Successfully\n");
    return e_success;
}
//Encode File Extension Function Definition
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo){  
    if(encode_data_to_image(file_extn,strlen(file_extn),encInfo)==e_failure){
        printf("[ERROR]=Couldn't Encode Secret File Extension\n");
        return e_failure;
    }
    printf("[SUCCESS]-Encoded Secret File Extension Successfully\n");
    return e_success;
}
//Function Definition to Encode File Size
Status encode_secret_file_size(uint file_size,EncodeInfo *encInfo){     
    char arr[32];
    fread(arr,32,1,encInfo->fptr_src_image);
    for(int i=0;i<32;i++){
        arr[i] = arr[i] & 0xFE | (file_size>>(31-i))& 1; 
    }
    fwrite(arr,32,1,encInfo->fptr_stego_image);                      
    printf("[SUCCESS]-Encoded Secret File Size Successfully\n");
    return e_success;
}
//Function Definition for Encoding File Data               
Status encode_secret_file_data(EncodeInfo *encInfo){      
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    for(int i=0;i<encInfo->size_secret_file;i++){
        if(fread(encInfo->secret_data,MAX_SECRET_BUF_SIZE,1,encInfo->fptr_secret)!=1){
            printf("[ERROR]-Couldnt Encode Secret File Data\n");
            return e_failure;
        }
        if(encode_data_to_image(encInfo->secret_data,1,encInfo)==e_failure){
            printf("[ERROR]-Couldnt Encode Secret File Data\n");
            return e_failure;
        }
    }
    printf("[SUCCESS]-Encoded Secret File Data Successfully\n");
    return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src_image,FILE *fptr_stego_image){
    char arr[1000];
    int num;
    //Copy Remaining Data to the image
    while((num=fread(arr,1,1000,fptr_src_image))>0){
        fwrite(arr,1,num,fptr_stego_image);
    }
    printf("[SUCCESS]-Copied Remaining bits from Source Image\n");
    return e_success;
}
//Function to Read 8 Bytes and Encode Size times
Status encode_data_to_image(const char *data, int size, EncodeInfo *encInfo){
    for(int i=0;i<size;i++){
        if(fread(encInfo->image_data,MAX_IMAGE_BUF_SIZE,1,encInfo->fptr_src_image)!=1){
            return e_failure;
        }//Function call to encode 1 byte in 8 bytes of image
        encode_byte_to_lsb(data[i],encInfo->image_data);
        if(fwrite(encInfo->image_data,MAX_IMAGE_BUF_SIZE,1,encInfo->fptr_stego_image)!=1){
            return e_failure;
        }
    }
    return e_success;
}//Function Definition to Encode one Byte of Data in 8 Bytes of Image Data
Status encode_byte_to_lsb(char data, char *image_buffer){
    for(int i=0;i<8;i++){
        image_buffer[i]= image_buffer[i] & 0xFE | data>>(7-i) & 0x01;
     }
     return e_success;
} 