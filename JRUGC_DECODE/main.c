#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <math.h>

char* temp500;
char* temp1m500k;
char* temp10;
FILE* C;
int path_length;
int pos;

void appendNumber(int value){
    int i = 0;
    for(i=pos+7;i>=pos;i--){
        temp1m500k[i] = (value & 1)+48;
        value >>= 1;
    }
    pos += 8;
}

long bindec(int length) {
int i;
long result=0;
    for (i=0;i<length;i++) {
        result+=(temp1m500k[i+pos]-48)*pow(2,length-i-1);
    }
pos+=length;
printf("%ld\n",result);
return result;
}

void bru_file_decode(char* src) {
    FILE* P;
    int c,i=0;
    P = fopen(src,"rb");
    while ((c = fgetc(P)) != EOF) {
        i++;
        if (i>129 && ((i-130)%16)<14) {
            appendNumber(c);
            temp1m500k[pos] = '\0';

        }
    }
    fclose(P);
}

void bru_file() {
    path_length = strlen(temp500);
    pos=0;
    DIR *dir;
    C = fopen(strcat(temp500,"compile.txt"),"w+");
    temp500[path_length]='\0';
    struct dirent *ent;
    dir = opendir (temp500);
    while ((ent = readdir (dir)) != NULL) {
        if (ent->d_name[10]=='.') {
            bru_file_decode(strcat(temp500,ent->d_name));
            temp500[path_length]='\0';
        }
    }
    closedir (dir);
    fprintf(C,"%s",temp1m500k);
    fclose(C);
}

void compile_file_read() {
    C = fopen(strcat(temp500,"compile.txt"),"r");
    fgets(temp1m500k,pos,C);
    fclose(C);
    pos = 0;
}

int main(void)
{
    temp1m500k = malloc(115000000*sizeof(char));
    temp500 = malloc(500*sizeof(char));
    sprintf(temp500,"C:\\Users\\ugc\\Desktop\\43052170116_JRU\\flash24h\\");
    bru_file();
    compile_file_read();
    bindec(8);
    bindec(8);
    bindec(8);
    bindec(8);
    free(temp1m500k);
    free(temp500);
    return 0;
}





