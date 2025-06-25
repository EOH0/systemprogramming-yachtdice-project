

# 🎲 YachtDice (Linux 기반 콘솔 게임)

  
 
**YachtDice**는 리눅스 환경에서 동작하는 콘솔 기반 주사위 게임입니다.


`ncurses` 라이브러리를 활용해 터미널 GUI로 구현되었으며, 다양한 시스템 콜과 시그널 핸들링을 통해 안정적이고 직관적인 인터페이스를 제공합니다.

## ✅ 개요
- 프로젝트 이름: YachtDice
- 프로젝트 개발 기간: 2025.05.01 - 2025.06.12

## 🧑‍💻 제작자

- 김선호 (EOH0)

- 김재원 (mincho)

## 🛠️ 개발 환경

  

- OS : Linux(WSL기반 Linux)

  

- Compiler : GCC

  

- Library : ncurses
  

## 🔧 컴파일 방법

  

- gcc -o game game.c -lncurses

  

		game.c: 메인 소스 코드
		lncurses: ncurses 라이브러리 링크

  
  

## ▶️ 실행 방법

- ./game

## 📖 게임 룰

  ![YachtDice](https://mblogthumb-phinf.pstatic.net/MjAyMDA5MDRfMjA5/MDAxNTk5MTQ4MTM4NTEz.5d_wY1wKg99PS_tu3L_Q0aPK6I8g-hxFsyHcj-WrSkYg.aVEvpy16kSu8UAIwxK47V4kZi1hYKD0nxCYfD8swWHMg.PNG.doek11/2020090317313132.mp4_20200904_004854.109.png?type=w800)

1. 주사위 5개를 굴려서 특정 조합을 만들어 점수를 얻는 것이다.

2. 자신의 차례에 최대 세 번 굴려서 다양한 조합을 시도할 수 있다.
	- 이때 주사위를 고정하여 원하는 조합을 완성할 수 있다.
3. 총 12 라운드로 구성되며, 매 라운드마다 플레이어는 해당 라운드에 사용할 득점 카테고리(이하 족보)를 선택한다.
	- 이때 한 번 선택한 족보는 다시 선택할 수 없다.
	- 족보는 주사위 값의 조합에 따라 점수가 달라지거나, 족보 별로 고정된 점수가 있을 수 있다.
  4. Yacht는 5개의 주사위 모두 같은 눈금으로 완성되었을 때 점수를 얻을 수 있고, 고정적으로 족보 내 가장 높은 점수인 50점을 획득한다.

## 🖥️ 구현
- 주사위 기능
	- 각 라운드 내 자신의 차례에 각 플레이어 당 최대 세 번까지 주사위 굴리기
	- 돌리지 않을 주사위 고정
	- 주사위 눈금을 기준으로 sort
	- 각 주사위 눈금 나온 횟수 저장
	
-  점수판 기능
	- 현재 주사위 상태에 따라 유효한 족보 탐색
	- 플레이어가 원하는 족보 선택
	- 점수 저장 및 불러오기
	
- 족보 기능
	 - 점수 기록 합산
	 - Bouns 점수 계산

## 🎮조작 방법
- 기본적으로 사용자의 키보드 입력으로 이루어집니다. 
- 초기 화면
- <a href="https://ibb.co/8g4Tz9X0"><img src="https://i.ibb.co/DfgsGQLt/2025-06-10-203520.png" alt="2025-06-10-203520" border="0"></a>
	 - "W" : 초기 화면 메뉴 선택 커서 이동 - 위
	- "S" : 초기 화면 메뉴 선택 커서 이동 - 아래
---
- Local Play를 시작할때 참여하는 플레이어 수 설정 (2 ~ 4 입력)
 - <a href="https://ibb.co/S4fMzJJH"><img src="https://i.ibb.co/5gKSP22C/image.png" alt="image" border="0"></a>
---
- 메인 게임 화면
 - <a href="https://ibb.co/F4DT0Zfp"><img src="https://i.ibb.co/hFmzZQn0/image.png" alt="image" border="0"></a>
 	- "G" : 주사위 굴리기 
	 	- life 값에 따라 플레이어 차례 당 3번
	- "J" : 주사위 고정/해제하기 
		- 입력 후 해당 주사위 번호 입력
	- "L" : 족보 선택하기 
		- 입력 후 해당 족보 번호 입력
	- "P" : 로비로 나가기 
		- 입력 즉시 로비로 나가기
	- "I" : 게임 저장하기 
		- save.txt 파일에 현재 라운드, 차례, 인원 수, 플레이어 각자의 점수판과 족보 선택여부를 저장
		- 가장 최근의 1개 게임만 저장
---
- 주사위 1회 굴렸을 때
-  <a href="https://ibb.co/N2YhpLT1"><img src="https://i.ibb.co/YBj98t0D/image.png" alt="image" border="0"></a>
  ---
- 주사위 고정/해제하기 위해 J 입력 후 주사위 숫자 입력
	- 주사위 번호가 1 - 5까지의 정수가 아닌 x인 경우 고정된 주사위
    
 - <a href="https://ibb.co/ds69b9cn"><img src="https://i.ibb.co/8n23z3BF/image.png" alt="image" border="0"></a>
-  <a href="https://ibb.co/p6nMMN9X"><img src="https://i.ibb.co/kVMwwzWc/image.png" alt="image" border="0"></a>
---
- 9번 FullHouse 선택 후 점수 입력
- <a href="https://ibb.co/cXZJwzCw"><img src="https://i.ibb.co/7NZvQ7RQ/image.png" alt="image" border="0"></a>
- <a href="https://ibb.co/TBbf4Ttp"><img src="https://i.ibb.co/bg20s3Xq/image.png" alt="image" border="0"></a>
---
- "I"를 입력해 저장 (초기 화면의 Load Local Play를 통해 불러오기)
<a href="https://ibb.co/TxZBKsdF"><img src="https://i.ibb.co/mVmCv3gM/image.png" alt="image" border="0"></a>

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

  
  
- #include <arpa/inet.h>


## ⚙️ 주요 기능 및 구성

  

### ncurses 기반 터미널 UI

- 커서 제어, 컬러 설정, 사용자 입력 처리 등

- 비동기적 입력 처리를 ncurses로 구현.

  

### 시그널 처리

- SIGWINCH : 터미널 창 크기 변경 대응

  

- SIGINT : Ctrl+C 종료 인터럽트 처리

  

- SIGTSTP : Ctrl+Z 중단에 대한 사용자 정의 동작 처리

  

### 시스템 콜 사용

  

- open, close, write, creat : 게임 데이터 저장 또는 로그 기록

  

- sigaction : 시그널 처리 구조적 구현

  
## 📂 파일 구성

  

1. game : 컴파일된 메인 소스코드 프로그램

  

2. game.c : 메인 소스 코드

  

3. README.md : 설명 문서(현재 파일)

  

4. save.txt : 진행 기록 저장 .txt 파일

  

## 📌 주의 사항

  

- ncurses 라이브러리가 설치되어있어야 합니다. 설치가 필요할 경우:

  

		sudo apt-get install libncurses5-dev libncursesw5-dev



- Network play의 경우, subnet mask를 통한 broadcast로 멀티플레이를 구현하려 했으나, 클라이언트에서 서버로의 통신은 가능하나, 서버에서 다시 클라이언트로 응답처리가 잘 안되어 절반만 구현되었습니다.

- Ctrl-C 인터럽트 핸들링이 구현되어있으며, 직접적으로 화면에 표시되지 않지만, ctrl-c 이후 y를 누르면 게임이 강제종료되며, n을 누르면 다시 정상 상태로 복귀됩니다.


 
