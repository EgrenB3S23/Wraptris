#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h> /* getch() and kbhit() */

#define YMAX 20
#define XMAX 10

// tetrominos
#define tI                                       \
	{                                            \
		{"    "}, {"OOOO"}, {"    "}, { "    " } \
	}
#define tO                                       \
	{                                            \
		{"    "}, {" OO "}, {" OO "}, { "    " } \
	}
#define tT                                       \
	{                                            \
		{"    "}, {"OOO "}, {" O  "}, { "    " } \
	}
#define tJ                                       \
	{                                            \
		{"    "}, {"OOO "}, {"  O "}, { "    " } \
	}
#define tL                                       \
	{                                            \
		{"    "}, {" OOO"}, {" O  "}, { "    " } \
	}
#define tS                                       \
	{                                            \
		{"    "}, {" OO "}, {"OO  "}, { "    " } \
	}
#define tZ                                       \
	{                                            \
		{"    "}, {"OO  "}, {" OO "}, { "    " } \
	}

// standard settings (not #defined because the user can change them. #TODO: Options have not been implemented.

/*
char keyUp = 'w';
char keyDown = 's';
char keyLeft = 'a';
char keyRight = 'd';
char keyConfirm = 13;   //13 = enter.

char keySpecialEvent = 'p';

char cFilled = 'O';
char cEmpty = ' ';
char cGhost = '-';
char cBorder = '#';
*/
#define keyUp 'w'
#define keyDown 's'
#define keyLeft 'a'
#define keyRight 'd'
#define keyConfirm 13 // 13 = enter.

#define keySpecialEvent 'p'

#define cFilled '#'
#define cEmpty ' '
#define cGhost '.'
#define cBorder '*'

int standardTickDelay = 1000;

int tickDelay = 1000;

char playArea[YMAX][XMAX];
char currPiece[4][4];
int currPieceX = 0;
int currPieceY = 0;
int ghostY = 0;
int currPieceXSize = 0;
int currPieceYSize = 0;
int score = 0;
int backToMenu = 0;
int highScore[10];
char highScoreNames[10][20];


void shuffleRow(int y)
{
	// For each cell in row Y except the last cell, swap with a random later position.
	int x = 0;
	char temp = '!';
	int roll = 0;

	for (x = 0; x <= XMAX - 1; x++)
	{
		roll = rand() % XMAX - x;
		temp = playArea[y][x];
		playArea[y][x] = playArea[y][x + roll];
		playArea[y][x + roll] = temp;
	}
}

void earthquake()
{
	int y = 0;
	for (y = 0; y <= YMAX - 1; y++)
	{
		shuffleRow(y);
	}
}

int menu()
{
	/*
	prints the menu and lets the player make a selection. (for example, play and exit)

	returns the value of the selection. top option = 0.

	current values:
	0: nothing selected. (should never occur)
	1: play
	2: options (#TODO)
	3: exit
	*/
	char screen[25][80];
	int x = 0;
	int y = 0;
	int selection = 1;
	int picked = 0; // 0 until an option has been picked. 1 when picked.
	int somethingHappened = 1;
	char input = ' '; // last key pressed by player.

	backToMenu = 0;

	do
	{
		// handle input
		if (kbhit())
		{
			input = getch();

			switch (input)
			{
			case keyUp:
				if (selection > 1)
				{
					selection--;
				}
				somethingHappened = 1;
				break;
			case keyDown:
				if (selection < 3)
				{
					selection++;
				}
				somethingHappened = 1;
				break;
				//          case 'k' :
			case keyConfirm:
				picked = 1;
				// somethingHappened = 1;    //we don't need to rerender if we're done here!
				break;
			}
		}

		if (somethingHappened) //(if something changed, rerender the screen)
		{
			somethingHappened = 0; // we checked it already, so unless anything changes again, we don't want to rerender everything.

			/*
				screen rendering follows! (
			*/

			// clear screen array
			for (y = 0; y <= 24; y++)
			{
				for (x = 0; x <= 79; x++)
				{
					screen[y][x] = ' ';
				}
			}

			// menu borders
			for (x = 23; x <= 56; x++)
			{
				screen[4][x] = cBorder;
				screen[19][x] = cBorder;
			}
			for (y = 5; y <= 18; y++)
			{
				screen[y][23] = cBorder;
				screen[y][56] = cBorder;
			}

			// tedious text...

			screen[6][36] = 'W';
			screen[6][37] = 'r';
			screen[6][38] = 'a';
			screen[6][39] = 'p';
			screen[6][40] = 't';
			screen[6][41] = 'r';
			screen[6][42] = 'i';
			screen[6][43] = 's';
			screen[6][44] = '!';

			screen[9][30] = 'P';
			screen[9][31] = 'l';
			screen[9][32] = 'a';
			screen[9][33] = 'y';

			screen[11][30] = 'O';
			screen[11][31] = 'p';
			screen[11][32] = 't';
			screen[11][33] = 'i';
			screen[11][34] = 'o';
			screen[11][35] = 'n';
			screen[11][36] = 's';

			screen[13][30] = 'E';
			screen[13][31] = 'x';
			screen[13][32] = 'i';
			screen[13][33] = 't';

			// selection pointer
			screen[7 + 2 * selection][28] = '>';

			// clear and then print the screen
			system("cls");
			for (y = 0; y <= 24; y++)
			{
				for (x = 0; x <= 78; x++) // never prints x = 79. sorry :PpPpp
				{
					printf("%c", screen[y][x]);
				}
				if (y != 24)
				{
					printf("\n");
				}
			}
		}
	} while (picked == 0);

	return selection;
}

