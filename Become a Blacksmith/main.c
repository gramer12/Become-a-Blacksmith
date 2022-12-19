#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

#define ITEM_LIST_SIZE 58 // itemList의 크기
#define INVENTORY_SIZE 8
#define SHOP_SIZE 6

struct itemList { // 아이템들의 정보를 담는 구조체
	int code;
	char name[30];
	int price;
	int rarity;
	int type;
}item[ITEM_LIST_SIZE];

struct shop { // 상점의 정보를 담는 구조체
	char name[30];
	int rarity;
	int price;
	char toolTip[100];
}shop[SHOP_SIZE];

struct inventory { // 인벤토리의 정보를 담는 구조체
	int code;
	int reinforce;
	int quality;
	int enchant;
	int enchantTry;
	int price;
}inven[INVENTORY_SIZE];

struct reinforce { // 강화 성공률 및 강화 성공시 가격 증가량을 담는 구조체
	int success;
	double rate;
}reinforce[20];

// 내용이 없거나, 선택되지 않았거나 하는 등 공란을 표시하는 숫자는 -1로 통일
// gotoxy() 괄호 안에 n - strlen( 문자열/2 ) 은 n을 기준으로 문자 길이를 측정해서 /2 한 값 만큼 x좌표를 이동해서 문자를 가운데정렬처럼 보이게 하기 위함
// toolTip 함수들의 경우 ask의 값에 따라서 다른 텍스트를 출력하도록 구현함

int gold = 0; // 골드 소지금
int screen = 3; // 현재 화면을 숫자로 나타낸 값
				// -1:최초실행 시 0:강화 1:가방 2:상점 3:메인
int menu = 0; // 메뉴창이 열렸는지 닫혔는지
			  // 0:메뉴창 닫힘 1:메뉴창 열림
int selectItem = -1; // 현재 선택된 아이템이 무엇인지 저장하는 전역 변수. -1은 현재 선택된 아이템 없음
int choiceMenu = 0; //선택 정보 저장값(메뉴)
int choice1 = 0; //선택 정보 저장값(다용도 1)
int choice2 = 0; //선택 정보 저장값(다용도 2)
int choiceNum = 0; // 몇개의 선택지를 가진 질문을 보여주는지
int ask = 0; // 현재 물어보는 질문이 어떤건지 판단하는 용도
int temp = 0; // 임시 데이터 저장용
int keyInputLock = 0; // 1일경우 키보드가 입력되지 않는 용도

void cursor(int n); // 커서를 지우는 함수
void gotoxy(int x, int y); // 특정 좌표로 커서를 이동하는 함수

void setting(int x); // 게임 실행에 필요한 환경을 설정
void keybordInput(); // 키보드 입력값에 따라 판단하여 명령 실행
void keybordInputUp(); // 방향키 위쪽을 누르면 실행하는 명령
void keybordInputDown(); // 방향키 아래쪽을 누르면 실행하는 명령
void keybordInputLeft(); // 방향키 왼쪽을 누르면 실행하는 명령
void keybordInputRight(); // 방향키 오른쪽을 누르면 실행하는 명령
void keybordInputSpace(); // 스페이스바를 눌렀을 때 실행하는 명령
void drawSelection(char *location); // ▶ ◀ 를 화면에 그려주는 함수. 문자로 미리 정해 둔 위치를 매개변수로 넘기면 그 위치에 알아서 화살표를 그려준다.
void deleteSelection(char *location); // ▶ ◀ 를 화면에서 지우는 함수. 화살표를 새로 그리기 전에 실행하지 않으면 화면에 잔상처럼 화살표가 남게 된다.
void rarityFontColor(int x); // 아이템의 희귀도(숫자)에 따라서 글자색을 설정한다.
void qualityFontColor(int x); // 아이템의 품질(숫자)에 따라서 글자색을 설정한다.
char *numToQuality(int x); // 아이템의 품질(숫자)에 따라서 다른 글자를 반환해준다.
char *numToType(int x); // 아이템의 종류(숫자)에 따라서 다른 글자를 반환해준다.
char *commify(double val, char *buf, int round); // 천 단위 쉼표 찍어주는 함수
void menuScreen(); // 메뉴화면을 그리는 함수
void reinforceScreen(); // 강화 화면을 그리는 함수
void reinforceScreenTxt(); // 강화 화면에 내용을 출력하는 함수.
void inventoryScreen(); // 인벤토리를 그리는 함수
void mainScreen(); // 게임 실행시 최초화면을 그리는 함수
void shopScreen(); // 상점을 그리는 함수
void deleteToolTip(); // 화면 하단부의 툴팁을 지우는 함수. 새로 툴팁을 출력하기 전에 지우지 않으면 글자가 겹칠 수 있다. (안녕하세요두산이마르고닳도록)
void menuToolTip(); // 메뉴화면의 툴팁 출력
void mainToolTip(); // 메인화면의 툴팁 출력
void inventoryToolTip(); // 인벤토리의 툴팁 출력
void reinforceToolTip(); // 강화화면의 툴팁 출력
void shopToolTip(); // 상점의 툴팁 출력
void sell(); // 아이템 판매
void printfGold(); // 화면에 골드 출력
void inventorySort(struct inventory *inven); // 인벤토리 내용 정렬
void saveData(); // 데이터 저장
void buy(); // 아이템 구매
void buyNomal(int itemCodeStart, int itemCodeRange, int luckyItemCodeStart, int luckyItemCodeRange); // 아이템 구매(일반) 아이템 코드의 시작값과 범위를 지정해서 넘겨줘야 한다.
void buyAllRandom(); // 아이템 구매(올랜덤)
void itemBuyResultToolTip(int invenCode); // 아이템 구매 결과 툴팁 출력
void itemReinforce(); // 아이템 강화하는 함수
void itemEnchant(); // 아이템에 마법부여하는 함수

void main() {
	system("Become a Blacksmith");
	system("mode con cols=81 lines=25"); // 콘솔 창 크기 변경
	cursor(0); // 커서 숨기기
	mainScreen();
	while (1) { // 계속 반복하며 키보드 입력을 받음
		if (keyInputLock == 0) {
			keybordInput();
		}
	}
}

void cursor(int n) { // 커서를 숨기기 위한 함수
	HANDLE hConsole;
	CONSOLE_CURSOR_INFO ConsoleCursor;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	ConsoleCursor.bVisible = n;
	ConsoleCursor.dwSize = 1;

	SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

void gotoxy(int x, int y) { // 커서를 원하는 위치로 이동이동시키기 위한 함수
	COORD CursorPosition = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), CursorPosition);
}

