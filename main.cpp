// O. Paszkiewicz 2022 - Project for Introduction to Programming


#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include"conio2.h"
#include"conio2.cpp"

#define NUMBER_OF_CHARACTERS_FILE_NAME 1000 /* max number of characters for filename */

/* GAME OPTIONS */
#define BOARDSIZE_OPTION_1 '1' /* Options for board sizes - menu */
#define BOARDSIZE_OPTION_2 '2'
#define BOARDSIZE_OPTION_3 '3'
#define BOARDSIZE_OPTION_4 '4'
#define WHITEPLAYERTURN 1 /* on the 2D array white player is indicated by 1 (= white stone placed)*/
#define BLACKPLAYERTURN 2 /* black player indicated by 2 (= black stone placed)*/
#define CLEARTURN 0 /* 0 - no stone placed */

/* CONTROLS  */
#define KEY_UP 0x48 /* walking around the board, by arrows */
#define KEY_DOWN 0x50
#define KEY_LEFT 0x4b
#define KEY_RIGHT 0x4d
#define KEY_CONFIRM 'c' //0x0d /* confirm = enter */
#define KEY_CANCEL 27 /* cancel = esc */
#define KEY_PLACE_STONE 'i' /* place the stone on the board = i */
#define KEY_SAVE 's' /* save the game = s */
#define KEY_LOAD 'l' /* load the game = l */
#define KEY_FINISH 'f' /* finish the game and show the score = f */
#define KEY_Q 'q' /* quit game = q */
#define KEY_SPACE ' ' /* for finding if space was pressed */
#define START_GAME 'n' /* starting the game = n */
#define HELP_KEY_h 'h' /* show controls/help = h */
#define KEY_DELETE_1 127 /* for writting, delete the character = delete */
#define KEY_DELETE_2 8 /* for writting, delete the character = backspace */

/* COSMETICS */
#define STONE "o" /* the character printed as stone */
#define BACKGROUND_COLOR 6 //BROWN - background color
#define BLACKPLAYER_COLOR 0 //BLACK - black player color
#define WHITEPLAYER_COLOR 15 //WHITE - white player color
#define INITIAL_TEXT_COLOR 7 //LIGHTGRAY - text color

struct COORDS_t /* coordinates */
{
    int x;
	int y;
};
struct SCORE_t /* score, keeping in black player and white player score */
{
	int white;
	int black;
};
struct FINALSCORE_t /* difference from score by variables being floats instead of ints */
{
    float white;
    float black;
};
struct BOARDSTATES_t /* to keep iside previous states of board */
{
    int **twomoveback;
    int **onemoveback;
};

struct SAVE_t /* struct for storing all variables to save and load */
{
    SCORE_t score;
    COORDS_t coords;
    int board_size;
    int turn;
    int handicapped;
    int handCount;
    COORDS_t boardshift;
    int **board;
    BOARDSTATES_t prevboard;
};

struct INITIALIZATION_t /* struct for storing board size and the current shift of board screen (variable needed for scrolling) - so that the board can be initialized */
{
    int board_size;
    COORDS_t boardshift;
};

/* board */
int **clearBOARD(int board_size); /* initialize board full of 0s - empty spaces */
int chooseBOARDoption(COORDS_t coords); /* for menu that shows you options for board sizes - it returns which options the player is choosing */
int getSizeOfBoard(COORDS_t coords); /* get size of window */
void deleteBoard (int **board, int size); /* delete allocated memory */
void copyBoard(int **source, int **target, int size);
int compareBoards(int **firstBoard, int **secondBoard, int size);
void initializeBoardScreen(INITIALIZATION_t loadfile, int **vir_board, COORDS_t coords); /* print the board with stones placed on it */
COORDS_t moveAroundBoard(COORDS_t coords, COORDS_t max, COORDS_t min, int zn); /* changing coords when moving by arrows */

/* file options - saving and loading */
void saveFile(SAVE_t savefile, int **vir_board, BOARDSTATES_t prevBoardState, COORDS_t coords);
SAVE_t loadFile(int **vir_board, BOARDSTATES_t prevBoardState, COORDS_t coords);
char * fileName(COORDS_t coords); /* get file name input */

/* other */
FINALSCORE_t countScore(SCORE_t score, int handicappedCount, int **vir_board, int size);
void printSTONE(COORDS_t coords, int turn); /* print stone on the board */
COORDS_t getSizeOfScreen();

/* searching the board implementation */
int checkLiberties(const COORDS_t coords, const int max, int **vir_board, int r_turn); /* find if the stone has any liberties */
int checkIfNotExistInTwoArrays(COORDS_t * tab1, COORDS_t * tab2, COORDS_t coords, int tab1Flag, int tab2Flag); /* check if the position was not already checked, taken for cheking */
int checkIfNotIn(COORDS_t * tab1, COORDS_t coords, int tab1Flag); /* check if coords are not already in coords array */
COORDS_t * findCaptured(COORDS_t coords, int max, int **vir_board, int turn); /* find all captured stones */
int checkIfpossiblePlacement(COORDS_t coords, int max, int **vir_board, int turn); /* chack if it is possible to place a stone */

/* graphical */
int changeBackground(int back);
void printBOARDoptions(COORDS_t coords);
void clearBOARDoptions(COORDS_t coords);
void printInstruction(COORDS_t coords);
void printHELP(COORDS_t coords);
void printWRONGKEY(COORDS_t coords);
void printBOARD(COORDS_t coords, COORDS_t start, int size);
void displaySCORE(COORDS_t coords, SCORE_t score);
void displayPOSITION(COORDS_t coords, COORDS_t pos, const COORDS_t board);
void initializeSCREEN(int background, COORDS_t coords);



