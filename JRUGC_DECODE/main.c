#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <windows.h>
#include <winuser.h>
#include <wingdi.h>
#include <CommCtrl.h>
#include <stdbool.h>
#include <time.h>

//Size of the buffer to read the entire file
#define Master_Bluffer_Size 115000000
//Size of the buffer to store the path name
#define Path_Buffer_Size 500
//Size of the buffer to store the message
#define Message_Buffer_Size 300
//Size of the buffer to store the message details temporarily
#define Details_Bindec_Buffer_Size 10
//Size of the buffer to store distance and time informations from detailed file
#define Details_Data_Buffer_Size 31
//Size of the buffer to store window texts
#define Window_Buffer_Size 31
//Mean number of messages for storage
#define Mean_Message_Number_Count 400000
//Mean number of details for storage
#define Mean_Details_Number 100000000

//Code for clicked button message
#define BTN_CLICKED_MESSAGE 9998
//Code for selection changed message
#define SEL_CHANGE_MESSAGE 9999

char* Path_Buffer;
char* Master_Bluffer;
char* Hexdec_Buffer;
char* Message_Buffer;
char* Details_Buffer;
char* Details_Bindec_Buffer;
char* Details_Data_Buffer;
bool control;
bool shift;
FILE* Compiled_File;
int path_length;
long long file_length;
long long General_Position;
int treated=0;
long long Message_Position=0;
int Details_Position;
int Reference_Message=-1;
unsigned long long Time_Storage[Mean_Message_Number_Count];
unsigned long long Distance_Storage[Mean_Message_Number_Count];
int Message_Details_Storage[Mean_Details_Number];

HWND MainWindow,MessageWindow,ProgressBarWindow,QuitButtonWindow,MessageHeaderWindow,FromWindow,ToWindow,TimeWindow,DistanceWindow,SpeedWindow,SetButtonWindow,GoToButtonWindow,ClearButtonWindow;
HFONT Main_Font;
HBRUSH Main_Brush,Gray_Brush,Orange_Edit_Brush,Green_Edit_Brush,Purple_Edit_Brush,Background_brush;

const char g_szClassName[] = "myWindowClass";

void appendNumber(int value){
    long long i;
    for(i=General_Position+7;i>=General_Position;i--){
        Master_Bluffer[i] = (value & 1)+48;
        value >>= 1;
    }
    General_Position += 8;
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

void hexdec(int length) {
    int i = 0;
    int num = 0;
    do {
        int b = ((Master_Bluffer[General_Position+i])=='1'?1:0);
        i++;
        num = (num<<1)|b;
    } while (i<length);
    Hexdec_Buffer[length]='\0';
    sprintf(Hexdec_Buffer,"%X",num);
    General_Position+=length;
    treated+=length;
}

long bindec(int length) {
int i;
long result=0;
    for (i=0;i<length;i++) {
        if ((Master_Bluffer[i+General_Position]!=48)) result+=power(length-i-1);
    }
General_Position+=length;
treated+=length;
return result;
}

void bndtls(int length) {
int i;
int result=0;
    for (i=0;i<length;i++) {
        if ((Master_Bluffer[i+General_Position]!=48)) result+=power(length-i-1);
    }
    General_Position+=length;
    treated+=length;
    Message_Details_Storage[Details_Position]=result;
    Details_Position++;
}

int bndtls_r(int length) {
int i;
int result=0;
    for (i=0;i<length;i++) {
        if ((Master_Bluffer[i+General_Position]!=48)) result+=power(length-i-1);
    }
    General_Position+=length;
    treated+=length;
    Message_Details_Storage[Details_Position]=result;
    Details_Position++;
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
            Master_Bluffer[General_Position] = '\0';
        }
    }
    fclose(P);
}

void bru_file() {
    path_length = strlen(Path_Buffer);
    General_Position=0;
    DIR *dir;
    Compiled_File = fopen(strcat(Path_Buffer,"compile.txt"),"r");
    Path_Buffer[path_length]='\0';
    if (Compiled_File==NULL) {
        Compiled_File = fopen(strcat(Path_Buffer,"compile.txt"),"w+");
        Path_Buffer[path_length]='\0';
        struct dirent *ent;
        dir = opendir (Path_Buffer);
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_name[10]=='.') {
                bru_file_decode(strcat(Path_Buffer,ent->d_name));
                Path_Buffer[path_length]='\0';
            }
        }
        closedir (dir);
        fprintf(Compiled_File,"%s",Master_Bluffer);
        file_length=General_Position;
        General_Position = 0;
    } else {
        fseek(Compiled_File,0,SEEK_END);
        file_length=ftell(Compiled_File);
    }
    fclose(Compiled_File);
    Compiled_File=NULL;
}