void setting(int x) { // 게임 실행에 필요한 환경을 설정
	int i;
	FILE *itemList, *shopList, *systemf, *saveFile;

	itemList = fopen("data\\system\\itemList.txt", "r");
	if (itemList == NULL) {
		return 0;
	}

	for (i = 0; i < ITEM_LIST_SIZE; i++) {
		fscanf(itemList, "%d/%[^\n^/]/%d/%d/%d\n", &item[i].code, &item[i].name, &item[i].price, &item[i].rarity, &item[i].type);
	}

	fclose(itemList);

	shopList = fopen("data\\system\\shop.txt", "r");
	if (shopList == NULL) {
		return 1;
	}

	for (i = 0; i < SHOP_SIZE; i++) {
		fscanf(shopList, "%[^\n^/]/%d/%d/%[^\n^/]\n", &shop[i].name, &shop[i].rarity, &shop[i].price, &shop[i].toolTip);
	}

	fclose(itemList);

	systemf = fopen("data\\system\\system.txt", "r");

	for (i = 0; i < 20; i++) {
		fscanf(systemf, "%d/%lf\n", &reinforce[i].success, &reinforce[i].rate);
	}


	if (x == 0) { // 전달받은 값이 0이면 인벤토리의 내용을 기존 세이브파일에서 불러옴
		saveFile = fopen("data\\system\\saveFile.txt", "r");

		fscanf(saveFile, "%d\n", &gold);

		for (i = 0; i < INVENTORY_SIZE; i++) {
			fscanf(saveFile, "%d/%d/%d/%d/%d/%d\n", &inven[i].code, &inven[i].reinforce, &inven[i].quality, &inven[i].enchant, &inven[i].enchantTry, &inven[i].price);
		}

		fclose(saveFile);
	}
	else { // 아니면 골드를 5천원으로 설정하고 인벤토리의 내용을 초기화
		gold = 5000;

		for (i = 0; i < INVENTORY_SIZE; i++) {
			inven[i].code = -1;
			inven[i].reinforce = -1;
			inven[i].quality = -1;
			inven[i].enchant = -1;
			inven[i].enchantTry = -1;
			inven[i].price = -1;
		}

		saveData(); // 초기화된 내용 저장
	}
}

void keybordInput() {
	int key = getch();

	switch (key) {
	case 13: // 엔터누르면 메뉴띄움
		PlaySound("data\\sound\\key", NULL, SND_ASYNC);
		if (screen != 3) {
			menuScreen();
		}
		break;
	case 32: // 스페이스 누르면 명령 실행
		PlaySound("data\\sound\\key", NULL, SND_ASYNC);
		keybordInputSpace();
		break;
	case 224: // 방향키 명령 실행
		PlaySound("data\\sound\\key", NULL, SND_ASYNC);
		key = getch();
		switch (key) {
		case 72:
			keybordInputUp();
			break;
		case 80:
			keybordInputDown();
			break;
		case 75:
			keybordInputLeft();
			break;
		case 77:
			keybordInputRight();
			break;
		}
		break;
	}
}

void keybordInputUp() {
	if (menu == 1) { // 메뉴가 떠있을 때
		deleteSelection("menu"); // choiceMenu 의 값을 기준으로 기존 화살표를 지움
		choiceMenu--;
		if (choiceMenu == -1) { // 메뉴가 총 5개(0 1 2 3 4) 이므로 0보다 작으면 4로 변경
			choiceMenu = 4;
		}
		menuToolTip(); // 메뉴 툴팁 띄워줌
		drawSelection("menu"); // 화살표 그림
	}
	else if (ask == 0) {
		if (screen == 0 && selectItem != -1) {
			deleteSelection("reinforce");
			choice1--;
			if (choice1 == -1) {
				choice1 = 1;
			}
			drawSelection("reinforce");
		}
		else if (screen == 1 || screen == 2) {
			deleteSelection("inven");
			choice1--;
			if (choice1 == -1) {
				choice1 = 7;
			}
			if (screen == 2) {
				shopToolTip();
			}
			drawSelection("inven");
		}
		else if (screen == 3) {
			deleteSelection("main");
			choice1--;
			if (choice1 == -1) {
				choice1 = 1;
			}
			mainToolTip();
			drawSelection("main");
		}
	}

}

void keybordInputDown() {
	if (menu == 1) {
		deleteSelection("menu");
		choiceMenu++;
		if (choiceMenu == 5) {
			choiceMenu = 0;
		}
		menuToolTip();
		drawSelection("menu");
	}
	else if (ask == 0) {
		if (screen == 0 && selectItem != -1) {
			deleteSelection("reinforce");
			choice1++;
			if (choice1 == 2) {
				choice1 = 0;
			}
			drawSelection("reinforce");
		}
		else if (screen == 1 || screen == 2) {
			deleteSelection("inven");
			choice1++;

			if (choice1 == 8) {
				choice1 = 0;
			}
			if (screen == 2) {
				shopToolTip();
			}
			drawSelection("inven");
		}
		else if (screen == 3) {
			deleteSelection("main");
			choice1++;
			if (choice1 == 2) {
				choice1 = 0;
			}
			mainToolTip();
			drawSelection("main");
		}
	}
}

void keybordInputLeft() {
	if (screen == 0 || screen == 1 || screen == 2) {
		if (choiceNum > 0) {
			deleteSelection("select");
			choice2--;
			if (choice2 < 0) {
				choice2 = choiceNum;
			}
			drawSelection("select");
		}
	}
}

void keybordInputRight() {
	if (screen == 0 || screen == 1 || screen == 2) {
		if (choiceNum > 0) {
			deleteSelection("select");
			choice2++;
			if (choice2 > choiceNum) {
				choice2 = 0;
			}
			drawSelection("select");
		}
	}
}

