#ifndef __CHESS_APP_H
#define __CHESS_APP_H

/* Includes */
#include "configuration.h"
#include <lcd_NHD-C12864LZ.h>
#include <mpgl2-ehdw-02.h>

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

typedef struct {
    Square start;
    Square end;
} Move;

typedef struct Node {
    fnCode_type data;
    // uint8_t priority;
    struct Node* next;
} Node;

typedef struct Queue {
    Node *front, *rear;
} Queue;

/* Function Declarations */
/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void ChessAppInitialize(void);
void ChessAppRunActiveState(void);

/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/* State Machine Declarations */
static void Chess_Menu(void);
static void Chess_Game_Selecting_Direction(void);
static void Chess_Game_Selecting_Square(void);
static void Chess_Game_Lift_Piece(void);
static void Chess_Game_Place_Piece(void);
static void Chess_Game_Update_Player_Symbols(void);
static void Chess_Game_Flash_Highlighted_Square(void);
static void Chess_Game_Flash_Movement_Indicator(void);
static void Chess_Game_Move_Highlighted_Square_Part_1(void);
static void Chess_Game_Move_Highlighted_Square_Part_2(void);
static void Chess_Game_Attempt_Move(void);
static void Chess_Game_Validate_Piece_Movement(void);
static void Chess_Game_Validate_Check_Status(void);
static void Chess_Game_Deselect_Selected_Square(void);
static void Chess_Game_Change_Movement_Direction(void);
static void Chess_Game_Find_Valid_Move(void);
static void Chess_Game_Close_Movement_Direction_Menu(void);
static void Chess_Game_Selecting_Square(void);
static void ChessAppSM_Error(void);

/* chess_utils.c */
void print_str (u8 str[]);
void print_str_var (u8 name[], u8 val[]);
void print_int_var (u8 name[], u32 val);

/* chess_lcd.c */
void flash_arrow(MovementDirection direction);
void draw_movement_direction(MovementDirection direction);
PixelAddressType get_square_pixel_coordinates(uint8_t rank_index, uint8_t file_index);
void draw_player_symbols(Colour turn);
void draw_turn_symbol();
void draw_movement_symbol();
void draw_menu(GameResult previous_result, Colour turn);
void draw_line(int start_row, int start_col, int length, int row_increment, int col_increment);
void colour_square(int square_row, int square_col);
void draw_selected_square_indicator(Square square);
void draw_piece(uint8_t piece, uint8_t rank, uint8_t file);
void highlight_square(Square square, uint8_t content, bool highlight_selected);
void draw_square(Square square, uint8_t content);
void draw_pieces();
void draw_board();
void draw_game_interface();

/* chess_game.c */
Move get_move_to_make();
void set_move_to_make(Move move);
Square get_white_king_square();
void set_white_king_square(Square new_square);
Square get_black_king_square();
void set_black_king_square(Square new_square);
int8_t* get_direction_option(MovementDirection direction_option);
void set_attempting_special_pawn_move(bool new_status);
bool get_attempting_special_pawn_move();
void set_attempting_short_castle(bool new_status);
bool get_attempting_short_castle();
void set_attempting_long_castle(bool new_status);
bool get_attempting_long_castle();
void set_attempting_en_passant(bool new_status);
bool get_attempting_en_passant();
void set_white_pawn_special_move_status(uint8_t file, bool new_status);
bool get_white_pawn_special_move_status(uint8_t file);
void set_black_pawn_special_move_status(uint8_t file, bool new_status);
bool get_black_pawn_special_move_status(uint8_t file);
Square get_en_passantable_square();
void set_en_passantable_square(Square new_en_passantable_square);
MovementDirection get_direction();
void set_direction(MovementDirection new_direction);
Square get_highlighted_square();
void set_highlighted_square(Square new_highlighted_square);
Square get_selected_square();
void set_selected_square(Square new_selected_square);
GameResult get_previous_result();
void set_previous_result(GameResult new_previous_result);
uint8_t get_square_content(uint8_t file, uint8_t rank);
void set_square_content(uint8_t file, uint8_t rank, uint8_t content);
Colour get_turn();
void set_turn(Colour new_turn);
bool get_white_short_castle_privileges();
void set_white_short_castle_privileges(bool new_privileges);
bool get_white_long_castle_privileges();
void set_white_long_castle_privileges(bool new_privileges);
bool get_black_short_castle_privileges();
void set_black_short_castle_privileges(bool new_privileges);
bool get_black_long_castle_privileges();
void set_black_long_castle_privileges(bool new_privileges);
bool coordinates_are_valid(uint8_t row, uint8_t col);
uint8_t get_piece_colour(uint8_t piece);
Colour get_square_colour (int row, int col);
void change_turn();
bool is_path_clear(uint8_t row_1, uint8_t col_1, uint8_t row_2, uint8_t col_2);
bool piece_can_move(uint8_t row_1, uint8_t col_1, uint8_t row_2, uint8_t col_2);
bool is_valid_move();
void reset_special_move_attempt_flags();
void move_start_to_end();
void undo_move_start_to_end();
void reset_game_data();
bool get_king_in_check();
bool set_king_in_check(bool status);
bool get_has_valid_move();
bool set_has_valid_move(bool status);

/* Macros */
#define IS_SQUARE_NULL(s) ((s).row == -1)
#define ARE_SQUARES_EQUAL(s1, s2) ((s1).row == (s2).row && (s1).col == (s2).col)

/* Constants / Definitions */
#define SQUARE_PIXEL_SIZE (u8)7
#define BOARD_BOTTOM_LEFT_PIXEL_ROW (u8)59
#define BOARD_BOTTOM_LEFT_PIXEL_COL (u8)35
#define DISPLAY_ROW_DIRECTION (int8_t)-1
#define DISPLAY_COL_DIRECTION (u8)1
#define QUEUE_TIMER_MAX 250
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

#define WHITE_PAWN_START_RANK 6
#define BLACK_PAWN_START_RANK 1

/* Bitmaps */
/*** Selected Square ***/
extern const u8 selected_square_overlay[7][1];

/*** Arrow Bitmaps ***/
extern const u8 up_arrow[10][2];
extern const u8 down_arrow[10][2];
extern const u8 left_arrow[10][2];
extern const u8 right_arrow[10][2];
extern const u8 invisible_arrow[10][2];

/*** Word Bitmaps ***/
extern const u8 word_WHITE[7][3];
extern const u8 word_BLACK[7][4];
extern const u8 word_TURN[18][1];
extern const u8 word_MOVEMENT[40][1];

/*** Piece Bitmaps ***/
extern const u8 empty_square[7][1];
extern const u8 white_pawn[7][1];
extern const u8 white_knight[7][1];
extern const u8 white_bishop[7][1];
extern const u8 white_rook[7][1];
extern const u8 white_queen[7][1];
extern const u8 white_king[7][1];
extern const u8 black_pawn[7][1];
extern const u8 black_knight[7][1];
extern const u8 black_bishop[7][1];
extern const u8 black_rook[7][1];
extern const u8 black_queen[7][1];
extern const u8 black_king[7][1];

#endif /* __CHESS_APP_H */