int toHighScore(int scoreToAdd, char *name)
{
	/*
	receives score and  name, and puts them into the high score list if applicable.

	returns position (1-10), or 0 if the new score didn't make it into the list.
	*/

	int pos = 0;
	int y = 0;
	int x = 0;
	int isNewHigh = 0;
	for (pos = 0; pos <= 9; pos++)
	{
		if (scoreToAdd > highScore[pos])
		{
			// move all lower scores 1 step down
			for (y = 9; y >= pos + 1; y--)
			{
				highScore[y] = highScore[y - 1];
				// manual strcpy:
				for (x = 0; x <= 19; x++)
				{
					highScoreNames[y][x] = highScoreNames[y - 1][x];
				}
			}

			highScore[pos] = scoreToAdd;
			// manual strcpy:
			for (x = 0; x <= 19; x++)
			{
				highScoreNames[y][x] = name[x];
			}
			break;
		}
	}

	if (isNewHigh)
	{
		return pos + 1;
	}
	else
	{
		return 0;
	}
}

int lineIsFull(int y)
{
	// checks if a line is full, returns 1 if it is. 0 if it isn't.
	int x = 0;
	int soFarSoGood = 1;

	for (x = 0; x <= 9; x++)
	{
		if (playArea[y][x] != cFilled)
		{
			soFarSoGood = 0;
		}
	}
	return soFarSoGood;
}

void rotatePiece()
{
	int x = 0;
	int y = 0;
	int newPieceX = currPieceX;
	int newPieceY = currPieceY;
	int moveIsOk = 1;

	/*
		int oldPieceXSize = currPieceXSize;
		char newPiece[currPieceXSize][currPieceYSize];

		currPieceXSize = currPieceYSize;
		currPieceySize = oldPieceYSize;
	*/
	char oldPiece[4][4];
	char newPiece[4][4];

	for (y = 0; y <= 3; y++)
	{
		for (x = 0; x <= 3; x++)
		{
			oldPiece[y][x] = currPiece[y][x];
		}
	}

	// inner
	newPiece[1][1] = oldPiece[2][1];
	newPiece[1][2] = oldPiece[1][1];
	newPiece[2][2] = oldPiece[1][2];
	newPiece[2][1] = oldPiece[2][2];

	// corners
	newPiece[0][0] = oldPiece[3][0];
	newPiece[0][3] = oldPiece[0][0];
	newPiece[3][3] = oldPiece[0][3];
	newPiece[3][0] = oldPiece[3][3];

	// the rest
	newPiece[0][1] = oldPiece[2][0];
	newPiece[0][2] = oldPiece[1][0];

	newPiece[1][3] = oldPiece[0][1];
	newPiece[2][3] = oldPiece[0][2];

	newPiece[3][2] = oldPiece[1][3];
	newPiece[3][1] = oldPiece[2][3];

	newPiece[2][0] = oldPiece[3][2];
	newPiece[1][0] = oldPiece[3][1];

	// after out of bounds check, check for collision with debris
	for (y = 0; y <= 3; y++)
	{
		for (x = 0; x <= 3; x++)
		{
			if (newPiece[y][x] == cFilled && playArea[y + newPieceY][x + newPieceX] == cFilled)
			{
				moveIsOk = 0;
			}
		}
	}

	if (moveIsOk)
	{
		for (y = 0; y <= 3; y++)
		{
			for (x = 0; x <= 3; x++)
			{
				currPiece[y][x] = newPiece[y][x];
				currPieceX = newPieceX;
				currPieceY = newPieceY;
			}
		}
	}
}