void keybordInputSpace() {
	if (menu == 1) {
		menu = 0;
		switch (choiceMenu) {
		case 0:
			choice1 = 0;
			reinforceScreen();
			break;
		case 1:
			inventoryScreen();
			break;
		case 2:
			shopScreen();
			break;
		case 3:
			mainScreen();
			break;
		case 4:
			exit(1);
			break;
		}
	}
	else {
		double priceBonus = 1;

		switch (screen) {
		case 0:
			if (selectItem != -1) { // 아이템을 선택했는지 확인
				switch (ask) {
				case 0:
					choiceNum = 1;
					ask = 1;
					reinforceToolTip();
					choice2 = 0;
					drawSelection("select");
					break;
				case 1: // 정말 강화하시겠습니까?
					if (inven[selectItem].enchant == 5) { // 강화비용 감소 마법부여가 있는지
						priceBonus = 0.5;
					}

					if (choice2 == 0) {
						switch (choice1) {
						case 0: // 강화하기일 때
							if (gold < (int)(inven[selectItem].price * 0.1 * priceBonus)) { // 강화비용보다 돈이 적지 않은지
								choiceNum = 0;
								ask = 2;
								reinforceToolTip();
								drawSelection("select");
							}
							else { // 아니면 강화함
								choice2 = 0;
								ask = 0;
								choiceNum = 0;
								deleteToolTip();
								itemReinforce();
							}
							break;
						case 1: // 마부하기일 때
							if (gold < (int)(inven[selectItem].price * 0.1 * (inven[selectItem].enchantTry + 1))) {
								choiceNum = 0;
								ask = 2;
								reinforceToolTip();
								drawSelection("select");
							}
							else {
								choice2 = 0;
								ask = 0;
								choiceNum = 0;
								deleteToolTip();
								itemEnchant();
							}
							break;
						}
					}
					else if (choice2 == 1) {
						choice2 = 0;
						ask = 0;
						choiceNum = 0;
						deleteToolTip();
					}
					break;
				case 2:
					choiceNum = 0;
					ask = 0;
					deleteToolTip();
					break;
				}
			}
			break;
		case 1: // 가방을 연 상태
			if (inven[choice1].code != -1) {
				switch (ask) {
				case 0: // 질문을 하지 않은 상태
					choiceNum = 2;
					ask = 1;
					inventoryToolTip();
					choice2 = 0;
					drawSelection("select");
					break;
				case 1: // 아이템을 강화할지 팔건지 그만둘지 묻는 상태
					switch (choice2) {
					case 0: // 0번 선택 - 강화기에 올린다
						selectItem = choice1;
						choice1 = 0;
						choiceMenu = 0;
						reinforceScreen();
						break;
					case 1: // 1번 선택 - 아이템을 정말 팔건지 묻는 질문으로 넘어간다
						choiceNum = 1;
						choice2 = 0;
						ask = 2;
						inventoryToolTip();
						drawSelection("select");
						break;
					case 2: // 2번 선택 - 선택을 취소하고 다시 아이템을 고른다
						choiceNum = 0;
						ask = 0;
						inventoryToolTip();
						break;
					}
					break;
				case 2: // 아이템을 정말 팔건지 묻는 상태
					switch (choice2) {
					case 0: // 네
						choiceNum = 0;
						ask = 0;
						deleteSelection("select");
						inventoryToolTip();
						temp = 0;
						choice2 = 0;
						drawSelection("inven");
						sell();
						inventoryScreen();
						break;
					case 1: // 아니요
						choiceNum = 2;
						ask = 1;
						inventoryToolTip();
						choice2 = 0;
						drawSelection("select");
						break;
					}
				}
				break;
			}
			break;
		case 2:
			switch (ask) {
			case 0: // 상점에서 스페이스바를 누르지 않은 상태
				if (choice1 < SHOP_SIZE) {
					choiceNum = 1;
					ask = 1;
					shopToolTip();
					choice2 = 0;
					drawSelection("select");
				}
				break;
			case 1:// 정말 살건지 물어봄
				switch (choice2) {
				case 0: // 네
					if (inven[INVENTORY_SIZE - 1].code != -1) { // 가방에 빈 공간 확인
						choiceNum = 0;
						ask = 2;
						shopToolTip();
						drawSelection("select");
					}
					else if (gold < shop[choice1].price) { // 돈이 부족하진 않는지 확인
						choiceNum = 0;
						ask = 3;
						shopToolTip();
						drawSelection("select");
					}
					else { // 문제 없으면 구매처리
						choiceNum = 0;
						ask = 4;
						choice2 = 0;
						buy();
						drawSelection("select");
					}
					break;
				case 1: // 아니요
					choiceNum = 0;
					ask = 0;
					shopToolTip();
					break;
				}
				break;
			case 2:
				choiceNum = 0;
				ask = 0;
				shopToolTip();
				break;
			case 3:
				choiceNum = 0;
				ask = 0;
				shopToolTip();
				break;
			case 4:
				choiceNum = 0;
				ask = 0;
				shopToolTip();
				break;
			}
			break;
		case 3: // 메인화면에서 스페이스바를 누르면
			switch (choice1) {
			case 0: // 이어하기 선택 시
				setting(0);
				break;
			case 1: // 새로하기 선택 시
				setting(1);
				break;
			}
			screen = -1;
			menuScreen();
			break;
		}
	}
}

void drawSelection(char *location) { // 화살표를 그릴 위치를 입력받아 화면에 그려주는 함수
	int x1, x2, y;

	if (!strcmp(location, "menu")) {
		x1 = 33;
		x2 = 45;
		y = 9 + choiceMenu * 2;
	}
	else if (!strcmp(location, "inven")) {
		x1 = -1;
		x2 = 75;
		y = 3 + choice1 * 2;
	}
	else if (!strcmp(location, "select")) {
		x1 = -1;
		x2 = 75 - (choiceNum * 16) + choice2 * 16;
		y = 23;
	}
	else if (!strcmp(location, "reinforce")) {
		x1 = 62;
		x2 = 74;
		y = 4 + choice1 * 2;
	}
	else if (!strcmp(location, "main")) {
		x1 = 31;
		x2 = 48;
		y = 17 + choice1 * 2;
	}

	if (x1 != -1) {
		gotoxy(x1, y);
		printf("▶");
	}

	if (x2 != -1) {
		gotoxy(x2, y);
		printf("◀");
	}
}

void deleteSelection(char *location) { // 화살표를 지울 위치를 입력받아 화면에서 지워주는 함수
	int x1, x2, y;

	if (!strcmp(location, "menu")) {
		x1 = 33;
		x2 = 45;
		y = 9 + choiceMenu * 2;
	}
	else if (!strcmp(location, "inven")) {
		x1 = -1;
		x2 = 75;
		y = 3 + choice1 * 2;
	}
	else if (!strcmp(location, "select")) {
		x1 = -1;
		x2 = 75 - (choiceNum * 16) + choice2 * 16;
		y = 23;
	}
	else if (!strcmp(location, "reinforce")) {
		x1 = 62;
		x2 = 74;
		y = 4 + choice1 * 2;
	}
	else if (!strcmp(location, "main")) {
		x1 = 31;
		x2 = 48;
		y = 17 + choice1 * 2;
	}

	if (x1 != -1) {
		gotoxy(x1, y);
		printf("  ");
	}

	if (x2 != -1) {
		gotoxy(x2, y);
		printf("  ");
	}
}

