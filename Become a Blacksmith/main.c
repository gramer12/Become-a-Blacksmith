#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

#define ITEM_LIST_SIZE 58 // itemList�� ũ��
#define INVENTORY_SIZE 8
#define SHOP_SIZE 6

struct itemList { // �����۵��� ������ ��� ����ü
	int code;
	char name[30];
	int price;
	int rarity;
	int type;
}item[ITEM_LIST_SIZE];

struct shop { // ������ ������ ��� ����ü
	char name[30];
	int rarity;
	int price;
	char toolTip[100];
}shop[SHOP_SIZE];

struct inventory { // �κ��丮�� ������ ��� ����ü
	int code;
	int reinforce;
	int quality;
	int enchant;
	int enchantTry;
	int price;
}inven[INVENTORY_SIZE];

struct reinforce { // ��ȭ ������ �� ��ȭ ������ ���� �������� ��� ����ü
	int success;
	double rate;
}reinforce[20];

// ������ ���ų�, ���õ��� �ʾҰų� �ϴ� �� ������ ǥ���ϴ� ���ڴ� -1�� ����
// gotoxy() ��ȣ �ȿ� n - strlen( ���ڿ�/2 ) �� n�� �������� ���� ���̸� �����ؼ� /2 �� �� ��ŭ x��ǥ�� �̵��ؼ� ���ڸ� �������ó�� ���̰� �ϱ� ����
// toolTip �Լ����� ��� ask�� ���� ���� �ٸ� �ؽ�Ʈ�� ����ϵ��� ������

int gold = 0; // ��� ������
int screen = 3; // ���� ȭ���� ���ڷ� ��Ÿ�� ��
				// -1:���ʽ��� �� 0:��ȭ 1:���� 2:���� 3:����
int menu = 0; // �޴�â�� ���ȴ��� ��������
			  // 0:�޴�â ���� 1:�޴�â ����
int selectItem = -1; // ���� ���õ� �������� �������� �����ϴ� ���� ����. -1�� ���� ���õ� ������ ����
int choiceMenu = 0; //���� ���� ���尪(�޴�)
int choice1 = 0; //���� ���� ���尪(�ٿ뵵 1)
int choice2 = 0; //���� ���� ���尪(�ٿ뵵 2)
int choiceNum = 0; // ��� �������� ���� ������ �����ִ���
int ask = 0; // ���� ����� ������ ����� �Ǵ��ϴ� �뵵
int temp = 0; // �ӽ� ������ �����
int keyInputLock = 0; // 1�ϰ�� Ű���尡 �Էµ��� �ʴ� �뵵

void cursor(int n); // Ŀ���� ����� �Լ�
void gotoxy(int x, int y); // Ư�� ��ǥ�� Ŀ���� �̵��ϴ� �Լ�

