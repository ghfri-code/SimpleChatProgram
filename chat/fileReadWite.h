#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "Parameters.h"

void *myThreadFun(void *vargp);
int setBufferFromFile(char fileNameInput[1000],unsigned char fileBuffer[MaxFileSize]);
void writeFileFromBuffer(char fileNameWrite[1000],unsigned char fileBuffer[MaxFileSize],int fileSize);

void *myThreadFun(void *vargp)
{
    sleep(1);
    //createAndConnectSocket(2349);
    printf("Printing GeeksQuiz from Thread \n");
    return NULL;
}
int setBufferFromFile(char fileNameInput[1000],unsigned char fileBuffer[MaxFileSize]){
    int fileSize;
    FILE *fptr;
    fptr = fopen(fileNameInput,"rb");
    if (fptr){
        fseek(fptr, 0L, SEEK_END);
        fileSize = ftell(fptr);
        rewind(fptr);
        if(fileSize<MaxFileSize){
            fread(fileBuffer,fileSize,1,fptr); // read whole bytes to our buffer
        }{
            printf("file is too big\n");
        }
        fclose(fptr);
        printf("File size : %d byte\n",fileSize);
        return fileSize;
    }else{
        printf("Error! opening file");
        return -1;
    }
};
void setBufferFromFileII(char fileNameInput[1000], char* fileBuffer){

    FILE *file;
    unsigned long fileLen;

    //Open file
    file = fopen(fileNameInput, "rb");
    if (!file)
    {
        fprintf(stderr, "\nUnable to open file %s", fileNameInput);
        return;
    }

    //Get file length
    fseek(file, 0, SEEK_END);
    fileLen=ftell(file);
    fseek(file, 0, SEEK_SET);

    //Allocate memory
    fileBuffer=(char *)malloc(fileLen+1);
    if (!fileBuffer)
    {
        fprintf(stderr, "Memory error!");
                                fclose(file);
        return;
    }

    //Read file contents into buffer
    fread(fileBuffer, fileLen, 1, file);
    fclose(file);


}
void writeFileFromBuffer(char fileNameWrite[1000],unsigned char fileBuffer[MaxFileSize],int fileSize){
    FILE *write_ptr;
    write_ptr = fopen(fileNameWrite,"wb");  // w for write, b for binary
    if (write_ptr){
        fwrite(fileBuffer,fileSize,1,write_ptr); // write 10 bytes from our buffer
        fclose(write_ptr);
        printf("File %s  succesfully writed\n",fileNameWrite);
    }else{
        printf("Error! opening file");
    }
};
void writeFileFromBufferII(char fileNameWrite[1000], char fileBuffer[MaxFileSize],int fileSize){

    /* Write your buffer to disk. */
    FILE *pFile ;
            pFile = fopen(fileNameWrite,"wb");

    if (pFile){
        fwrite(fileBuffer, fileSize, 1, pFile);
        puts("Wrote to file!");
    }
    else{
        puts("Something wrong writing to File.");
    }
    free(fileBuffer);
    fclose(pFile);


}
void setBufferFromTextFile(char fileNameInput[1000], char* fileBuffer){

      FILE * stream;
      stream = fopen(fileNameInput, "rb");
      fseek(stream, 0, SEEK_END);
      long fileLen=ftell(stream);
      fseek(stream, 0, SEEK_SET);

      fread(fileBuffer,1, fileLen, stream);
      fclose(stream);

}
void writeTextFileFromBuffer(char fileNameWrite[1000], char fileBuffer[MaxFileSize],int filesize){


    FILE* pFile = fopen(fileNameWrite,"wb");

    if (pFile){
        fwrite(fileBuffer, filesize, 1, pFile);
        puts("Wrote to file!");
    }
    else{
        puts("Something wrong writing to File.");
    }

    fclose(pFile);



}