int main() {
    /* BOARD AND INSTRUCTION PLACEMENT */
    const COORDS_t instr_placement = {2, 1}, startGame_instr = { 50, 1}, board_pos = {50, 4};

    /* VARIABLES */
	int zn = 0, back = BACKGROUND_COLOR, new_game_started = 0, board_size_option = 3; /* zn - pressed key, back - background, new_game_stated - flag for checking if the game was started, board_size_option - initally 3 (player input) */
	SCORE_t score = {0, 0};
    COORDS_t coords = {board_pos.x+1, board_pos.y+1}; /* current cursor position in the program, not on board*/
    COORDS_t max = {coords.x, coords.y}; /* board border (max x and max y) */
    const COORDS_t min = { coords.x, coords.y}; /* board border (min x and min y)*/
	int boardSize[4] = {9, 13, 19, 1}, **virtual_board; /* board size to choose frome, board */
    BOARDSTATES_t prevBoardState; /* saving a previous move, so it does not repeat */
	virtual_board = clearBOARD(boardSize[board_size_option]); /* initializing board (fist one of size 1x1) */
    prevBoardState.onemoveback = clearBOARD(boardSize[board_size_option]);
    prevBoardState.twomoveback = clearBOARD(boardSize[board_size_option]);

#ifndef __cplusplus
	Conio2_Init();
#endif

	settitle("O. Paszkiewicz");

	_setcursortype(_NOCURSOR);
	do {
		initializeSCREEN(back, instr_placement); /* initialize scree with instructions */

        if( zn!= START_GAME)
        {
            zn = getch();
        }

        if(zn == HELP_KEY_h)
        {
        	printInstruction(instr_placement); /* after pressing h-help show instructions */
            getch();
        }
		else if(zn == KEY_SPACE)
        {
            back = changeBackground(back);
        }
        else if(zn == START_GAME)
        {
            coords = {board_pos.x+1, board_pos.y+1};
            new_game_started = 1;
            /* delete */
            deleteBoard(virtual_board, boardSize[board_size_option]);
            deleteBoard(prevBoardState.onemoveback, boardSize[board_size_option]);
            deleteBoard(prevBoardState.twomoveback, boardSize[board_size_option]);
            /**/
            board_size_option = chooseBOARDoption(startGame_instr);
			score = {0, 0};
			if(board_size_option == 3)
			{
				boardSize[board_size_option] = getSizeOfBoard(startGame_instr);
                clearBOARDoptions(startGame_instr);
			}
            virtual_board = clearBOARD(boardSize[board_size_option]); /* initialize board in the choosen size */
            prevBoardState.onemoveback = clearBOARD(boardSize[board_size_option]);
            prevBoardState.twomoveback = clearBOARD(boardSize[board_size_option]);
        }
        else if (zn!=KEY_Q)
        {
            printWRONGKEY(startGame_instr);
        }

        if(new_game_started)
        {
            int turn = BLACKPLAYERTURN, handicapped = 1, handCount = 0; /* white player is the starting one */
            COORDS_t boardShift = {0, 0};
            COORDS_t screenSize = getSizeOfScreen();
            INITIALIZATION_t bbshiftsize;
            int isBoardShift = 0;
            /* cosmetics */
            printBOARD(board_pos, boardShift, boardSize[board_size_option]);
            displaySCORE(instr_placement, score);
            displayPOSITION(instr_placement, {coords.x+boardShift.x,coords.y+boardShift.y}, board_pos);
            /**/
            max = { board_pos.x+(boardSize[board_size_option]*2), board_pos.y+boardSize[board_size_option]}; /* get board border */
            while(zn!=KEY_Q && zn!=KEY_FINISH &&(new_game_started || zn!=START_GAME)){ /* while not pressed quit or start a new game */
                new_game_started = 0;
                printSTONE(coords,turn);
                zn = getch();
                if(zn == 0) {	
                    zn = getch();
                    /* moving */
                    printSTONE(coords,virtual_board[(coords.x+boardShift.x-min.x)/2][coords.y+boardShift.y-min.y]);
					coords = moveAroundBoard(coords, max, min, zn);
                    isBoardShift = 0;
                    //
                    if(max.x<coords.x+boardShift.x) coords.x-=2;
                    else if(max.y<coords.y+boardShift.y) coords.y--;
                    else if (screenSize.x-1<coords.x) { coords.x-=2; boardShift.x+=2; isBoardShift = 1; }
                    else if (screenSize.y-1<coords.y) { coords.y--; boardShift.y++; isBoardShift = 1; }
                    else if(min.x>coords.x)
                    {
                        coords.x+=2;
                        if(boardShift.x>0)  { boardShift.x-=2; isBoardShift = 1; }
                    }
                    else if(min.y>coords.y)
                    {
                        coords.y++;
                        if(boardShift.y>0) {  boardShift.y--; isBoardShift = 1; }
                    }
                    //
                    if(isBoardShift == 1)
                    {
                        // initialize screen;
                        bbshiftsize.board_size = boardSize[board_size_option];
                        bbshiftsize.boardshift.x = boardShift.x; bbshiftsize.boardshift.y = boardShift.y;
                        initializeSCREEN(back, instr_placement);
                        initializeBoardScreen(bbshiftsize, virtual_board, board_pos);
                        displayPOSITION(instr_placement, {coords.x+boardShift.x, coords.y+boardShift.y}, board_pos);
                        displaySCORE(instr_placement, score);
                        gotoxy(coords.x, coords.y);
                    }
                    else
                    {
                        printSTONE(coords,turn);
                        displayPOSITION(instr_placement, {coords.x+boardShift.x, coords.y+boardShift.y}, board_pos);
                    }
                }
                else if(zn == KEY_PLACE_STONE && handicapped == 1)
                {
                    copyBoard(virtual_board, prevBoardState.onemoveback, boardSize[board_size_option]);
                    virtual_board[(coords.x+boardShift.x-min.x)/2][coords.y+boardShift.y-min.y] = turn;
                    handCount++;
                }
                else if(zn == KEY_CANCEL && handicapped == 1)
                {
                    copyBoard(prevBoardState.onemoveback, virtual_board, boardSize[board_size_option]);
                    // initialize board screen
                    bbshiftsize.board_size = boardSize[board_size_option];
                    bbshiftsize.boardshift.x = boardShift.x; bbshiftsize.boardshift.y = boardShift.y;
                    initializeSCREEN(back, instr_placement);
                    initializeBoardScreen(bbshiftsize, virtual_board, board_pos);
                    displayPOSITION(instr_placement, {coords.x+boardShift.x, coords.y+boardShift.y}, board_pos);
                    displaySCORE(instr_placement, score);
                    gotoxy(coords.x, coords.y);
                    //
                    handCount--;
                }
                else if(zn == KEY_CONFIRM && handicapped == 1)
                {
                    handicapped = 0;
                    turn = WHITEPLAYERTURN;
                }
                else if(zn == KEY_PLACE_STONE && handicapped == 0){
                    if(checkIfpossiblePlacement({(coords.x+boardShift.x-min.x)/2, coords.y+boardShift.y-min.y}, boardSize[board_size_option], virtual_board, turn))
                    {
                        virtual_board[(coords.x+boardShift.x-min.x)/2][coords.y+boardShift.y-min.y] = turn;
                        COORDS_t * checkCaptured = findCaptured({(coords.x+boardShift.x-min.x)/2, coords.y+boardShift.y-min.y}, boardSize[board_size_option], virtual_board, turn);
                        if((compareBoards(virtual_board, prevBoardState.twomoveback, boardSize[board_size_option]) || compareBoards(virtual_board, prevBoardState.onemoveback, boardSize[board_size_option])) && (checkCaptured[0].x == -1 || checkCaptured[1].x == -1))
                        {
                            virtual_board[(coords.x+boardShift.x-min.x)/2][coords.y+boardShift.y-min.y] = CLEARTURN;
                        }
                        else
                        {
                            copyBoard(prevBoardState.onemoveback, prevBoardState.twomoveback, boardSize[board_size_option]);
                            copyBoard(virtual_board, prevBoardState.onemoveback, boardSize[board_size_option]);
                            int i_check = 0;
                            while(checkCaptured[i_check].x!=-1)
                            {
                                virtual_board[checkCaptured[i_check].x][checkCaptured[i_check].y] = CLEARTURN;
                                i_check++;
                                if(turn == WHITEPLAYERTURN) score.white++;
                                else if (turn == BLACKPLAYERTURN) score.black++;
                            }
                            if(checkCaptured[0].x!=-1)
                            {
                                bbshiftsize.board_size = boardSize[board_size_option];
                                bbshiftsize.boardshift.x = boardShift.x; bbshiftsize.boardshift.y = boardShift.y;
                                initializeSCREEN(back, instr_placement);
                                initializeBoardScreen(bbshiftsize, virtual_board, board_pos);
                                displayPOSITION(instr_placement, {coords.x+boardShift.x, coords.y+boardShift.y}, board_pos);
                            }
                            free(checkCaptured);
                            if (turn == WHITEPLAYERTURN) turn = BLACKPLAYERTURN;
                            else turn = WHITEPLAYERTURN;
                            gotoxy(coords.x,coords.y);
                            displaySCORE(instr_placement, score);
                        }
                    }
                }
                else if(zn == HELP_KEY_h)
                {
                    printInstruction(instr_placement);
                    getch();
                    printHELP(instr_placement);
                    displaySCORE(instr_placement, score);
                    displayPOSITION(instr_placement, {coords.x+boardShift.x, coords.y+boardShift.y}, board_pos);
                }
                else if(zn == KEY_SAVE)
                {
                    SAVE_t saveF;
                    saveF.board_size = boardSize[board_size_option];
                    saveF.coords.x = coords.x; saveF.coords.y = coords.y;
                    saveF.score.black = score.black; saveF.score.white = score.white;
                    saveF.turn = turn;
                    saveF.handicapped = handicapped;
                    saveF.handCount = handCount;
                    saveF.boardshift.x = boardShift.x; saveF.boardshift.y = boardShift.y;
                    saveFile(saveF, virtual_board, prevBoardState, startGame_instr);
                }
                else if(zn == KEY_LOAD)
                {
                    SAVE_t loadF;
                    loadF = loadFile(virtual_board, prevBoardState, startGame_instr);
                    if(loadF.board_size != -1)
                    {
                        deleteBoard(virtual_board, boardSize[board_size_option]);
                        deleteBoard(prevBoardState.onemoveback, boardSize[board_size_option]);
                        deleteBoard(prevBoardState.twomoveback, boardSize[board_size_option]);
                        board_size_option = 3;
                        boardSize[board_size_option] = loadF.board_size;
                        virtual_board = clearBOARD(boardSize[board_size_option]);
                        prevBoardState.onemoveback = clearBOARD(boardSize[board_size_option]);
                        prevBoardState.twomoveback = clearBOARD(boardSize[board_size_option]);
                        copyBoard(loadF.board,virtual_board, boardSize[board_size_option]); deleteBoard(loadF.board,boardSize[board_size_option]);
                        copyBoard(loadF.prevboard.onemoveback,prevBoardState.onemoveback, boardSize[board_size_option]); deleteBoard(loadF.prevboard.onemoveback,boardSize[board_size_option]);
                        copyBoard(loadF.prevboard.twomoveback,prevBoardState.twomoveback, boardSize[board_size_option]); deleteBoard(loadF.prevboard.twomoveback,boardSize[board_size_option]);
                        coords.x = loadF.coords.x; coords.y = loadF.coords.y;
                        score.black = loadF.score.black; score.white = loadF.score.white;
                        turn = loadF.turn;
                        handicapped = loadF.handicapped;
                        handCount = loadF.handCount;
                        boardShift.x = loadF.boardshift.x; boardShift.y = loadF.boardshift.y;
                        max = { board_pos.x+(boardSize[board_size_option]*2), board_pos.y+boardSize[board_size_option]};
                    }
                    else
                    {
                        loadF.board_size = boardSize[board_size_option];
                        loadF.coords.x = coords.x; loadF.coords.y = coords.y;
                        loadF.score.black = score.black; loadF.score.white = score.white;
                        loadF.turn = turn;
                        loadF.handicapped = handicapped;
                        loadF.handCount = handCount;
                        loadF.boardshift.x = boardShift.x; loadF.boardshift.y = boardShift.y;
                    }
                    bbshiftsize.board_size = loadF.board_size;
                    bbshiftsize.boardshift.x = loadF.boardshift.x; bbshiftsize.boardshift.y = loadF.boardshift.y;
                    initializeSCREEN(back, instr_placement);
                    initializeBoardScreen(bbshiftsize, virtual_board, board_pos);
                    displayPOSITION(instr_placement, {coords.x+boardShift.x, coords.y+boardShift.y}, board_pos);
                    displaySCORE(instr_placement, score);
                    gotoxy(coords.x, coords.y);
                }
                else if(zn == KEY_FINISH)
                {
                    char comment[30];
                    FINALSCORE_t finalScore;
                    finalScore = countScore(score, handCount, virtual_board, boardSize[board_size_option]);
                    initializeSCREEN(back, instr_placement);
                    gotoxy(startGame_instr.x, startGame_instr.y+2);
                    cputs("Final Score:");
                    gotoxy(startGame_instr.x, startGame_instr.y+3);
                    sprintf(comment,"Black: %0.1f, White: %0.1f", finalScore.black, finalScore.white);
                    cputs(comment);
                    getch();
                }
            }
        }

	} while (zn != KEY_Q);

    deleteBoard(virtual_board, boardSize[board_size_option]);
    deleteBoard(prevBoardState.onemoveback, boardSize[board_size_option]);
    deleteBoard(prevBoardState.twomoveback, boardSize[board_size_option]);
	_setcursortype(_NORMALCURSOR);
	return 0;
}

