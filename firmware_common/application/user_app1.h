/*!*********************************************************************************************************************
@file user_app1.h                                                                
@brief Header file for user_app1

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
1. Follow the instructions at the top of user_app1.c
2. Use ctrl-h to find and replace all instances of "user_app1" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

**********************************************************************************************************************/

#ifndef __USER_APP1_H
#define __USER_APP1_H

/* Type Definitions */
typedef enum {
    RESULT_NONE = 0,
    RESULT_CHECKMATE = 1,
    RESULT_RESIGNATION = 2,
    RESULT_DRAW = 3
} GameResult;

typedef enum {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
} MovementDirection;

typedef enum {
    BLACK = 0,
    WHITE = 1,
} Colour;

typedef struct {
    int8_t row;
    int8_t col;
} Square;

/* Function Declarations */
/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void UserApp1Initialize(void);
void UserApp1RunActiveState(void);

/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/* State Machine Declarations */
static void Chess_Menu(void);
static void Chess_Game_Selecting_Direction(void);
static void Chess_Game_Selecting_Square(void);
static void Chess_Game_Debug(void);
static void Chess_Game_Run_Test(void);
static void Chess_Game_Lift_Piece(void);
static void Chess_Game_Place_Piece(void);
static void Chess_Game_Update_Player_Symbols(void);
static void UserApp1SM_Error(void);

/* Macros */
#define IS_SQUARE_NULL(s) ((s).row == -1)
#define ARE_SQUARES_EQUAL(s1, s2) ((s1).row == (s2).row && (s1).col == (s2).col)

/* Constants / Definitions */
#define SQUARE_PIXEL_SIZE (u8)7
#define BOARD_BOTTOM_LEFT_PIXEL_ROW (u8)59
#define BOARD_BOTTOM_LEFT_PIXEL_COL (u8)35
#define DISPLAY_ROW_DIRECTION (int8_t)-1
#define DISPLAY_COL_DIRECTION (u8)1
#define SQUARE_FLASH_RATE (u16)500
#define ARROW_FLASH_RATE (u16)500
#define DRAW_TIMER_MAX 250
#define PLAYER_SYMBOL_START_COL (u8)5
#define WHITE_PLAYER_SYMBOL_WIDTH (u8)23
#define BLACK_PLAYER_SYMBOL_WIDTH (u8)25
#define WHITE_PLAYER_SYMBOL_START_ROW (u8)56
#define BLACK_PLAYER_SYMBOL_START_ROW (u8)4
#define CUSTOM_FONT_HEIGHT (u8)5
#define ARROW_SIZE (u8)10
#define ARROW_START_ROW (u8)28
#define ARROW_START_COL (u8)102
#define DIRECTION_OPTIONS (u8)4
#define BOARD_SIZE (u8)8
#define EMPTY_SQUARE (u8)0
#define WHITE_PAWN (u8)1
#define WHITE_KNIGHT (u8)2
#define WHITE_BISHOP (u8)3
#define WHITE_ROOK (u8)4
#define WHITE_QUEEN (u8)5
#define WHITE_KING (u8)6
#define BLACK_PAWN (u8)7
#define BLACK_KNIGHT (u8)8
#define BLACK_BISHOP (u8)9
#define BLACK_ROOK (u8)10
#define BLACK_QUEEN (u8)11
#define BLACK_KING (u8)12

#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5
#define G 6
#define H 7

#define ONE 7
#define TWO 6
#define THREE 5
#define FOUR 4
#define FIVE 3
#define SIX 2
#define SEVEN 1
#define EIGHT 0

#endif /* __USER_APP1_H */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
