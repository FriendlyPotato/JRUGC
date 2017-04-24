#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <windows.h>
#include <winuser.h>
#include <wingdi.h>
#include <CommCtrl.h>
#include <stdbool.h>

//Size of the buffer to read the entire file
#define Master_Bluffer_Size 115000000
//Size of the buffer to store the path name
#define Path_Buffer_Size 500
//Size of the buffer to store the message
#define Message_Buffer_Size 300
//Size of the buffer to store the message details permanently
#define Details_Buffer_Size 500000
//Size of the buffer to store the message details temporarily
#define Details_Bindec_Buffer_Size 10

//Code for clicked button message
#define BN_CLICKED_MESSAGE 9998
//Code for selection changed message
#define SEL_CHANGE_MESSAGE 9999

char* Path_Buffer;
char* Master_Bluffer;
char* Hexdec_Buffer;
char* Message_Buffer;
char* Details_Buffer;
char* Details_Bindec_Buffer;
bool control;
bool shift;
FILE* Detailed_File;
FILE* Compiled_File;
int path_length;
long long file_length;
long long pos;
int treated=0;
HWND MainWindow,MessageWindow,ProgressBarWindow,QuitButtonWindow,MessageHeaderWindow,FromWindow,ToWindow,TimeWindow,DistanceWindow,SpeedWindow;
HFONT Main_Font;
HBRUSH Main_Brush,Gray_Brush,Orange_Edit_Brush,Green_Edit_Brush,Purple_Edit_Brush,Background_brush;


const char g_szClassName[] = "myWindowClass";


void appendNumber(int value){
    long i;
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

void hexdec(int length) {
    int i = 0;
    int num = 0;
    do {
        int b = ((Master_Bluffer[pos+i])=='1'?1:0);
        i++;
        num = (num<<1)|b;
    } while (i<length);
    Hexdec_Buffer[length]='\0';
    sprintf(Hexdec_Buffer,"%X",num);
    pos+=length;
    treated+=length;
}

long bindec(int length) {
int i;
long result=0;
    for (i=0;i<length;i++) {
        if ((Master_Bluffer[i+pos]!=48)) result+=power(length-i-1);
    }
pos+=length;
treated+=length;
return result;
}

void details_bindec(int length) {

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
    path_length = strlen(Path_Buffer);
    pos=0;
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
        file_length=pos;
        pos = 0;
    } else {
        fseek(Compiled_File,0,SEEK_END);
        file_length=ftell(Compiled_File);
    }
    fclose(Compiled_File);
    Compiled_File=NULL;
}

void compile_file_read() {
    SendMessage(ProgressBarWindow,PBM_SETPOS,0,0);
    ShowWindow(ProgressBarWindow, 1);
    char* Id_Name_Buffer = malloc(25*sizeof(char));
    char* List_Buffer = malloc(Message_Buffer_Size*sizeof(char));
    char* Q_DIR_Buffer = malloc(10*sizeof(char));
    char* Q_DLRBG_Buffer = malloc(10*sizeof(char));
    char* Driver_Id_Buffer = malloc(10*sizeof(char));
    char* M_LEVEL_Buffer = malloc(10*sizeof(char));
    char* M_MODE_Buffer = malloc(17*sizeof(char));
    double q_scale;
    Compiled_File = fopen(strcat(Path_Buffer,"compile.txt"),"r");
    Path_Buffer[path_length]='\0';
    fgets(Master_Bluffer,file_length,Compiled_File);
    fclose(Compiled_File);
    SendMessage(MessageWindow,LB_INITSTORAGE,(WPARAM)373333,(WPARAM)55999950);
    SendMessage(MessageWindow,WM_SETREDRAW,0,0);
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
        int v_train = 5*bindec(7);
        int id_driver = bindec(32);
        if (id_driver > 999999 && id_driver < 10000000) sprintf(Driver_Id_Buffer,"%dX",id_driver); else if (id_driver==-1) sprintf(Driver_Id_Buffer,"NC"); else sprintf(Driver_Id_Buffer,"%d",id_driver);
        hexdec(28); pos+=4; treated+=4;
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
        pos+=length-treated;
        sprintf(List_Buffer,"%s\t%02d/%02d/%02d  %02d:%02d:%02d:%03d    %.1f\t%d\t%d\t%d m\t%s/%s\t%d/%d\t%d km/h\t%s\t%s\t%s\t%s",Id_Name_Buffer,day,month,year,hour,minute,second,tts,q_scale,nid_c,nid_bg,d_lrbg,Q_DIR_Buffer,Q_DLRBG_Buffer,l_doubtunder,l_doubtover,v_train,Driver_Id_Buffer,Hexdec_Buffer,M_LEVEL_Buffer,M_MODE_Buffer);
        int index = SendMessage(MessageWindow,LB_ADDSTRING,(WPARAM)0, (LPARAM)List_Buffer);
        SendMessage(MessageWindow,LB_SETITEMDATA,index,pos);
        SendMessage(ProgressBarWindow,PBM_SETPOS,(int)(100*pos/file_length),0);
        UpdateWindow(ProgressBarWindow);
        switch(id) {
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
    }
    SendMessage(MessageWindow,WM_SETREDRAW,1,0);
    free(Id_Name_Buffer);
    free(List_Buffer);
    free(Q_DIR_Buffer);
    free(Q_DLRBG_Buffer);
    free(Driver_Id_Buffer);
    free(M_LEVEL_Buffer);
    free(M_MODE_Buffer);
    pos=0;
    ShowWindow(ProgressBarWindow, 0);
    Detailed_File = fopen(strcat(Path_Buffer,"details.txt"),"w+");
    Path_Buffer[path_length]='\0';
    fprintf(Detailed_File,"%s",Details_Buffer);
    fclose(Detailed_File);
}