int changeBackground(int back){
    back = (back + 1) % 16; /* 16 diff colors */
    if(back == WHITE) return BLUE;
    else if(back == LIGHTGRAY) return LIGHTBLUE;
    return back;
}

int **clearBOARD(int board_size){
    int **arr = (int **)malloc(sizeof(int*)*board_size);
    for(int i = 0; i<board_size; i++){
        arr[i] = (int*)malloc(sizeof(int)*board_size); 
        for(int j=0; j<board_size; j++){
            arr[i][j] = 0;
        }
    }
    return arr;
}

void deleteBoard (int **board, int size)
{
    for(int i = 0; i < size; i++) free(board[i]);
    free(board);
}

COORDS_t moveAroundBoard (COORDS_t coords, COORDS_t max, COORDS_t min, int zn){
	if(zn == KEY_UP) coords.y--;	
	else if(zn == KEY_DOWN) coords.y++;
	else if(zn == KEY_LEFT) coords.x-=2;
	else if(zn == KEY_RIGHT) coords.x+=2;

	return coords;
}

void printSTONE(COORDS_t coords, int turn){
    if(turn == 0){
        gotoxy(coords.x,coords.y);
        cputs(" "); 
    }
    else{
        if(turn == 1) textcolor(WHITEPLAYER_COLOR);
        else textcolor(BLACKPLAYER_COLOR);
        gotoxy(coords.x,coords.y);
        cputs(STONE);
        textcolor(INITIAL_TEXT_COLOR);
    }
}