void track_to_train_paquet(int nid_packet) {
    int i,j,iterations,iterations_bis,switcher,switcher_bis,memory,memory_bis;
    switch(nid_packet) {
        case 0:
            bndtls(6);
        break;
        case 2:
            bndtls(2);bndtls(13);bndtls(7);
        break;
        case 3:
            bndtls(2);bndtls(13);bndtls(2);bndtls(15);iterations = bndtls_r(5);
            for (i=0;i<iterations;i++) {
                bndtls(10);
            }
            bndtls(7);bndtls(7);bndtls(7);bndtls(7);bndtls(7);bndtls(15);bndtls(1);bndtls(1);bndtls(7);bndtls(7);bndtls(15);bndtls(8);bndtls(15);bndtls(2);bndtls(8);bndtls(1);bndtls(15);bndtls(1);
        break;
        case 5:
            bndtls(2);bndtls(13);bndtls(2);bndtls(15);switcher = bndtls_r(1);
            if (switcher==1) {
                bndtls(10);
            }
            bndtls(14);bndtls(1);bndtls(2);bndtls(6);iterations = bndtls_r(5);
            for (i=0;i<iterations;i++) {
                bndtls(15);switcher = bndtls_r(1);
                if (switcher==1) {
                    bndtls(10);
                }
                bndtls(14);bndtls(1);bndtls(2);bndtls(6);
            }
        break;
        case 6:
            bndtls(2);bndtls(13);switcher = bndtls_r(1);bndtls(6);bndtls(10);
            if (switcher==1) {
                bndtls(8);
            }
        break;
        case 12:
            bndtls(2);bndtls(13);bndtls(2);bndtls(7);bndtls(7);bndtls(10);iterations = bndtls_r(5);
            for (i=0;i<iterations;i++) {
                bndtls(15);switcher = bndtls_r(1);
                if (switcher==1) {
                    bndtls(10);bndtls(15);
                }
            }
            bndtls(15);switcher = bndtls_r(1);
            if (switcher==1) {
                bndtls(10);bndtls(15);
            }
            switcher = bndtls_r(1);
            if (switcher==1) {
                bndtls(10);bndtls(15);
            }
            switcher = bndtls_r(1);
            if (switcher==1) {
                bndtls(15);bndtls(7);
            }
            switcher = bndtls_r(1);
            if (switcher==1) {
                bndtls(15);bndtls(10);bndtls(15);bndtls(7);
            }
        break;
        case 13:
            bndtls(2);bndtls(13);bndtls(2);switcher = bndtls_r(1);
            if (switcher==1) {
                bndtls(10);
            }
            bndtls(14);bndtls(15);iterations = bndtls_r(5);
            for (i=0;i<iterations;i++) {
                switcher = bndtls_r(1);
                if (switcher==1) {
                    bndtls(10);
                }
                bndtls(14);bndtls(15);
            }
        break;
        case 15:
            bndtls(2);bndtls(13);bndtls(2);bndtls(7);bndtls(10);iterations = bndtls_r(5);
            for (i=0;i<iterations;i++) {
                bndtls(15);switcher = bndtls_r(1);
                if (switcher==1) {
                    bndtls(10);bndtls(15);
                }
            }
            bndtls(15);switcher = bndtls_r(1);
            if (switcher==1) {
                bndtls(10);bndtls(15);
            }
            switcher = bndtls_r(1);
            if (switcher==1) {
                bndtls(10);bndtls(15);
            }
            switcher = bndtls_r(1);
            if (switcher==1) {
                bndtls(15);bndtls(7);
            }
            switcher = bndtls_r(1);
            if (switcher==1) {
                bndtls(15);bndtls(10);bndtls(15);bndtls(7);
            }
        break;
        case 16:
            bndtls(2);bndtls(13);bndtls(2);bndtls(15);
        break;
        case 21:
            bndtls(2);bndtls(13);bndtls(2);bndtls(15);bndtls(1);bndtls(8);iterations = bndtls_r(5);
            for (i=0;i<iterations;i++) {
                bndtls(15);bndtls(1);bndtls(8);
            }
        break;
        case 27:
            bndtls(2);bndtls(13);bndtls(2);bndtls(15);bndtls(7);bndtls(1);iterations = bndtls_r(5);
            for (i=0;i<iterations;i++) {
                bndtls(4);bndtls(7);
            }
            iterations = bndtls_r(5);
            for (i=0;i<iterations;i++) {
                bndtls(15);bndtls(7);bndtls(1);iterations_bis = bndtls_r(5);
                for (j=0;j<iterations_bis;j++) {
                    bndtls(4);bndtls(7);
                }
            }
        break;
        case 39:
            bndtls(2);bndtls(13);bndtls(2);bndtls(15);bndtls(8);
        break;
        case 40:
            bndtls(2);bndtls(13);bndtls(2);bndtls(15);bndtls(10);
        break;
        case 41:
            bndtls(2);bndtls(13);bndtls(2);bndtls(15);switcher = bndtls_r(3);
            if (switcher==1) {
                bndtls(8);
            }
            bndtls(15);iterations = bndtls_r(5);
            for (i=0;i<iterations;i++) {
                switcher = bndtls_r(3);
                if (switcher==1) {
                    bndtls(8);
                }
                bndtls(15);
            }
        break;
    }
}

