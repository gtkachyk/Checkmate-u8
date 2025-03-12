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
    RESULT_DRAW = 3,
    RESULT_PUZZLE_FAILED = 4
} GameResult;

typedef enum {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
} MovementDirection;

typedef enum {
    KNIGHT = 0,
    BISHOP = 1,
    ROOK = 2,
    QUEEN = 3, 
    NO_PROMOTION = 4
} PromotionOption;

typedef enum {
    NO_COLOUR = -1,
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
    PromotionOption promotion_selection;
} Move;

typedef struct Node {
    fnCode_type data;
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
static void Selecting_Direction(void);
static void Selecting_Square(void);
static void Lift_Piece(void);
static void Place_Piece(void);
static void Update_Player_Symbols(void);
static void Flash_Highlighted_Square(void);
static void Flash_Movement_Indicator(void);
static void Move_Highlighted_Square_Part_1(void);
static void Move_Highlighted_Square_Part_2(void);
static void Validate_Move_Castle_Through_Check(void);
static void Validate_Move_Piece_Movement(void);
static void Validate_Move_Self_Inflicted_Check(void);
static void Deselect_Selected_Square(void);
static void Change_Movement_Direction(void);
static void Post_Move_Check_Is_In_Check(void);
static void Post_Move_Check_Has_Valid_Move(void);
static void Post_Move_Check_Is_Sufficient_Material(void);
static void Post_Move_Check_Is_Draw_By_Repetition(void);
static void Remove_EnPassantable_Piece(void);
static void Change_Promotion_Option(void);
static void Selecting_Promotion(void);
static void Close_Movement_Direction_Menu(void);
static void Set_Promotion_Square(void);
static void Selecting_Square(void);
static void ChessAppSM_Error(void);
static void Process_Valid_Move(void);
static void Setup_Post_Move_Analysis(void);
static void Post_Move_Check_Is_Draw_By_Fifty_Move_Rule(void);
static void Draw_Board(void);
static void Draw_Interface(void);
bool get_skip_reset();

/* chess_utils.c */
void print_str (u8 str[]);
void print_str_var (u8 name[], u8 val[]);
void print_int_var (u8 name[], u32 val);
void print_square(u8 name[], Square square);
void print_state();
void print_move(u8 name[], Move move);
Node* create_node(fnCode_type new_data);
Queue* create_queue();
bool is_empty(Queue* q);
void enqueue(Queue* q, fnCode_type new_data);
void dequeue(Queue* q);
fnCode_type get_front(Queue* q);

/* chess_lcd.c */
uint8_t get_menu_parts_drawn();
void set_menu_parts_drawn(uint8_t new_val);
uint8_t get_board_parts_drawn();
void set_board_parts_drawn(uint8_t new_val);
uint8_t get_interface_parts_drawn();
void set_interface_parts_drawn(uint8_t new_val);
void flash_arrow(MovementDirection direction);
void draw_movement_direction(MovementDirection direction);
PixelAddressType get_square_pixel_coordinates(uint8_t rank_index, uint8_t file_index);
void draw_player_symbols(Colour turn);
void draw_turn_symbol();
void draw_movement_symbol();
void draw_menu();
void draw_line(int start_row, int start_col, int length, int row_increment, int col_increment);
void colour_square(int square_row, int square_col);
void draw_selected_square_indicator(Square square);
void draw_piece(uint8_t piece, uint8_t rank, uint8_t file);
void highlight_square(Square square, uint8_t content, bool highlight_selected);
void draw_square(Square square, uint8_t content);
void draw_pieces();
void draw_board();
void draw_game_interface();
void set_check_indicator();
void draw_error_message();

/* chess_game.c */
void set_fifty_move_rule_counter(int8_t new_val);
int8_t get_fifty_move_rule_counter();
void set_preserve_validation_flags(bool state);
bool get_preserve_validation_flags();
void set_current_puzzle(uint8_t puzzle);
uint8_t get_current_puzzle();
void set_puzzle_mode(bool state);
bool get_puzzle_mode();
void set_move_to_make_part_two(Move move);
Move get_move_to_make_part_two();
void set_king_loses_short_castle_privileges(bool new_status);
bool get_king_loses_short_castle_privileges();
void set_king_loses_long_castle_privileges(bool new_status);
bool get_king_loses_long_castle_privileges();
void set_pawn_loses_special_move_privilege(bool new_status);
bool get_pawn_loses_special_move_privilege();
uint8_t* get_promotion_option(PromotionOption promotion_option, Colour turn);
PromotionOption get_promotion();
void set_promotion(PromotionOption new_promotion);
void set_attempting_promotion(bool new_status);
bool get_attempting_promotion();
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
void remove_white_pawn_special_move_privileges(uint8_t file);
bool get_white_pawn_special_move_privileges(uint8_t file);
void remove_black_pawn_special_move_privileges(uint8_t file);
bool get_black_pawn_special_move_privileges(uint8_t file);
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
void reset_movement_validation_flags();
void move_start_to_end();
void undo_move_start_to_end();
void reset_game_data();
bool get_king_in_check();
void set_king_in_check(bool status);
bool get_has_valid_move();
void set_has_valid_move(bool status);
bool insufficient_material();
bool draw_by_repetition();
void push_to_last_eight_moves(Move move);
Move create_move(int8_t start_col, int8_t start_row, int8_t end_col, int8_t end_row, PromotionOption promotion_option);

/* Macros */
#define IS_SQUARE_NULL(s) ((s).row == -1)
#define IS_MOVE_NULL(m) (IS_SQUARE_NULL((m).start) && IS_SQUARE_NULL((m).end))
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
#define PROMOTION_OPTIONS (u8)4
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

#define DIRECTION_SELECTION_BUTTON 2

#define NULL_SQUARE (Square){.col = -1, .row = -1}
#define NULL_MOVE (Move){.start = NULL_SQUARE, .end = NULL_SQUARE, .promotion_selection = NO_PROMOTION}
#define NO_MOVE 0
#define PART_ONE 1
#define PART_TWO 2
#define NO_DEBUG_ACTION 5
#define DEBUG_TIMER_MAX 500
#define UNDEFINED_COLOUR (uint8_t)2
#define MENU_PARTS (uint8_t)6
#define BOARD_PARTS (uint8_t)4
#define INTERFACE_PARTS (uint8_t)5

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