int checkLiberties(const COORDS_t coords, const int max, int **vir_board, int r_turn) /* returns 0 if no liberties, returns 1 if there are liberties */
{
	int check = 0;
	if(coords.x+1<=max-1) {if(vir_board[coords.x+1][coords.y] != CLEARTURN) check++;}
	else check ++;
	if(coords.x-1>=0) {if(vir_board[coords.x-1][coords.y] != CLEARTURN) check++;}
	else check ++;
	if(coords.y+1<=max-1) {if(vir_board[coords.x][coords.y+1] != CLEARTURN) check++;}
	else check ++;
	if(coords.y-1>=0) {if(vir_board[coords.x][coords.y-1] != CLEARTURN) check++;}
	else check ++;
		
	if(check == 4) return 0;
	return 1;
}

int checkIfNotIn(COORDS_t * tab1, COORDS_t coords, int tab1Flag)
{
    for(int i=0; i<tab1Flag; i++)
    {
        if(tab1[i].x == coords.x && tab1[i].y == coords.y) return 0;
    }
    return 1; /* if does not extist */
}

int checkIfNotExistInTwoArrays(COORDS_t * tab1, COORDS_t * tab2, COORDS_t coords, int tab1Flag, int tab2Flag)
{
    int check = 0;
    check = checkIfNotIn(tab1, coords, tab1Flag);
    if(check == 0) return 0;
    check = checkIfNotIn(tab2, coords, tab2Flag);
    return check;
}