void compile_file_read() {
    MSG Escape_Message;
    SendMessage(ProgressBarWindow,PBM_SETPOS,0,0);
    ShowWindow(ProgressBarWindow, 1);
    int reference_tts,delta_tts,index=-1,v_train=0;
    char* Id_Name_Buffer = malloc(25*sizeof(char));
    char* List_Buffer = malloc(Message_Buffer_Size*sizeof(char));
    char* Q_DIR_Buffer = malloc(10*sizeof(char));
    char* Q_DLRBG_Buffer = malloc(10*sizeof(char));
    char* Driver_Id_Buffer = malloc(10*sizeof(char));
    char* M_LEVEL_Buffer = malloc(10*sizeof(char));
    char* M_MODE_Buffer = malloc(17*sizeof(char));
    double q_scale;
    unsigned long long delta_time = 0,delta_distance = 0,time_increment = 0;
    struct tm reference_timestamp = {0};
    struct tm current_timestamp = {0};
    reference_timestamp.tm_isdst=-1;
    current_timestamp.tm_isdst=-1;

    Details_Position = 0;

    Compiled_File = fopen(strcat(Path_Buffer,"compile.txt"),"r");
    Path_Buffer[path_length]='\0';
    fgets(Master_Bluffer,file_length,Compiled_File);
    fclose(Compiled_File);
    SendMessage(MessageWindow,LB_INITSTORAGE,(WPARAM)373333,(WPARAM)55999950);
    SendMessage(MessageWindow,WM_SETREDRAW,0,0);

    while (General_Position<file_length) {
        treated=0;
        int id = bindec(8);
        int l_message = bindec(10);
        int length = 112*((int)((l_message+1)/14)+1);
        if (length==8288) {
            PeekMessage(&Escape_Message,NULL,256,256,PM_REMOVE);
            if (Escape_Message.wParam==27) break;
            bindec(94);
            continue;
        }
        switch(id) {
            case 197:
            sprintf(Id_Name_Buffer,"ETAL_AUTO_ODOMETRIE");
            break;
            case 199:
            sprintf(Id_Name_Buffer,"CHANGEMENT_DE_MODE");
            break;
            case 200:
            sprintf(Id_Name_Buffer,"CHANGEMENT_DE_NIVEAU");
            break;
            case 198:
            sprintf(Id_Name_Buffer,"INFO_VITESSES_DISTANCE");
            break;
            case 2:
            sprintf(Id_Name_Buffer,"DATA_ENTRY_COMPLETED");
            break;
            case 3:
            sprintf(Id_Name_Buffer,"EMERGENCY_BRAKE_STATE");
            break;
            case 6:
            sprintf(Id_Name_Buffer,"MESSAGE_FROM_BALISE");
            break;
            case 9:
            sprintf(Id_Name_Buffer,"MESSAGE_FROM_RBC");
            break;
            case 10:
            sprintf(Id_Name_Buffer,"MESSAGE_TO_RBC");
            break;
            case 15:
            sprintf(Id_Name_Buffer,"PLAIN TEXT MESSAGE");
            break;
            case 11:
            sprintf(Id_Name_Buffer,"DRIVER_ACTIONS");
            break;
            case 5:
            sprintf(Id_Name_Buffer,"EVENTS");
            break;
            case 1:
            sprintf(Id_Name_Buffer,"GENERAL_MESSAGE");
            break;
        }

        current_timestamp.tm_year = bindec(7)+100;
        current_timestamp.tm_mon = bindec(4)-1;
        current_timestamp.tm_mday = bindec(5);
        current_timestamp.tm_hour = bindec(5);
        current_timestamp.tm_min = bindec(6);
        current_timestamp.tm_sec = bindec(6);
        int tts = 50*bindec(5);
        delta_tts=tts-reference_tts;

        time_increment = (unsigned long long)(1000*(unsigned long long)difftime(mktime(&current_timestamp),mktime(&reference_timestamp)))+(unsigned long long)(delta_tts);
        delta_time +=time_increment;
        reference_timestamp = current_timestamp;
        reference_tts=tts;
        delta_distance+=(time_increment*v_train);
        if (index==-1 || current_timestamp.tm_year ==172) {
            Distance_Storage[index+1]=0;
            Time_Storage[index+1]=0;
            delta_time=0;
        } else {
            Distance_Storage[index+1]=delta_distance;
            Time_Storage[index+1]=delta_time;
        }
        int q_scale_read = bindec(2);
        if (q_scale_read==1) q_scale = 1.0;
        if (q_scale_read==0) q_scale = 0.1;
        if (q_scale_read==2) q_scale = 10.0;
        int nid_c = bindec(10);
        int nid_bg = bindec(15)/2;
        int d_lrbg = bindec(14)*q_scale;
        int q_dir = bindec(2);
        switch(q_dir) {
            case 0:
                sprintf(Q_DIR_Buffer,"Reverse");
            break;
            case 1:
                sprintf(Q_DIR_Buffer,"Nominal");
            break;
            case 2:
                sprintf(Q_DIR_Buffer,"Both");
            break;
        }
        int q_dlrbg = bindec(2);
        switch(q_dlrbg) {
            case 0:
                sprintf(Q_DLRBG_Buffer,"Reverse");
            break;
            case 1:
                sprintf(Q_DLRBG_Buffer,"Nominal");
            break;
            case 2:
                sprintf(Q_DLRBG_Buffer,"Both");
            break;
        }
        int l_doubtover = q_scale*bindec(15);
        int l_doubtunder = q_scale*bindec(15);
        v_train = 5*bindec(7);
        int id_driver = bindec(32);
        if (id_driver > 999999 && id_driver < 10000000) sprintf(Driver_Id_Buffer,"%dX",id_driver); else if (id_driver==-1) sprintf(Driver_Id_Buffer,"NC"); else sprintf(Driver_Id_Buffer,"%d",id_driver);
        hexdec(28); General_Position+=4; treated+=4;
        int m_level = bindec(3);
        switch(m_level) {
            case 0:
                sprintf(M_LEVEL_Buffer,"Level 0");
            break;
            case 1:
                sprintf(M_LEVEL_Buffer,"Level STM");
            break;
            case 2:
                sprintf(M_LEVEL_Buffer,"Level 1");
            break;
            case 3:
                sprintf(M_LEVEL_Buffer,"Level 2");
            break;
            case 4:
                sprintf(M_LEVEL_Buffer,"Level 3");
            break;
        }
        int m_mode = bindec(4);
        switch(m_mode) {
            case 0:
                sprintf(M_MODE_Buffer,"Full Supervision");
            break;
            case 1:
                sprintf(M_MODE_Buffer,"On Sight");
            break;
            case 2:
                sprintf(M_MODE_Buffer,"Staff Responsible");
            break;
            case 3:
                sprintf(M_MODE_Buffer,"Shunting");
            break;
            case 4:
                sprintf(M_MODE_Buffer,"Unfitted");
            break;
            case 5:
                sprintf(M_MODE_Buffer,"Sleeping");
            break;
            case 6:
                sprintf(M_MODE_Buffer,"Stand By");
            break;
            case 7:
                sprintf(M_MODE_Buffer,"Trip");
            break;
            case 8:
                sprintf(M_MODE_Buffer,"Post Trip");
            break;
            case 9:
                sprintf(M_MODE_Buffer,"System Failure");
            break;
            case 10:
                sprintf(M_MODE_Buffer,"Isolation");
            break;
            case 11:
                sprintf(M_MODE_Buffer,"Non Leading");
            break;
            case 12:
                sprintf(M_MODE_Buffer,"Limited Supervision");
            break;
            case 13:
                sprintf(M_MODE_Buffer,"National System");
            break;
            case 14:
                sprintf(M_MODE_Buffer,"Reversing");
            break;
            case 15:
                sprintf(M_MODE_Buffer,"Passive Shunting");
            break;
        }
        sprintf(List_Buffer,"%s\t%02d/%02d/%02d  %02d:%02d:%02d:%03d    %.1f\t%d\t%d\t%d m\t%s/%s\t%d/%d\t%d km/h\t%s\t%s\t%s\t%s",Id_Name_Buffer,current_timestamp.tm_mday,current_timestamp.tm_mon+1,current_timestamp.tm_year-100,current_timestamp.tm_hour,current_timestamp.tm_min,current_timestamp.tm_sec,tts,q_scale,nid_c,nid_bg,d_lrbg,Q_DIR_Buffer,Q_DLRBG_Buffer,l_doubtunder,l_doubtover,v_train,Driver_Id_Buffer,Hexdec_Buffer,M_LEVEL_Buffer,M_MODE_Buffer);
        index = SendMessage(MessageWindow,LB_ADDSTRING,(WPARAM)0, (LPARAM)List_Buffer);
        SendMessage(MessageWindow,LB_SETITEMDATA,index,Details_Position);

        //Ecriture des données supplémentaires aux messages

        switch(id) {
            case 2:
                bndtls(7);bndtls(15);bndtls(12);bndtls(3);bndtls(13);bndtls(10);bndtls(10);bndtls(13);bndtls(10);bndtls(13);bndtls(10);bndtls(13);
                bndtls(10);bndtls(13);bndtls(10);bndtls(13);bndtls(10);bndtls(13);bndtls(3);bndtls(13);bndtls(10);bndtls(10);bndtls(13);bndtls(10);
                bndtls(13);bndtls(10);bndtls(13);bndtls(10);bndtls(13);bndtls(10);bndtls(13);bndtls(10);bndtls(13);bndtls(12);bndtls(12);bndtls(12);
                bndtls(8);bndtls(7);bndtls(8);bndtls(8);bndtls(2);bndtls(1);bndtls(10);bndtls(14);bndtls(16);bndtls(16);bndtls(16);bndtls(16);
            break;
            case 3:
                bndtls(1);
            break;
            case 5:
                bndtls(8);
            break;
            case 6:
                bndtls(1);bndtls(7);bndtls(1);bndtls(3);bndtls(3);bndtls(2);bndtls(8);bndtls(10);bndtls(14);bndtls(1);
                while((Message_Details_Storage[Details_Position]=-bindec(8))!=-255) {
                    track_to_train_paquet(Message_Details_Storage[Details_Position]);
                    Details_Position++;
                }
                Details_Position++;
            break;
            case 9:

            break;
            case 10:

            break;
            case 11:
                bndtls(8);
            break;
            case 15:

            break;
            case 198:
                bndtls(10);bndtls(15);bndtls(2);bndtls(10);bndtls(10);
            break;
            case 199:
                bndtls(4);bndtls(2);
            break;
            case 200:
                bndtls(3);bndtls(8);bndtls(2);
            break;
        }

        //Fin d'écriture des données supplémentaires aux messages

        SendMessage(ProgressBarWindow,PBM_SETPOS,(int)(100*General_Position/file_length),0);
        UpdateWindow(ProgressBarWindow);
        General_Position+=length-treated;
    }
    SendMessage(MessageWindow,WM_SETREDRAW,1,0);

    free(Id_Name_Buffer);
    free(List_Buffer);
    free(Q_DIR_Buffer);
    free(Q_DLRBG_Buffer);
    free(Driver_Id_Buffer);
    free(M_LEVEL_Buffer);
    free(M_MODE_Buffer);

    General_Position=0;
    ShowWindow(ProgressBarWindow, 0);
}