void newPiece()
{
	// define tI {{"    "},{"OOOO"},{"    "},{"    "}}
	// define tO {{"    "},{" OO "},{" OO "},{"    "}}
	// define tT {{"    "},{"OOO "},{" O  "},{"    "}}
	// define tJ {{"    "},{"OOO "},{"  O "},{"    "}}
	// define tL {{"    "},{" OOO"},{" O  "},{"    "}}
	// define tS {{"    "},{" OO "},{"OO  "},{"    "}}
	// define tZ {{"    "},{"OO  "},{" OO "},{"    "}}

	// standard blocks only: variants = 7
	int variants = 7;
	int r = rand() % variants;

	int x = 0;
	int y = 0;
	int i = 0;
	// currPieceX = 4; (removed due to remake)
	currPieceY = -1;
	char pieceToGet = ' ';

	for (y = 0; y <= 3; y++)
	{
		for (x = 0; x <= 3; x++)
		{
			currPiece[y][x] = ' ';
		}
	}

	switch (r)
	{
	case 0:
		pieceToGet = 'I';
		currPieceXSize = 4;
		currPieceYSize = 1;
		currPiece[1][0] = cFilled;
		currPiece[1][1] = cFilled;
		currPiece[1][2] = cFilled;
		currPiece[1][3] = cFilled;
		break;
	case 1:
		pieceToGet = 'O';
		currPieceXSize = 2;
		currPieceYSize = 2;
		currPiece[1][1] = cFilled;
		currPiece[1][2] = cFilled;
		currPiece[2][1] = cFilled;
		currPiece[2][2] = cFilled;
		break;
	case 2:
		pieceToGet = 'T';
		currPieceXSize = 3;
		currPieceYSize = 2;
		currPiece[1][0] = cFilled;
		currPiece[1][1] = cFilled;
		currPiece[1][2] = cFilled;
		currPiece[2][1] = cFilled;
		break;
	case 3:
		pieceToGet = 'J';
		currPieceXSize = 3;
		currPieceYSize = 2;
		currPiece[1][0] = cFilled;
		currPiece[1][1] = cFilled;
		currPiece[1][2] = cFilled;
		currPiece[2][2] = cFilled;
		break;
	case 4:
		pieceToGet = 'L';
		currPieceXSize = 3;
		currPieceYSize = 2;
		currPiece[1][0] = cFilled;
		currPiece[1][1] = cFilled;
		currPiece[1][2] = cFilled;
		currPiece[2][0] = cFilled;
		break;
	case 5:
		pieceToGet = 'S';
		currPieceXSize = 3;
		currPieceYSize = 2;
		currPiece[1][1] = cFilled;
		currPiece[1][2] = cFilled;
		currPiece[2][0] = cFilled;
		currPiece[2][1] = cFilled;
		break;
	case 6:
		pieceToGet = 'Z';
		currPieceXSize = 3;
		currPieceYSize = 2;
		currPiece[1][0] = cFilled;
		currPiece[1][1] = cFilled;
		currPiece[2][1] = cFilled;
		currPiece[2][2] = cFilled;
		break;
	// case 7 and above = nonstandard pieces.
	case 7:
		// O O
		//  O
		//  O
		pieceToGet = '.';
		currPieceXSize = 3;
		currPieceYSize = 2;
		currPiece[0][0] = cFilled;
		currPiece[0][2] = cFilled;
		currPiece[1][1] = cFilled;
		currPiece[2][1] = cFilled;
		break;
	case 8:
		// O
		// O
		//  OO
		pieceToGet = 'V';
		currPieceXSize = 3;
		currPieceYSize = 2;
		currPiece[0][0] = cFilled;
		currPiece[1][0] = cFilled;
		currPiece[2][1] = cFilled;
		currPiece[2][2] = cFilled;
		break;
	case 9:
		// O
		// OO
		pieceToGet = 'v';
		currPieceXSize = 3;
		currPieceYSize = 2;
		currPiece[1][1] = cFilled;
		currPiece[2][1] = cFilled;
		currPiece[2][2] = cFilled;
		break;
	case 10:
		// O
		// O
		//  O
		pieceToGet = 'l';
		currPieceXSize = 3;
		currPieceYSize = 2;
		currPiece[0][0] = cFilled;
		currPiece[1][0] = cFilled;
		currPiece[2][1] = cFilled;
		break;
	// O
	// O
	// O
	case 11:
		pieceToGet = 'j';
		currPieceXSize = 3;
		currPieceYSize = 2;
		currPiece[0][1] = cFilled;
		currPiece[1][0] = cFilled;
		currPiece[2][0] = cFilled;
		break;
	}
	// printf("pieceToGet: %c\nr: %d", pieceToGet, r);

	// for each line filled, clear it and let debris fall down
	for (i = 0; i <= 19; i++)
	{
		if (lineIsFull(i))
		{
			int temp = score;
			score++;
			if (temp % 10 > score % 10) // every 10 points...
			{
				tickDelay = tickDelay * (float)0.9; //...increase the tick speed.
			}
			for (y = i; y >= 0; y--)
			{
				for (x = 0; x <= 9; x++)
				{
					playArea[y][x] = playArea[y - 1][x];
				}
			}
			// y=0 always becomes cleared
			for (x = 0; x <= 9; x++)
			{
				playArea[0][x] = cEmpty;
			}
		}
	}

	// Has the player lost?
	for (x = 0; x <= 9; x++)
	{
		if (playArea[0][x] == cFilled)
		{
			backToMenu = 1;
			// init();
		}
	}
}