COORDS_t * findCaptured(COORDS_t coords, int max, int **vir_board, int turn)
{
	int r_turn = 0;
	COORDS_t neighbours[4] = {{coords.x+1,coords.y}, {coords.x-1,coords.y}, {coords.x,coords.y+1}, {coords.x,coords.y-1}};
	COORDS_t * captured = (COORDS_t *)malloc(sizeof(COORDS_t*)*(max*max+1));
	captured[0] = {-1, -1}; /* nothing */
    int capturedFlag = 0;
    if(turn == WHITEPLAYERTURN) r_turn = BLACKPLAYERTURN;
    else r_turn = WHITEPLAYERTURN;
    for (int i = 0; i<4; i++)
    { 
        if((neighbours[i].x<=max-1) && (neighbours[i].y<=max-1) && (neighbours[i].x>=0) && (neighbours[i].y>=0))
        {
            if (vir_board[neighbours[i].x][neighbours[i].y] == r_turn && (checkIfNotIn(captured, neighbours[i],capturedFlag)))
            {
                COORDS_t * checked = (COORDS_t *)malloc(sizeof(COORDS_t*)*(max*max+1));
                COORDS_t currentCoords = {neighbours[i].x, neighbours[i].y};
                int liberties = 0, currentCheck = 0, currentCapturedFlag = capturedFlag;
                checked[currentCheck] = {currentCoords.x, currentCoords.y}; currentCheck++;
                checked[currentCheck] = {-1, -1};
                while(liberties == 0 && (checked[0].x!=-1))
                {
                    liberties = checkLiberties(currentCoords, max, vir_board, r_turn);
                    if(currentCheck!=0) currentCheck--; checked[currentCheck] = {-1,-1};
                    if(liberties == 0)
                    {
                        if(checkIfNotIn(captured, currentCoords,capturedFlag)) {captured[capturedFlag] = currentCoords; capturedFlag++; captured[capturedFlag]={-1,-1};} /*check if in captured and check if in checked and check if there is a diff*/
                        if(currentCoords.x+1<=max-1) { if(vir_board[currentCoords.x+1][currentCoords.y] == r_turn)
                        {
                            if(checkIfNotExistInTwoArrays(checked, captured, {currentCoords.x+1, currentCoords.y}, currentCheck, capturedFlag))
                            {
                                checked[currentCheck] = {currentCoords.x+1, currentCoords.y}; currentCheck++; checked[currentCheck] = {-1,-1};
                            }
                        } }
                        if(currentCoords.x-1>=0) { if(vir_board[currentCoords.x-1][currentCoords.y] == r_turn)
                        {
                            if(checkIfNotExistInTwoArrays(checked, captured, {currentCoords.x-1, currentCoords.y}, currentCheck, capturedFlag))
                            {
                                checked[currentCheck] = {currentCoords.x-1, currentCoords.y}; currentCheck++; checked[currentCheck] = {-1,-1};
                            }
                        } }
                        if(currentCoords.y+1<=max-1) { if(vir_board[currentCoords.x][currentCoords.y+1] == r_turn)
                        {
                            if(checkIfNotExistInTwoArrays(checked, captured, {currentCoords.x, currentCoords.y+1}, currentCheck, capturedFlag))
                            {
                                checked[currentCheck] = {currentCoords.x, currentCoords.y+1}; currentCheck++; checked[currentCheck] = {-1,-1};
                            }
                        } }
                        if(currentCoords.y-1>=0) { if(vir_board[currentCoords.x][currentCoords.y-1] == r_turn)
                        {
                            if(checkIfNotExistInTwoArrays(checked, captured, {currentCoords.x, currentCoords.y-1}, currentCheck, capturedFlag))
                            {
                                checked[currentCheck] = {currentCoords.x, currentCoords.y-1}; currentCheck++; checked[currentCheck] = {-1,-1};
                            }
                        } }
                        if(currentCheck !=0) currentCoords = checked[currentCheck-1];
                    }
                }
                if(liberties == 1)
                {
                    for(int ij = currentCapturedFlag ; ij< capturedFlag; ij++) captured[ij] = {-1, -1};
                    capturedFlag = currentCapturedFlag;
                }
                free(checked);
            }
        }
    }
	return captured;
}