LRESULT CALLBACK WndProc(HWND hwndm, UINT msg, WPARAM wParam, LPARAM lParam) {
    int tabs[11]={120,352,412,490,568,705,785,860,935,1000,1080};
    switch(msg)
    {
        HDC hdc ;
        PAINTSTRUCT ps ;
        PMEASUREITEMSTRUCT pmis;
        LPDRAWITEMSTRUCT Item;
        HBRUSH MESSAGE_BRUSH;
        RECT rect ;
        case WM_MEASUREITEM:
            pmis = (PMEASUREITEMSTRUCT) lParam;
            pmis->itemHeight = 17;
            return TRUE;
        break;
        case WM_COMMAND:
            if (HIWORD(wParam)==LBN_SELCHANGE) {
                HWND hwndselchange = LOWORD(lParam);
                PostMessage(hwndselchange,SEL_CHANGE_MESSAGE,0,0);
                return true;
            }
            if (HIWORD(wParam)==BN_CLICKED) {
                PostMessage(hwndm,BTN_CLICKED_MESSAGE,(WPARAM)lParam,(LPARAM)0);
                return true;
            }
        return DefWindowProc(hwndm, msg, wParam, lParam);
        case WM_CTLCOLORSTATIC:
            hdc = (HDC)wParam;
            if ((HWND)lParam==TimeWindow || (HWND)lParam==DistanceWindow || (HWND)lParam==SpeedWindow) {
                SetBkColor(hdc, RGB(255,173,92));
                return (INT_PTR)Orange_Edit_Brush;
            }
            SetBkColor(hdc, RGB(30,139,200));
            SetTextColor(hdc,RGB(255,0,255));
            return (INT_PTR)Background_brush;
        break;
        case WM_CTLCOLOREDIT:
            //if ((HWND)lParam==hwnd28) return 0;
            hdc = (HDC) wParam;
            if ((HWND)lParam==FromWindow || (HWND)lParam==ToWindow) {
                SetBkColor(hdc, RGB(211,145,255));
                return (INT_PTR)Purple_Edit_Brush;
            }
                SetBkColor(hdc, RGB(92,255,97));
                return (INT_PTR)Green_Edit_Brush;
        break;
        case WM_DRAWITEM:
            Item = (LPDRAWITEMSTRUCT) lParam;
            int len = SendMessage(Item->hwndItem , LB_GETTEXT, (WPARAM)Item->itemID, (LPARAM)Message_Buffer);
                if (len>2) {
                    if (Item->hwndItem==MessageHeaderWindow) {
                        SetBkColor(Item->hDC, RGB(255,255,255));
                        SetTextColor(Item->hDC, RGB(0,0,0));
                    }
                    else {
                        if (Item->itemState & ODS_FOCUS || Item->itemState & ODS_SELECTED || Item->itemID==Reference_Message)
                        {
                            SetTextColor(Item->hDC, RGB(255,255,255));
                            if (Message_Buffer[0]==67 && Message_Buffer[14]==77) {
                                MESSAGE_BRUSH = CreateSolidBrush(RGB(157,0, 255));
                                SetBkColor(Item->hDC, RGB(157, 0, 255));
                                FillRect(Item->hDC, &Item->rcItem, (HBRUSH)MESSAGE_BRUSH);
                                DeleteObject(MESSAGE_BRUSH);
                            }
                            else if (Message_Buffer[0]==68 && Message_Buffer[1]==65) {
                                MESSAGE_BRUSH = CreateSolidBrush(RGB(255,90,215));
                                SetBkColor(Item->hDC, RGB(255,90,215));
                                FillRect(Item->hDC, &Item->rcItem, (HBRUSH)MESSAGE_BRUSH);
                                DeleteObject(MESSAGE_BRUSH);
                            }
                            else if (Message_Buffer[0]==73) {
                                MESSAGE_BRUSH = CreateSolidBrush(RGB(90,255,150));
                                SetBkColor(Item->hDC, RGB(90,255,150));
                                FillRect(Item->hDC, &Item->rcItem, (HBRUSH)MESSAGE_BRUSH);
                                DeleteObject(MESSAGE_BRUSH);
                            }
                            else if (Message_Buffer[0]==67 && Message_Buffer[14]==78) {
                                MESSAGE_BRUSH = CreateSolidBrush(RGB(0,0, 255));
                                SetBkColor(Item->hDC, RGB(0, 0, 255));
                                FillRect(Item->hDC, &Item->rcItem, (HBRUSH)MESSAGE_BRUSH);
                                DeleteObject(MESSAGE_BRUSH);
                            }
                            else if (Message_Buffer[0]==77 && Message_Buffer[13]==66) {
                                MESSAGE_BRUSH = CreateSolidBrush(RGB(0,200, 255));
                                SetBkColor(Item->hDC, RGB(0,200, 255));
                                FillRect(Item->hDC, &Item->rcItem, (HBRUSH)MESSAGE_BRUSH);
                                DeleteObject(MESSAGE_BRUSH);
                            }
                            else if (Message_Buffer[0]==77 && Message_Buffer[13]==82) {
                                MESSAGE_BRUSH = CreateSolidBrush(RGB(0,220, 0));
                                SetBkColor(Item->hDC, RGB(0,220, 0));
                                FillRect(Item->hDC, &Item->rcItem, (HBRUSH)MESSAGE_BRUSH);
                                DeleteObject(MESSAGE_BRUSH);
                            }
                            else if (Message_Buffer[0]==77 && Message_Buffer[8]==84 && Message_Buffer[11]==82) {
                                MESSAGE_BRUSH = CreateSolidBrush(RGB(255,175, 0));
                                SetBkColor(Item->hDC, RGB(255,175, 0));
                                FillRect(Item->hDC, &Item->rcItem, (HBRUSH)MESSAGE_BRUSH);
                                DeleteObject(MESSAGE_BRUSH);
                            }
                            else if (Message_Buffer[0]==69 && Message_Buffer[1]==77) {
                                MESSAGE_BRUSH = CreateSolidBrush(RGB(255,0, 0));
                                SetBkColor(Item->hDC, RGB(255,0, 0));
                                FillRect(Item->hDC, &Item->rcItem, (HBRUSH)MESSAGE_BRUSH);
                                DeleteObject(MESSAGE_BRUSH);
                            }
                            else {
                                MESSAGE_BRUSH = CreateSolidBrush(RGB(0,0, 0));
                                SetBkColor(Item->hDC, RGB(0, 0, 0));
                                FillRect(Item->hDC, &Item->rcItem, (HBRUSH)MESSAGE_BRUSH);
                                DeleteObject(MESSAGE_BRUSH);
                            }
                        }
                        else
                        {
                            SetTextColor(Item->hDC, RGB(0,0,0));
                            SetBkColor(Item->hDC, RGB(220, 220, 220));
                            FillRect(Item->hDC, &Item->rcItem, (HBRUSH)Gray_Brush);
                            if (Message_Buffer[0]==73) SetTextColor(Item->hDC, RGB(90,255,150));
                            if (Message_Buffer[0]==68 && Message_Buffer[1]==65) SetTextColor(Item->hDC, RGB(255,90,215));
                            if (Message_Buffer[0]==67 && Message_Buffer[14]==77) SetTextColor(Item->hDC, RGB(157,0,255));
                            if (Message_Buffer[0]==67 && Message_Buffer[14]==78) SetTextColor(Item->hDC, RGB(0,0,255));
                            if (Message_Buffer[0]==77 && Message_Buffer[13]==66) SetTextColor(Item->hDC, RGB(0,200,255));
                            if (Message_Buffer[0]==77 && Message_Buffer[13]==82) SetTextColor(Item->hDC, RGB(0,220,0));
                            if (Message_Buffer[0]==77 && Message_Buffer[8]==84 && Message_Buffer[11]==82) SetTextColor(Item->hDC, RGB(255,175,0));
                            if (Message_Buffer[0]==69 && Message_Buffer[1]==77) SetTextColor(Item->hDC, RGB(255,0,0));
                        }
                    }
                    TabbedTextOut(Item->hDC,Item->rcItem.left, Item->rcItem.top,(LPCSTR)Message_Buffer,len,11,(LPINT)&tabs,140);
                }
            return TRUE;
        case WM_PAINT:
            hdc = BeginPaint (hwndm, &ps);
            GetClientRect(hwndm, &rect);
            SetBkColor(hdc, RGB(230,50,2));
            SetBkMode(hdc,TRANSPARENT);
            FillRect(hdc,&ps.rcPaint,Main_Brush);
            SelectObject(hdc,Main_Font);
		  if (hwndm==MainWindow) {
                DrawText(hdc, TEXT("Message list : "), -1, &rect, DT_CENTER );
                DrawText(hdc, TEXT("Copyrights UGC : 2016-2999"), -1, &rect, DT_LEFT );
                TextOut(hdc,1470,95,"From (dd/mm/yy) : ",18);
                TextOut(hdc,1470,155,"To (dd/mm/yy) : ",16);
                TextOut(hdc,1470,580,"Line : ",6);
                TextOut(hdc,1470,620,"Message : ",10);
                TextOut(hdc,1470,660,"Date : ",6);
                TextOut(hdc,1470,700,"Train                               Balise",42);
                TextOut(hdc,1470,740,"Packet              Radio Message",33);
                TextOut(hdc,1470,780,"Speed (min-max): ",18);
                TextOut(hdc,1470,820,"Level                                Mode",41);
                TextOut(hdc,1568,837,"-",1);
                TextOut(hdc,1568,757,"-",1);
                TextOut(hdc,1568,797,"-",1);
                TextOut(hdc,1568,717,"-",1);
                TextOut(hdc,1625,490,"CSV",3);
                TextOut(hdc,1470,490,"Encode",6);
                TextOut(hdc,350,525,"Reference",10);
                TextOut(hdc,600,525,"Elapsed time : ",14);
                TextOut(hdc,850,525,"Distance : ",10);
                TextOut(hdc,1100,525,"Average speed : ",16);
                TextOut(hdc,1320,525,"Start / Stop : ",14);
                TextOut(hdc,1366,545,"Day : ",5);
            }
            EndPaint (hwndm, &ps);
            return 0 ;
        default:
            return DefWindowProc(hwndm, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    Gray_Brush = CreateSolidBrush(RGB(220,220,220));
    Main_Brush = CreateSolidBrush(RGB(30,139,200));
    Background_brush = CreateSolidBrush(RGB(30,139,200));
    Green_Edit_Brush =  CreateSolidBrush(RGB(92,255,97));
    Orange_Edit_Brush = CreateSolidBrush(RGB(255,173,92));
    Purple_Edit_Brush = CreateSolidBrush(RGB(211,145,255));
    MSG Current_Message;
    WNDCLASSEX wc;
    unsigned long long Current_Time;
    unsigned long long Current_Distance;
    unsigned long long Reference_Time;
    unsigned long long Reference_Distance;
    unsigned long long Delta_Time;
    unsigned long long Delta_Distance;
    unsigned long long Average_Speed;
    int Message_Count=0;
    int Delta_Hour;
    int Delta_Minute;
    int Delta_Second;

    Main_Font = CreateFont(15,6,0,0,600,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, DEFAULT_PITCH,TEXT("Arial"));

    char* DistanceWindow_Buffer = malloc(Window_Buffer_Size*sizeof(char));
    char* TimeWindow_Buffer = malloc(Window_Buffer_Size*sizeof(char));
    char* SpeedWindow_Buffer = malloc(Window_Buffer_Size*sizeof(char));

    Master_Bluffer = malloc(Master_Bluffer_Size*sizeof(char));
    Path_Buffer = malloc(Path_Buffer_Size*sizeof(char));
    Hexdec_Buffer = malloc(32*sizeof(char));
    Message_Buffer = malloc(Message_Buffer_Size*sizeof(char));
    Details_Data_Buffer = malloc(Details_Data_Buffer_Size*sizeof(char));
    Details_Data_Buffer[30]='\0';

    sprintf(Path_Buffer,"C:\\Users\\ugc\\Desktop\\43052170116_JRU\\flash24h\\");

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(hInstance,"MAINICON");
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(hInstance,"MAINICWinMainON");
    RegisterClassEx(&wc);

    MainWindow = CreateWindowEx(WS_EX_CLIENTEDGE,g_szClassName,"JRUGC - Outil interne THIF de décodage des JRU - V 3.0.0-P",WS_OVERLAPPEDWINDOW|WS_MAXIMIZE,0, 0, 1700, 1000,NULL, NULL, hInstance, NULL);
    MessageWindow = CreateWindowEx(0,"LISTBOX" ,"Data",WS_CHILD |WS_VSCROLL |WS_BORDER|WS_THICKFRAME | LBS_USETABSTOPS | LBS_NOTIFY | LBS_HASSTRINGS | LBS_OWNERDRAWFIXED,15,70,1450,450,MainWindow,(HMENU) NULL,hInstance,NULL);
    ProgressBarWindow = CreateWindowEx(0,PROGRESS_CLASS,NULL,WS_CHILD |WS_BORDER |PBS_SMOOTH,200,0,250,25,MainWindow,(HMENU) NULL,hInstance,NULL);
    QuitButtonWindow = CreateWindowEx((DWORD)0,"BUTTON","QUIT",WS_CHILD |WS_BORDER,1595,5,75,25,MainWindow,(HMENU) NULL,hInstance,NULL);
    MessageHeaderWindow = CreateWindowEx(0,"LISTBOX" ,"Data",WS_CHILD |WS_HSCROLL |WS_VSCROLL |WS_BORDER|WS_THICKFRAME | LBS_USETABSTOPS | LBS_OWNERDRAWFIXED | LBS_NOTIFY | LBS_HASSTRINGS,15,30,1450,15,MainWindow,(HMENU) NULL,hInstance,NULL);
    FromWindow = CreateWindowEx((DWORD)0,"EDIT","",WS_CHILD |WS_BORDER,1470,120,200,20,MainWindow,(HMENU) NULL,hInstance,NULL);
    ToWindow = CreateWindowEx((DWORD)0,"EDIT","",WS_CHILD |WS_BORDER,1470,180,200,20,MainWindow,(HMENU) NULL,hInstance,NULL);
    TimeWindow = CreateWindowEx((DWORD)0,"EDIT","No Reference",WS_CHILD |WS_BORDER | ES_READONLY,600,545,200,20,MainWindow,(HMENU) NULL,hInstance,NULL);
    DistanceWindow = CreateWindowEx((DWORD)0,"EDIT","No Reference",WS_CHILD |WS_BORDER | ES_READONLY,850,545,200,20,MainWindow,(HMENU) NULL,hInstance,NULL);
    SpeedWindow = CreateWindowEx((DWORD)0,"EDIT","No Reference",WS_CHILD |WS_BORDER | ES_READONLY,1100,545,200,20,MainWindow,(HMENU) NULL,hInstance,NULL);
    SetButtonWindow = CreateWindowEx((DWORD)0,"BUTTON","Set",WS_CHILD |WS_BORDER,300,545,50,20,MainWindow,(HMENU) NULL,hInstance,NULL);
    GoToButtonWindow = CreateWindowEx((DWORD)0,"BUTTON","GoTo",WS_CHILD |WS_BORDER,360,545,50,20,MainWindow,(HMENU) NULL,hInstance,NULL);
    ClearButtonWindow = CreateWindowEx((DWORD)0,"BUTTON","Clear",WS_CHILD |WS_BORDER,420,545,50,20,MainWindow,(HMENU) NULL,hInstance,NULL);

    SendMessage(MessageHeaderWindow,LB_ADDSTRING,(WPARAM)0,(LPARAM)"Message\tDate        Time                  Scale(m)\tNID_C\tNID_LRBG\tD_LRBG\tD/D\tL+/L-\tV (km/h)\tDRIVER\tTRAIN\tLEVEL\tMODE");

    ShowWindow(MainWindow, 3);
    ShowWindow(MessageWindow, nCmdShow);
    ShowWindow(QuitButtonWindow, nCmdShow);
    ShowWindow(MessageHeaderWindow, nCmdShow);
    ShowWindow(FromWindow, nCmdShow);
    ShowWindow(ToWindow, nCmdShow);
    ShowWindow(TimeWindow, nCmdShow);
    ShowWindow(DistanceWindow, nCmdShow);
    ShowWindow(SpeedWindow, nCmdShow);
    ShowWindow(SetButtonWindow, nCmdShow);
    ShowWindow(GoToButtonWindow, nCmdShow);
    ShowWindow(ClearButtonWindow, nCmdShow);

    SendMessage(MessageWindow,WM_SETFONT,(WPARAM)Main_Font,(LPARAM)TRUE);
    SendMessage(MessageHeaderWindow,WM_SETFONT,(WPARAM)Main_Font,(LPARAM)TRUE);
    SendMessage(QuitButtonWindow,WM_SETFONT,(WPARAM)Main_Font,(LPARAM)TRUE);
    SendMessage(DistanceWindow,WM_SETFONT,(WPARAM)Main_Font,(LPARAM)TRUE);
    SendMessage(TimeWindow,WM_SETFONT,(WPARAM)Main_Font,(LPARAM)TRUE);
    SendMessage(SpeedWindow,WM_SETFONT,(WPARAM)Main_Font,(LPARAM)TRUE);
    SendMessage(SetButtonWindow,WM_SETFONT,(WPARAM)Main_Font,(LPARAM)TRUE);
    SendMessage(GoToButtonWindow,WM_SETFONT,(WPARAM)Main_Font,(LPARAM)TRUE);
    SendMessage(ClearButtonWindow,WM_SETFONT,(WPARAM)Main_Font,(LPARAM)TRUE);

    while (GetMessage(&Current_Message, NULL, 0, 0)>0)
    {
        Message_Position = SendMessage(MessageWindow,LB_GETCURSEL,0,0);
        Current_Time = Time_Storage[Message_Position];
        Current_Distance = Distance_Storage[Message_Position];
        Details_Position = SendMessage(MessageWindow,LB_GETITEMDATA,Message_Position,0);

        if (Current_Message.message == SEL_CHANGE_MESSAGE) {
            if (Reference_Message>=0) {
                Delta_Hour = 0;
                Delta_Minute = 0;
                Delta_Second = 0;

                if (Reference_Message<=Message_Position) {
                    Delta_Time = Current_Time-Reference_Time;
                    Delta_Distance = Current_Distance-Reference_Distance;
                }
                else {
                    Delta_Time = Reference_Time-Current_Time;
                    Delta_Distance = Reference_Distance-Current_Distance;
                }

                if (Delta_Time>0) Average_Speed = (3600*Delta_Distance/Delta_Time); else Average_Speed=0;

                while (Delta_Time>=3600000) {Delta_Time-=3600000; Delta_Hour++;}
                while (Delta_Time>=60000) {Delta_Time-=60000; Delta_Minute++;}
                while (Delta_Time>=1000) {Delta_Time-=1000; Delta_Second++;}

                sprintf(TimeWindow_Buffer,"%02d h %02d m %02d s %03I64u ms",Delta_Hour,Delta_Minute,Delta_Second,Delta_Time);
                sprintf(DistanceWindow_Buffer,"%I64u km %03I64u m",Delta_Distance/3600000,(Delta_Distance/3600)%1000);
                sprintf(SpeedWindow_Buffer,"%I64u km/h",Average_Speed/3600);

                SetWindowText(TimeWindow,TimeWindow_Buffer);
                SetWindowText(DistanceWindow,DistanceWindow_Buffer);
                SetWindowText(SpeedWindow,SpeedWindow_Buffer);
            }
        }

        if (Current_Message.message==WM_KEYDOWN) {
            if (Current_Message.wParam==13) {
                SendMessage(MessageWindow,LB_RESETCONTENT,0,0);
                bru_file();
                compile_file_read();
                Message_Count=SendMessage(MessageWindow,LB_GETCOUNT,0,0);
            }
            if (Current_Message.wParam==VK_SHIFT) shift = true;
            if (Current_Message.wParam==VK_CONTROL) control = true;
            if (Current_Message.wParam=='Q' && control) break;
        }
        if (Current_Message.message==WM_KEYUP) {
            if (Current_Message.wParam==VK_SHIFT) shift = false;
            if (Current_Message.wParam==VK_CONTROL) control = false;
        }
        if (Current_Message.message==BTN_CLICKED_MESSAGE) {
            if (Current_Message.wParam==(WPARAM)QuitButtonWindow) break;
        }
        if (((Current_Message.message==WM_LBUTTONDBLCLK && Current_Message.hwnd==MessageWindow) || (Current_Message.message==WM_KEYDOWN && Current_Message.wParam=='S')) || (Current_Message.message==BTN_CLICKED_MESSAGE && Current_Message.wParam==(WPARAM)SetButtonWindow)) {
            Reference_Message = Message_Position;
            Reference_Time = Time_Storage[Message_Position];
            Reference_Distance = Distance_Storage[Message_Position];
            SetWindowText(TimeWindow,"00 h 00 m 00 s 000 ms");
            SetWindowText(DistanceWindow,"0 km 000 m");
            SetWindowText(SpeedWindow,"0 km/h");
            RedrawWindow(MessageWindow,NULL,NULL,RDW_INVALIDATE);
            continue;
        }
        if ((Current_Message.message==WM_KEYDOWN && Current_Message.wParam=='C' && Reference_Message>=0) || (Current_Message.message==BTN_CLICKED_MESSAGE && Current_Message.wParam==(WPARAM)ClearButtonWindow)) {
            Reference_Message = -1;
            SetWindowText(TimeWindow,"No Reference");
            SetWindowText(DistanceWindow,"No Reference");
            SetWindowText(SpeedWindow,"No Reference");
            RedrawWindow(MessageWindow,NULL,NULL,RDW_INVALIDATE);
            continue;
        }
        if ((Current_Message.message==WM_KEYDOWN && Current_Message.wParam=='G' && Reference_Message>=0) || (Current_Message.message==BTN_CLICKED_MESSAGE && Current_Message.wParam==(WPARAM)GoToButtonWindow)) {
            SendMessage(MessageWindow,LB_SETCURSEL,Reference_Message,0);
            SetFocus(MessageWindow);
            continue;
        }
        TranslateMessage(&Current_Message);
        DispatchMessage(&Current_Message);
    }

    free(Master_Bluffer);
    free(Message_Buffer);
    free(Path_Buffer);
    free(Hexdec_Buffer);
    free(Details_Data_Buffer);
    free(DistanceWindow_Buffer);
    free(TimeWindow_Buffer);
    free(SpeedWindow_Buffer);

    DeleteObject(Gray_Brush);
    DeleteObject(Main_Brush);
    DeleteObject(Main_Font);

    return 0;
}

