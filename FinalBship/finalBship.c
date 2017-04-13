/*****************
Ryan, Kevin, Josh
BattleShip
Group B
Due 10/23/16
******************/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>
#define PAUSE system("pause")
#define CLS system("cls")
#define ROW 10
#define COL 10
#define NUMSCORES 10
#define NUMSHIPS 5
#define TOTALHIT 17
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

//Menu
char runMenu();
void printMenu();

// Places the Ships
void randomShips(int shipLocation[][COL]);
void randomCoordinates(int* X, int* Y);
int testLocations(int board[][COL], int direction, int shipLength, int startX, int startY);

// Plays the game
void runBattleShip(int board[][COL], int shipHitCount[], int* missleCount);
void printBoard(int board[][COL], int shipHitCount[], int i, int j);
int fireMissle(int board[][COL], int shipLocation[][COL], int shipHitCount[], int* missleCount);

// Checks to see if the game is over
int checkWin(int shipHitCount[], int missleCount);
char wonGame(int missleCount, int highScores[], int numScores);

// Manages the High Scores
void newHighScore(int highScores[], int* numScores);
void printHighScores(int highScores[], int numScores);
void sortHighScores(int numScores, int unSort[]);
void writeHighScores(int highScores[], int numScores);
void readHighScores(int highScores[], int* numScores);

// Starts a new game
void newGame(int a[][COL], int b[][COL], int c[]);

// Saves & Resume a game
void saveGame(int missleCount, int gameBoard[][COL], int shipLocation[][COL]);
void resumeGame(int* missleCount, int gameBoard[][COL], int shipLocation[][COL]);

//Input Functions Containing Error Checking
int scanInt();
int scanBattleChar();
float scanFloat();
char scanChar();

int main(void)
{
	int shipLocation[ROW][COL] = { { 0 }, { 0 } }; // This is the array which contains the locations of the battleships
	int gameBoard[ROW][COL] = { { 0 }, { 0 } }; // This is the array which the user modifies by shooting missles
	int highScores[NUMSCORES] = { 0 }; // This array stores the highscores from previously played games
	int missleCount = 0; // This counter keeps track of the number of missles fires in the current game
	int numScores = 0; // This int keeps track of the number of high scores
	char menu; // this char is used to navigate the menu
	int shipHitCount[NUMSHIPS] = { 0 }; // this array keeps track of which ships are hit and how often
	char nextMissle; // this character is used to verify if the user would like repeat an action
	char nextGame;
	int missleFlag;

	srand(time(NULL));

	randomShips(shipLocation); // creates the locations of the ships

	readHighScores(highScores, &numScores); // reads the bin file containing the high scores


	
	/////////////USE THIS FOR TESTING//////////////////
	int x;
	int y;
	printf("   a  b  c  d  e  f  g  h  i  j\n");
	for (y = 0; y < ROW; y++)
	{
		printf("%2d", y + 1);
		for (x = 0; x < COL; x++)
			printf(" %d ", shipLocation[y][x]);
		printf("\n");
	}
	PAUSE;
	///////////////////////////////////////////////////
	

	do
	{
		CLS;
		menu = runMenu(); //Menu functions

		switch (menu)
		{
		case 'F': // Fire a Missle
			do
			{
				missleFlag = 0;
				missleFlag = fireMissle(gameBoard, shipLocation, shipHitCount, &missleCount);
			} while (missleFlag);
			break;

		case 'H':
			CLS;
			printHighScores(highScores, numScores);
			break;
		case 'N': // Start Over 
			newGame(gameBoard, shipLocation, shipHitCount);
			randomShips(shipLocation);
			missleCount = 0;
			break;
		case 'S':
			saveGame(missleCount, gameBoard, shipLocation);
			return 0;
			break;
		case 'R':
			resumeGame(&missleCount, gameBoard, shipLocation);
			break;
		case 'X': // Exit
			break;
		default:
			printf("\n\tPlease Enter a Valid Option\n");
			PAUSE;
			break;
		}


		if (!checkWin(shipHitCount, missleCount)) // checks whether or not the game is over
		{
			nextGame = wonGame(missleCount, highScores, numScores); // user has won
			if (nextGame == 'Y')
			{
				menu = 'N';
				newGame(gameBoard, shipLocation, shipHitCount);
				randomShips(shipLocation);
				missleCount = 0;
				readHighScores(highScores, &numScores); // reads the bin file containing the high scores
			}
			else
				menu = 'X';
		}

	} while (menu != 'X');
	PAUSE;
	return 0;
}// End Main