void rarityFontColor(int x) {
	switch (x) {
	case 0:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
		break;
	case 1:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
		break;
	case 2:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
		break;
	case 3:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 5);
		break;
	case 4:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
		break;
	case 5:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
		break;
	}
}

void qualityFontColor(int x) {
	switch (x) {
	case 0:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
		break;
	case 1:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		break;
	case 2:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
		break;
	case 3:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
		break;
	case 4:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
		break;
	}
}

char *numToQuality(int x)
{
	char *str = malloc(sizeof(char) * 6);

	switch (x) {
	case 0:
		strcpy(str, "최하급");
		break;
	case 1:
		strcpy(str, "하급");
		break;
	case 2:
		strcpy(str, "중급");
		break;
	case 3:
		strcpy(str, "상급");
		break;
	case 4:
		strcpy(str, "최상급");
		break;
	}

	return str;
}

char *numToType(int x)
{
	char *str = malloc(sizeof(char) * 6);

	switch (x) {
	case 0:
		strcpy(str, "도검");
		break;
	case 1:
		strcpy(str, "단검");
		break;
	case 2:
		strcpy(str, "둔기");
		break;
	case 3:
		strcpy(str, "도끼");
		break;
	case 4:
		strcpy(str, "지팡이");
		break;
	case 5:
		strcpy(str, "활");
		break;
	case 6:
		strcpy(str, "총");
		break;
	case 7:
		strcpy(str, "마법봉");
		break;
	case 8:
		strcpy(str, "장착무기");
		break;
	}

	return str;
}

char *numToEnchant(int x)
{
	char *str = malloc(sizeof(char) * 6);

	switch (x) {
	case 0:
		strcpy(str, "없음");
		break;
	case 1:
		strcpy(str, "축복");
		break;
	case 2:
		strcpy(str, "명품");
		break;
	case 3:
		strcpy(str, "잠재");
		break;
	case 4:
		strcpy(str, "보호");
		break;
	case 5:
		strcpy(str, "경화");
		break;
	}

	return str;
}

char *commify(double val, char *buf, int round) { // 천 단위 구분 쉼표 찍어주는 함수
	static char *result;
	char *nmr;
	int dp, sign;
	result = buf;
	if (round < 0)                        /*  Be sure round-off is positive  */
		round = -round;

	nmr = _fcvt(val, round, &dp, &sign);   /*  Convert number to a string     */

	if (sign)                             /*  Prefix minus sign if negative  */
		*buf++ = '-';

	if (dp <= 0) {                         /*  Check if number is less than 1 */
		if (dp < -round)                    /*  Set dp to max(dp, -round)      */
			dp = -round;
		*buf++ = '0';                       /*  Prefix with "0."               */
		*buf++ = '.';
		while (dp++)                        /*  Write zeros following decimal  */
			*buf++ = '0';                     /*     point                       */
	}
	else {                                /*  Number is >= 1, commify it     */
		while (dp--) {
			*buf++ = *nmr++;
			if (dp % 3 == 0)
				*buf++ = dp ? ',' : ' ';
		}
	}

	strcpy(buf, nmr);
	strcat(buf, "G");
	return result;                        /*  following dec pt              */
}

void menuScreen() {
	if (menu == 0) {
		gotoxy(0, 0);
		printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
		printf("┃                                                                            ┃\n");
		printf("┃                                                                            ┃\n");
		printf("┃                                                                            ┃\n");
		printf("┃                                                                            ┃\n");
		printf("┃                           ┏━━━━━━━━━┓                           ┃\n");
		printf("┃                           ┃       Menu       ┃                           ┃\n");
		printf("┃                           ┣━━━━━━━━━┫                           ┃\n");
		printf("┃                           ┃                  ┃                           ┃\n");
		printf("┃                           ┃      강  화      ┃                           ┃\n");
		printf("┃                           ┃                  ┃    Select : Arrow Kyes    ┃\n");
		printf("┃                           ┃      가  방      ┃                           ┃\n");
		printf("┃                           ┃                  ┃                           ┃\n");
		printf("┃                           ┃      상  점      ┃        OK : Space         ┃\n");
		printf("┃                           ┃                  ┃                           ┃\n");
		printf("┃                           ┃      메  인      ┃                           ┃\n");
		printf("┃                           ┃                  ┃      Menu : Enter         ┃\n");
		printf("┃                           ┃      종  료      ┃                           ┃\n");
		printf("┃                           ┃                  ┃                           ┃\n");
		printf("┃                           ┗━━━━━━━━━┛                           ┃\n");
		printf("┃                                                                            ┃\n");
		printf("┃                                                                            ┃\n");
		printf("┃                                                                            ┃\n");
		printf("┃                                                                            ┃\n");
		printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");

		menu = 1;

		if (screen == -1) {
			choiceMenu = 0;
		}
		else {
			choiceMenu = screen;
		}

		menuToolTip();
		drawSelection("menu");
	}
}

void reinforceScreen() {
	gotoxy(0, 0);
	printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━┓\n");
	printf("┃                                                        ┃      목  록      ┃\n");
	printf("┃                                                        ┣━━━━━━━━━┫\n");
	printf("┃                                                        ┃                  ┃\n");
	printf("┃                                                        ┃      강  화      ┃\n");
	printf("┃                                                        ┃                  ┃\n");
	printf("┃                                                        ┃      마  부      ┃\n");
	printf("┃                                                        ┃                  ┃\n");
	printf("┃                                                        ┃                  ┃\n");
	printf("┃                                                        ┃                  ┃\n");
	printf("┃                                                        ┃                  ┃\n");
	printf("┃                                                        ┃                  ┃\n");
	printf("┃                                                        ┃                  ┃\n");
	printf("┃                                                        ┃                  ┃\n");
	printf("┃                                                        ┃                  ┃\n");
	printf("┃                                                        ┃                  ┃\n");
	printf("┃                                                        ┃                  ┃\n");
	printf("┃                                                        ┃                  ┃\n");
	printf("┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━┫\n");
	printf("┃                                                                            ┃\n");
	printf("┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");
	printf("┃                                                                            ┃\n");
	printf("┃                                                                            ┃\n");
	printf("┃                                                                            ┃\n");
	printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");

	screen = 0;
	choice2 = 0;
	ask = 0;
	choiceNum = 0;
	drawSelection("reinforce");

	reinforceScreenTxt();
}

