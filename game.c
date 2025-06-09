//gcc -o game game.c -lncurses

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <string.h>
#include <form.h>
#include <fcntl.h>
#include <sys/stat.h>

#define WIDTH 40
#define HEIGHT 20
#define MAX_LEN 300
#define SSVAL 15
#define LSVAL 30
#define YACHTVAL 50

#define N_DICE     5
#define DICE_SIDES 6
#define N_CAT      13

volatile sig_atomic_t quit_requested = 0;
volatile sig_atomic_t pause_requested = 0;

int game_over = 0;
int score = 0;
int paused = 0;
int max_x, max_y;
int flag = 0;
////////////////////////////////////////////////////////
/* Control variables */
//menu entry general number
int uniquenumber[100] = { 0 };
//scene manager general number
int scene = 0;
//preemitive controller handle number
int preem_handle = 0;
//package string list
char package_list[100][100] = {0}; 
//dice management
int diceVal[5] = {0, 0, 0, 0, 0};
int MaskVal[5] = {0, 0, 0, 0, 0};
int sortedDice[5];
int diceCombination[13] = { 0 };
int diceCnt[7] = { 0 }; // index 1부터 저장
int playerCombination[5][13];
int MaskCombination[5][13] = { { 0 } };
int life = 3;
// player list
char* playerlist[4] = {"1p", "2p", "3p", "4p"};
int gamemode = 0;
int key = 0;
int playerun = 0;
// turn management
int turn = 0; // 현재 턴 플레이어 인덱스
int rnd = 0;
////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////
/* Game set factor */
//게임 환경 초기화 및 종료, 게임의 저장 및 불러오기
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
    for (int i = 0; i <= 4; i++) {
        for (int j = 0; j < 13; j++) {
            playerCombination[i][j] = 0;
        }
    }

    //인터럽트 셋팅
    struct sigaction sa;    
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_resize;
    sigaction(SIGWINCH, &sa, NULL);

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = quit_check;
    sigaction(SIGINT, &sa, NULL);

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = paused_check;
    sigaction(SIGTSTP, &sa, NULL);

}
void loadGameState() {
    int fd = open("save.txt", O_RDONLY);
    if (fd == -1) {
        return ;
    }

    char buffer[128];
    int i = 0, j = 0;

    FILE *fp = fdopen(fd, "r");
    if (fp == NULL) {
        close(fd);
        return;
    }

    fgets(buffer, sizeof(buffer), fp);
    sscanf(buffer, "%d", &rnd);

    fgets(buffer, sizeof(buffer), fp);
    sscanf(buffer, "%d", &turn);

    fgets(buffer, sizeof(buffer), fp);
    buffer[strcspn(buffer, "\n")] = 0;
    strcpy(package_list[0], buffer);


    for (int i = 1; i <= 4; i++) {
        for (int j = 0; j <= 12; j++) {
            int score;
            fgets(buffer, sizeof(buffer), fp);
            sscanf(buffer, "%d", &score);
            playerCombination[i][j] = score;
        }
    }

    for (int i = 1; i <= 4; i++) {
        for (int j = 0; j <= 12; j++) {
            int loadMask;
            fgets(buffer, sizeof(buffer), fp);
            sscanf(buffer, "%d", &loadMask);
            MaskCombination[i][j] = loadMask;
        }
    }

    fclose(fp);
}