/*******************************************************************
Function: Comment Header
Data Needed:

Data Returned:

Purpose:

*******************************************************************/


/*******************************************************************
Function: runMenu

Data Needed:
None

Data Returned:
char

Purpose:
This function controls the menu of the program. It returns a
character that will be used within the switch in main.
*******************************************************************/
char runMenu()
{
	char result;

	do
	{
		printMenu(); // prints the menu
		result = scanChar(); // character returning function
	} while (result == '0'); // value will be '0' if the user is a fool, or if they enter an invalid entry in scanChar

	return result;
}// END runMenu


/*******************************************************************
Function: printMenu
Data Needed:
None
Data Returned:
None
Purpose:
Prints the menu
*******************************************************************/
void printMenu()
{
	printf("\n\n\t*****************************"
		"\n\t* 'F' : Fire a Missle       *"
		"\n\t* 'H' : View High Scores    *"
		"\n\t* 'N' : Start a New Game    *"
		"\n\t* 'S' : Save a Current Game *"
		"\n\t* 'R' : Resume a Past Game  *"
		"\n\t* 'X' : Exit the Program    *"
		"\n\t*****************************"
		"\n\tEntry : ");
	return;
} // END printMenu


/**********************************************************************
Function: randomShips

Data Needed:
1) Array that will contain the ship locations

Data Returned:
None

Purpose:
This function places the ships used to play the game. It obtains a
random direction, then will cycle through random x/y coordinates
until it finds a valid location for the ship
***********************************************************************/
void randomShips(int shipLocation[][COL])
{
	int direction; // direction 0 = up, 1 = down, 2 = left, 3 = right
	int randX; // x coordinate
	int randY; // y coordinate
	int shipCount = 0; // number of ships placed. NOTE: This value will also be used with a +1
	int placeShip; // counter used with shipLength[] to place the correct number of ships
	int shipLength[NUMSHIPS] = { 2, 5, 4, 3, 3 }; // the lengths of the various ships used within the game
	int test; // used as a boolean to determine when a ship is successfully placed


	for (shipCount; shipCount < NUMSHIPS; shipCount++) // This loop runs once for each ship
	{
		test = 1; // setting bool to TRUE
		direction = rand() % 4; // random number to determine the location that will be used for the ship

		switch (direction) // switch used with the direction of the ship
		{
		case UP:
			do
			{
				randomCoordinates(&randX, &randY); // generates a random (x,y) coordinate
				if (testLocations(shipLocation, direction, shipLength[shipCount], randX, randY)) // this will return true if the function that tests whether the location is valid is true
				{
					for (placeShip = 0; placeShip < shipLength[shipCount]; placeShip++) // places the ship based on the length of the shipLength.
						// placeShip is the counter for the number of locations that have been placed
						shipLocation[randY - placeShip][randX] = shipCount + 1; // shipLocation[randY - placeShip][randX] when we think about how an array is done in our mind the first column is the 
					// "y" coordinate in terms of a coordinate system to go up (array[5][5] -> array[3][5]) we need to decrease y in value so 
					// for each iteration we subtract the number of placed locations from the origional coordinate to keep track of where 
					// we are withing the array
					test = 0;	// Because the ship was successfully placed we change our bool value to false to break the loop. this value will reset at the beginning of the next ship.
				}
			} while (test); // If a ship is not successfully placed due to either another ship being in the way, or the edge of the board the loop will generate new numbers and try again
			break;

		case DOWN:
			do
			{
				randomCoordinates(&randX, &randY); // generates a random (x,y) coordinate
				if (testLocations(shipLocation, direction, shipLength[shipCount], randX, randY)) // this will return true if the function that tests whether the location is valid is true
				{
					for (placeShip = 0; placeShip < shipLength[shipCount]; placeShip++)// places the ship based on the length of the shipLength.
						// placeShip is the counter for the number of locations that have been placed
						shipLocation[randY + placeShip][randX] = shipCount + 1;// shipLocation[randY + placeShip][randX] when we think about how an array is done in our mind the first column is the 
					// "y" coordinate in terms of a coordinate system to go down (array[5][5] -> array[7][5]) we need to increase y in value so 
					// for each iteration we add the number of placed locations from the origional coordinate to keep track of where 
					// we are withing the array
					test = 0;// Because the ship was successfully placed we change our bool value to false to break the loop. this value will reset at the beginning of the next ship.
				}
			} while (test); // If a ship is not successfully placed due to either another ship being in the way, or the edge of the board the loop will generate new numbers and try again
			break;

		case LEFT:
			do
			{
				randomCoordinates(&randX, &randY); // generates a random (x,y) coordinate
				if (testLocations(shipLocation, direction, shipLength[shipCount], randX, randY)) // this will return true if the function that tests whether the location is valid is true
				{
					for (placeShip = 0; placeShip < shipLength[shipCount]; placeShip++)// places the ship based on the length of the shipLength.
						// placeShip is the counter for the number of locations that have been placed
						shipLocation[randY][randX - placeShip] = shipCount + 1; // [randY][randX - placeShip] when we think about how an array is done in our mind the second column is the 
					// "x" coordinate in terms of a coordinate system to go left (array[5][5] -> array[5][3]) we need to decrease x in value so 
					// for each iteration we subtract the number of placed locations from the origional coordinate to keep track of where 
					// we are withing the array
					test = 0;// Because the ship was successfully placed we change our bool value to false to break the loop. this value will reset at the beginning of the next ship.
				}
			} while (test); // If a ship is not successfully placed due to either another ship being in the way, or the edge of the board the loop will generate new numbers and try again
			break;

		case RIGHT:
			do
			{
				randomCoordinates(&randX, &randY); // generates a random (x,y) coordinate
				if (testLocations(shipLocation, direction, shipLength[shipCount], randX, randY)) // this will return true if the function that tests whether the location is valid is true
				{
					for (placeShip = 0; placeShip < shipLength[shipCount]; placeShip++)// places the ship based on the length of the shipLength.
						// placeShip is the counter for the number of locations that have been placed
						shipLocation[randY][randX + placeShip] = shipCount + 1; // [randY][randX - placeShip] when we think about how an array is done in our mind the second column is the 
					// "x" coordinate in terms of a coordinate system to go right (array[5][5] -> array[5][7]) we need to increase x in value so 
					// for each iteration we add the number of placed locations from the origional coordinate to keep track of where 
					// we are withing the array
					test = 0;// Because the ship was successfully placed we change our bool value to false to break the loop. this value will reset at the beginning of the next ship.
				}
			} while (test); // If a ship is not successfully placed due to either another ship being in the way, or the edge of the board the loop will generate new numbers and try again
			break;
		} // End Direction Switch
	} // End Ship Placement
	return;
} // END randomShips