void reinforceScreenTxt() {
	int starCount = 0;
	int enchantBonus = 0;
	double rateBonus = 1;
	char star[100] = "";
	char *quality;
	char *type;
	char price[100] = "";
	char price1[50] = "";
	char price2[50] = "";
	char *enchant, enchantTxt[100] = "";

	if (selectItem == -1) {
		gotoxy(12, 3);
		printf("가방에서 강화할 아이템을 선택하세요");
	}
	else {
		for (int i = 0; i < ITEM_LIST_SIZE; i++) {
			if (inven[selectItem].code == item[i].code) {
				rarityFontColor(item[i].rarity);
				gotoxy(29 - (int)(strlen(item[i].name) / 2), 2);
				printf("%s", item[i].name);
				// 여기까지 아이템 이름 출력
				for (int i = 0; i < inven[selectItem].reinforce; i++) {
					strcat(star, "★");
					starCount++;
					if (starCount == 5) { //별 갯수가 카운트가 5개면 공백 한번 출력하고 카운트 0으로
						strcat(star, " ");
						starCount = 0;
					}
				}
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
				gotoxy(29 - (int)(strlen(star) / 2), 4);
				printf("%s", star);
				//여기까지 별 출력
				qualityFontColor(inven[selectItem].quality);
				quality = numToQuality(inven[selectItem].quality);
				type = numToType(item[i].type);
				gotoxy(29 - (int)((strlen(quality) + strlen(type) + 1) / 2), 6);
				printf("%s %s", quality, type);
				//여기까지 아이템 종류 및 품질 출력
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				if (inven[selectItem].reinforce < 9) {
					gotoxy(24, 8);
				}
				else if (inven[selectItem].reinforce == 9) {
					gotoxy(24, 8);
				}
				else if (inven[selectItem].reinforce > 9) {
					gotoxy(23, 8);
				}

				printf("%d", inven[selectItem].reinforce); printf("성  >  "); printf("%d", inven[selectItem].reinforce + 1); printf("성");
				//여기까지 몇성인지 문자 출력
				if (inven[selectItem].enchant == 1) {
					enchantBonus = 5;
				}

				gotoxy(22, 10);
				printf("성공확률 : %d%%", reinforce[inven[selectItem].reinforce].success + enchantBonus);
				//여기까지 강화확률 출력
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
				*commify(inven[selectItem].price, price1, 0);

				if (inven[selectItem].enchant == 3) {
					rateBonus = 1.1;
				}

				*commify((int)(inven[selectItem].price * reinforce[inven[selectItem].reinforce].rate * rateBonus), price2, 0);
				gotoxy(27, 12);
				printf("  >  ");
				gotoxy(27 - strlen(price1), 12);
				printf("%s", price1);
				gotoxy(32, 12);
				printf("%s", price2);
				//여기까지 강화전 가격 > 강화 후 가격 출력


				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
				if (inven[selectItem].enchant == 0) {
					gotoxy(23, 14);
					printf("마법부여 없음");
				}
				else {
					enchant = numToEnchant(inven[selectItem].enchant);
					gotoxy(29 - (int)(strlen(enchant) / 2), 14);
					printf("%s", enchant);

					switch (inven[selectItem].enchant) {
					case 0:
						strcpy(enchantTxt, " ");
						break;
					case 1:
						strcpy(enchantTxt, "강화성공률 5% 증가");
						break;
					case 2:
						strcpy(enchantTxt, "판매 시 20%의 추가 골드 획득");
						break;
					case 3:
						strcpy(enchantTxt, "강화 성공 시 가격 10% 추가 증가");
						break;
					case 4:
						strcpy(enchantTxt, "강화 실패 시 50%의 확률로 파괴되지 않음");
						break;
					case 5:
						strcpy(enchantTxt, "강화 비용 50% 감소");
						break;
					}

					gotoxy(29 - (int)(strlen(enchantTxt) / 2), 16);
					printf("%s", enchantTxt);
					//여기까지 마법부여 이름 및 내용 출력
				}
				break;
			}
		}
	}

	printfGold();
}

void inventoryScreen() {
	char *quality, price[20], *enchant;
	gotoxy(0, 0);
	printf("┏━━━━━━━━━━━━━━━━┳━━┳━━━┳━━━┳━━━━━━━┳━━┓\n");
	printf("┃             이  름             ┃    ┃ 품질 ┃ 마부 ┃    가  격    ┃ ◀ ┃\n");
	printf("┣━━━━━━━━━━━━━━━━╋━━╋━━━╋━━━╋━━━━━━━╋━━┫\n");
	printf("┃                                ┃    ┃      ┃      ┃              ┃    ┃\n");
	printf("┣━━━━━━━━━━━━━━━━╋━━╋━━━╋━━━╋━━━━━━━╋━━┫\n");
	printf("┃                                ┃    ┃      ┃      ┃              ┃    ┃\n");
	printf("┣━━━━━━━━━━━━━━━━╋━━╋━━━╋━━━╋━━━━━━━╋━━┫\n");
	printf("┃                                ┃    ┃      ┃      ┃              ┃    ┃\n");
	printf("┣━━━━━━━━━━━━━━━━╋━━╋━━━╋━━━╋━━━━━━━╋━━┫\n");
	printf("┃                                ┃    ┃      ┃      ┃              ┃    ┃\n");
	printf("┣━━━━━━━━━━━━━━━━╋━━╋━━━╋━━━╋━━━━━━━╋━━┫\n");
	printf("┃                                ┃    ┃      ┃      ┃              ┃    ┃\n");
	printf("┣━━━━━━━━━━━━━━━━╋━━╋━━━╋━━━╋━━━━━━━╋━━┫\n");
	printf("┃                                ┃    ┃      ┃      ┃              ┃    ┃\n");
	printf("┣━━━━━━━━━━━━━━━━╋━━╋━━━╋━━━╋━━━━━━━╋━━┫\n");
	printf("┃                                ┃    ┃      ┃      ┃              ┃    ┃\n");
	printf("┣━━━━━━━━━━━━━━━━╋━━╋━━━╋━━━╋━━━━━━━╋━━┫\n");
	printf("┃                                ┃    ┃      ┃      ┃              ┃    ┃\n");
	printf("┣━━━━━━━━━━━━━━━━┻━━┻━━━┻━━━┻━━━━━━━┻━━┫\n");
	printf("┃                                                                            ┃\n");
	printf("┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");
	printf("┃                                                                            ┃\n");
	printf("┃                                                                            ┃\n");
	printf("┃                                                                            ┃\n");
	printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");

	inventorySort(inven);

	screen = 1;
	choice1 = 0;
	choice2 = 0;
	ask = 0;
	choiceNum = 0;
	drawSelection("inven");
	inventoryToolTip();

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
	gotoxy(37, 1);
	printf("★");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

	for (int i = 0; i < INVENTORY_SIZE; i++) {
		if (inven[i].code != -1) {
			for (int j = 0; item[j].code < ITEM_LIST_SIZE; j++) {
				if (inven[i].code == item[j].code) {
					rarityFontColor(item[j].rarity);
					gotoxy(18 - (int)(strlen(item[j].name) / 2), 3 + 2 * i);
					printf("%s", item[j].name);
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					//아이템 이름 출력
					gotoxy(38 - ((int)inven[i].reinforce < 10 ? 0 : 1), 3 + 2 * i); //칸 맞춤을 위해 숫자가 10보다 크면 좌표 한칸 땡김
					printf("%d", inven[i].reinforce);
					//아이템 강화수치 출력
					qualityFontColor(inven[i].quality);
					quality = numToQuality(inven[i].quality);
					gotoxy(45 - (int)(strlen(quality) / 2), 3 + 2 * i);
					printf("%s", quality);
					//여기까지 품질 글자 출력
					if (inven[i].enchant == 0) {
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
					}
					else {
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
					}

					enchant = numToEnchant(inven[i].enchant);
					gotoxy(53 - (int)(strlen(enchant) / 2), 3 + 2 * i);
					printf("%s", enchant);
					//여기까지 마부 출력
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
					*commify(inven[i].price, price, 0);
					gotoxy(71 - strlen(price), 3 + 2 * i);
					printf("%s", price);
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					//여기까지 가격 출력
				}
			}
		}
	}
	printfGold();
}