int saveGameState() {
    int fd = open("save.txt", O_WRONLY | O_TRUNC, 0644); // rw-r--r--
    if (fd == -1) {
        fd = creat("save.txt", 0644);
        if(fd == -1) {
            return -1; // 파일 생성 실패
        }
    }

    char buffer[128];

    sprintf(buffer, "%d\n", rnd);
    write(fd, buffer, strlen(buffer));

    sprintf(buffer, "%d\n", turn);
    write(fd, buffer, strlen(buffer));

    sprintf(buffer, "%s\n", package_list[0]);
    write(fd, buffer, strlen(buffer));

    for (int i = 1; i <= 4; i++) {
        for (int j = 0; j <= 12; j++) {
            sprintf(buffer, "%d\n", playerCombination[i][j]);
            write(fd, buffer, strlen(buffer));
            memset(buffer, 0, sizeof(buffer));
        }
    }

    for (int i = 1; i <= 4; i++) {
        for (int j = 0; j <= 12; j++) {
            sprintf(buffer, "%d\n", MaskCombination[i][j]);
            write(fd, buffer, strlen(buffer));
            memset(buffer, 0, sizeof(buffer));
        }
    }

    close(fd);

    return 0;
}
////////////////////////////////////////////////////////
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
int menuEntry(int un, int N){
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
void listV(int N, char* list[], int x, int y){
    for(int i = 0; i < N; i++){ //visual area
        mvprintw(y + i,x,"%s", list[i]);
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
////////////////////////////////////////////////////////
/* Scenes Example */
void scene_example(){
    //visual area

    //entry area
    switch(preem_handle){
        case 0:
            break;
        case 1:
            break;
    }
}
////////////////////////////////////////////////////////
/* Scene manager Sector */
void mainscene(){
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
    
    menuV(0,4,(char*[]){"New Local Play", "Load Local Play", "Player VS COM", "Network Play"},100 - 45,26);
    if(preem_handle == 1){
        mvprintw(25,4,"The number of players (2~4) :");
        inputV(package_list[0],5,26);
    }
    //entry area
    switch(preem_handle){
        case 0:
            int a = menuEntry(0, 3);
            if(a == 0){
                preem_handle = 1;
            }
            if(a == 1){
                struct stat st;
                if (stat("save.txt", &st) == -1) {
                    mvprintw(25,4,"Cannot load game state, file not found.");
                    return ;
                }

                if (st.st_size != 0) {
                    preem_handle = 0;
                    loadGameState();
                    scene = 1;
                }
            }
            if(a == 2){
                strcpy(package_list[0], "2");
                preem_handle = 0;
                scene = 1;
                gamemode = 1;
                playerun = 0;
                key = 0;
                playerlist[1] = "COM";
            }
            break;
        case 1:
            a = inputEntry(package_list[0]);
            if(a != -1 && atoi(package_list[0]) >= 2 && atoi(package_list[0]) <= 4) {
                preem_handle = 0;
                scene = 1;
                gamemode = 0;
            }
            break;
    }
}
////////////////////////////////////////////////////////
void diceSort() {
    for (int i = 0; i < 5; i++) {
        for (int j = i; j < 5; j++) {
            if (sortedDice[i] > sortedDice[j]) {
                int t = sortedDice[i];
                sortedDice[i] = sortedDice[j];
                sortedDice[j] = t;
            }
        }
    }
}
void calculDiceVal() {
    diceSort();
    for (int cali = 0; cali < 13; cali++) { // 족보 값을 저장하는 cardCombination의 인덱스
        if (cali < 6) { // 1 ~ 6
            int numSum = 0;
            for (int i = 0; i < 5; i++) {
                if (sortedDice[i] == cali + 1) {
                    numSum += cali + 1;
                }
                diceCombination[cali] = numSum;
            }
        }
        // else if (i == 6) { // 숙제는 플레이어 점수판에 표기
        //     int HWSum = 0;
        //     for (int i = 0; i < 5; i++) {
        //         HWSum += diceCombination[i];
        //     }
        //     if (HWSum >= 63) {
        //         int HWScore = 35;
        //         diceCombination[i] = HWScore;
        //     }
        // }
        else if (cali >= 6) {
            switch (cali) {
            case 6: // choice
                int choiceSum = 0;
                for (int i = 0; i < 5; i++) {
                    choiceSum += sortedDice[i];
                }
                diceCombination[cali] = choiceSum;
                break;
            case 7: // Four of a kind
                int FoK = 0;
                for (int i = 1; i <= 6; i++) {
                    if (diceCnt[i] == 4) {
                        FoK = i * 4;
                    }
                }
                diceCombination[cali] = FoK;
                break;
            case 8: // Full House
                int FH = 0;
                for (int i = 1; i <= 6; i++) {
                    for (int j = 1; j <= 6; j++) {
                        if ((diceCnt[i] == 3 && diceCnt[j] == 2) || (diceCnt[i] == 2 && diceCnt[j] == 3)) {
                            FH = i * 3 + j * 2;
                            break;
                        }
                    }
                }
                diceCombination[cali] = FH;
                break;
            case 9: // SmallStraight
                int SS = 0;
                int SSmul;
                if (diceCnt[1] == 0 || diceCnt[2] == 0) {
                    SSmul = diceCnt[3];
                    for (int i = 4; i <= 6; i++) {
                        SSmul *= diceCnt[i];
                    }
                }
                else if (diceCnt[1] == 0 || diceCnt[6] == 0) {
                    SSmul = diceCnt[2];
                    for (int i = 3; i <= 5; i++) {
                        SSmul *= diceCnt[i];
                    }
                }
                else if (diceCnt[5] == 0 || diceCnt[6] == 0) {
                    SSmul = diceCnt[1];
                    for (int i = 1; i <= 4; i++) {
                        SSmul *= diceCnt[i];
                    }
                }
                if (SSmul == 1) {
                    SS = SSVAL;
                }
                diceCombination[cali] = SS;
                break;
            case 10: // LargeStraight
                int LS = 0; int LSflag = 0;
                int LSmul = 1;
                if (diceCnt[1] == 0) {
                    for (int i = 2; i <= 6; i++) {
                        LSmul *= diceCnt[i];
                        LSflag = 1;
                    }
                }
                else if (diceCnt[6] == 0) {
                    for (int i = 1; i <= 5; i++) {
                        LSmul *= diceCnt[i];
                        LSflag = 1;
                    }
                }
                if (LSmul == 1 && LSflag) {
                    LS = LSVAL;
                }
                diceCombination[cali] = LS;
                break;
            case 11:
                int yacht = 0;
                for (int i = 1; i <= 6; i++) {
                    if (diceCnt[i] == 5) {
                        yacht = YACHTVAL;
                    }
                }
                diceCombination[cali] = yacht;
                break;
            }
        }
    }
}
void mainScoreBoard() {
    for (int y = 1; y <= 18; y++) {
        for (int x = 1; x <= 24; x++) {
            if ((x == 1 || x == 24) && (y == 1 || y == 18)) {
                mvprintw(y, x, "+");
            }
            else if (x == 1 || x == 24) {
                mvprintw(y, x, "|");
            }
            else if (y == 1 || y == 18) {
                mvprintw(y, x, "-");
            }
        }
    }
    refresh();
    if (flag) calculDiceVal();

    // mvprintw(3, 5, ": ");
    int x_start = 3, x_end = 20;
    mvprintw(2, x_start,   "1.Ones");
    if(MaskCombination[turn + 1][0] == 0)           
        mvprintw(2, x_end,   ": %d", diceCombination[0]);
    else
        mvprintw(2, x_end,   ": -");
    mvprintw(3, x_start,   "2.Twos");
    if(MaskCombination[turn + 1][1] == 0)           
        mvprintw(3, x_end,   ": %d", diceCombination[1]);
    else
        mvprintw(3, x_end,   ": -");
    mvprintw(4, x_start,   "3.Threes");
    if(MaskCombination[turn + 1][2] == 0)           
        mvprintw(4, x_end,   ": %d", diceCombination[2]);
    else
        mvprintw(4, x_end,   ": -");
    mvprintw(5, x_start,   "4.Fours");
    if(MaskCombination[turn + 1][3] == 0)           
        mvprintw(5, x_end,   ": %d", diceCombination[3]);
    else
        mvprintw(5, x_end,   ": -");
    mvprintw(6, x_start,   "5.Fives");
    if(MaskCombination[turn + 1][4] == 0)           
        mvprintw(6, x_end,   ": %d", diceCombination[4]);
    else
        mvprintw(6, x_end,   ": -");
    mvprintw(7, x_start,   "6.Sixes");
    if(MaskCombination[turn + 1][5] == 0)           
        mvprintw(7, x_end,   ": %d", diceCombination[5]);
    else
        mvprintw(7, x_end,   ": -");
    mvprintw(8, x_start,   "7.Choice");
    if(MaskCombination[turn + 1][6] == 0)           
        mvprintw(8, x_end,   ": %d", diceCombination[6]);
    else
        mvprintw(8, x_end,   ": -");
    mvprintw(9, x_start,   "8.Fourofakind");
    if(MaskCombination[turn + 1][7] == 0)           
        mvprintw(9, x_end,   ": %d", diceCombination[7]);
    else
        mvprintw(9, x_end,   ": -");
    mvprintw(10, x_start,  "9.FullHouse");
    if(MaskCombination[turn + 1][8] == 0)           
        mvprintw(10, x_end,   ": %d", diceCombination[8]);
    else
        mvprintw(10, x_end,   ": -");
    mvprintw(11, x_start,  "10.SmallStraight");
    if(MaskCombination[turn + 1][9] == 0)           
        mvprintw(11, x_end,   ": %d", diceCombination[9]);
    else
        mvprintw(11, x_end,   ": -");
    mvprintw(12, x_start,  "11.LargeStraight");
    if(MaskCombination[turn + 1][10] == 0)           
        mvprintw(12, x_end,   ": %d", diceCombination[10]);
    else
        mvprintw(12, x_end,   ": -");
    mvprintw(13, x_start,  "12.YACHT");
    if(MaskCombination[turn + 1][11] == 0)           
        mvprintw(13, x_end,   ": %d", diceCombination[11]);
    else
        mvprintw(13, x_end,   ": -");
    // mvprintw(13, 21, "%d %d %d %d %d %d", diceCnt[1], diceCnt[2], diceCnt[3], diceCnt[4], diceCnt[5], diceCnt[6]); // 주사위 눈금별 주사위 개수
    // if (diceCombination[10] != 0 || (diceCnt[1] == 1 && diceCnt[2] == 1 && diceCnt[3] == 1 && diceCnt[4] == 1 && diceCnt[5] == 1) || (diceCnt[2] == 1 && diceCnt[3] == 1 && diceCnt[4] == 1 && diceCnt[5] == 1 && diceCnt[6] == 1)) { // 원하는 족보값에서 정지하기
    //     quit_requested = 1;
    // }
    refresh();
}
void rollDices() {
    for (int i = 1; i <= 6; i++) diceCnt[i] = 0;

    for (int i = 0; i < 5; i++)  {
        if(MaskVal[i] == 0) { // 마스크가 0인 주사위만 굴림
            diceVal[i] = rand() % 6 + 1; // 1 ~ 6
        }
        sortedDice[i] = diceVal[i];
        diceCnt[diceVal[i]] += 1;
    }
}
void diceviewer(int value, int x, int y, int un){
    if(MaskVal[un - 1] == 1) { // 마스크가 1인 주사위는 출력하지 않음
        un = 'X';
    }
    else{
        un = un + '0';
    }
    switch (value) {
        case 1:
            mvprintw(y, x,     "%c---------------+", un);
            mvprintw(y + 1, x, "|               |");
            mvprintw(y + 2, x, "|               |");
            mvprintw(y + 3, x, "|               |");
            mvprintw(y + 4, x, "|       O       |");
            mvprintw(y + 5, x, "|               |");
            mvprintw(y + 6, x, "|               |");
            mvprintw(y + 7, x, "|               |");
            mvprintw(y + 8, x, "+---------------+");
            break;
        case 2:
            mvprintw(y, x,     "%c---------------+", un);
            mvprintw(y + 1, x, "|               |");
            mvprintw(y + 2, x, "|           O   |");
            mvprintw(y + 3, x, "|               |");
            mvprintw(y + 4, x, "|               |");
            mvprintw(y + 5, x, "|               |");
            mvprintw(y + 6, x, "|   O           |");
            mvprintw(y + 7, x, "|               |");
            mvprintw(y + 8, x, "+---------------+");
            break;
        case 3:
            mvprintw(y, x,     "%c---------------+", un);
            mvprintw(y + 1, x, "|               |");
            mvprintw(y + 2, x, "|           O   |");
            mvprintw(y + 3, x, "|               |");
            mvprintw(y + 4, x, "|       O       |");
            mvprintw(y + 5, x, "|               |");
            mvprintw(y + 6, x, "|   O           |");
            mvprintw(y + 7, x, "|               |");
            mvprintw(y + 8, x, "+---------------+");
            break;
        case 4:
            mvprintw(y, x,     "%c---------------+", un);
            mvprintw(y + 1, x, "|               |");
            mvprintw(y + 2, x, "|   O       O   |");
            mvprintw(y + 3, x, "|               |");
            mvprintw(y + 4, x, "|               |");
            mvprintw(y + 5, x, "|               |");
            mvprintw(y + 6, x, "|   O       O   |");
            mvprintw(y + 7, x, "|               |");
            mvprintw(y + 8, x, "+---------------+");
            break;
        case 5:
            mvprintw(y, x,     "%c---------------+", un);
            mvprintw(y + 1, x, "|               |");
            mvprintw(y + 2, x, "|   O       O   |");
            mvprintw(y + 3, x, "|               |");
            mvprintw(y + 4, x, "|       O       |");
            mvprintw(y + 5, x, "|               |");
            mvprintw(y + 6, x, "|   O       O   |");
            mvprintw(y + 7, x, "|               |");
            mvprintw(y + 8, x, "+---------------+");
            break;
        case 6:
            mvprintw(y, x,     "%c---------------+", un);
            mvprintw(y + 1, x, "|               |");
            mvprintw(y + 2, x, "|   O       O   |");
            mvprintw(y + 3, x, "|               |");
            mvprintw(y + 4, x, "|   O       O   |");
            mvprintw(y + 5, x, "|               |");
            mvprintw(y + 6, x, "|   O       O   |");
            mvprintw(y + 7, x, "|               |");
            mvprintw(y + 8, x, "+---------------+");
            break;
        default:
            mvprintw(y, x,     "%c---------------+", un);
            mvprintw(y + 1, x, "|               |");
            mvprintw(y + 2, x, "|               |");
            mvprintw(y + 3, x, "|               |");
            mvprintw(y + 4, x, "|               |");
            mvprintw(y + 5, x, "|               |");
            mvprintw(y + 6, x, "|               |");
            mvprintw(y + 7, x, "|               |");
            mvprintw(y + 8, x, "+---------------+");
            break;
        }
}

void playerScoreBoard(int playerNum, int xi, int yi) { // 1p ~ 4p 범용적으로 써야함
    // mainScoreBoard 이후 가로 110칸
    int x_start = 26, x_end = 53; 
    for (int i = 1; i <= playerNum; i++) {
        mvprintw(yi + 2, xi + (x_start + x_end) / 2 - 1, "%s", playerlist[i - 1]);
        for (int y = 1; y <= HEIGHT - 2; y++) {
            for (int x = x_start; x <= x_end; x++) {
                if ((x == x_start || x == x_end) && (y == 1 || y == HEIGHT - 2)) {
                    mvprintw(yi + y, xi + x, "+");
                }
                else if (x == x_start || x == x_end) {
                    mvprintw(yi + y, xi + x, "|");
                }
                else if (y == 1 || y == HEIGHT - 2) {
                    mvprintw(yi + y, xi + x, "-");
                }
            }
        }
        mvprintw(yi + 3, xi + x_start + 5,  "Ones");           mvprintw(yi + 3, xi + x_start + 20,  ": %d", playerCombination[i][0]);
        mvprintw(yi + 4, xi + x_start + 5,  "Twos");           mvprintw(yi + 4, xi + x_start + 20,  ": %d", playerCombination[i][1]);
        mvprintw(yi + 5, xi + x_start + 5,  "Threes");         mvprintw(yi + 5, xi + x_start + 20,  ": %d", playerCombination[i][2]);
        mvprintw(yi + 6, xi + x_start + 5,  "Fours");          mvprintw(yi + 6, xi + x_start + 20,  ": %d", playerCombination[i][3]);
        mvprintw(yi + 7, xi + x_start + 5,  "Fives");          mvprintw(yi + 7, xi + x_start + 20,  ": %d", playerCombination[i][4]);
        mvprintw(yi + 8, xi + x_start + 5,  "Sixes");          mvprintw(yi + 8, xi + x_start + 20,  ": %d", playerCombination[i][5]);
        mvprintw(yi + 9, xi + x_start + 5,  "Choice");         mvprintw(yi + 9, xi + x_start + 20,  ": %d", playerCombination[i][6]);
        mvprintw(yi + 10, xi + x_start + 5, "Fourofakind");    mvprintw(yi + 10, xi + x_start + 20, ": %d", playerCombination[i][7]);
        mvprintw(yi + 11, xi + x_start + 5, "FullHouse");      mvprintw(yi + 11, xi + x_start + 20, ": %d", playerCombination[i][8]);
        mvprintw(yi + 12, xi + x_start + 5, "SmallStraight");  mvprintw(yi + 12, xi + x_start + 20, ": %d", playerCombination[i][9]);
        mvprintw(yi + 13, xi + x_start + 5, "LargeStraight");  mvprintw(yi + 13, xi + x_start + 20, ": %d", playerCombination[i][10]);
        mvprintw(yi + 14, xi + x_start + 5, "YACHT");          mvprintw(yi + 14, xi + x_start + 20, ": %d", playerCombination[i][11]);
        mvprintw(yi + 15, xi + x_start + 5,  "35+ Bonus!");
        int sum = playerCombination[i][0] + playerCombination[i][1] + playerCombination[i][2] + playerCombination[i][3] + playerCombination[i][4] + playerCombination[i][5];
        if(sum >= 63){
            playerCombination[i][12] = 35; // 35점 보너스
        }
        mvprintw(yi + 15, xi + x_start + 20, ": %d", playerCombination[i][12]);
        x_start += 27;
        x_end += 27;
    }
}

char announcement[100] = "";

//AI interface
int computeScore(int dice[N_DICE], int player_id) {
    int cnt[7] = {0}, sumAll = 0;
    for (int i = 0; i < N_DICE; i++) {
        cnt[dice[i]]++;
        sumAll += dice[i];
    }

    int best = 0;
    // 1~6 (Upper section)
    for (int face = 1; face <= 6; face++) {
        if (!MaskCombination[player_id+1][face-1]) {
            int s = cnt[face] * face;
            if (s > best) best = s;
        }
    }
    // 7: Choice
    if (!MaskCombination[player_id+1][6]) {
        if (sumAll > best) best = sumAll;
    }
    // 8: Four of a Kind
    if (!MaskCombination[player_id+1][7]) {
        for (int face = 1; face <= 6; face++) {
            if (cnt[face] >= 4) {
                if (sumAll > best) best = sumAll;
                break;
            }
        }
    }
    // 9: Full House (3+2)
    if (!MaskCombination[player_id+1][8]) {
        int has3 = 0, has2 = 0;
        for (int face = 1; face <= 6; face++) {
            if (cnt[face] == 3) has3 = 1;
            if (cnt[face] == 2) has2 = 1;
        }
        if (has3 && has2 && sumAll > best) best = sumAll;
    }
    // 10: Small Straight (any 연속 4개)
    if (!MaskCombination[player_id+1][9]) {
        if ((cnt[1] && cnt[2] && cnt[3] && cnt[4]) ||
            (cnt[2] && cnt[3] && cnt[4] && cnt[5]) ||
            (cnt[3] && cnt[4] && cnt[5] && cnt[6])) {
            if (SSVAL > best) best = SSVAL;
        }
    }
    // 11: Large Straight (연속 5개)
    if (!MaskCombination[player_id+1][10]) {
        if ((cnt[1]==1 && cnt[2]==1 && cnt[3]==1 && cnt[4]==1 && cnt[5]==1) ||
            (cnt[2]==1 && cnt[3]==1 && cnt[4]==1 && cnt[5]==1 && cnt[6]==1)) {
            if (LSVAL > best) best = LSVAL;
        }
    }
    // 12: Yacht (5 of a kind)
    if (!MaskCombination[player_id+1][11]) {
        for (int face = 1; face <= 6; face++) {
            if (cnt[face] == 5 && YACHTVAL > best) {
                best = YACHTVAL;
                break;
            }
        }
    }
    // 13: Bonus (upper sum ≥63) 은 메인 점수판에서 처리
    return best;
}

//--------------------------------------------------
// 2) 남은 리롤로 기대값 계산: 재귀적으로 free dice 순회
//--------------------------------------------------
static int  freePos[N_DICE];
static int  tempDice[N_DICE];

double expectedValue_rec(int idx, int freeCnt, int rollsLeft, int hold_mask, int player_id) {
    if (idx == freeCnt) {
        // 완성된 tempDice[] 에서 스코어 계산
        return computeScore(tempDice, player_id);
    }
    double sum = 0.0;
    for (int v = 1; v <= DICE_SIDES; v++) {
        tempDice[ freePos[idx] ] = v;
        sum += expectedValue_rec(idx+1, freeCnt, rollsLeft, hold_mask, player_id);
    }
    return sum / DICE_SIDES;
}

double expectedValue(int hold_mask, int rollsLeft, int player_id) {
    // 1) mask 비트=1인 주사위는 고정, 나머지는 리롤 대상
    int freeCnt = 0;
    for (int i = 0; i < N_DICE; i++) {
        tempDice[i] = diceVal[i];
        if (!((hold_mask>>i)&1)) {
            freePos[freeCnt++] = i;
        }
    }
    // 2) 더 리롤 기회가 있으면 재귀 호출
    if (rollsLeft > 0) {
        double sum = 0.0;
        // 모든 free dice를 한번 리롤 → 기대값 재귀적으로 계산
        // 단순화를 위해 “한 번에 전부 다시 굴린다” 가정
        sum = expectedValue_rec(0, freeCnt, rollsLeft-1, hold_mask, player_id);
        return sum;
    }
    // 더 이상 리롤이 없으면 현 상태로 스코어
    return computeScore(tempDice, player_id);
}

//--------------------------------------------------
// 3) 남은 리롤과 마스크로 최적 홀드 결정
//--------------------------------------------------
int find_best_mask(int rollsLeft, int player_id) {
    double bestEv = -1.0;
    int bestMask = 0;
    for (int mask = 0; mask < (1<<N_DICE); mask++) {
        double ev = expectedValue(mask, rollsLeft, player_id);
        if (ev > bestEv) {
            bestEv = ev;
            bestMask = mask;
        }
    }
    return bestMask;
}

//--------------------------------------------------
// 4) AI Section 완전 구현
//--------------------------------------------------
void AI_section(int playerNum) {
    // playerNum: 0-based
    // life: 남은 리롤 횟수 (최대 3)
    int x_align = 100;
    int y_align = 33;
    while (life > 0) {
        // 최적 홀드 마스크 계산
        int hold = find_best_mask(life-1, playerNum);
        // MaskVal 갱신
        for (int i = 0; i < N_DICE; i++)
            MaskVal[i] = ((hold>>i)&1);

        // 주사위 리롤
        rollDices();
        mvprintw(y_align, x_align, "AI Rolled Dices: ");
        // 굴린 주사위 상태 출력
        for (int i = 0; i < N_DICE; i++) {
            mvprintw(y_align + 1 + i, x_align, "Dice %d: %d (Mask: %d)\n", i+1, diceVal[i], MaskVal[i]);
        }
        mvprintw(y_align + 7, x_align, "===============\n");
        refresh();
        sleep(2);
        life--;
    }

    // 최종 점수 기록
    calculDiceVal();  // global diceCombination[] 갱신

    int best_cat = -1, best_score = -1;
    for (int c = 0; c < N_CAT; c++) {
        if (!MaskCombination[playerNum+1][c] && diceCombination[c] > best_score) {
            best_score = diceCombination[c];
            best_cat = c;
        }
    }
    if (best_cat >= 0) {
        playerCombination[playerNum+1][best_cat]   = best_score;
        MaskCombination[playerNum+1][best_cat] = 1;
    }
    // 턴 정리
    extern char announcement[];
    if (best_cat >= 0) {
        sprintf(announcement, "AI select: No.%d Combination, score : %d", best_cat+1, best_score);
    }
    extern char package_list[][100];
    extern int turn, rnd;
    preem_handle = 0;
    for (int i = 0; i < N_DICE; i++) MaskVal[i] = diceVal[i] = 0;
    for (int i = 0; i < N_CAT;  i++) diceCombination[i] = 0;
    if (turn+1 < atoi(package_list[0])) turn++;
    else { turn = 0; rnd++; }
    life = 3;
}
////

void scene_1(){
    //visual area
    int diceX, diceY;
    diceX = 1;
    diceY = 20;
    flag = diceVal[0] * diceVal[1] * diceVal[2] * diceVal[3] * diceVal[4];

    diceviewer(diceVal[0], diceX, diceY, 1);
    diceviewer(diceVal[1], diceX + 20, diceY, 2);
    diceviewer(diceVal[2], diceX + 40, diceY, 3);
    diceviewer(diceVal[3], diceX + 60, diceY, 4);
    diceviewer(diceVal[4], diceX + 80, diceY, 5);

    mvprintw(32,0, "Yacht Dice : Roll a dice life = %d", life);
    mvprintw(31,70, ":: Now %s's Turn!", playerlist[turn]);
    mvprintw(33,0, ":: Round - %d", rnd + 1);
    playerScoreBoard(atoi(package_list[0]), 0, 0);
    
    if(strlen(announcement) == 0) {
        mvprintw(32, 70, "G : Roll Dices, J : Fix Dices, L : Write Score");
        mvprintw(33, 70, "P : Quit the game, I : Save Game");
    }
    else{
        mvprintw(32, 70, "%s", announcement);
    }
    
    if(rnd >= 12){
        preem_handle = 0;
        strcpy(announcement, "Game is done. Press y to move to score scene, or P to quit.");
    }
    mainScoreBoard();
    //entry area
    /*
    AI interface adding
    */
    switch(preem_handle){
        case 0:
        int a = handlingEntry(7, "gjlp*yi");
        key = 1;
        if(gamemode == 1 && playerun == turn){
            key = 0;
        }
        else if(gamemode == 1 && playerun != turn){
            key = 1;
            AI_section(turn);
        }
        else if(gamemode == 0){
            key = 0;
        }
        if(a != -1 && key == 0){
            if(a == 0 && life > 0 && rnd < 12){
                rollDices();
                life--;
            }
            else if(a != 3 && diceVal[0] != 0  && rnd < 12){
                preem_handle = a;
            }
            else if(a == 3){
                preem_handle = 3; // Reset preem_handle to reset the game
            }
            else if(a == 4 && rnd < 12){
                life = 10000; // Reset preem_handle to reset the game
            }
            else if(a == 5){
                if(rnd == 12){
                    strcpy(announcement, "");
                    scene = 2; // Move to the score scene
                }
            }
            else if(a == 6){
                int q = saveGameState();
                if(q == 0){
                    strcpy(announcement, "Game saved successfully.");
                }
                else{
                    strcpy(announcement, "Failed to save game.");
                }
            }
        }
        break;
        case 1:
                strcpy(announcement, "What dice do you want to fix or Release ? (1~5) or 0 to Cancel.");
                a = handlingEntry(6, "012345");
                if(a != -1 && diceVal[0] != 0){
                    if(a >= 1 && a <= 5){
                        int index = a - 1; // Convert character to index
                        if(MaskVal[index] == 0){
                            MaskVal[index] = 1; // Set mask to 1 to fix the dice
                            strcpy(announcement, "Fixed Dice.");
                        }
                        else if(MaskVal[index] == 1){
                            MaskVal[index] = 0; // Set mask to 0 to release the dice
                            strcpy(announcement, "Released Dice.");
                        }
                    }
                    else if(a == 0){
                        strcpy(announcement, "");
                        preem_handle = 0; // Reset preem_handle to roll again
                    }
                }
            break;
        case 2:
            inputV(package_list[1], 65, 32);
            strcpy(announcement, "Please enter your dice score number to record it.");
            a = inputEntry(package_list[1]);
            if(a == 1 && atoi(package_list[1]) >= 1 && atoi(package_list[1]) <= 12 && MaskCombination[turn + 1][atoi(package_list[1]) - 1] == 0) {
                playerCombination[turn + 1][atoi(package_list[1]) - 1] = diceCombination[atoi(package_list[1]) - 1];
                MaskCombination[turn + 1][atoi(package_list[1]) - 1] = 1;
                //printf("%d",diceCombination[atoi(package_list[1]) - 1]);
                preem_handle = 0; // Reset preem_handle to roll again
                for(int i = 0; i < 5; i++) {
                    MaskVal[i] = 0; // Reset all masks
                }
                for(int i = 0; i < 5; i++) {
                    diceVal[i] = 0; // Reset all masks
                }
                for(int i = 0; i < 13; i++) {
                    diceCombination[i] = 0; // Reset all masks
                }
                if(turn + 1 < atoi(package_list[0])) {
                    turn++;
                } else {
                    turn = 0; 
                    rnd++;
                }
                life = 3;
                strcpy(announcement, "");
                strcpy(package_list[1], ""); // Reset input cache
            }
            else if(a == 1){
                strcpy(announcement, "Unavailable score number or already used.");
            }
        break;
        case 3:
            preem_handle = 0; // Reset preem_handle to roll again
            for(int i = 0; i < 5; i++) {
                    MaskVal[i] = 0; // Reset all masks
                }
                for(int i = 0; i < 5; i++) {
                    diceVal[i] = 0; // Reset all masks
                }
                for(int i = 0; i < 13; i++) {
                    diceCombination[i] = 0; // Reset all masks
                }
                for(int i = 0; i < atoi(package_list[0]); i++) {
                    for(int j = 0; j < 13; j++) {
                        playerCombination[i + 1][j] = 0;
                    }
                }
                for(int i = 0; i < atoi(package_list[0]); i++) {
                    for(int j = 0; j < 13; j++) {
                        MaskCombination[i + 1][j] = 0;
                    }
                }
                rnd = 0;
                turn = 0;
                life = 3;
                strcpy(announcement, "");
                strcpy(package_list[1], ""); // Reset input cache
            scene = 0;
        break;
    }
}

void scene_2(){
    //visual area
    playerScoreBoard(atoi(package_list[0]), 0, 0);
    //entry area
    switch(preem_handle){
        case 0:
            break;
        case 1:
            break;
    }
}
////////////////////////////////////////////////////////
/* main core process -> There is Scene manage sector */
int main(){
    init_game();
    //start

    //update
    while(!game_over){
        if (quit_requested) {
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
            ////////////////////////////////////////////////////////
            /* Scene manage Sector */
            clear();
            switch(scene){ //scene manager
                case 0:
                    mainscene();
                    break;
                case 1:
                    refresh();
                    scene_1();
                    break;
                case 2:
                    refresh();
                    scene_2();
                    //scene setting
                    break;
                case 3:
                    refresh();
                    //scene setting
                    break;
                default:
                    continue;
            }
            refresh();
            ////////////////////////////////////////////////////////
        }
        usleep(100000);
    }
    end_game();
    return 0;
}