/********************************************************************
Function: testLocations

Data Needed:
1) Array that will contain the ship locations
2) Direction the ships will be facing
3) Length of the ship
4) x coordinate
5) y coordinate

Data Returned:
int

Purpose:
This function will determine whether or not the current location
for a ship is valid or not. It does this by "looking" ahead
and testing the values contained in each space the ship will
occupy looking for a '0' value. If the edge of the board is
reached, or if another ship is located at that coordinate the
function will return FALSE.
********************************************************************/
int testLocations(int board[][COL], int direction, int shipLength, int startX, int startY)
{
	int validSpot = 0; // Bool value initialed to false.
	int ship; // Counter used for the loops. will be initialized to 0 upon the start of the loop
	switch (direction)
	{
	case UP:
		for (ship = 0; ship < shipLength; ship++) // loop that will check each space until the counter reaches the ship's length.
		{
			if (startY - shipLength < 0) // This test checks whether or not there is enough room from the y coordinate to the top
			{
				validSpot = 0;
				return validSpot; //returns false
			}
			if (board[startY - ship][startX] == 0) //tests whether or not the location has a ship already in it.
				validSpot = 1; // because the ship passed the previous test, and there is no ship at this coordinate currently BOOL is set to true
			else
			{
				validSpot = 0; // this usually will run if there is a ship already in the location 
				return validSpot; //returns false
			}
		}
		break;

	case DOWN:
		for (ship = 0; ship < shipLength; ship++) // loop that will check each space until the counter reaches the ship's length.
		{
			if (startY + ship >= 9) // This test checks whether or not there is enough room from the y coordinate to the bottom
			{
				validSpot = 0;
				return validSpot; //returns false
			}
			if (board[startY + ship][startX] == 0) //tests whether or not the location has a ship already in it.
				validSpot = 1; // because the ship passed the previous test, and there is no ship at this coordinate currently BOOL is set to true
			else
			{
				validSpot = 0; // this usually will run if there is a ship already in the location 
				return validSpot; //returns false
			}
		}
		break;

	case LEFT:
		for (ship = 0; ship < shipLength; ship++) // loop that will check each space until the counter reaches the ship's length.
		{
			if (startX - ship < 0) // This test checks whether or not there is enough room from the x coordinate to the left
			{
				validSpot = 0;
				return validSpot; //returns false
			}

			if (board[startY][startX - ship] == 0) //tests whether or not the location has a ship already in it.
				validSpot = 1; // because the ship passed the previous test, and there is no ship at this coordinate currently BOOL is set to true

			else
			{
				validSpot = 0; // this usually will run if there is a ship already in the location 
				return validSpot; //returns false
			}
		}
		break;

	case RIGHT:
		for (ship = 0; ship < shipLength; ship++) // loop that will check each space until the counter reaches the ship's length.
		{
			if (startX + ship > 9) // This test checks whether or not there is enough room from the x coordinate to the right
			{
				validSpot = 0;
				return validSpot; //returns false
			}
			if (board[startY][startX + ship] == 0) //tests whether or not the location has a ship already in it.
				validSpot = 1; // because the ship passed the previous test, and there is no ship at this coordinate currently BOOL is set to true
			else
			{
				validSpot = 0; // this usually will run if there is a ship already in the location 
				return validSpot; //returns false
			}
		}
		break;
	} // End direction switch
	return validSpot; //Usually returns true due to the return 0 statements being nested within the switch.
} // END of testLocations