void movePiece(char dir)
{
	int y = 0;
	int x = 0;
	int moveIsOk = 1;

	// make sure it doesn't go out of bounds
	for (y = 0; y <= 3; y++)
	{
		for (x = 0; x <= 3; x++)
		{

			if (currPiece[y][x] == cFilled)
			{
				// check collision with debris
				if (dir == 'l' && playArea[y + currPieceY][x + currPieceX - 1] == cFilled)
				{
					moveIsOk = 0;
				}
				// check collision with debris
				if (dir == 'r' && playArea[y + currPieceY][x + currPieceX + 1] == cFilled)
				{
					moveIsOk = 0;
				}
			}
		}
	}

	if (moveIsOk)
	{
		switch (dir)
		{
		case 'l':
			currPieceX = (currPieceX + 9) % 10;
			break;
		case 'r':
			currPieceX = (currPieceX + 1) % 10;
			break;
		}
	}
}

int lowerPiece()
{
	/*
	checks if lowering the piece is a legal move, and does so if it is.
	if it isn't, it adds the piece to playArea[][] and calls for a new piece.

	return value:
	0 if move is not ok.
	1 if move is ok
	*/
	int y = 0;
	int x = 0;
	int moveIsOk = 1;

	for (y = 0; y <= 3; y++)
	{
		for (x = 0; x <= 3; x++)
		{
			// check collision with ground
			if (currPiece[y][x] == cFilled && y + currPieceY >= 19)
			{
				moveIsOk = 0;
			}
			// check collision with debris
			if (currPiece[y][x] == cFilled && playArea[y + currPieceY + 1][(x + currPieceX) % 10] == cFilled)
			{
				moveIsOk = 0;
			}
		}
	}

	if (moveIsOk)
	{
		currPieceY++;
	}
	else
	{
		for (y = 0; y <= 3; y++)
		{
			for (x = 0; x <= 3; x++)
			{
				if (currPiece[y][x] == cFilled)
				{
					playArea[y + currPieceY][(x + currPieceX) % 10] = cFilled;
				}
			}
		}
		newPiece();
	}
	return moveIsOk;
}