void shopScreen() {
	char price[20];
	gotoxy(0, 0);
	printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━┳━━┓\n");
	printf("┃                        이  름                        ┃    가  격    ┃ ◀ ┃\n");
	printf("┣━━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━╋━━┫\n");
	printf("┃                                                      ┃              ┃    ┃\n");
	printf("┣━━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━╋━━┫\n");
	printf("┃                                                      ┃              ┃    ┃\n");
	printf("┣━━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━╋━━┫\n");
	printf("┃                                                      ┃              ┃    ┃\n");
	printf("┣━━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━╋━━┫\n");
	printf("┃                                                      ┃              ┃    ┃\n");
	printf("┣━━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━╋━━┫\n");
	printf("┃                                                      ┃              ┃    ┃\n");
	printf("┣━━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━╋━━┫\n");
	printf("┃                                                      ┃              ┃    ┃\n");
	printf("┣━━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━╋━━┫\n");
	printf("┃                                                      ┃              ┃    ┃\n");
	printf("┣━━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━╋━━┫\n");
	printf("┃                                                      ┃              ┃    ┃\n");
	printf("┣━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━┻━━┫\n");
	printf("┃                                                                            ┃\n");
	printf("┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");
	printf("┃                                                                            ┃\n");
	printf("┃                                                                            ┃\n");
	printf("┃                                                                            ┃\n");
	printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");

	screen = 2;
	choice1 = 0;
	choice2 = 0;
	ask = 0;
	choiceNum = 0;
	drawSelection("inven");
	shopToolTip();

	for (int i = 0; i < SHOP_SIZE; i++) {
		rarityFontColor(shop[i].rarity);
		gotoxy(28 - (int)(strlen(shop[i].name) / 2), 3 + 2 * i);
		printf("%s", shop[i].name);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		//여기까지 물건 이름 출력
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
		*commify(shop[i].price, price, 0);
		gotoxy(71 - strlen(price), 3 + 2 * i);
		printf("%s", price);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		//여기까지 가격 출력
	}
	printfGold();
}

void mainScreen() {
	gotoxy(0, 0);
	printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
	printf("┃                                                                            ┃\n");
	printf("┃         ■■■    ■■■■    ■■■    ■■    ■    ■  ■■■■         ┃\n");
	printf("┃         ■    ■  ■        ■        ■    ■  ■■■■  ■               ┃\n");
	printf("┃         ■■■    ■■■■  ■        ■    ■  ■    ■  ■■■■         ┃\n");
	printf("┃         ■    ■  ■        ■        ■    ■  ■    ■  ■               ┃\n");
	printf("┃         ■■■    ■■■■    ■■■    ■■    ■    ■  ■■■■         ┃\n");
	printf("┃                                                                            ┃\n");
	printf("┃                                     a                                      ┃\n");
	printf("┃                                                                            ┃\n");
	printf("┃   ■■   ■       ■     ■■ ■  ■   ■■ ■  ■ ■■■ ■■■ ■  ■    ┃\n");
	printf("┃   ■  ■ ■      ■■  ■     ■  ■ ■     ■■■   ■     ■   ■  ■    ┃\n");
	printf("┃   ■■   ■     ■  ■ ■     ■■     ■   ■  ■   ■     ■   ■■■    ┃\n");
	printf("┃   ■  ■ ■     ■■■ ■     ■  ■     ■ ■  ■   ■     ■   ■  ■    ┃\n");
	printf("┃   ■■   ■■■ ■  ■   ■■ ■  ■ ■■   ■  ■ ■■■   ■   ■  ■    ┃\n");
	printf("┃                                                                            ┃\n");
	printf("┃                                                                            ┃\n");
	printf("┃                                 이 어 하 기                                ┃\n");
	printf("┃                                                                            ┃\n");
	printf("┃                                 새 로 하 기                                ┃\n");
	printf("┃                                                                            ┃\n");
	printf("┃                                                                            ┃\n");
	printf("┃                                                                            ┃\n");
	printf("┃                                                                            ┃\n");
	printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");

	gold = 0;
	screen = 3;
	menu = 0;
	selectItem = -1;
	choiceMenu = 0;
	choice1 = 0;
	choice2 = 0;
	choiceNum = 0;
	ask = 0;
	temp = 0;
	keyInputLock = 0;

	mainToolTip();
	drawSelection("main");
}

void deleteToolTip() {
	gotoxy(2, 21);
	printf("                                                                            ");
	gotoxy(2, 22);
	printf("                                                                            ");
	gotoxy(2, 23);
	printf("                                                                            ");
}