/*********************************************************************
Function: randomCoordinates

Data Needed:
1) pointer to the x coordinate
2) pointer to the y coordinate

Data Returned:
None, but the function is passing by reference.

Purpose:
This function simply generates two random numbers for the (x,y)
pairs used for the random ship placement.
*********************************************************************/
void randomCoordinates(int* X, int* Y)
{
	*X = rand() % 10; // generates a random integer between 0-9
	*Y = rand() % 10; // generates a random integer between 0-9
	return;
} // END of randomCoordinates


/*********************************************************************
Function: fireMissle

Data Needed:
1) board[][COL] is the "gameboard" this array is modified based
off of the user's decisions
2) shipLocations[][COL] is the array which contains the locations
of the ships.
3) shipHitCount[] is an array which acts as a counter. Each location
in the array coincides with one of the ships in the
program. As ships are hit this counter ticks up to
alert another function to when a ship is sunk.
4) *missleCount is the number of missles fired to the board. This
variable is passed by reverence to allow it to be
modified within different scopes.

Data Returned:
NONE, but modifies variables which are passed by reference

Purpose:
This function runs the "game" it allows the user to fire missles
at the gameboard. this function both recieves input from the user,
and processes that data.
**********************************************************************/
int fireMissle(int board[][COL], int shipLocation[][COL], int shipHitCount[], int* missleCount)
{
	int locationX, locationY;
	int missleFlag = 1;
	
	runBattleShip(board, shipHitCount, missleCount);
	printf("\n\t'0' to stop Firing");
	printf("\n\tEnter a letter coordinate: ");
	locationX = scanBattleChar();

	if (locationX == -1)	//	If the user input's '0' then they indicate another shot is not wanted.
	{
		missleFlag = 0;
		return missleFlag;
	}

	printf("\tEnter a number coordinate: ");
	locationY = scanInt();

	if (locationY == 0)	//	If the user input's '0' then they indicate another shot is not wanted.
	{
		missleFlag = 0;
		return missleFlag;
	}
	locationY--;	// This is to modify what the user reads to array syntax
	//locationY = scanInt() - 1;

	/************************************************************
	*	This next section performs some minor error checking	*
	************************************************************/
	if (locationY < 0 || locationY > 9)
		locationY = -999;

	if (locationX == -999 || locationY == -999)
	{
		CLS;
		printf("\tINVALID COORDINATES\n");
		PAUSE;
		return missleFlag;
	}

	if (board[locationY][locationX] != 0)
	{
		CLS;
		printf("\n\tYou have already Fired Here!\n");
		PAUSE;
		return missleFlag;
	}
	//					END OF CHECKING
	//********************************************************

	if (shipLocation[locationY][locationX] != 0) //checks if the array location is empty
	{
		board[locationY][locationX] = shipLocation[locationY][locationX]; //TEST THIS WITHIN THE NEXT IF STATEMENT
		if (board[locationY][locationX] > 0)
			shipHitCount[(board[locationY][locationX] - 1)]++; //increments the shipHitCount
	}
	else
		board[locationY][locationX] = -1; // if shipLocation[y][x] == 0 the board is set to -1 to indicate a miss

	(*missleCount)++; // incriments the missleCount. This is done after error checking to confirm a valid missle

	runBattleShip(board, shipHitCount, missleCount);
	if (board[locationY][locationX] > 0)
		printf("\n\t*******\n\t* HIT *\n\t*******\n");
	else
		printf("\n\t********\n\t* MISS *\n\t********\n");
	return missleFlag;
} // END fireMissle