int lowerGhost()
{
	/*
	Basically a copy of lowerPiece but used for the "ghost" piece.
	ghost piece = indicator where your piece will land if you drop it.

	returns 0 if ghostY wasn't changed.
	(which indicates collision and that current ghostY is correct)
	To be called multiple times until it returns 0. changes ghostY in each iteration.

	remember to set ghostY = currPieceY before each batch of this function is called.
	*/

	int y = 0;
	int x = 0;
	int moveIsOk = 1;

	for (y = 0; y <= 3; y++)
	{
		for (x = 0; x <= 3; x++)
		{
			// check collision with ground
			if (currPiece[y][x] == cFilled && y + ghostY >= 19)
			{
				moveIsOk = 0;
			}
			// check collision with debris
			if (currPiece[y][x] == cFilled && playArea[y + ghostY + 1][(x + currPieceX) % 10] == cFilled)
			{
				moveIsOk = 0;
			}
		}
	}

	if (moveIsOk)
	{
		ghostY++;
	}
	return moveIsOk;
}

void printPiece()
{
	// not used
	int y = 0;
	int x = 0;
	for (y = 0; y <= 3; y++)
	{
		for (x = 0; x <= 3; x++)
		{
			printf("%c", currPiece[y][x]);
		}
		printf("\n");
	}
}

void init()
{
	int x = 0;
	int y = 0;
	score = 0;
	tickDelay = 1000;
	for (y = 0; y <= YMAX - 1; y++)
	{
		for (x = 0; x <= XMAX - 1; x++)
		{
			playArea[y][x] = cEmpty;
		}
	}
	newPiece();
}

void render(int gameIsOver)
{
	int x = 0;
	int y = 0;
	int i = 0;
	char screen[25][80];
	system("cls");

	// clear
	for (y = 0; y <= 24; y++)
	{
		for (x = 0; x <= 79; x++)
		{
			screen[y][x] = ' ';
		}
	}

	// frame
	for (y = 0; y <= 24; y++)
	{
		screen[y][0] = cBorder;
		// screen[y][79] = '#';
		screen[y][11] = cBorder;
	}
	for (x = 0; x <= 11; x++)
	{
		screen[0][x] = cBorder;
		screen[3][x] = cBorder;
		screen[24][x] = cBorder;
	}

	// frame for static window
	for (y = 1; y <= 23; y++)
	{
		screen[y][22] = '|';
	}
	for (x = 12; x <= 21; x++)
	{
		screen[3][x] = '-';
		screen[24][x] = '-';
	}
	screen[24][21] = '+';

	// score
	screen[1][1] = 'S';
	screen[1][2] = 'c';
	screen[1][3] = 'o';
	screen[1][4] = 'r';
	screen[1][5] = 'e';
	screen[1][6] = ':';

	if (gameIsOver)
	{
		screen[2][1] = 'G';
		screen[2][2] = 'a';
		screen[2][3] = 'm';
		screen[2][4] = 'e';
		screen[2][5] = ' ';
		screen[2][6] = 'O';
		screen[2][7] = 'v';
		screen[2][8] = 'e';
		screen[2][9] = 'r';
		screen[2][10] = '!';
	}

	// play area
	for (y = 0; y <= 19; y++)
	{
		for (x = 0; x <= 9; x++)
		{
			// standard:
			// screen[y+4][x+1] = playArea[y][x];

			//"borderless" aka "follow the piece"
			screen[y + 4][(x + 3 - currPieceX + 10) % 10 + 1] = playArea[y][x];
		}
	}

	// play area static ("real")
	for (y = 0; y <= 19; y++)
	{
		for (x = 0; x <= 9; x++)
		{
			// standard:
			screen[y + 4][x + 12] = playArea[y][x];
		}
	}

	if (gameIsOver == 0) // don't display the piece if the game is over
	{

		// get correct ghostY
		ghostY = currPieceY;
		while (lowerGhost())
		{
		} // lowerGhost() returns 1 if it should be run again

		// current piece
		for (y = 0; y <= 3; y++)
		{
			for (x = 0; x <= 3; x++)
			{
				if (currPiece[y][x] == cFilled)
				{
					screen[y + 4 + currPieceY][x + 4] = cFilled;
					// render "ghost piece"
					screen[y + 4 + ghostY][x + 4] = cGhost;
				}
			}
		}

		// current piece static
		for (y = 0; y <= 3; y++)
		{
			for (x = 0; x <= 3; x++)
			{
				if (currPiece[y][x] == cFilled)
				{
					screen[y + 4 + currPieceY][(x + currPieceX) % 10 + 12] = cFilled;
				}
			}
		}
	}

	// get a string from the score (which is an int)
	char scoreString[4] = "";
	sprintf(scoreString, "%d", score);

	// print the screen

	// y = 0
	printf("############----------+\n");
	// y = 1
	printf("#Score:%d", score);
	for (x = 1; x <= 4 - strlen(scoreString); x++)
	{
		printf(" ");
	}
	printf("#          |High score: \n");

	for (y = 2; y <= 24; y++)
	{
		for (x = 0; x <= 22; x++)
		{
			printf("%c", screen[y][x]);
		}

		// print score
		if (gameIsOver)
		{
			int endFound = 0;
			if (y <= 11)
			{
				printf("%d.", y - 1);
				if (y <= 10)
				{
					printf(" ");
				}
				for (i = 0; i <= 18; i++)
				{
					if (highScoreNames[y - 2][i] == '\0')
					{
						endFound = 1;
					}
					if (endFound)
					{
						printf(" ");
					}
					else
					{
						printf("%c", highScoreNames[y - 2][i]);
					}
				}
				printf("  %d", highScore[y - 2]);
			}
		}
		if (y != 24)
		{
			printf("\n");
		}
	}

	// debug
	printf("X: %d. Y: %d", currPieceX, currPieceY);
}

