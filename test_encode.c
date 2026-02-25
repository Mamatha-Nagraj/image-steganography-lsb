#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
//Collection of Arguments
int main(int argc, char* argv[])                      
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    //validating Command Line Arguments,if its less than Two
    if(argc>2){   
        //validating Command Line Arguments for Encoding                                     
        if(strcmp(argv[1],"-e")==0){
            if(argc<4 || argc>5){                     
                printf("[ERROR]-Insufficient/Invalid Arguments::::\n");
                printf("For Encoding::USAGE: ./lsb_steg -e <src.bmp> <secret.txt> <output.bmp>\n");
                return 1;
            }
        }
        //Validating Command Line Arguments for Decoding
        else if(strcmp(argv[1],"-d")==0){            
            if(argc<3 || argc>4){                    
                printf("[ERROR]-Insufficient/Invalid Arguments::::\n");
                printf("For Decoding::USAGE: ./lsb_steg -d <stego.bmp> <secret.txt>\n");
                return 1;
            }
        }else if(strcmp(argv[1],"-e") && strcmp(argv[1],"-d")){
            printf("[ERROR]-Invalid Arguments\n");
            return 1;
        }
    }
    else{    //Printing Usage Message
        printf("[ERROR]-Insufficient Arguments::::\n");
        printf("For Encoding::USAGE: ./lsb_steg -e <src.bmp> <secret.txt> <output.bmp>\n");
        printf("For Decoding::USAGE: ./lsb_steg -d <stego.bmp> <secret.txt>\n");
        return 1;
    }
    //Operation Type Checking in Command Line Arguments
    OperationType op_type=check_operation_type(argv);    
    //Encoding     
    if(op_type==e_encode){                                   
        if(read_and_validate_encode_args(argv,&encInfo)==e_success){
            if(do_encoding(&encInfo)==e_failure){
                printf("------------------------------[ERROR]-------------------------------\n");
                printf("-------------------------ENCODING FAILED----------------------------\n");
                return 1;
            }
        }else{
            printf("[ERROR]-Arguments are Invalid\n");
            return 1;
        }
    //Decoding
    }else if(op_type==e_decode){                            
        if(read_and_validate_decode_args(argv,&decInfo)==e_success){
            if(do_decoding(&decInfo)==e_failure){
                printf("------------------------------[ERROR]-------------------------------\n");
                printf("-------------------------DECODING FAILED----------------------------\n");
                return 1;
            }
        }else{
            printf("[ERROR]-Arguments are Invalid\n");
            return 1;
        }
    }else{
        printf("[ERROR]-UNSUPPORTED COMMAND");
        return 1;
    }
    return 0;
}