/*******************************************************************
Function: runBattleShip

Data Needed:
1) board[][COL] the array containing the game board
2) shipHitCount[] is the array contining the number of hits for
the ships to be sunk
3) *missleCount misslecount is passed by reference, but it may be
possible to achieve the same result with value instead

Data Returned:
NONE, This process primarily will be printing out the game board

Purpose:
This function handles converting our gameboard into what the
user thinks of when playing battleship. Going from an integer
array to a gameboard which will display hits and misses.
*******************************************************************/
void runBattleShip(int board[][COL], int shipHitCount[], int* missleCount)
{
	CLS;
	int i;
	int j;
	printf("\n\n\t    a  b  c  d  e  f  g  h  i  j\n"
		"\t   ------------------------------");
	for (i = 0; i < ROW; i++)
	{
		printf("\n\t%2d|", i + 1);// prints the numbered columns for the board
		for (j = 0; j < COL; j++)
		{
			printBoard(board, shipHitCount, i, j);  // sends the array and location to a function to print. 
			// this may be simpler to do by passing a single location 
			// instead of the full array

			if (j == (COL - 1)) // checks if we are at the end of the gameboard
				printf("|"); // prints the edge of the board
		}
	}
	printf("\n\t   ------------------------------\n");// prints the bottom of the board
	printf("\n\tMissle Count: %d\n", *missleCount); // prints the missle count for the user's knowledge
} // END runBattleShip