void oneGame()
{
	clock_t start = clock(), diff;
	char input = ' ';
	init();
	newPiece();

	int msec = diff * 1000 / CLOCKS_PER_SEC;
	int tempTime = msec;
	int somethingHappened = 0;

	// The game "hub". This is the loop that is constantly being looped while the game is running.
	do
	{
		diff = clock() - start;
		int msec = diff * 1000 / CLOCKS_PER_SEC;
		input = ' ';
		somethingHappened = 0;

		// this happens every time >tickDelay< milliseconds has passed since the last time it happened.
		if (msec > tempTime + tickDelay)
		{
			lowerPiece();
			somethingHappened = 1;
			tempTime = msec;
		}

		if (kbhit())
		{
			input = getch();
		}

		switch (input)
		{
		case keyUp:
			rotatePiece();
			somethingHappened = 1;
			break;
		case keyLeft:
			movePiece('l');
			somethingHappened = 1;
			break;
		case keyRight:
			movePiece('r');
			somethingHappened = 1;
			break;
		case keyDown:
			// repeats lowerPiece()
			while (lowerPiece())
			{
			}
			somethingHappened = 1;
			tempTime = msec;
			break;
		case keySpecialEvent:
			// earthquake!
			earthquake();
			somethingHappened = 1;
			break;
		}

		if (somethingHappened)
		{
			render(0); // 0 = game is not over
		}

	} while (input != 'n' && backToMenu == 0);

	char tempName[19] = "Erik";
	toHighScore(score, tempName);
	// display something about losing
	render(1); // 1 = game is over
	getch();
}

int main()
{
	int x;
	int y;
	// clear high score arrays (#TODO: no file i/o at this time)
	for (y = 0; y <= 9; y++)
	{
		highScore[y] = 10 - y;
		for (x = 0; x <= 18; x++)
		{
			highScoreNames[y][x] = 'A';
		}
		highScoreNames[y][19] = '\0';
	}

	srand(time(NULL));

	int quit = 0;
	int selection = 0;

	while (quit == 0)
	{
		selection = menu();

		switch (selection)
		{
		case 1: // play
			oneGame();
			break;
		case 2: // options. #TODO
			quit = 1;
			break;
		case 3:
			quit = 1;
			break;
		}
	}

	system("cls");
	printf("Bye bye!\n");
	return 0;
}