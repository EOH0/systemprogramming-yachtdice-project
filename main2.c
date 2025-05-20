#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <string.h>
#include <form.h>
#include <stdlib.h>

#define WIDTH 150
#define HEIGHT 52
#define MAX_LEN 300

volatile sig_atomic_t quit_requested = 0;
volatile sig_atomic_t pause_requested = 0;

int game_over = 0;
int score = 0;
int paused = 0;
int max_x, max_y;
int dice_row = 15;

char nickname[100] = "Player";
int diceVal[5] = {0, 0, 0, 0, 0};

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
    refresh();
    
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
void sceneFrame() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if ((x == 0 || x == WIDTH - 1) && (y == 0 || y == HEIGHT - 1)) {
                mvprintw(y, x, "+");
            }
            else if (x == 0 || x == WIDTH - 1) {
                mvprintw(y, x, "|");
            }
            else if (y == 0 || y == HEIGHT - 1) {
                mvprintw(y, x, "-");
            }
        }
    }
    refresh();
}

void scoreBoard(char** playerlist) {
    for (int y = 1; y <= HEIGHT - 2; y++) {
        for (int x = 1; x <= 24; x++) {
            if ((x == 1 || x == 24) && (y == 1 || y == HEIGHT - 2)) {
                mvprintw(y, x, "+");
            }
            else if (x == 1 || x == 24) {
                mvprintw(y, x, "|");
            }
            else if (y == 1 || y == HEIGHT - 2) {
                mvprintw(y, x, "-");
            }
        }
    }
    refresh();

    // mvprintw(3, 5, ": ");
    mvprintw(2, 3, "Ones");           mvprintw(2, 17, ":");
    mvprintw(3, 3, "Twos");           mvprintw(3, 17, ":");
    mvprintw(4, 3, "Threes");         mvprintw(4, 17, ":");
    mvprintw(5, 3, "Fours");          mvprintw(5, 17, ":");
    mvprintw(6, 3, "Fives");          mvprintw(6, 17, ":");
    mvprintw(7, 3, "Sixes");          mvprintw(7, 17, ":");
    mvprintw(8, 3, "(Homework)");     mvprintw(8, 17, ":");
    mvprintw(9, 3, "Choice");         mvprintw(9, 17, ":");
    mvprintw(10, 3, "Fourofakind");   mvprintw(10, 17, ":");
    mvprintw(11, 3, "FullHouse");     mvprintw(11, 17, ":");
    mvprintw(12, 3, "SmallStraight"); mvprintw(12, 17, ":");
    mvprintw(13, 3, "LargeStraight"); mvprintw(13, 17, ":");
    mvprintw(14, 3, "YACHT");         mvprintw(14, 17, ":");
    refresh();
    // 점수판 밑으로 33칸 여유있음 가로는 22칸
}
void rollDices() {
    for (int i = 0; i < 5; i++)  {
        diceVal[i] = rand() % 6 + 1;
    }
}
void printDiceFrame(int x_end, int y_end, int row, int diceV) {
    int flag = diceVal[0] * diceVal[1] * diceVal[2] * diceVal[3] * diceVal[4];
    for (int x = 2; x <= x_end; x++) {
        for (int y = 0; y <= y_end; y++) {
            int cur_y = row + y;
            if ((x == 2 || x == x_end) && (y !=  0 && y != y_end)) {
                mvprintw(cur_y, x, "|");
            } else if ((y == 0 || y == y_end) && (x != 2 && x != x_end)) {
                mvprintw(cur_y, x, "-");
            } else if ((y == 0 || y == y_end) && (x == 2 || x == x_end)) {
                mvprintw(cur_y, x, "+");
            }
        }    
    }
    if (flag) {
        int tempRow = row + 1;
        int col = 3;
        switch (diceV) {
        case 1:
            mvprintw(tempRow++, col, "           ");
            mvprintw(tempRow++, col, "           ");
            mvprintw(tempRow++, col, "     O     ");
            mvprintw(tempRow++, col, "           ");
            mvprintw(tempRow++, col, "           ");
            break;
        case 2:
            mvprintw(tempRow++, col, "           ");
            mvprintw(tempRow++, col, "  O        ");
            mvprintw(tempRow++, col, "           ");
            mvprintw(tempRow++, col, "        O  ");
            mvprintw(tempRow++, col, "           ");
            break;
        case 3:
            mvprintw(tempRow++, col, "           ");
            mvprintw(tempRow++, col, "  O        ");
            mvprintw(tempRow++, col, "     O     ");
            mvprintw(tempRow++, col, "        O ");
            mvprintw(tempRow++, col, "           ");
            break;
        case 4:
            mvprintw(tempRow++, col, "           ");
            mvprintw(tempRow++, col, "  O     O  ");
            mvprintw(tempRow++, col, "           ");
            mvprintw(tempRow++, col, "  O     O  ");
            mvprintw(tempRow++, col, "           ");
            break;
        case 5:
            mvprintw(tempRow++, col, "           ");
            mvprintw(tempRow++, col, "  O     O  ");
            mvprintw(tempRow++, col, "     O     ");
            mvprintw(tempRow++, col, "  O     O  ");
            mvprintw(tempRow++, col, "           ");
            break;
        case 6:
            mvprintw(tempRow++, col, "           ");
            mvprintw(tempRow++, col, "  O     O  ");
            mvprintw(tempRow++, col, "  O     O  ");
            mvprintw(tempRow++, col, "  O     O  ");
            mvprintw(tempRow++, col, "           ");
            break;
        }
        // if (x == x_end / 2 && y == y_end / 2) {
            // 주사위 내부 가로 11칸, 세로 5칸
            // mvprintw(cur_y, x + 1, "%d", diceV); // 주사위 값 출력
        //  }
    }
    refresh();
}
void printDice(int row, int diceV) {
    refresh();
    int x_end = 14;
    int y_end = 6;
    printDiceFrame(x_end, y_end, row, diceV);
}
void scene1(char** playerlist, char* button){
    //visual area
    sceneFrame();
    scoreBoard(playerlist);

    int tempRow = dice_row;
    for(int i = 0; i < 5; i++) {
        // mvprintw(tempRow + i, 27, "%d", diceVal[i]); // 주사위 값 체크하는 문장
        printDice(tempRow + i, diceVal[i]);
        tempRow += 6;
    }
    

    char roll = getch();
    if(roll == 'f') { // f 누르면 주사위 굴림
        rollDices();
    }
    // inputV(cachetext,30,26);
    
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
    char* playerlist[4] = {"1p", "2p", "3p", "4p"};
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