/*******************************************************************
Function: printBoard

Data Needed:
1) board[][COL] array containing the gameboard
2) shipHitCount[]   is the array contining the number of hits for
the ships to be sunk.
3) i counter for the ROW
4) j counter for the COL
Data Returned:
NONE. This function prints only.

Purpose:
This function converts our integer based array into the desired
gameboard for the user to interface with.
*******************************************************************/
void printBoard(int board[][COL], int shipHitCount[], int i, int j)
{
	/********************************************************************
	*						0: Miss										*
	*		NOTE: if shipHitCount != shipLength							*
	*				this function will print H 							*
	*				instead of the ship name							*
	*																	*
	*						1: Crusier									*
	*						2: Aircraft Carrier							*
	*						3: Battleship								*
	*						4: Submarine								*
	*						5: Destroyer								*
	*********************************************************************/
	switch (board[i][j]) // Switch dominated by the value stored within the array. (i may simplify this function slightly)
	{
	case 0:
		printf(" * ");
		break;
	case -1:
		printf(" M ");
		break;
	case 1:
		if (shipHitCount[0] == 2)
			printf(" C ");
		else
			printf(" H ");
		break;

	case 2:
		if (shipHitCount[1] == 5)
			printf(" A ");
		else
			printf(" H ");
		break;

	case 3:
		if (shipHitCount[2] == 4)
			printf(" B ");
		else
			printf(" H ");
		break;

	case 4:
		if (shipHitCount[3] == 3)
			printf(" S ");
		else
			printf(" H ");
		break;

	case 5:
		if (shipHitCount[4] == 3)
			printf(" D ");
		else
			printf(" H ");
		break;
	}
} // END printBoard


/*******************************************************************
scanInt, ScanFloat both check to ensure that the value assigned
through scanf() is valid for that particular data type
*******************************************************************/
int scanInt()
{

	int intNum;

	if (scanf("%d", &intNum)) //returns true if the input is an int
		return intNum;
	else
	{
		return -999;
	}
}


/*******************************************************************
scanInt, ScanFloat both check to ensure that the value assigned
through scanf() is valid for that particular data type
*******************************************************************/
float scanFloat()
{

	float num;

	if (scanf("%f", &num)) //returns true if the input is a float
		return num;
	else
	{
		return -999;
	}
}


/*******************************************************************
scanChar is similar to the other two except it has an additional
condition to change lower case letters to upper case,
this version of scanChar() has been slightly modified to change a
character chosen to a coordinate for the battleship game.
*******************************************************************/
int scanBattleChar()
{

	char result;

	if (scanf(" %c", &result)) //returns true if the input is a char
	{
		if (isupper(result));
		result = toupper(result);

		switch (result)
		{
		case '0':
			return -1;
		case 'A':
			return 0;
		case 'B':
			return 1;
		case 'C':
			return 2;
		case 'D':
			return 3;
		case 'E':
			return 4;
		case 'F':
			return 5;
		case 'G':
			return 6;
		case 'H':
			return 7;
		case 'I':
			return 8;
		case 'J':
			return 9;
		default:
			return -999;
		}
	}
	else
	{
		return -999;
	}
}


/*******************************************************************
scanChar is similar to the other two except it has an additional
condition to change lower case letters to upper case
*******************************************************************/
char scanChar()
{

	char result;

	if (scanf(" %c", &result)) //returns true if the input is a char
	{
		if (isupper(result));
		result = toupper(result);
	}
	else
	{
		(result) = '0';
		printf("\n*******************"
			"\n*  Invalid Entry  *"
			"\n*******************\n");
		PAUSE;
	}

	return result;
}