void setting(int x); // ���� ���࿡ �ʿ��� ȯ���� ����
void keybordInput(); // Ű���� �Է°��� ���� �Ǵ��Ͽ� ��� ����
void keybordInputUp(); // ����Ű ������ ������ �����ϴ� ���
void keybordInputDown(); // ����Ű �Ʒ����� ������ �����ϴ� ���
void keybordInputLeft(); // ����Ű ������ ������ �����ϴ� ���
void keybordInputRight(); // ����Ű �������� ������ �����ϴ� ���
void keybordInputSpace(); // �����̽��ٸ� ������ �� �����ϴ� ���
void drawSelection(char *location); // �� �� �� ȭ�鿡 �׷��ִ� �Լ�. ���ڷ� �̸� ���� �� ��ġ�� �Ű������� �ѱ�� �� ��ġ�� �˾Ƽ� ȭ��ǥ�� �׷��ش�.
void deleteSelection(char *location); // �� �� �� ȭ�鿡�� ����� �Լ�. ȭ��ǥ�� ���� �׸��� ���� �������� ������ ȭ�鿡 �ܻ�ó�� ȭ��ǥ�� ���� �ȴ�.
void rarityFontColor(int x); // �������� ��͵�(����)�� ���� ���ڻ��� �����Ѵ�.
void qualityFontColor(int x); // �������� ǰ��(����)�� ���� ���ڻ��� �����Ѵ�.
char *numToQuality(int x); // �������� ǰ��(����)�� ���� �ٸ� ���ڸ� ��ȯ���ش�.
char *numToType(int x); // �������� ����(����)�� ���� �ٸ� ���ڸ� ��ȯ���ش�.
char *commify(double val, char *buf, int round); // õ ���� ��ǥ ����ִ� �Լ�
void menuScreen(); // �޴�ȭ���� �׸��� �Լ�
void reinforceScreen(); // ��ȭ ȭ���� �׸��� �Լ�
void reinforceScreenTxt(); // ��ȭ ȭ�鿡 ������ ����ϴ� �Լ�.
void inventoryScreen(); // �κ��丮�� �׸��� �Լ�
void mainScreen(); // ���� ����� ����ȭ���� �׸��� �Լ�
void shopScreen(); // ������ �׸��� �Լ�
void deleteToolTip(); // ȭ�� �ϴܺ��� ������ ����� �Լ�. ���� ������ ����ϱ� ���� ������ ������ ���ڰ� ��ĥ �� �ִ�. (�ȳ��ϼ���λ��̸�����⵵��)
void menuToolTip(); // �޴�ȭ���� ���� ���
void mainToolTip(); // ����ȭ���� ���� ���
void inventoryToolTip(); // �κ��丮�� ���� ���
void reinforceToolTip(); // ��ȭȭ���� ���� ���
void shopToolTip(); // ������ ���� ���
void sell(); // ������ �Ǹ�
void printfGold(); // ȭ�鿡 ��� ���
void inventorySort(struct inventory *inven); // �κ��丮 ���� ����
void saveData(); // ������ ����
void buy(); // ������ ����
void buyNomal(int itemCodeStart, int itemCodeRange, int luckyItemCodeStart, int luckyItemCodeRange); // ������ ����(�Ϲ�) ������ �ڵ��� ���۰��� ������ �����ؼ� �Ѱ���� �Ѵ�.
void buyAllRandom(); // ������ ����(�÷���)
void itemBuyResultToolTip(int invenCode); // ������ ���� ��� ���� ���
void itemReinforce(); // ������ ��ȭ�ϴ� �Լ�
void itemEnchant(); // �����ۿ� �����ο��ϴ� �Լ�

void main() {
	system("Become a Blacksmith");
	system("mode con cols=81 lines=25"); // �ܼ� â ũ�� ����
	cursor(0); // Ŀ�� �����
	mainScreen();
	while (1) { // ��� �ݺ��ϸ� Ű���� �Է��� ����
		if (keyInputLock == 0) {
			keybordInput();
		}
	}
}

void cursor(int n) { // Ŀ���� ����� ���� �Լ�
	HANDLE hConsole;
	CONSOLE_CURSOR_INFO ConsoleCursor;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	ConsoleCursor.bVisible = n;
	ConsoleCursor.dwSize = 1;

	SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

void gotoxy(int x, int y) { // Ŀ���� ���ϴ� ��ġ�� �̵��̵���Ű�� ���� �Լ�
	COORD CursorPosition = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), CursorPosition);
}

