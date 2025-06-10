# 🎲 YachtDice (Linux 기반 콘솔 게임)  


**YachtDice**는 리눅스 환경에서 동작하는 콘솔 기반 주사위 게임입니다.


`ncurses` 라이브러리를 활용해 터미널 GUI로 구현되었으며, 다양한 시스템 콜과 시그널 핸들링을 통해 안정적이고 직관적인 인터페이스를 제공합니다.

-----------------------

 ## 🔧 컴파일 방법

- gcc -o game game.c -lncurses

		game.c: 메인 소스 코드

		lncurses: ncurses 라이브러리 링크
  


## ▶️ 실행 방법

- ./game



## 📜 사용한 헤더 파일

- #include <ncurses.h>

- #include <stdlib.h>

- #include <time.h>

- #include <unistd.h>

- #include <signal.h>

- #include <math.h>

- #include <string.h>

- #include <form.h>

- #include <fcntl.h>

- #include <sys/stat.h>



## ⚙️ 주요 기능 및 구성

### ncurses 기반 터미널 UI
- 커서 제어, 컬러 설정, 사용자 입력 처리 등  

### 시그널 처리
- SIGWINCH : 터미널 창 크기 변경 대응

- SIGINT : Ctrl+C 종료 인터럽트 처리

- SIGTSTP : Ctrl+Z 중단에 대한 사용자 정의 동작 처리

### 시스템 콜 사용

- open, close, write, creat : 게임 데이터 저장 또는 로그 기록

- sigaction : 시그널 처리 구조적 구현

  

### 게임 룰

- Yahtzee (YachtDice) 룰 기반

- 점수판 구성, 주사위 굴리기, Small Straight 등 다양한 족보 구현

  

## 📂 파일 구성

1. game : 컴파일된 메인 소스코드 프로그램

2. game.c : 메인 소스 코드

3. README.md : 설명 문서(현재 파일)

4. save.txt : 진행 기록 저장 .txt 파일

  

## 🛠️ 개발 환경

- OS : Linux(WSL기반 Linux)

- Compiler : GCC

- Library : ncurses

  

## 📌 주의 사항

- ncurses 라이브러리가 설치되어있어야 합니다. 설치가 필요할 경우:

		sudo apt-get install libncurses5-dev libncursesw5-dev

  

## 🧑‍💻 제작자

- 김선호 (EOH0) 

- 김재원 (mincho)