/*******************************************************************
Function: checkWin

Data Needed:
1) shipHitCount[]	The array containing the number of times
each ship has been hit by the user
2)missleCount		NOTE: Remove missleCount, in a previous
version this variable did something,
however in this function we do not use it

Data Returned:
int: The number of hits left to sink the ships

Purpose:
if the sum of shipHitCount is equal to the total number of
holes in the ship this function will return a value to let MAIN
know that the game has been won.
*******************************************************************/
int checkWin(int shipHitCount[], int missleCount)
{
	int totalHit = TOTALHIT;
	int count;
	for (count = 0; count < NUMSHIPS; count++) // runs for each ship
		totalHit -= shipHitCount[count];
	return totalHit;
} // END checkWin


/*******************************************************************
Function: wonGame

Data Needed:
1) missleCount
2) highScores[] is the array containing previous scores
3) numScores is an int of the number of scores stored within
highScores[]

Data Returned:
NONE

Purpose:
This function runs when the user has sunk all of the ships
stored in the array.
*******************************************************************/
char wonGame(int missleCount, int highScores[], int numScores)
{
	char again;
	CLS;
	printf("\n\t**************************"
		"\n\t*     CONGRATULATIONS    *"
		"\n\t* You won in %3d missles *"
		"\n\t**************************\n\n", missleCount);
	if (numScores < NUMSCORES)
		highScores[numScores] = missleCount;
	else if (missleCount < highScores[numScores])
		highScores[numScores] = missleCount;

	numScores++;

	// now our highscores are stored
	sortHighScores(numScores, highScores); // Sorts highscores in terms of lowest score first
	writeHighScores(highScores, numScores); // Writes the bin file containing highScores
	printHighScores(highScores, numScores); // Outputs the highScores to the user
	printf("\n\tPlay Again?(Y/N) :");
	again = scanChar();

	return again;
} // END wonGame



/*******************************************************************
Function: sortHighScores

Data Needed:
1) numScores	is an int which contains the number of scores
in the array
2) unsort[]		is the array we wish to sort
Data Returned:
NONE, However we will be modifying our array which is passed
by referenced. So nothing needs to be "caught," but values
will be modified within this function

Purpose:
To sort our unsorted array from best scores to worst
*******************************************************************/
void sortHighScores(int numScores, int unSort[])
{
	char switchMade; // a bool esque value
	int temp; // a temporary value used to store values which will be swapped
	int count; // loop counter

	do // begin do while
	{
		switchMade = 'N'; // bool initialized to FALSE
		for (count = 0; count < (numScores - 1); count++) // This will run once for each location in the array
		{
			if (unSort[count] > unSort[count + 1]) // checks to see if the next value is greater than the previous
			{
				//standard bubblesort
				temp = unSort[count + 1];
				unSort[count + 1] = unSort[count];
				unSort[count] = temp;
				//end bubblesort
				switchMade = 'Y'; // bool initialized to TRUE
			}
		}
	} while (switchMade == 'Y'); // end do while
	return;
} // END sortHighScores


/************************************************************************
Function: printHighScores

Data Needed:
1) highScores[]	an array which contains our previously stored scores
2) numScores an int which represents the number of stored scores

Data Returned:
NONE

Purpose:
This function will print out the highscores in order of least
required missles to greatest.
***********************************************************************/
void printHighScores(int highScores[], int numScores)
{
	int count;
	printf("\tGame#\tScore\n");
	for (count = 0; count < numScores; count++) // Loop to read the array
	{
		if (highScores[count])
			printf("\t%2d:\t%d\n", count + 1, highScores[count]);
		else
			break;
	}
	PAUSE;
	return;
} // END printHighScores


/*******************************************************************
Function: readHighScores

Data Needed:
1) The array which we will be storing our highScores
2) The LOCATION of our scores counter (numScores)

Data Returned:
NONE

Purpose:
This function reads our bin file containing our stored highScores
and populates both our games counter, and our highScores array.
*******************************************************************/
void readHighScores(int highScores[], int* numScores)
{
	FILE* scoresPtr;
	scoresPtr = fopen("highScores.bin", "rb");
	if (scoresPtr == NULL) // if no file is found this creates a file
	{
		scoresPtr = fopen("highScores.bin", "wb");
		if (scoresPtr == NULL)
		{
			CLS;
			printf("Something is wrong with the file location\n");
			exit(2);
		}
		fclose(scoresPtr);
		return;
	}

	fread(numScores, sizeof(int), 1, scoresPtr);
	fread(highScores, sizeof(int), *numScores, scoresPtr);

	fclose(scoresPtr);
	return;
} // END readHighScores