void setting(int x) { // ���� ���࿡ �ʿ��� ȯ���� ����
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


	if (x == 0) { // ���޹��� ���� 0�̸� �κ��丮�� ������ ���� ���̺����Ͽ��� �ҷ���
		saveFile = fopen("data\\system\\saveFile.txt", "r");

		fscanf(saveFile, "%d\n", &gold);

		for (i = 0; i < INVENTORY_SIZE; i++) {
			fscanf(saveFile, "%d/%d/%d/%d/%d/%d\n", &inven[i].code, &inven[i].reinforce, &inven[i].quality, &inven[i].enchant, &inven[i].enchantTry, &inven[i].price);
		}

		fclose(saveFile);
	}
	else { // �ƴϸ� ��带 5õ������ �����ϰ� �κ��丮�� ������ �ʱ�ȭ
		gold = 5000;

		for (i = 0; i < INVENTORY_SIZE; i++) {
			inven[i].code = -1;
			inven[i].reinforce = -1;
			inven[i].quality = -1;
			inven[i].enchant = -1;
			inven[i].enchantTry = -1;
			inven[i].price = -1;
		}

		saveData(); // �ʱ�ȭ�� ���� ����
	}
}

void keybordInput() {
	int key = getch();

	switch (key) {
	case 13: // ���ʹ����� �޴����
		PlaySound("data\\sound\\key", NULL, SND_ASYNC);
		if (screen != 3) {
			menuScreen();
		}
		break;
	case 32: // �����̽� ������ ��� ����
		PlaySound("data\\sound\\key", NULL, SND_ASYNC);
		keybordInputSpace();
		break;
	case 224: // ����Ű ��� ����
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
	if (menu == 1) { // �޴��� ������ ��
		deleteSelection("menu"); // choiceMenu �� ���� �������� ���� ȭ��ǥ�� ����
		choiceMenu--;
		if (choiceMenu == -1) { // �޴��� �� 5��(0 1 2 3 4) �̹Ƿ� 0���� ������ 4�� ����
			choiceMenu = 4;
		}
		menuToolTip(); // �޴� ���� �����
		drawSelection("menu"); // ȭ��ǥ �׸�
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
			if (selectItem != -1) { // �������� �����ߴ��� Ȯ��
				switch (ask) {
				case 0:
					choiceNum = 1;
					ask = 1;
					reinforceToolTip();
					choice2 = 0;
					drawSelection("select");
					break;
				case 1: // ���� ��ȭ�Ͻðڽ��ϱ�?
					if (inven[selectItem].enchant == 5) { // ��ȭ��� ���� �����ο��� �ִ���
						priceBonus = 0.5;
					}

					if (choice2 == 0) {
						switch (choice1) {
						case 0: // ��ȭ�ϱ��� ��
							if (gold < (int)(inven[selectItem].price * 0.1 * priceBonus)) { // ��ȭ��뺸�� ���� ���� ������
								choiceNum = 0;
								ask = 2;
								reinforceToolTip();
								drawSelection("select");
							}
							else { // �ƴϸ� ��ȭ��
								choice2 = 0;
								ask = 0;
								choiceNum = 0;
								deleteToolTip();
								itemReinforce();
							}
							break;
						case 1: // �����ϱ��� ��
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
		case 1: // ������ �� ����
			if (inven[choice1].code != -1) {
				switch (ask) {
				case 0: // ������ ���� ���� ����
					choiceNum = 2;
					ask = 1;
					inventoryToolTip();
					choice2 = 0;
					drawSelection("select");
					break;
				case 1: // �������� ��ȭ���� �Ȱ��� �׸����� ���� ����
					switch (choice2) {
					case 0: // 0�� ���� - ��ȭ�⿡ �ø���
						selectItem = choice1;
						choice1 = 0;
						choiceMenu = 0;
						reinforceScreen();
						break;
					case 1: // 1�� ���� - �������� ���� �Ȱ��� ���� �������� �Ѿ��
						choiceNum = 1;
						choice2 = 0;
						ask = 2;
						inventoryToolTip();
						drawSelection("select");
						break;
					case 2: // 2�� ���� - ������ ����ϰ� �ٽ� �������� ����
						choiceNum = 0;
						ask = 0;
						inventoryToolTip();
						break;
					}
					break;
				case 2: // �������� ���� �Ȱ��� ���� ����
					switch (choice2) {
					case 0: // ��
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
					case 1: // �ƴϿ�
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
			case 0: // �������� �����̽��ٸ� ������ ���� ����
				if (choice1 < SHOP_SIZE) {
					choiceNum = 1;
					ask = 1;
					shopToolTip();
					choice2 = 0;
					drawSelection("select");
				}
				break;
			case 1:// ���� ����� ���
				switch (choice2) {
				case 0: // ��
					if (inven[INVENTORY_SIZE - 1].code != -1) { // ���濡 �� ���� Ȯ��
						choiceNum = 0;
						ask = 2;
						shopToolTip();
						drawSelection("select");
					}
					else if (gold < shop[choice1].price) { // ���� �������� �ʴ��� Ȯ��
						choiceNum = 0;
						ask = 3;
						shopToolTip();
						drawSelection("select");
					}
					else { // ���� ������ ����ó��
						choiceNum = 0;
						ask = 4;
						choice2 = 0;
						buy();
						drawSelection("select");
					}
					break;
				case 1: // �ƴϿ�
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
		case 3: // ����ȭ�鿡�� �����̽��ٸ� ������
			switch (choice1) {
			case 0: // �̾��ϱ� ���� ��
				setting(0);
				break;
			case 1: // �����ϱ� ���� ��
				setting(1);
				break;
			}
			screen = -1;
			menuScreen();
			break;
		}
	}
}

void drawSelection(char *location) { // ȭ��ǥ�� �׸� ��ġ�� �Է¹޾� ȭ�鿡 �׷��ִ� �Լ�
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
		printf("��");
	}

	if (x2 != -1) {
		gotoxy(x2, y);
		printf("��");
	}
}

void deleteSelection(char *location) { // ȭ��ǥ�� ���� ��ġ�� �Է¹޾� ȭ�鿡�� �����ִ� �Լ�
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
		strcpy(str, "���ϱ�");
		break;
	case 1:
		strcpy(str, "�ϱ�");
		break;
	case 2:
		strcpy(str, "�߱�");
		break;
	case 3:
		strcpy(str, "���");
		break;
	case 4:
		strcpy(str, "�ֻ��");
		break;
	}

	return str;
}

char *numToType(int x)
{
	char *str = malloc(sizeof(char) * 6);

	switch (x) {
	case 0:
		strcpy(str, "����");
		break;
	case 1:
		strcpy(str, "�ܰ�");
		break;
	case 2:
		strcpy(str, "�б�");
		break;
	case 3:
		strcpy(str, "����");
		break;
	case 4:
		strcpy(str, "������");
		break;
	case 5:
		strcpy(str, "Ȱ");
		break;
	case 6:
		strcpy(str, "��");
		break;
	case 7:
		strcpy(str, "������");
		break;
	case 8:
		strcpy(str, "��������");
		break;
	}

	return str;
}

char *numToEnchant(int x)
{
	char *str = malloc(sizeof(char) * 6);

	switch (x) {
	case 0:
		strcpy(str, "����");
		break;
	case 1:
		strcpy(str, "�ູ");
		break;
	case 2:
		strcpy(str, "��ǰ");
		break;
	case 3:
		strcpy(str, "����");
		break;
	case 4:
		strcpy(str, "��ȣ");
		break;
	case 5:
		strcpy(str, "��ȭ");
		break;
	}

	return str;
}

char *commify(double val, char *buf, int round) { // õ ���� ���� ��ǥ ����ִ� �Լ�
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
		printf("��������������������������������������������������������������������������������\n");
		printf("��                                                                            ��\n");
		printf("��                                                                            ��\n");
		printf("��                                                                            ��\n");
		printf("��                                                                            ��\n");
		printf("��                           ����������������������                           ��\n");
		printf("��                           ��       Menu       ��                           ��\n");
		printf("��                           ����������������������                           ��\n");
		printf("��                           ��                  ��                           ��\n");
		printf("��                           ��      ��  ȭ      ��                           ��\n");
		printf("��                           ��                  ��    Select : Arrow Kyes    ��\n");
		printf("��                           ��      ��  ��      ��                           ��\n");
		printf("��                           ��                  ��                           ��\n");
		printf("��                           ��      ��  ��      ��        OK : Space         ��\n");
		printf("��                           ��                  ��                           ��\n");
		printf("��                           ��      ��  ��      ��                           ��\n");
		printf("��                           ��                  ��      Menu : Enter         ��\n");
		printf("��                           ��      ��  ��      ��                           ��\n");
		printf("��                           ��                  ��                           ��\n");
		printf("��                           ����������������������                           ��\n");
		printf("��                                                                            ��\n");
		printf("��                                                                            ��\n");
		printf("��                                                                            ��\n");
		printf("��                                                                            ��\n");
		printf("��������������������������������������������������������������������������������");

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
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                                        ��      ��  ��      ��\n");
	printf("��                                                        ����������������������\n");
	printf("��                                                        ��                  ��\n");
	printf("��                                                        ��      ��  ȭ      ��\n");
	printf("��                                                        ��                  ��\n");
	printf("��                                                        ��      ��  ��      ��\n");
	printf("��                                                        ��                  ��\n");
	printf("��                                                        ��                  ��\n");
	printf("��                                                        ��                  ��\n");
	printf("��                                                        ��                  ��\n");
	printf("��                                                        ��                  ��\n");
	printf("��                                                        ��                  ��\n");
	printf("��                                                        ��                  ��\n");
	printf("��                                                        ��                  ��\n");
	printf("��                                                        ��                  ��\n");
	printf("��                                                        ��                  ��\n");
	printf("��                                                        ��                  ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                                                            ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                                                            ��\n");
	printf("��                                                                            ��\n");
	printf("��                                                                            ��\n");
	printf("��������������������������������������������������������������������������������");

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
		printf("���濡�� ��ȭ�� �������� �����ϼ���");
	}
	else {
		for (int i = 0; i < ITEM_LIST_SIZE; i++) {
			if (inven[selectItem].code == item[i].code) {
				rarityFontColor(item[i].rarity);
				gotoxy(29 - (int)(strlen(item[i].name) / 2), 2);
				printf("%s", item[i].name);
				// ������� ������ �̸� ���
				for (int i = 0; i < inven[selectItem].reinforce; i++) {
					strcat(star, "��");
					starCount++;
					if (starCount == 5) { //�� ������ ī��Ʈ�� 5���� ���� �ѹ� ����ϰ� ī��Ʈ 0����
						strcat(star, " ");
						starCount = 0;
					}
				}
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
				gotoxy(29 - (int)(strlen(star) / 2), 4);
				printf("%s", star);
				//������� �� ���
				qualityFontColor(inven[selectItem].quality);
				quality = numToQuality(inven[selectItem].quality);
				type = numToType(item[i].type);
				gotoxy(29 - (int)((strlen(quality) + strlen(type) + 1) / 2), 6);
				printf("%s %s", quality, type);
				//������� ������ ���� �� ǰ�� ���
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

				printf("%d", inven[selectItem].reinforce); printf("��  >  "); printf("%d", inven[selectItem].reinforce + 1); printf("��");
				//������� ����� ���� ���
				if (inven[selectItem].enchant == 1) {
					enchantBonus = 5;
				}

				gotoxy(22, 10);
				printf("����Ȯ�� : %d%%", reinforce[inven[selectItem].reinforce].success + enchantBonus);
				//������� ��ȭȮ�� ���
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
				//������� ��ȭ�� ���� > ��ȭ �� ���� ���


				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
				if (inven[selectItem].enchant == 0) {
					gotoxy(23, 14);
					printf("�����ο� ����");
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
						strcpy(enchantTxt, "��ȭ������ 5% ����");
						break;
					case 2:
						strcpy(enchantTxt, "�Ǹ� �� 20%�� �߰� ��� ȹ��");
						break;
					case 3:
						strcpy(enchantTxt, "��ȭ ���� �� ���� 10% �߰� ����");
						break;
					case 4:
						strcpy(enchantTxt, "��ȭ ���� �� 50%�� Ȯ���� �ı����� ����");
						break;
					case 5:
						strcpy(enchantTxt, "��ȭ ��� 50% ����");
						break;
					}

					gotoxy(29 - (int)(strlen(enchantTxt) / 2), 16);
					printf("%s", enchantTxt);
					//������� �����ο� �̸� �� ���� ���
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
	printf("��������������������������������������������������������������������������������\n");
	printf("��             ��  ��             ��    �� ǰ�� �� ���� ��    ��  ��    �� �� ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                ��    ��      ��      ��              ��    ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                ��    ��      ��      ��              ��    ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                ��    ��      ��      ��              ��    ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                ��    ��      ��      ��              ��    ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                ��    ��      ��      ��              ��    ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                ��    ��      ��      ��              ��    ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                ��    ��      ��      ��              ��    ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                ��    ��      ��      ��              ��    ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                                                            ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                                                            ��\n");
	printf("��                                                                            ��\n");
	printf("��                                                                            ��\n");
	printf("��������������������������������������������������������������������������������");

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
	printf("��");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

	for (int i = 0; i < INVENTORY_SIZE; i++) {
		if (inven[i].code != -1) {
			for (int j = 0; item[j].code < ITEM_LIST_SIZE; j++) {
				if (inven[i].code == item[j].code) {
					rarityFontColor(item[j].rarity);
					gotoxy(18 - (int)(strlen(item[j].name) / 2), 3 + 2 * i);
					printf("%s", item[j].name);
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					//������ �̸� ���
					gotoxy(38 - ((int)inven[i].reinforce < 10 ? 0 : 1), 3 + 2 * i); //ĭ ������ ���� ���ڰ� 10���� ũ�� ��ǥ ��ĭ ����
					printf("%d", inven[i].reinforce);
					//������ ��ȭ��ġ ���
					qualityFontColor(inven[i].quality);
					quality = numToQuality(inven[i].quality);
					gotoxy(45 - (int)(strlen(quality) / 2), 3 + 2 * i);
					printf("%s", quality);
					//������� ǰ�� ���� ���
					if (inven[i].enchant == 0) {
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
					}
					else {
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
					}

					enchant = numToEnchant(inven[i].enchant);
					gotoxy(53 - (int)(strlen(enchant) / 2), 3 + 2 * i);
					printf("%s", enchant);
					//������� ���� ���
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
					*commify(inven[i].price, price, 0);
					gotoxy(71 - strlen(price), 3 + 2 * i);
					printf("%s", price);
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					//������� ���� ���
				}
			}
		}
	}
	printfGold();
}

void shopScreen() {
	char price[20];
	gotoxy(0, 0);
	printf("��������������������������������������������������������������������������������\n");
	printf("��                        ��  ��                        ��    ��  ��    �� �� ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                                      ��              ��    ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                                      ��              ��    ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                                      ��              ��    ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                                      ��              ��    ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                                      ��              ��    ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                                      ��              ��    ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                                      ��              ��    ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                                      ��              ��    ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                                                            ��\n");
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                                                            ��\n");
	printf("��                                                                            ��\n");
	printf("��                                                                            ��\n");
	printf("��������������������������������������������������������������������������������");

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
		//������� ���� �̸� ���
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
		*commify(shop[i].price, price, 0);
		gotoxy(71 - strlen(price), 3 + 2 * i);
		printf("%s", price);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		//������� ���� ���
	}
	printfGold();
}

void mainScreen() {
	gotoxy(0, 0);
	printf("��������������������������������������������������������������������������������\n");
	printf("��                                                                            ��\n");
	printf("��         ����    �����    ����    ���    ��    ��  �����         ��\n");
	printf("��         ��    ��  ��        ��        ��    ��  �����  ��               ��\n");
	printf("��         ����    �����  ��        ��    ��  ��    ��  �����         ��\n");
	printf("��         ��    ��  ��        ��        ��    ��  ��    ��  ��               ��\n");
	printf("��         ����    �����    ����    ���    ��    ��  �����         ��\n");
	printf("��                                                                            ��\n");
	printf("��                                     a                                      ��\n");
	printf("��                                                                            ��\n");
	printf("��   ���   ��       ��     ��� ��  ��   ��� ��  �� ���� ���� ��  ��    ��\n");
	printf("��   ��  �� ��      ���  ��     ��  �� ��     ����   ��     ��   ��  ��    ��\n");
	printf("��   ���   ��     ��  �� ��     ���     ��   ��  ��   ��     ��   ����    ��\n");
	printf("��   ��  �� ��     ���� ��     ��  ��     �� ��  ��   ��     ��   ��  ��    ��\n");
	printf("��   ���   ���� ��  ��   ��� ��  �� ���   ��  �� ����   ��   ��  ��    ��\n");
	printf("��                                                                            ��\n");
	printf("��                                                                            ��\n");
	printf("��                                 �� �� �� ��                                ��\n");
	printf("��                                                                            ��\n");
	printf("��                                 �� �� �� ��                                ��\n");
	printf("��                                                                            ��\n");
	printf("��                                                                            ��\n");
	printf("��                                                                            ��\n");
	printf("��                                                                            ��\n");
	printf("��������������������������������������������������������������������������������");

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
		strcpy(toolTip, "�������� ��ȭ�մϴ�.");
		break;
	case 1:
		strcpy(toolTip, "�������� �������� Ȯ���մϴ�.");
		break;
	case 2:
		strcpy(toolTip, "���ο� �������� �����մϴ�.");
		break;
	case 3:
		strcpy(toolTip, "���� ȭ������ ���ư��ϴ�.");
		break;
	case 4:
		strcpy(toolTip, "������ �����մϴ�.");
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
		strcpy(toolTip, "����� ������ �ҷ��ɴϴ�.");
		break;
	case 1:
		strcpy(toolTip, "���� �����͸� �����ϰ� ���� �����մϴ�.");
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
		printf("�������� �������� Ȯ���մϴ�.");
		break;
	case 1:
		for (int i = 0; i < ITEM_LIST_SIZE; i++) {
			if (inven[choice1].code == item[i].code) {
				printf("%s ��(��) ��� �Ͻðڽ��ϱ�?", item[i].name);
				gotoxy(2, 23);
				printf("                                    ��ȭ            �Ǹ�            ���    ");
				break;
			}
		}
		break;
	case 2:
		for (int i = 0; i < ITEM_LIST_SIZE; i++) {
			if (inven[choice1].code == item[i].code) {
				printf("���� %s ��(��) �Ǹ��Ͻðڽ��ϱ�?", item[i].name);
				gotoxy(2, 23);
				printf("                                                      ��          �ƴϿ�    ");
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
			printf("���� %s ��(��) �����Ͻðڽ��ϱ�?", shop[choice1].name);
			gotoxy(2, 23);
			printf("                                                      ��          �ƴϿ�    ");
			break;
		case 2:
			PlaySound("data\\sound\\error", NULL, SND_ASYNC);
			printf("���濡 �� ������ �����ϴ�.");
			gotoxy(2, 23);
			printf("                                                                    Ȯ��    ");
			break;
		case 3:
			PlaySound("data\\sound\\error", NULL, SND_ASYNC);
			printf("�������� �����մϴ�.");
			gotoxy(2, 23);
			printf("                                                                    Ȯ��    ");
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
			printf("���� �������� ��ȭ�Ͻðڽ��ϱ�? �Ʒ��� ����� �����ϼž� �մϴ�.");
			gotoxy(2, 23);
			printf("                                                      ��          �ƴϿ�    ");
			*commify((int)(inven[selectItem].price * 0.1 * priceBonus), reinforcePrice, 0);
			break;
		case 1:
			printf("���� �����ۿ� �����ο��� �Ͻðڽ��ϱ�? �Ʒ��� ����� �����ϼž� �մϴ�.");
			gotoxy(2, 23);
			printf("                                                      ��          �ƴϿ�    ");
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
		printf("�������� �����մϴ�.");
		gotoxy(2, 23);
		printf("                                                                    Ȯ��    ");
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
	double qualityRate[5] = { 0.9, 0.95, 1, 1.05, 1.1 }; //������ ǰ���� ���� �ٸ� ���� ���� ��

	for (i = 0; inven[i].code != -1; i++) {} //�κ��丮 ����ü�� ��ĭ ã�� ��. �κ��丮 ������� ������ �̹� �� �� �߱⶧���� ���� ���� ����
	gold -= shop[choice1].price;

	if (lucky < 95) { //luckey�� 95���� ������ �׳� ������
		dice = rand() % itemCodeRange + itemCodeStart;
		inven[i].code = dice;
	}
	else { //�ƴϸ� �� �ܰ� ���� ������
		dice = rand() % luckyItemCodeRange + luckyItemCodeStart;
		inven[i].code = dice;
	}
	//�κ��丮�� ����ü�� ��� ���� �������� ������ �����ؼ� �־���
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

	printf(" (��)�� ���Խ��ϴ�.");

	gotoxy(2, 23);
	printf("                                                                    Ȯ��    ");
}

void itemReinforce() {
	srand((unsigned)time(NULL));

	int dice = rand() % 100;

	keyInputLock = 1; //��ȭ���� Ű�Է� ����

	if (inven[selectItem].enchant == 5) { //���ο� ���� �����ؼ� ��� ����
		gold = gold - (int)(inven[selectItem].price * 0.1 * 0.5);
	}
	else {
		gold = gold - (int)(inven[selectItem].price * 0.1);
	}

	gotoxy(4, 22);

	PlaySound("data\\sound\\reinforce", NULL, SND_ASYNC);

	for (int i = 0; i < 36; i++) { //���ื�� �׸���
		printf("��");
		Sleep(inven[selectItem].reinforce < 10 ? 50 + inven[selectItem].reinforce * 5 : 110); //��ȭ ��ġ�� ���� �ٸ� sleep�ð� ����. 10�϶� �ִ�
	}

	PlaySound(NULL, 0, 0);

	if (dice < reinforce[inven[selectItem].reinforce].success + (inven[selectItem].enchant == 1 ? 5 : 0)) { // �������ڰ� ��ȭ ���� ���� ���ں��� ������
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
		printf("��ȭ�� �����Ͽ����ϴ�!");
	}
	else { //�ƴϸ�
		PlaySound("data\\sound\\fail", NULL, SND_ASYNC);
		Sleep(2000);

		dice = rand() % 2;
		if (inven[selectItem].enchant == 4 && dice == 0) { //���� ��ȣ ���ΰ� �Ǿ��ְ� ������ ��ȣ�� ����������
			reinforceScreen();

			deleteToolTip();
			gotoxy(4, 22);
			printf("���⿡ �ο��� ������ ���� ���� �������� ��ȣ�Ǿ����ϴ�.");
		}
		else { //�ƴϸ�
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
			printf("��ȭ�� �����Ͽ� �������� �ı��Ǿ����ϴ�.");
		}
	}
	saveData();
	keyInputLock = 0;
}

void itemEnchant() {
	srand((unsigned)time(NULL));

	int dice = rand() % 10;
	int enchantList[10] = { 5, 5, 5, 5, 4, 4, 3, 1, 1, 2 }; //Ư�� ���δ� �� ������ �ʵ��� �ϱ� ���� �迭�� ������ �� ����

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