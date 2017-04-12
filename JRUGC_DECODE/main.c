#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

char* path_buffer;
char* Master_Bluffer;
char* temp10;
FILE* C;
int path_length;
long file_length;
long pos;
int treated;

void appendNumber(int value){
    int i = 0;
    for(i=pos+7;i>=pos;i--){
        Master_Bluffer[i] = (value & 1)+48;
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
        result+=(Master_Bluffer[i+pos]-48)*power(length-i-1);
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
            Master_Bluffer[pos] = '\0';
        }
    }
    fclose(P);
}

void bru_file() {
    path_length = strlen(path_buffer);
    pos=0;
    DIR *dir;
    C = fopen(strcat(path_buffer,"compile.txt"),"w+");
    path_buffer[path_length]='\0';
    struct dirent *ent;
    dir = opendir (path_buffer);
    while ((ent = readdir (dir)) != NULL) {
        if (ent->d_name[10]=='.') {
            bru_file_decode(strcat(path_buffer,ent->d_name));
            path_buffer[path_length]='\0';
        }
    }
    closedir (dir);
    fprintf(C,"%s",Master_Bluffer);
    fclose(C);
}

void compile_file_read() {
    char* id_name_buffer = malloc(25*sizeof(char));
    double q_scale;
    C = fopen(strcat(path_buffer,"compile.txt"),"r");
    fgets(Master_Bluffer,pos,C);
    fclose(C);
    pos = 0;
    while (pos<file_length) {
        treated=0;
        int id = bindec(8);
        int l_message = bindec(10);
        int length = 112*((int)((l_message+1)/14)+1);
        if (length==8288) {
            bindec(94);
            continue;
        }
        switch(id) {
            case 197:
            sprintf(id_name_buffer,"ETAL_AUTO_ODOMETRIE");
            break;
            case 199:
            sprintf(id_name_buffer,"CHANGEMENT_DE_MODE");
            break;
            case 200:
            sprintf(id_name_buffer,"CHANGEMENT_DE_NIVEAU");
            break;
            case 198:
            sprintf(id_name_buffer,"INFO_VITESSES_DISTANCE");
            break;
            case 2:
            sprintf(id_name_buffer,"DATA_ENTRY_COMPLETED");
            break;
            case 3:
            sprintf(id_name_buffer,"EMERGENCY_BRAKE_STATE");
            break;
            case 6:
            sprintf(id_name_buffer,"MESSAGE_FROM_BALISE");
            break;
            case 9:
            sprintf(id_name_buffer,"MESSAGE_FROM_RBC");
            break;
            case 10:
            sprintf(id_name_buffer,"MESSAGE_TO_RBC");
            break;
            case 15:
            sprintf(id_name_buffer,"PLAIN TEXT MESSAGE");
            break;
            case 11:
            sprintf(id_name_buffer,"DRIVER_ACTIONS");
            break;
            case 5:
            sprintf(id_name_buffer,"EVENTS");
            break;
        }
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
        int m_mode = bindec(4);
        pos+=length-treated;
//        printf("%02d/%02d/%02d  %02d:%02d:%02d:%03d\n",day,month,year,hour,minute,second,tts);
    }
    free(id_name_buffer);
}

int main(void)
{
    Master_Bluffer = malloc(115000000*sizeof(char));
    path_buffer = malloc(500*sizeof(char));
    sprintf(path_buffer,"C:\\Users\\ugc\\Desktop\\43052170116_JRU\\flash24h\\");
    bru_file();
    file_length=pos;
    compile_file_read();
    free(Master_Bluffer);
    free(path_buffer);
    return 0;
}