int CheckIfExtistChainLiberties(COORDS_t coords, int max, int **vir_board, int turn)
{
    int r_turn;
    if(turn == WHITEPLAYERTURN) r_turn = BLACKPLAYERTURN;
    else r_turn = WHITEPLAYERTURN;

    COORDS_t * checked = (COORDS_t *)malloc(sizeof(COORDS_t*)*(max*max+1));
    COORDS_t * afterChecked = (COORDS_t *)malloc(sizeof(COORDS_t*)*(max*max+1));
    COORDS_t currentCoords = {coords.x, coords.y};
    int liberties = 0, currentCheck = 0, afterCheckedFlag = 0;
    afterChecked[afterCheckedFlag] = {-1, -1};
    checked[currentCheck] = {currentCoords.x, currentCoords.y}; currentCheck++;
    checked[currentCheck] = {-1, -1};

    while(liberties == 0 && checked[0].x!=-1)
    {
        liberties = checkLiberties(currentCoords, max, vir_board, turn);
        if(currentCheck!=0) currentCheck--; checked[currentCheck] = {-1,-1};
        if(liberties == 0)
        {
            if(checkIfNotIn(afterChecked, currentCoords, afterCheckedFlag)) {afterChecked[afterCheckedFlag] = currentCoords; afterCheckedFlag++; afterChecked[afterCheckedFlag]={-1,-1};}
            if(currentCoords.x+1<=max-1) {if(vir_board[currentCoords.x+1][currentCoords.y] == turn)
            {
                if(checkIfNotExistInTwoArrays(checked, afterChecked, {currentCoords.x+1, currentCoords.y}, currentCheck, afterCheckedFlag))
                {
                    checked[currentCheck] = {currentCoords.x+1, currentCoords.y}; currentCheck++; checked[currentCheck] = {-1,-1};
                }
            } }
            if(currentCoords.x-1>=0) {if(vir_board[currentCoords.x-1][currentCoords.y] == turn)
            {
                if(checkIfNotExistInTwoArrays(checked, afterChecked, {currentCoords.x-1, currentCoords.y}, currentCheck, afterCheckedFlag))
                {
                    checked[currentCheck] = {currentCoords.x-1, currentCoords.y}; currentCheck++; checked[currentCheck] = {-1,-1};
                }
            } }
            if(currentCoords.y+1<=max-1) {if(vir_board[currentCoords.x][currentCoords.y+1] == turn)
            {
                if(checkIfNotExistInTwoArrays(checked, afterChecked, {currentCoords.x, currentCoords.y+1}, currentCheck, afterCheckedFlag))
                {
                    checked[currentCheck] = {currentCoords.x, currentCoords.y+1}; currentCheck++; checked[currentCheck] = {-1,-1};
                }
            } }
            if(currentCoords.y-1>=0) {if(vir_board[currentCoords.x][currentCoords.y-1] == turn)
            {
                if(checkIfNotExistInTwoArrays(checked, afterChecked, {currentCoords.x, currentCoords.y-1}, currentCheck, afterCheckedFlag))
                {
                    checked[currentCheck] = {currentCoords.x, currentCoords.y-1}; currentCheck++; checked[currentCheck] = {-1,-1};
                }
            } }
            if(currentCheck !=0) currentCoords = checked[currentCheck-1];
        }
    }
    free(checked);
    free(afterChecked);
    return liberties;
}

int checkIfpossiblePlacement(COORDS_t coords, int max, int **vir_board, int turn){
    if(vir_board[coords.x][coords.y]!=CLEARTURN) return 0;

    if(coords.x+1<=max-1) {if(vir_board[coords.x+1][coords.y] == CLEARTURN) return 1;}
    if(coords.x-1>=0) {if(vir_board[coords.x-1][coords.y] == CLEARTURN) return 1;}
    if(coords.y+1<=max-1) {if(vir_board[coords.x][coords.y+1] == CLEARTURN) return 1;}
    if(coords.y-1>=0) {if(vir_board[coords.x][coords.y-1] == CLEARTURN) return 1;}

    vir_board[coords.x][coords.y] = turn;
    int liberties = CheckIfExtistChainLiberties(coords, max, vir_board, turn);
    vir_board[coords.x][coords.y] = CLEARTURN;
    if(liberties == 1) return 1;

    COORDS_t * findPlacement;
    vir_board[coords.x][coords.y] = turn;
    findPlacement = findCaptured(coords, max, vir_board, turn);
    vir_board[coords.x][coords.y] = CLEARTURN;
    if(findPlacement[0].x!=-1) return 1;

    return 0;
}

FINALSCORE_t countScore(SCORE_t score, int handicappedCount, int **vir_board, int size){
    FINALSCORE_t finalScore = {(float)score.white, (float)score.black};

    if(handicappedCount >1) finalScore.white+=0.5;
    else finalScore.white+=6.5;
    return finalScore;
}

void copyBoard(int **source, int **target, int size)
{
    for(int i=0; i<size; i++)
    {
        for(int j=0; j<size; j++)
        {
            target[i][j] = source[i][j];
        }
    }
}

/* returns 1 if are the same, returns 0 if are different */
int compareBoards(int **firstBoard, int **secondBoard, int size){
    for (int i= 0; i<size; i++)
    {
        for(int j = 0; j<size; j++)
        {
            if(firstBoard[i][j]!=secondBoard[i][j]) return 0;
        }
    }
    return 1;
}

int getSizeOfBoard(COORDS_t coords)
{
    char zn = 't',
     zn_2[2];
    int numberOfDecimals = 0, numberOutput = 0, input = 0;
    gotoxy(coords.x,coords.y);
    cputs("Board size:");
    gotoxy(coords.x,coords.y+1);
    while(zn != KEY_CONFIRM)
    {
        zn = getch();
        if(zn == KEY_DELETE_1 || zn == KEY_DELETE_2)
        {
            if(numberOfDecimals>0)
            {
                numberOutput = (numberOutput-input)/10;
                gotoxy(wherex()-1,wherey());
                cputs(" ");
                gotoxy(wherex()-1,wherey());
                numberOfDecimals--;
            }
        }
        else if ( zn >= '0' && zn <= '9' )
        {
            sprintf(zn_2,"%c",zn);
            sscanf(zn_2, "%d", &input);
            numberOutput = (numberOutput*10)+input;
            cputs(zn_2);
            numberOfDecimals++;
        }
    }
    if(numberOutput == 0) return 9;
    return numberOutput;
}