void menuToolTip() {
	char toolTip[100];
	deleteToolTip();
	gotoxy(2, 21);
	switch (choiceMenu) {
	case 0:
		strcpy(toolTip, "아이템을 강화합니다.");
		break;
	case 1:
		strcpy(toolTip, "보유중인 아이템을 확인합니다.");
		break;
	case 2:
		strcpy(toolTip, "새로운 아이템을 구입합니다.");
		break;
	case 3:
		strcpy(toolTip, "메인 화면으로 돌아갑니다.");
		break;
	case 4:
		strcpy(toolTip, "게임을 종료합니다.");
		break;
	}
	gotoxy(40 - (int)(strlen(toolTip) / 2), 22);
	printf("%s", toolTip);
}

void mainToolTip() {
	char toolTip[100];
	deleteToolTip();
	gotoxy(2, 21);
	switch (choice1) {
	case 0:
		strcpy(toolTip, "저장된 게임을 불러옵니다.");
		break;
	case 1:
		strcpy(toolTip, "기존 데이터를 삭제하고 새로 시작합니다.");
		break;
	}
	gotoxy(40 - (int)(strlen(toolTip) / 2), 22);
	printf("%s", toolTip);
}

void inventoryToolTip() {
	deleteToolTip();
	gotoxy(2, 21);
	switch (ask) {
	case 0:
		printf("보유중인 아이템을 확인합니다.");
		break;
	case 1:
		for (int i = 0; i < ITEM_LIST_SIZE; i++) {
			if (inven[choice1].code == item[i].code) {
				printf("%s 을(를) 어떻게 하시겠습니까?", item[i].name);
				gotoxy(2, 23);
				printf("                                    강화            판매            취소    ");
				break;
			}
		}
		break;
	case 2:
		for (int i = 0; i < ITEM_LIST_SIZE; i++) {
			if (inven[choice1].code == item[i].code) {
				printf("정말 %s 을(를) 판매하시겠습니까?", item[i].name);
				gotoxy(2, 23);
				printf("                                                      예          아니요    ");
				break;
			}
		}
		break;
	}
}

void shopToolTip() {
	deleteToolTip();
	gotoxy(2, 21);
	if (choice1 < SHOP_SIZE) {
		switch (ask) {
		case 0:
			printf("%s", shop[choice1].toolTip);
			break;
		case 1:
			printf("정말 %s 을(를) 구매하시겠습니까?", shop[choice1].name);
			gotoxy(2, 23);
			printf("                                                      예          아니요    ");
			break;
		case 2:
			PlaySound("data\\sound\\error", NULL, SND_ASYNC);
			printf("가방에 빈 공간이 없습니다.");
			gotoxy(2, 23);
			printf("                                                                    확인    ");
			break;
		case 3:
			PlaySound("data\\sound\\error", NULL, SND_ASYNC);
			printf("소지금이 부족합니다.");
			gotoxy(2, 23);
			printf("                                                                    확인    ");
			break;
		}
	}
}

void reinforceToolTip() {
	deleteToolTip();
	gotoxy(2, 21);
	double priceBonus = 1;
	char reinforcePrice[100];

	if (inven[selectItem].enchant == 5) {
		priceBonus = 0.5;
	}

	switch (ask) {
	case 0:
		break;
	case 1:
		switch (choice1) {
		case 0:
			printf("위의 아이템을 강화하시겠습니까? 아래의 비용을 지불하셔야 합니다.");
			gotoxy(2, 23);
			printf("                                                      예          아니요    ");
			*commify((int)(inven[selectItem].price * 0.1 * priceBonus), reinforcePrice, 0);
			break;
		case 1:
			printf("위의 아이템에 마법부여를 하시겠습니까? 아래의 비용을 지불하셔야 합니다.");
			gotoxy(2, 23);
			printf("                                                      예          아니요    ");
			*commify((int)(inven[selectItem].price * 0.1 * (inven[selectItem].enchantTry + 1)), reinforcePrice, 0);
			break;
		}
		gotoxy(29 - (int)(strlen(reinforcePrice) / 2), 23);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
		printf("%s", reinforcePrice);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		break;
	case 2:
		PlaySound("data\\sound\\error", NULL, SND_ASYNC);
		printf("소지금이 부족합니다.");
		gotoxy(2, 23);
		printf("                                                                    확인    ");
		break;
	}
}

void sell() {
	srand((unsigned)time(NULL));

	int sound = rand() % 2;

	switch (sound) {
	case 0:
		PlaySound("data\\sound\\sell01", NULL, SND_ASYNC);
		break;
	case 1:
		PlaySound("data\\sound\\sell02", NULL, SND_ASYNC);
		break;
	}

	if (inven[selectItem].enchant == 2) {
		gold += (inven[choice1].price * 1.2);
	}
	else {
		gold += inven[choice1].price;
	}


	if (gold >= 1000000000) {
		gold = 999999999;
	}

	inven[choice1].code = -1;
	inven[choice1].reinforce = -1;
	inven[choice1].quality = -1;
	inven[choice1].enchant = -1;
	inven[choice1].enchantTry = -1;
	inven[choice1].price = -1;

	saveData();
	printfGold();
}

