#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <pthread.h>

char* temp10;
char* temp500;
FILE* C;
int path_length;
pthread_t thread_files [250];

void bru_file_decode(char* src) {
    FILE* P;
    int c,i=0;
    P = fopen(src,"rb");
    while ((c = fgetc(P)) != EOF) {
        i++;
        if (i>129 && ((i-130)%16)<14) {
            itoa(c+256,temp10,2);
            fprintf(C,"%s",&temp10[1]);
        }
    }
    fclose(P);
    pthread_exit(NULL);
}

void bru_file_id(char* path, char* temp10) {
    path_length = strlen(path);
    DIR *dir;
    int n=1;
    C = fopen(strcat(path,"compile.txt"),"w+");
    path[path_length]='\0';
    struct dirent *ent;
    dir = opendir (path);
    while ((ent = readdir (dir)) != NULL) {
        if (ent->d_name[10]=='.') {
            printf("%s\n",ent->d_name);
            pthread_create(&thread_files[n], NULL, bru_file_decode, strcat(path,ent->d_name));
            pthread_join(&thread_files[n], NULL);
            n++;
            path[path_length]='\0';
        }
    }
    closedir (dir);
    fclose(C);
}

int main(void)
{
    temp500 = malloc(500*sizeof(char));
    temp10 = malloc(10*sizeof(char));
    sprintf(temp500,"C:\\Users\\ugc\\Desktop\\43052170116_JRU\\flash24h\\");
    bru_file_id(temp500,temp10);
    free(temp500);
    free(temp10);
    return 0;
}