char * fileName(COORDS_t coords){
    static char filename[NUMBER_OF_CHARACTERS_FILE_NAME+5] = "";
    char zn=']', zn_2[2];
    int numberOfCharacters = 0;
    sprintf(filename,"");

    gotoxy(coords.x,coords.y);
    cputs("File name: ");
    gotoxy(coords.x, coords.y+1);
    while( zn != KEY_CONFIRM && numberOfCharacters < NUMBER_OF_CHARACTERS_FILE_NAME)
    {
        zn = getch();
        if (zn == KEY_DELETE_1 || zn == KEY_DELETE_2)
        {
            if(numberOfCharacters>0)
            {
                numberOfCharacters--;
                filename[numberOfCharacters]='\0';
                gotoxy(wherex()-1,wherey());
                cputs(" ");
                gotoxy(wherex()-1,wherey());
            }
            if(numberOfCharacters == 0) sprintf(filename,"");
        }
        else if (( zn >= '0' && zn <= '9' ) || ( zn >= 'A' && zn <= 'Z' ) || ( zn >= 'a' && zn <= 'z' ) || zn == '_' || zn == '-' || zn == '.')
        {
            sprintf(filename,"%s%c",filename,zn);
            sprintf(zn_2,"%c",zn);
            cputs(zn_2);
            numberOfCharacters++;
        }
    }
    if(filename[0] == '\0')
    {
        sprintf(filename, "default.txt");
    }
    else sprintf(filename, "%s.txt",filename);
    gotoxy(coords.x,coords.y);
    cputs("           ");
    gotoxy(coords.x,coords.y+1);
    for(int i = 0; i<numberOfCharacters; i++) cputs(" ");
    return filename;
}

void initializeBoardScreen(INITIALIZATION_t loadfile, int **vir_board, COORDS_t coords){ 
    COORDS_t screenSize = getSizeOfScreen();
    COORDS_t printBoardSize = {0, 0};
    if(coords.x+(loadfile.board_size*2)>screenSize.x) printBoardSize.x = (screenSize.x - coords.x)/2;
    else printBoardSize.x = loadfile.board_size;
    if(coords.y+loadfile.board_size>screenSize.y) printBoardSize.y = screenSize.y - coords.y - 1;
    else printBoardSize.y = loadfile.board_size;

    for(int i = 0; i<printBoardSize.x; i++)
    {
        for(int j = 0; j<printBoardSize.y; j++)
        {
            printSTONE({coords.x+1+(i*2), coords.y+1+j},vir_board[i+(loadfile.boardshift.x/2)][j+loadfile.boardshift.y]);
        }
    }
    printBOARD(coords, loadfile.boardshift, loadfile.board_size);
}

void saveFile(SAVE_t savefile, int **vir_board, BOARDSTATES_t prevBoardState, COORDS_t coords){
    char * filename = fileName(coords);
    FILE * fileTXT = fopen(filename, "w+");

    if (fileTXT == NULL) perror("Failed to open file: ");
    else
    {
        fprintf(fileTXT,"%d\n", savefile.board_size);
        for(int i=0; i<savefile.board_size;i++)
        {
            for(int j=0; j<savefile.board_size;j++)
            {
                fprintf(fileTXT, "%d", vir_board[i][j]);
            }
            fputs("\n",fileTXT);
        }
        for(int i=0; i<savefile.board_size;i++)
        {
            for(int j=0; j<savefile.board_size;j++)
            {
                fprintf(fileTXT, "%d", prevBoardState.onemoveback[i][j]);
            }
            fputs("\n",fileTXT);
        }
        for(int i=0; i<savefile.board_size;i++)
        {
            for(int j=0; j<savefile.board_size;j++)
            {
                fprintf(fileTXT, "%d", prevBoardState.twomoveback[i][j]);
            }
            fputs("\n",fileTXT);
        }
        fprintf(fileTXT, "%d %d\n", savefile.score.black, savefile.score.white);
        fprintf(fileTXT,"%d %d\n",savefile.coords.x,savefile.coords.y);
        fprintf(fileTXT,"%d\n", savefile.turn);
        fprintf(fileTXT,"%d\n", savefile.handicapped);
        fprintf(fileTXT,"%d\n", savefile.handCount);
        fprintf(fileTXT,"%d %d\n", savefile.boardshift.x, savefile.boardshift.y);
        fclose(fileTXT);
    }
}

SAVE_t loadFile(int **vir_board, BOARDSTATES_t prevBoardState, COORDS_t coords){
    SAVE_t loadfile;
    char * filename = fileName(coords);
    FILE * fileTXT = fopen(filename, "r");
    char c, c_2[2]; int number, size, i_var;

    if (fileTXT == NULL)
    {
        loadfile.board_size = -1;
        perror("Failed to open file: ");
    }
    else
    {
        fscanf(fileTXT,"%d", &size);
        loadfile.board_size = size;
        fgetc(fileTXT);
        loadfile.board = clearBOARD(size);
        loadfile.prevboard.onemoveback = clearBOARD(size);
        loadfile.prevboard.twomoveback = clearBOARD(size);

        for(int i = 0; i< size; i++)
        {
            for(int j = 0; j<size; j++)
            {
                c = fgetc(fileTXT);
                sprintf(c_2,"%c",c);
                sscanf(c_2, "%d", &number); //changing char to int
                loadfile.board[i][j] = number;
            }
            fgetc(fileTXT);
        }
        for(int i = 0; i< size; i++)
        {
            for(int j = 0; j<size; j++)
            {
                c = fgetc(fileTXT);
                sprintf(c_2,"%c",c);
                sscanf(c_2, "%d", &number); //changing char to int
                loadfile.prevboard.onemoveback[i][j] = number;
            }
            fgetc(fileTXT);
        }
        for(int i = 0; i< size; i++)
        {
            for(int j = 0; j<size; j++)
            {
                c = fgetc(fileTXT);
                sprintf(c_2,"%c",c);
                sscanf(c_2, "%d", &number); //changing char to int
                loadfile.prevboard.twomoveback[i][j] = number;
            }
            fgetc(fileTXT);
        }
        fscanf(fileTXT,"%d %d %d %d %d %d %d %d %d", &loadfile.score.black, &loadfile.score.white, &loadfile.coords.x , &loadfile.coords.y, &loadfile.turn, &loadfile.handicapped, &loadfile.handCount, &loadfile.boardshift.x, &loadfile.boardshift.y);
        fclose(fileTXT);
    }
    return loadfile;
}

