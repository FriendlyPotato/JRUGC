#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

char* temp500;
char* temp1m500k;
char* temp10;
FILE* C;
int path_length;
long file_length;
long pos;
int treated;

void appendNumber(int value){
    int i = 0;
    for(i=pos+7;i>=pos;i--){
        temp1m500k[i] = (value & 1)+48;
        value >>= 1;
    }
    pos += 8;
}

long power(int degree)
{
    long result = 1;
    long term = 2;
    while (degree)
    {
        if (degree & 1)
            result *= term;
        term *= term;
        degree >>= 1;
    }
    return result;
}

long bindec(int length) {
int i;
long result=0;
    for (i=0;i<length;i++) {
        result+=(temp1m500k[i+pos]-48)*power(length-i-1);
    }
pos+=length;
treated+=length;
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
    printf("Encoding...\n");
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
    double q_scale;
    printf("Decoding...\n");
    C = fopen(strcat(temp500,"compile.txt"),"r");
    fgets(temp1m500k,pos,C);
    fclose(C);
    pos = 0;
    while (pos<file_length) {
        int id = bindec(8);
        int l_message = bindec(10);
        int length = 112*(floor((l_message+1)/14)+1);
        if (length==8288) {
            bindec(94);
            continue;
        }

//        if (id==1)   printf("GENERAL_MESSAGE");
//        if (id==197) printf("ETAL_AUTO_ODOMETRIE");
//        if (id==199) printf("CHANGEMENT_DE_MODE");
//        if (id==200) printf("CHANGEMENT_DE_NIVEAU");
//        if (id==198) printf("INFO_VITESSES_DISTANCE");
//        if (id==2)   printf("DATA_ENTRY_COMPLETED");
//        if (id==3)   printf("EMERGENCY_BRAKE_STATE");
//        if (id==6)   printf("MESSAGE_FROM_BALISE");
//        if (id==9)   printf("MESSAGE_FROM_RBC");
//        if (id==10)  printf("MESSAGE_TO_RBC");
//        if (id==15)  printf("PLAIN TEXT MESSAGE");
//        if (id==11)  printf("DRIVER_ACTIONS");
//        if (id==5)   printf("EVENTS");
        int year = bindec(7);
        int month = bindec(4);
        int day = bindec(5);
        int hour = bindec(5);
        int minute = bindec(6);
        int second = bindec(6);
        int tts = 50*bindec(5);
        int q_scale_read = bindec(2);
        if (q_scale_read==1) q_scale = 1.0;
        if (q_scale_read==0) q_scale = 0.1;
        if (q_scale_read==2) q_scale = 10.0;
        int nid_c = bindec(10);
        int nid_bg = bindec(15)/2;
        int d_lrbg = bindec(14)*q_scale;
        int q_dir = bindec(2);
        int q_dlrbg = bindec(2);
        int l_doubtover = q_scale*bindec(15);
        int l_doubtunder = q_scale*bindec(15);
        int v_train = 5*bindec(7);
        int id_driver_0 = bindec(32);
        unsigned long nid_operational = bindec(28); pos+=4; treated+=4;
        int m_level = bindec(3);
        pos+=length-treated;
        treated=0;
        printf("%02d/%02d/%02d  %02d:%02d:%02d:%03d\n",day,month,year,hour,minute,second,tts);
    }
}

int main(void)
{
    temp1m500k = malloc(115000000*sizeof(char));
    temp500 = malloc(500*sizeof(char));
    sprintf(temp500,"C:\\Users\\ugc\\Desktop\\43052170116_JRU\\flash24h\\");
    bru_file();
    file_length=pos;
    compile_file_read();
    free(temp1m500k);
    free(temp500);
    return 0;
}