/************************************************************************
Function: writeHighScores

Data Needed:
1) highScores[]		contains stored highScores
2) numScores		refrences the number of highscores in the array

Data Returned:
NONE

Purpose:
This function writes the binary file we will be using to store
the best scores.
***********************************************************************/
void writeHighScores(int highScores[], int numScores)
{
	FILE* scoresPtr;
	scoresPtr = fopen("highScores.bin", "wb");

	if (scoresPtr == NULL) // Terminates the program when an error occurs writing the file
	{
		printf("Error WRITING the File");
		PAUSE;
		exit(1);
	}

	fwrite(&numScores, sizeof(int), 1, scoresPtr);
	fwrite(highScores, sizeof(int), numScores, scoresPtr);
	fclose(scoresPtr);
	return;
} // END writeHighScores


/***********************************************************************
Function: newGame

Data Needed:
Two arrays which will be initialized to 0

Data Returned:
NONE, but we are heavily modifying the values stored in both arrays

Purpose:
This function resets the arrays to only contain 0 in every location
************************************************************************/
void newGame(int a[][COL], int b[][COL], int c[])
{
	int x;
	int y;

	for (y = 0; y < ROW; y++)
	{
		for (x = 0; x < COL; x++)
			a[y][x] = 0;
	}

	for (y = 0; y < ROW; y++)
	{
		for (x = 0; x < COL; x++)
			b[y][x] = 0;
	}

	for (y = 0; y < NUMSHIPS; y++)
		c[y] = 0;
	return;
} //END newGame


/***********************************************************************
Function: saveGame

Data Needed:
missleCount
gameBoard
shipLocation

Data Returned:
NONE

Purpose:
This function saves the data from the current game to be resumed at
a future time. The program terminates at the end.
************************************************************************/
void saveGame(int missleCount, int gameBoard[][COL], int shipLocation[][COL])
{
	FILE* savedGame;
	savedGame = fopen("savedGame.bin", "wb");
	if (savedGame == NULL)
	{
		fclose(savedGame);
		return;
	}

	fwrite(&missleCount, sizeof(int), 1, savedGame);
	fwrite(gameBoard, sizeof(int), (ROW*COL), savedGame);
	fwrite(shipLocation, sizeof(int), (ROW*COL), savedGame);

	fclose(savedGame);
	CLS;
	printf("THANK YOU FOR PLAYING\nGAME SAVED\n");
	PAUSE;
	return;
}

/***********************************************************************
Function: resumeGame

Data Needed:
missleCount
gameBoard
shipLocation

Data Returned:


Purpose:
This function repopulates our data from a saved file.
************************************************************************/
void resumeGame(int* missleCount, int gameBoard[][COL], int shipLocation[][COL])
{
	CLS;
	int ret;
	char fileName[] = "savedGame.bin";
	FILE* savedGame;

	savedGame = fopen(fileName, "rb");
	if (savedGame == NULL)
	{
		CLS;
		printf("\n\n\tNo Game Data was Found\n\n\t");
		PAUSE;
		return;
	}
	fread(missleCount, sizeof(int), 1, savedGame);
	fread(gameBoard, sizeof(int), (ROW*COL), savedGame);
	fread(shipLocation, sizeof(int), (ROW*COL), savedGame);

	fclose(savedGame);


	ret = remove(fileName);

	if (ret == 0)
	{
		printf("\n\n\tGame Successfully Resumed\n");
		PAUSE;
	}
	else
	{
		printf("Error: Resuming Game\n");
		PAUSE;
	}
	return;
}