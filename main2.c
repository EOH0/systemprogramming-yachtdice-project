#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <string.h>
#include <form.h>

#define WIDTH 40
#define HEIGHT 20
#define MAX_LEN 300

volatile sig_atomic_t quit_requested = 0;
volatile sig_atomic_t pause_requested = 0;

int game_over = 0;
int score = 0;
int paused = 0;
int max_x, max_y;

char nickname[100] = "Player";

/* Control variables */
//menu entry general number
int uniquenumber[100] = { 0 };
//scene manager general number
int scene = 0;
//preemitive controller handle number
int preem_handle = 0;
//input text cache
char cachetext[100] = "\0";

/*인터럽트 핸들링*/
//나가기 핸들링
void quit_check(int signum){
    quit_requested = 1;
}
//멈춤 핸들링
void paused_check(int signum){
    pause_requested = !pause_requested;
}
//입력 핸들링
void handle_input(){
    int ch = getch();
    switch(ch){
        case 'w':
            if(true) 0;
            break;
    }
}
//리사이즈 핸들링
void handle_resize(int sig){
    endwin();
    refresh();
    clear();
    getmaxyx(stdscr, max_y, max_x);
}

/* Game set factor */
//게임 환경 초기화 및 종료
void end_game(){
    endwin();
    //종료 후 활동
}
void init_game(){
    initscr();
    noecho();
    curs_set(0);
    timeout(100);
    srand(time(NULL));

    //초기값 셋팅
    getmaxyx(stdscr, max_y, max_x);

    //인터럽트 셋팅
    signal(SIGWINCH, handle_resize);
    signal(SIGINT, quit_check);
    signal(SIGTSTP, paused_check);
}


/* Entries */
//메뉴 엔트리

void menuV(int un, int N, char* button[], int x, int y){
    for(int i = 0; i < N; i++){ //visual area
        if(i == uniquenumber[un]){
            mvprintw(y + i,x,"%s <<", button[i]);
        }
        else{
            mvprintw(y + i,x,"%s", button[i]);
        }
    }
    refresh();
}
int menuEntry(int un, int N, char* button[]){
    int ch = getch();
    //printf("%d",ch);
    switch(ch){ //handle area
        case 'w':
            if(uniquenumber[un] > 0) uniquenumber[un]--;
            break;
        case 's':
            if(uniquenumber[un] < N - 1) uniquenumber[un]++;
            break;
        case 10:
            return uniquenumber[un];
            break;
    }
    return -1;
}
//List Entry
void listV(int N, char* button[], int x, int y){
    for(int i = 0; i < N; i++){ //visual area
        mvprintw(y + i,x,"%s", button[i]);
    }
    refresh();
}
//입력 엔트리
void inputV(char* inputcache, int x, int y){
    mvprintw(y,x,"%s", inputcache);
    refresh();
}
int inputEntry(char* inputcache){
    int len = strlen(inputcache);
    int ch = getch();
    if ((ch == KEY_BACKSPACE || ch == 127) && len > 0) {
        len--;
        inputcache[len] = '\0';
    }
    else if (ch >= 32 && ch <= 126 && len < 100 - 1) {
        inputcache[len++] = ch;
        inputcache[len] = '\0';
    }
    else if (ch == 10) {
        return 1;
    }
    return -1;
}
//Handling Entry
int handlingEntry(int size, char* inputunit){
    int ch = getch();
    for(int i = 0; i < size; i++){
        if(ch == inputunit[i]){
            return i;
        }
    }
    return -1;
}

/* Scene manager Sector */
void mainscene(char** menulist){
    //visual area
    mvprintw(3,5,
        "                                                         \n"
        "8b        d8                       88                    \n"
        " Y8,    ,8P                        88            ,d     \n"
        "  Y8,  ,8P                         88            88     \n"
        "   \"8aa8\"  ,adPPYYba,   ,adPPYba,  88,dPPYba,  MM88MMM  \n"
        "    `88'   \"\"     `Y8  a8\"     \"\"  88P'    \"8a   88     \n"
        "     88    ,adPPPPP88  8b          88       88   88     \n"
        "     88    88,    ,88  \"8a,   ,aa  88       88   88,    \n"
        "     88    `\"8bbdP\"Y8   `\"Ybbd8\"'  88       88   \"Y888  \n"
        "                                                         \n"
    );
    mvprintw(14,5,
        "                                               \n"
        "             88888888ba,    88    ,ad8888ba,   88888888888  \n"
        "             88      `\"8b   88   d8'    `\"8b  88           \n"
        "             88        `8b  88  d8'            88           \n"
        "             88         88  88  88             88aaaaa      \n"
        "             88         88  88  88             88\"\"\"\"\"      \n"
        "             88         8P  88  Y8,            88           \n"
        "             88      .a8P   88   Y8a.    .a8P  88           \n"
        "             88888888Y\"'    88    `\"Y8888Y\"'   88888888888  \n"
        "                                               \n"
    );
    
    
    mvprintw(25,4,"Player Name :");
    menuV(0,4,menulist,100 - 45,26);
    inputV(nickname,5,26);
    //entry area
    switch(preem_handle){
        case 0:
            int a = menuEntry(0,4,menulist);
            if(a == 3){
                preem_handle = 1;
            }
            else if(a != -1){
                scene = a + 1;
            }
            break;
        case 1:
            a = inputEntry(nickname);
            if(a != -1){
                preem_handle = 0;
            }
            break;
    }
}
void scene1(char** playerlist, char* button){
    //menuV(1,menusize,playerlist,100 - 45,15);
    //visual area
    inputV(cachetext,30,26);
    
    switch(preem_handle){
        case 0:
            int a = handlingEntry(2, button);
            if(a != -1){
                if(a == 0){
                    preem_handle = 1;
                }
                else{
                    scene = 0;
                }
            }
            break;
        case 1:
            a = inputEntry(cachetext);
            if(a != -1){
                preem_handle = 0;
            }
            break;
    }
    /*int a = menuEntry(1,menusize,playerlist);
            if(a != -1){
                preem_handle = 1;
            }*/
}

/* main core process */
int main(){
    init_game();

    //start
    char* menulist[4] = {"SinglePlayer","MultiPlayer","Network Play","Change Nickname"};
    char button[2] = {'p','b'};
    //update
    while(!game_over){
        if (quit_requested) { //ctrl c
            //기본 실행 내용
            refresh();
            int ch = getch();
            if (ch == 'y') {
                end_game();
            printf("Terminated by user\n");
                exit(0);
            } 
            else if(ch == 'n'){
                quit_requested = 0;
                continue;
            }
            else {
                continue;
            }
        }
        else if (pause_requested || paused) {
            //기본 실행 내용
            refresh();
            int ch = getch();
            if (ch == 'p' || ch == 26 /* Ctrl+Z */) {
                pause_requested = 0;
                paused = 0;
                clear();
                continue;
            } else {
                continue;
            }
        }
        else{
            clear();
            switch(scene){ //scene manager
                case 0:
                    mainscene(menulist);
                    break;
                case 1:
                    refresh();
                    scene1(menulist,button);
                    break;
                case 2:
                    refresh();
                    break;
                case 3:
                    refresh();
                    break;
                default:
                    continue;
            }
            refresh();
        }
        usleep(100000);
    }
    end_game();
    return 0;
}