LRESULT CALLBACK WndProc(HWND hwndm, UINT msg, WPARAM wParam, LPARAM lParam) {
    int tabs[13]={120,352,412,490,568,705,785,860,935,1000,1080,8000,610};
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
                PostMessage(hwndm,BN_CLICKED_MESSAGE,(WPARAM)lParam,(LPARAM)0);
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
                        if (Item->itemState & ODS_FOCUS || Item->itemState & ODS_SELECTED)
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
                    TabbedTextOut(Item->hDC,Item->rcItem.left, Item->rcItem.top,(LPCSTR)Message_Buffer,len,13,(LPINT)&tabs,140);
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
    MSG Msg;
    WNDCLASSEX wc;

    Main_Font = CreateFont(15,6,0,0,600,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, DEFAULT_PITCH,TEXT("Arial"));
    Master_Bluffer = malloc(Master_Bluffer_Size*sizeof(char));
    Path_Buffer = malloc(Path_Buffer_Size*sizeof(char));
    Hexdec_Buffer = malloc(32*sizeof(char));
    Message_Buffer = malloc(Message_Buffer_Size*sizeof(char));
    Details_Buffer = malloc(Details_Buffer_Size*sizeof(char));
    Details_Bindec_Buffer = malloc(Details_Bindec_Buffer_Size*sizeof(char));

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
    wc.hIconSm       = LoadIcon(hInstance,"MAINICON");
    RegisterClassEx(&wc);

    MainWindow = CreateWindowEx(WS_EX_CLIENTEDGE,g_szClassName,"JRUGC - Outil interne THIF de décodage des JRU - V2.2.0",WS_OVERLAPPEDWINDOW|WS_MAXIMIZE,0, 0, 1700, 1000,NULL, NULL, hInstance, NULL);
    MessageWindow = CreateWindowEx(0,"LISTBOX" ,"Data",WS_CHILD |WS_VSCROLL |WS_BORDER|WS_THICKFRAME | LBS_USETABSTOPS | LBS_NOTIFY | LBS_HASSTRINGS | LBS_OWNERDRAWFIXED,15,70,1450,450,MainWindow,(HMENU) NULL,hInstance,NULL);
    ProgressBarWindow = CreateWindowEx(0,PROGRESS_CLASS,NULL,WS_CHILD |WS_BORDER |PBS_SMOOTH,200,0,250,25,MainWindow,(HMENU) NULL,hInstance,NULL);
    QuitButtonWindow = CreateWindowEx((DWORD)0,"BUTTON","QUIT",WS_CHILD |WS_BORDER,1595,5,75,25,MainWindow,(HMENU) NULL,hInstance,NULL);
    MessageHeaderWindow = CreateWindowEx(0,"LISTBOX" ,"Data",WS_CHILD |WS_HSCROLL |WS_VSCROLL |WS_BORDER|WS_THICKFRAME | LBS_USETABSTOPS | LBS_OWNERDRAWFIXED | LBS_NOTIFY | LBS_HASSTRINGS,15,30,1450,15,MainWindow,(HMENU) NULL,hInstance,NULL);
    FromWindow = CreateWindowEx((DWORD)0,"EDIT","",WS_CHILD |WS_BORDER,1470,120,200,20,MainWindow,(HMENU) NULL,hInstance,NULL);
    ToWindow = CreateWindowEx((DWORD)0,"EDIT","",WS_CHILD |WS_BORDER,1470,180,200,20,MainWindow,(HMENU) NULL,hInstance,NULL);
    TimeWindow = CreateWindowEx((DWORD)0,"EDIT","",WS_CHILD |WS_BORDER | ES_READONLY,600,545,200,20,MainWindow,(HMENU) NULL,hInstance,NULL);
    DistanceWindow = CreateWindowEx((DWORD)0,"EDIT","",WS_CHILD |WS_BORDER | ES_READONLY,850,545,200,20,MainWindow,(HMENU) NULL,hInstance,NULL);
    SpeedWindow = CreateWindowEx((DWORD)0,"EDIT","",WS_CHILD |WS_BORDER | ES_READONLY,1100,545,200,20,MainWindow,(HMENU) NULL,hInstance,NULL);

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

    SendMessage(MessageWindow,WM_SETFONT,(WPARAM)Main_Font,(LPARAM)TRUE);
    SendMessage(MessageHeaderWindow,WM_SETFONT,(WPARAM)Main_Font,(LPARAM)TRUE);
    SendMessage(QuitButtonWindow,WM_SETFONT,(WPARAM)Main_Font,(LPARAM)TRUE);

    while (GetMessage(&Msg, NULL, 0, 0)>0)
    {
        if (Msg.message==WM_KEYDOWN) {
            if (Msg.wParam==13) {
                SendMessage(MessageWindow,LB_RESETCONTENT,0,0);
                bru_file();
                compile_file_read();
            }
            if (Msg.wParam==VK_SHIFT) shift = true;
            if (Msg.wParam==VK_CONTROL) control = true;
            if (Msg.wParam=='Q') break;
        }
        if (Msg.message==WM_KEYDOWN) {
            if (Msg.wParam==VK_SHIFT) shift = false;
            if (Msg.wParam==VK_CONTROL) control = false;
        }
        if (Msg.message==BN_CLICKED_MESSAGE) {
            if (Msg.wParam==(WPARAM)QuitButtonWindow) break;
        }
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    free(Master_Bluffer);
    free(Details_Buffer);
    free(Message_Buffer);
    free(Path_Buffer);
    free(Hexdec_Buffer);
    DeleteObject(Gray_Brush);
    DeleteObject(Main_Brush);
    DeleteObject(Main_Font);
    return 0;
}