int chooseBOARDoption(COORDS_t coords){
    printBOARDoptions(coords);
    int zn = getch();
    while(!(zn == BOARDSIZE_OPTION_1 || zn == BOARDSIZE_OPTION_2 || zn == BOARDSIZE_OPTION_3 || zn == BOARDSIZE_OPTION_4 )) zn = getch();
    clearBOARDoptions(coords);
    if (zn == BOARDSIZE_OPTION_1) return 0;
    else if (zn == BOARDSIZE_OPTION_2) return 1;
    else if (zn == BOARDSIZE_OPTION_3) return 2;
	return 3;
}
void printBOARDoptions(COORDS_t coords){
    gotoxy(coords.x, coords.y);
    cputs("1 = board size: 9x9  ");
    gotoxy(coords.x, coords.y+1);
    cputs("2 = board size: 13x13");
    gotoxy(coords.x, coords.y+2);
    cputs("3 = board size: 19x19"); 
	gotoxy(coords.x, coords.y+3);
	cputs("4 = non-standard size");
}
void clearBOARDoptions(COORDS_t coords){
	for(int i = coords.y; i<=coords.y+3; i++)
	{
		gotoxy(coords.x, i);
    	cputs("                                     ");
	} 
}
void printInstruction(COORDS_t coords){
    gotoxy(coords.x, coords.y);
    cputs("q       = exit                                ");
    gotoxy(coords.x, coords.y+1);
    cputs("cursors = moving                              ");
    gotoxy(coords.x, coords.y+2);
    cputs("n       = new game                            ");
    gotoxy(coords.x, coords.y+3);
    cputs("enter   = confirm choice and end player's turn");
    gotoxy(coords.x, coords.y+4);
    cputs("esc     = cancel current action               ");
    gotoxy(coords.x, coords.y+5);
    cputs("i       = place a stone on the board          ");
    gotoxy(coords.x, coords.y+6);
    cputs("s       = save the game state                 ");
    gotoxy(coords.x, coords.y+7);
    cputs("l       = load the game state                 ");
    gotoxy(coords.x, coords.y+8);
    cputs("f       = finish the game                     ");
    gotoxy(coords.x, coords.y+9);
    cputs("-- Press any key to exit help --               ");
}
void printHELP(COORDS_t coords){
    gotoxy(coords.x, coords.y);
    cputs("h = help                                      ");
    gotoxy(coords.x,coords.y+1);
    cputs("Oliwer Paszkiewicz, 187583                    ");
	for(int i=coords.y+2; i<=coords.y+9; i++)
	{
		gotoxy(coords.x,i);
    	cputs("                                              ");
	}
}
void printWRONGKEY(COORDS_t coords){
    gotoxy(coords.x, coords.y);
    cputs("Press n to start a new game, press h to learn controls :)");
    getch();
    gotoxy(coords.x,coords.y);
    cputs("                                                         ");
}

COORDS_t getSizeOfScreen()
{
    COORDS_t screenSize = {0, 0};
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    screenSize.x = csbi.srWindow.Right - csbi.srWindow.Left+1;
    screenSize.y = csbi.srWindow.Bottom - csbi.srWindow.Top+1;
    return screenSize;
}

void printBOARD(COORDS_t coords, COORDS_t start, int size){
    COORDS_t screenSize = getSizeOfScreen();
    int numberOfColumns = (size*2)+1;
    int numberOfRows = size+2;
    if(numberOfColumns>(screenSize.x-coords.x)) numberOfColumns = screenSize.x-coords.x;
    if(numberOfRows>(screenSize.y-coords.y)) numberOfRows = screenSize.y-coords.y;

    for(int i = 0; i<numberOfColumns; i++)
    {
        if(start.y==0)
        {
            gotoxy(coords.x+i,coords.y);
            cputs("#");
        }
        if((coords.y+size+1-start.y)<=screenSize.y)
        {
            if((coords.y+size+1)<screenSize.y) {gotoxy(coords.x+i,coords.y+size+1); cputs("#");}
            else { gotoxy(coords.x+i,screenSize.y); cputs("#");}
        }
    }
    for(int j = 0; j<numberOfRows; j++)
    {
        if(start.x==0)
        {
            gotoxy(coords.x,coords.y+j);
            cputs("#");
        }
        if((coords.x+(size*2)-start.x)<=screenSize.x)
        {
            if((coords.x+(size*2))<screenSize.x) { gotoxy(coords.x+(size*2),coords.y+j); cputs("#");}
            else { gotoxy(screenSize.x,coords.y+j); cputs("#"); }
        }
    }
}

void displaySCORE(COORDS_t coords, SCORE_t score){
    char score_p[36];
    sprintf(score_p, "White player score: %d ", score.white);
    gotoxy(coords.x,coords.y+2);
    cputs(score_p);
    sprintf(score_p, "Black player score: %d ", score.black);
    gotoxy(coords.x,coords.y+3);
    cputs(score_p);
}
void displayPOSITION(COORDS_t coords, COORDS_t pos, const COORDS_t board){
    char position[26];
    gotoxy(coords.x,coords.y+4);
    sprintf(position, "Position: %d %d  ", ((pos.x - board.x)+1)/2, pos.y - board.y);
    cputs(position);
}
void initializeSCREEN(int background, COORDS_t coords){
		textbackground(background);
		clrscr();
		textcolor(INITIAL_TEXT_COLOR);
        printHELP(coords);	
}
