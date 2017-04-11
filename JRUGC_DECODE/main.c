#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

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

void bru_file_id(char* path) {
    path_length = strlen(path);
    pos=0;
    DIR *dir;
    C = fopen(strcat(path,"compile.txt"),"w+");
    path[path_length]='\0';
    struct dirent *ent;
    dir = opendir (path);
    while ((ent = readdir (dir)) != NULL) {
        if (ent->d_name[10]=='.') {
            bru_file_decode(strcat(path,ent->d_name));
            path[path_length]='\0';
        }
    }
    closedir (dir);
    fprintf(C,"%s",temp1m500k);
    fclose(C);
}

int main(void)
{
    temp1m500k = malloc(1100000000);
    temp500 = malloc(500*sizeof(char));
    temp10 = malloc(10*sizeof(char));
    sprintf(temp500,"C:\\Users\\ugc\\Desktop\\43052170116_JRU\\flash24h\\");
    bru_file_id(temp500);
    free(temp1m500k);
    free(temp500);
    free(temp10);
    return 0;
}