void printfGold() {
	char assets[20];

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
	*commify(gold, assets, 0);

	gotoxy(3, 19);
	printf("                                                                         ");
	gotoxy(40 - (int)(strlen(assets) / 2), 19);

	printf("%s", assets);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

void inventorySort(struct inventory *inven) {
	struct inventory temp;

	for (int i = 0; i < INVENTORY_SIZE; i++) {
		for (int j = 0; j < i; j++) {
			if (inven[j].price < inven[i].price) {
				temp = inven[i];
				inven[i] = inven[j];
				inven[j] = temp;
			}
		}
	}

	selectItem = -1;
}

void saveData() {
	int i;
	FILE *saveFile;

	saveFile = fopen("data\\system\\saveFile.txt", "w");

	fprintf(saveFile, "%d\n", gold);

	for (i = 0; i < INVENTORY_SIZE; i++) {
		fprintf(saveFile, "%d/%d/%d/%d/%d/%d\n", inven[i].code, inven[i].reinforce, inven[i].quality, inven[i].enchant, inven[i].enchantTry, inven[i].price);
	}

	for (i = 0; i < 20; i++) {
		fprintf(saveFile, "%d/%lf\n", reinforce[i].success, reinforce[i].rate);
	}

	fclose(saveFile);
};

void buy() {
	switch (choice1) {
	case 0:
		buyNomal(0, 10, 10, 10);
		break;
	case 1:
		buyNomal(10, 10, 20, 10);
		break;
	case 2:
		buyNomal(20, 10, 30, 10);
		break;
	case 3:
		buyNomal(30, 10, 40, 10);
		break;
	case 4:
		buyNomal(40, 10, 50, 8);
		break;
	case 5:
		buyAllRandom();
		break;
	}

	inventorySort(inven);
}

void buyNomal(int itemCodeStart, int itemCodeRange, int luckyItemCodeStart, int luckyItemCodeRange) {
	srand((unsigned)time(NULL));

	PlaySound("data\\sound\\buy01", NULL, SND_ASYNC);

	int lucky = rand() % 100;
	int dice;
	int quality = rand() % 5;
	int i;
	double qualityRate[5] = { 0.9, 0.95, 1, 1.05, 1.1 }; //아이템 품질에 따라 다른 가격 변동 폭

	for (i = 0; inven[i].code != -1; i++) {} //인벤토리 구조체에 빈칸 찾는 중. 인벤토리 빈공간의 유무는 이미 한 번 했기때문에 따로 하지 않음
	gold -= shop[choice1].price;

	if (lucky < 95) { //luckey가 95보다 작으면 그냥 아이템
		dice = rand() % itemCodeRange + itemCodeStart;
		inven[i].code = dice;
	}
	else { //아니면 한 단계 높은 아이템
		dice = rand() % luckyItemCodeRange + luckyItemCodeStart;
		inven[i].code = dice;
	}
	//인벤토리의 구조체에 방금 뽑은 아이템의 정보를 복사해서 넣어줌
	inven[i].quality = quality;
	inven[i].reinforce = 0;
	inven[i].enchant = 0;
	inven[i].enchantTry = 0;
	inven[i].price = (int)(item[inven[i].code].price * qualityRate[quality]);

	itemBuyResultToolTip(i);
	saveData();
	printfGold();
}

void buyAllRandom() {
	srand((unsigned)time(NULL));

	PlaySound("data\\sound\\buy01", NULL, SND_ASYNC);

	int lucky = rand() % 100;
	int dice = rand() % ITEM_LIST_SIZE;
	int quality = rand() % 5;
	int i;
	double qualityRate[5] = { 0.9, 0.95, 1, 1.05, 1.1 };

	for (i = 0; inven[i].code != -1; i++) {}
	gold -= shop[choice1].price;

	inven[i].code = dice;
	inven[i].quality = quality;
	inven[i].reinforce = 0;
	inven[i].enchant = 0;
	inven[i].enchantTry = 0;
	inven[i].price = (int)(item[inven[i].code].price * qualityRate[quality]);

	itemBuyResultToolTip(i);
	saveData();
	printfGold();
}

void itemBuyResultToolTip(int invenCode) {
	deleteToolTip();
	gotoxy(2, 21);
	char *quality;

	qualityFontColor(inven[invenCode].quality);
	quality = numToQuality(inven[invenCode].quality);
	printf("%s ", quality);

	for (int i = 0; i < ITEM_LIST_SIZE; i++) {
		if (inven[invenCode].code == item[i].code) {
			rarityFontColor(item[i].rarity);
			printf("%s", item[i].name);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			break;
		}
	}

	printf(" (이)가 나왔습니다.");

	gotoxy(2, 23);
	printf("                                                                    확인    ");
}

void itemReinforce() {
	srand((unsigned)time(NULL));

	int dice = rand() % 100;

	keyInputLock = 1; //강화도중 키입력 방지

	if (inven[selectItem].enchant == 5) { //마부에 따라 할인해서 골드 차감
		gold = gold - (int)(inven[selectItem].price * 0.1 * 0.5);
	}
	else {
		gold = gold - (int)(inven[selectItem].price * 0.1);
	}

	gotoxy(4, 22);

	PlaySound("data\\sound\\reinforce", NULL, SND_ASYNC);

	for (int i = 0; i < 36; i++) { //진행막대 그리기
		printf("■");
		Sleep(inven[selectItem].reinforce < 10 ? 50 + inven[selectItem].reinforce * 5 : 110); //강화 수치에 따라 다른 sleep시간 적용. 10일때 최대
	}

	PlaySound(NULL, 0, 0);

	if (dice < reinforce[inven[selectItem].reinforce].success + (inven[selectItem].enchant == 1 ? 5 : 0)) { // 랜덤숫자가 강화 성공 기준 숫자보다 작으면
		PlaySound("data\\sound\\success", NULL, SND_ASYNC);
		Sleep(2000);
		if (inven[selectItem].enchant == 3) {
			inven[selectItem].price = (int)(inven[selectItem].price * reinforce[inven[selectItem].reinforce].rate * 1.1);
		}
		else {
			inven[selectItem].price = (int)(inven[selectItem].price * reinforce[inven[selectItem].reinforce].rate);
		}
		inven[selectItem].reinforce++;
		printf("%d", inven[selectItem].reinforce);
		reinforceScreen();

		deleteToolTip();
		gotoxy(4, 22);
		printf("강화에 성공하였습니다!");
	}
	else { //아니면
		PlaySound("data\\sound\\fail", NULL, SND_ASYNC);
		Sleep(2000);

		dice = rand() % 2;
		if (inven[selectItem].enchant == 4 && dice == 0) { //만약 보호 마부가 되어있고 아이템 보호에 성공했으면
			reinforceScreen();

			deleteToolTip();
			gotoxy(4, 22);
			printf("무기에 부여된 마법의 힘에 의해 아이템이 보호되었습니다.");
		}
		else { //아니면
			inven[selectItem].code = -1;
			inven[selectItem].reinforce = -1;
			inven[selectItem].quality = -1;
			inven[selectItem].enchant = -1;
			inven[selectItem].enchantTry = -1;
			inven[selectItem].price = -1;
			selectItem = -1;
			reinforceScreen();

			deleteToolTip();
			gotoxy(4, 22);
			printf("강화에 실패하여 아이템이 파괴되었습니다.");
		}
	}
	saveData();
	keyInputLock = 0;
}

void itemEnchant() {
	srand((unsigned)time(NULL));

	int dice = rand() % 10;
	int enchantList[10] = { 5, 5, 5, 5, 4, 4, 3, 1, 1, 2 }; //특정 마부는 잘 나오지 않도록 하기 위해 배열로 나오는 빈도 조절

	PlaySound("data\\sound\\enchant01", NULL, SND_ASYNC);

	keyInputLock = 1;

	gold = gold - (int)(inven[selectItem].price * 0.1 * (inven[selectItem].enchantTry + 1));

	gotoxy(4, 22);

	inven[selectItem].enchant = enchantList[dice];
	inven[selectItem].enchantTry++;

	reinforceScreen();

	saveData();
	keyInputLock = 0;
}