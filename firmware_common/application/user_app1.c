#include "configuration.h"
#include <lcd_NHD-C12864LZ.h>
#include <mpgl2-ehdw-02.h>

/* Constants */
const int8_t DIRECTIONS[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

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

/* Variables */
extern volatile u32 G_u32SystemTime1ms;
extern volatile u32 G_u32SystemTime1s;
extern volatile u32 G_u32SystemFlags;
extern volatile u32 G_u32ApplicationFlags;
volatile u32 G_u32UserApp1Flags;
static fnCode_type UserApp1_pfStateMachine;
int UserApp1_au8Name[];
static GameResult previous_result = RESULT_NONE;
static Colour turn = WHITE;
static MovementDirection direction = UP;
static Square highlighted_square = {7, 0};
static Square selected_square = {-1, -1};
static Colour highlighted_square_highlight_colour = 0;
static uint16_t highlighted_square_flash_timer = 0;
static uint16_t movement_arrow_flash_timer = ARROW_FLASH_RATE / 2;
static bool movement_arrow_visible = 1;

static uint8_t white_pawn_states[8] = {2, 2, 2, 2, 2, 2, 2, 2};
static uint8_t black_pawn_states[8] = {2, 2, 2, 2, 2, 2, 2, 2};

static uint8_t board[8][8] = {
    {10, 8, 9, 11, 12, 9, 8, 10}, 
    {7, 7, 7, 7, 7, 7, 7, 7}, 
    {0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0}, 
    {1, 1, 1, 1, 1, 1, 1, 1}, 
    {4, 2, 3, 5, 6, 3, 2, 4}
};
u8* movement_direction_sprites[4] = {&up_arrow[0][0], &right_arrow[0][0], &down_arrow[0][0], &left_arrow[0][0]};
u8* piece_sprites[13] = {
    &empty_square[0][0], 
    &white_pawn[0][0], &white_knight[0][0], &white_bishop[0][0], &white_rook[0][0], &white_queen[0][0], &white_king[0][0], 
    &black_pawn[0][0], &black_knight[0][0], &black_bishop[0][0], &black_rook[0][0], &black_queen[0][0], &black_king[0][0]
};

/* Functions */
PixelAddressType get_square_pixel_coordinates(int rank_index, int file_index) {
    return (PixelAddressType) {
        .u16PixelRowAddress = (BOARD_BOTTOM_LEFT_PIXEL_ROW + ((DISPLAY_ROW_DIRECTION * SQUARE_PIXEL_SIZE) * (BOARD_SIZE - rank_index))) - (1 * DISPLAY_ROW_DIRECTION),
        .u16PixelColumnAddress = BOARD_BOTTOM_LEFT_PIXEL_COL + ((DISPLAY_COL_DIRECTION * SQUARE_PIXEL_SIZE) * (file_index))
    };
}

Colour get_square_colour (int row, int col) {
    return ((row ^ col) % 2 == 0) ? WHITE : BLACK;
}

void draw_pieces() {
    for (int rank = 0; rank < BOARD_SIZE; rank++) {
        for (int file = 0; file < BOARD_SIZE; file++) {
            draw_piece(board[rank][file], rank, file);
        }
    }
}

void draw_piece(int piece, int rank, int file) {
    PixelAddressType pixel_coords = get_square_pixel_coordinates(rank, file);
    PixelBlockType image;
    image.u16RowStart = pixel_coords.u16PixelRowAddress;
    image.u16ColumnStart = pixel_coords.u16PixelColumnAddress;
    image.u16RowSize = SQUARE_PIXEL_SIZE;
    image.u16ColumnSize = SQUARE_PIXEL_SIZE;

    if (get_square_colour(rank, file) == WHITE) {
        LcdLoadBitmap(piece_sprites[piece], &image);
    }
    else {
        LcdLoadInverseBitmap(piece_sprites[piece], &image);
    }
}

void draw_line(int start_row, int start_col, int length, int row_increment, int col_increment) {
    PixelAddressType sTargetPixel = {start_row, start_col};
    for (int i = 0; i < length; i++) {
        LcdSetPixel(&sTargetPixel);
        sTargetPixel.u16PixelRowAddress += row_increment;
        sTargetPixel.u16PixelColumnAddress += col_increment;
    }
}

void colour_square(int square_row, int square_col) {
    for (int i = 0; i < SQUARE_PIXEL_SIZE; i++) {
        int row = square_row + (i * DISPLAY_ROW_DIRECTION);
        draw_line(row, square_col, SQUARE_PIXEL_SIZE, 0, DISPLAY_COL_DIRECTION);
    }
}

void draw_board() {
    LcdClearScreen();
    int board_pixel_dimensions = (SQUARE_PIXEL_SIZE * BOARD_SIZE) + 1;
    int outer_board_dimensions = board_pixel_dimensions + 1;
    int top_row = BOARD_BOTTOM_LEFT_PIXEL_ROW + (BOARD_SIZE * SQUARE_PIXEL_SIZE * DISPLAY_ROW_DIRECTION) + DISPLAY_ROW_DIRECTION;
    int bottom_row = BOARD_BOTTOM_LEFT_PIXEL_ROW - DISPLAY_ROW_DIRECTION;
    int left_col = BOARD_BOTTOM_LEFT_PIXEL_COL - DISPLAY_COL_DIRECTION;
    int right_col = BOARD_BOTTOM_LEFT_PIXEL_COL + (BOARD_SIZE * SQUARE_PIXEL_SIZE * DISPLAY_COL_DIRECTION) + DISPLAY_COL_DIRECTION;
    
    // Draw border
    draw_line(top_row + (1 * DISPLAY_COL_DIRECTION), left_col, outer_board_dimensions, 0, DISPLAY_COL_DIRECTION); // Top border
    draw_line(bottom_row, left_col, outer_board_dimensions, 0, DISPLAY_COL_DIRECTION); // Bottom border
    draw_line(bottom_row, left_col, outer_board_dimensions, DISPLAY_ROW_DIRECTION, 0); // Left border
    draw_line(bottom_row, right_col + (1 * DISPLAY_ROW_DIRECTION), outer_board_dimensions, DISPLAY_ROW_DIRECTION, 0); // Right border

    // Colour squares
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            if ((row + col) % 2 != 1) {
                int square_row = BOARD_BOTTOM_LEFT_PIXEL_ROW + (row * SQUARE_PIXEL_SIZE * DISPLAY_ROW_DIRECTION);
                int square_col = BOARD_BOTTOM_LEFT_PIXEL_COL + (col * SQUARE_PIXEL_SIZE * DISPLAY_COL_DIRECTION);
                colour_square(square_row, square_col);
            }
        }
    }

    // Draw pieces
    draw_pieces();
}

void draw_menu() {
    LcdClearScreen();
    PixelAddressType sTargetPixel = {5, 52};
    LcdLoadString("Menu", LCD_FONT_SMALL, &sTargetPixel);

    sTargetPixel.u16PixelRowAddress = 15;
    sTargetPixel.u16PixelColumnAddress = 10;
    LcdLoadString("Button 0 - New Game", LCD_FONT_SMALL, &sTargetPixel);

    sTargetPixel.u16PixelRowAddress = 35;
    sTargetPixel.u16PixelColumnAddress = 25;
    if (previous_result == RESULT_CHECKMATE) {
        if (turn == BLACK) {
            LcdLoadString("Black wins by", LCD_FONT_SMALL, &sTargetPixel);
        }
        else {
            LcdLoadString("White wins by", LCD_FONT_SMALL, &sTargetPixel);
        }
        sTargetPixel.u16PixelRowAddress = 45;
        sTargetPixel.u16PixelColumnAddress = 32;
        LcdLoadString("checkmate!", LCD_FONT_SMALL, &sTargetPixel);
    }
    else if (previous_result == RESULT_RESIGNATION) {
        if (turn == BLACK) {
            LcdLoadString("White wins by", LCD_FONT_SMALL, &sTargetPixel);
        }
        else {
            LcdLoadString("Black wins by", LCD_FONT_SMALL, &sTargetPixel);
        }
        sTargetPixel.u16PixelRowAddress = 45;
        sTargetPixel.u16PixelColumnAddress = 30;
        LcdLoadString("resignation!", LCD_FONT_SMALL, &sTargetPixel);
    }
    else if (previous_result == RESULT_DRAW) {
        LcdLoadString("Draw accepted by", LCD_FONT_SMALL, &sTargetPixel);
        sTargetPixel.u16PixelRowAddress = 45;
        sTargetPixel.u16PixelColumnAddress = 35;
        if (turn == BLACK) {
            LcdLoadString("black.", LCD_FONT_SMALL, &sTargetPixel);
        }
        else {
            LcdLoadString("white.", LCD_FONT_SMALL, &sTargetPixel);
        }
    }
}

void print_game_error(){
    LcdClearScreen();
    PixelAddressType sTargetPixel = {30, 30};
    LcdLoadString("Game error", LCD_FONT_SMALL, &sTargetPixel);
}

void resign() {
    previous_result = RESULT_RESIGNATION;
    draw_menu();
    UserApp1_pfStateMachine = Chess_Menu;
}

void highlight_square(Square square) {
    PixelAddressType pixel_coords = get_square_pixel_coordinates(square.row, square.col);
    PixelBlockType image = {
        .u16RowStart = pixel_coords.u16PixelRowAddress,
        .u16ColumnStart = pixel_coords.u16PixelColumnAddress,
        .u16RowSize = SQUARE_PIXEL_SIZE,
        .u16ColumnSize = SQUARE_PIXEL_SIZE
    };

    void (*drawFunc)(const u8*, const PixelBlockType*) = (highlighted_square_highlight_colour == WHITE) ? LcdLoadInverseBitmap : LcdLoadBitmap;

    drawFunc(piece_sprites[board[square.row][square.col]], &image);

    highlighted_square_highlight_colour ^= WHITE ^ BLACK;

    if (ARE_SQUARES_EQUAL(selected_square, highlighted_square)) {
        void (*overlayFunc)(const u8*, const PixelBlockType*) = (highlighted_square_highlight_colour == WHITE) ? LcdLoadBitmapSetOnly : LcdLoadInverseBitmapSetOnly;
        overlayFunc(&selected_square_overlay, &image);
    }
}

void flash_arrow() {
    PixelBlockType image = {
        .u16RowStart = ARROW_START_ROW,
        .u16ColumnStart = ARROW_START_COL,
        .u16RowSize = ARROW_SIZE,
        .u16ColumnSize = ARROW_SIZE
    };

    if (movement_arrow_visible) {
        LcdLoadBitmap(&invisible_arrow, &image);
        movement_arrow_visible = 0;
    }
    else {
        LcdLoadBitmap(movement_direction_sprites[direction], &image);
        movement_arrow_visible = 1;
    }
}

void draw_square(Square square) {
    PixelAddressType pixel_coords = get_square_pixel_coordinates(square.row, square.col);
    PixelBlockType image = {
        .u16RowStart = pixel_coords.u16PixelRowAddress,
        .u16ColumnStart = pixel_coords.u16PixelColumnAddress,
        .u16RowSize = SQUARE_PIXEL_SIZE,
        .u16ColumnSize = SQUARE_PIXEL_SIZE
    };

    if (get_square_colour(square.row, square.col) == WHITE) {
        LcdLoadBitmap(piece_sprites[board[square.row][square.col]], &image);
    }
    else {
        LcdLoadInverseBitmap(piece_sprites[board[square.row][square.col]], &image);
    }
}

void draw_selected_square_indicator(Square square) {
    PixelAddressType pixel_coords = get_square_pixel_coordinates(square.row, square.col);
    PixelBlockType image = {
        .u16RowStart = pixel_coords.u16PixelRowAddress,
        .u16ColumnStart = pixel_coords.u16PixelColumnAddress,
        .u16RowSize = SQUARE_PIXEL_SIZE,
        .u16ColumnSize = SQUARE_PIXEL_SIZE
    };

    if (get_square_colour(square.row, square.col) == WHITE) {
        LcdLoadBitmapSetOnly(&selected_square_overlay, &image);
    }
    else {
        LcdLoadInverseBitmapSetOnly(&selected_square_overlay, &image);
    }
}

void draw_movement_direction() {
    PixelBlockType image = {
        .u16RowStart = ARROW_START_ROW,
        .u16ColumnStart = ARROW_START_COL,
        .u16RowSize = ARROW_SIZE,
        .u16ColumnSize = ARROW_SIZE
    };

    LcdLoadBitmap(movement_direction_sprites[direction], &image);
}

void draw_player_symbols() {
    PixelBlockType image = {
        .u16RowStart = BLACK_PLAYER_SYMBOL_START_ROW,
        .u16ColumnStart = PLAYER_SYMBOL_START_COL,
        .u16RowSize = CUSTOM_FONT_HEIGHT + 2, // Extra 2 pixels for padding
        .u16ColumnSize = BLACK_PLAYER_SYMBOL_WIDTH
    };
    if (turn == BLACK) {
        LcdLoadInverseBitmap(&word_BLACK, &image);
    }
    else {
        LcdLoadBitmap(&word_BLACK, &image);
    }
    image.u16RowStart = WHITE_PLAYER_SYMBOL_START_ROW;
    image.u16ColumnSize = WHITE_PLAYER_SYMBOL_WIDTH;
    if (turn == WHITE) {
        LcdLoadInverseBitmap(&word_WHITE, &image);
    }
    else {
        LcdLoadBitmap(&word_WHITE, &image);
    }
}

void draw_turn_symbol() {
    PixelBlockType image;
    image.u16RowStart = 23;
    image.u16ColumnStart = 0;
    image.u16RowSize = 20;
    image.u16ColumnSize = CUSTOM_FONT_HEIGHT;
    LcdLoadBitmap(&word_TURN, &image);
}

void draw_movement_symbol() {
    PixelBlockType image;
    image.u16RowStart = 13;
    image.u16ColumnStart = 123;
    image.u16RowSize = 40;
    image.u16ColumnSize = CUSTOM_FONT_HEIGHT;
    LcdLoadBitmap(&word_MOVEMENT, &image);
}

void reset_board() {
    board[0][0] = 10;
    board[0][1] = 8;
    board[0][2] = 9;
    board[0][3] = 11;
    board[0][4] = 12;
    board[0][5] = 9;
    board[0][6] = 8;
    board[0][7] = 10;
    for (int i = 0; i < 8; i++) {
        board[1][i] = 7;
        board[2][i] = 0;
        board[3][i] = 0;
        board[4][i] = 0;
        board[5][i] = 0;
        board[6][i] = 1;
    }
    board[7][0] = 4;
    board[7][1] = 2;
    board[7][2] = 3;
    board[7][3] = 5;
    board[7][4] = 6;
    board[7][5] = 3;
    board[7][6] = 2;
    board[7][7] = 4;
}

void setup_new_game() {
    turn = WHITE;
    highlighted_square.row = 7;
    highlighted_square.col = 0;
    selected_square.row = -1;
    direction = UP;
    reset_board();
    draw_board();
    draw_movement_direction();
    draw_player_symbols();
    draw_turn_symbol();
    draw_movement_symbol();
    UserApp1_pfStateMachine = Chess_Game_Selecting_Direction;
}

bool is_valid_move(Square start_square, Square end_square) {
    uint8_t piece = board[start_square.row][start_square.col];

    // If there is no piece on the start square, return false
    if (piece == EMPTY_SQUARE) return FALSE;

    // If the move starts and ends on the same square, return false
    if (start_square.row == end_square.row && start_square.col == end_square.col) return FALSE;

    int8_t row_change = end_square.row - start_square.row;
    int8_t col_change = end_square.col - start_square.col;

    // If the movement of piece is blocked by another piece, return false
    if (piece != WHITE_KNIGHT && piece != BLACK_KNIGHT) {
        int row_direction = 0;
        int col_direction = 0;
        if (row_change >= 0) {
            row_direction = 1;
        }
        else {
            row_direction = -1;
        }
        if (col_change >= 0) {
            col_direction = 1;
        }
        else {
            col_direction = -1;
        }

        int i = start_square.row + row_direction;
        int j = start_square.col + col_direction;
        while (i == end_square.row && j == end_square.col) {
            if (board[i][j] != EMPTY_SQUARE) return FALSE;
            if (i != end_square.row) {
                i += row_direction;
            }
            if (j != end_square.col) {
                j += col_direction;
            }
        }

        // If there is a piece of the same colour as piece on the end square, return false
        if (turn == WHITE && board[i][j] > EMPTY_SQUARE && board[i][j] < WHITE_KING) return FALSE;
        if (turn == BLACK && board[i][j] > WHITE_KING && board[i][j] < BLACK_KING + 1) return FALSE;
    }

    // If the move violates the movement rules of piece
    if (piece == WHITE_PAWN) {
        if (row_change != -2 || row_change != -1) return FALSE;
        if (row_change == -2 && white_pawn_states[start_square.row] != 2) return FALSE;
        if (col_change < -1 || col_change > 1) return FALSE;
    }
    else if (piece == BLACK_PAWN) {
        if (row_change != 2 || row_change != 1) return FALSE;
        if (row_change == 2 && black_pawn_states[start_square.row] != 2) return FALSE;
        if (col_change < -1 || col_change > 1) return FALSE;
    }
    else if (piece == WHITE_KNIGHT || piece == BLACK_KNIGHT) {
        if (abs(row_change) == 2 && abs(col_change) != 1) return FALSE;
        if (abs(row_change) == 1 && abs(col_change) != 2) return FALSE;
    }
    else if (piece == WHITE_BISHOP || BLACK_BISHOP) {
        if (row_change != col_change) return FALSE;
    }
    else if (piece == WHITE_ROOK || piece == BLACK_ROOK) {
        if (!(row_change == 0 || col_change == 0)) return FALSE;
    }
    else if (piece == WHITE_QUEEN || piece == BLACK_QUEEN) {
        if ((row_change != col_change) && !(row_change == 0 || col_change == 0)) return FALSE;
    }
    else if (piece == WHITE_KING || piece == BLACK_KING) {
        if (row_change < -1 || row_change > 1 || col_change < -1 || col_change > 1) return FALSE;
    }
    else {
        return FALSE;
    }

    return TRUE;
}

void move_piece(Square start_square, Square end_square) {
    draw_piece(board[start_square.row][start_square.col], end_square.row, end_square.col);
    draw_piece(EMPTY_SQUARE, start_square.row, start_square.col);
    board[end_square.row][end_square.col] = board[start_square.row][start_square.col];
    board[start_square.row][start_square.col] = EMPTY_SQUARE;
}

void UserApp1Initialize(void) {
  if(1) {
    draw_menu();
    UserApp1_pfStateMachine = Chess_Menu;
  }
  else {
    UserApp1_pfStateMachine = UserApp1SM_Error;
  }
}

void UserApp1RunActiveState(void) {
  UserApp1_pfStateMachine();
}

// State Machine Function Definitions
static void Chess_Menu(void) {
    if (WasButtonPressed(BUTTON0)) {
        ButtonAcknowledge(BUTTON0);
        setup_new_game();
    }
}

static void Chess_Game_Selecting_Direction(void) {
    // Highlight items
    if (highlighted_square_flash_timer == 0) {
        highlight_square(highlighted_square);
        highlighted_square_flash_timer = SQUARE_FLASH_RATE;
        movement_arrow_flash_timer = ARROW_FLASH_RATE / 2;
    }
    else if (movement_arrow_flash_timer == 0){
        flash_arrow();
        movement_arrow_flash_timer = ARROW_FLASH_RATE;
        highlighted_square_flash_timer = SQUARE_FLASH_RATE / 2;
    }
    movement_arrow_flash_timer--;
    highlighted_square_flash_timer--;

    if (WasButtonPressed(BUTTON0)) {
        ButtonAcknowledge(BUTTON0);

        // Select next direction option
        direction = (direction + 1) % DIRECTION_OPTIONS;
        draw_movement_direction();
    }
    else if (WasButtonPressed(BUTTON1)) {
        ButtonAcknowledge(BUTTON1);

        // Close direction selection menu
        draw_movement_direction();
        UserApp1_pfStateMachine = Chess_Game_Selecting_Square;
    }
    else if (IsButtonHeld(BUTTON0, 2000)) {
        // Resign game
        resign();
    }
}

static void Chess_Game_Selecting_Square(void) {
    // Highlight items
    if (highlighted_square_flash_timer == 0) {
        highlight_square(highlighted_square);
        highlighted_square_flash_timer = SQUARE_FLASH_RATE;
        movement_arrow_flash_timer = ARROW_FLASH_RATE / 2;
    }
    highlighted_square_flash_timer--;

    if (WasButtonPressed(BUTTON0)) {
        ButtonAcknowledge(BUTTON0);

        // Move highlighted square in the selected direction
        draw_square(highlighted_square);
        if (ARE_SQUARES_EQUAL(selected_square, highlighted_square)) {
            draw_selected_square_indicator(selected_square);
        }
        highlighted_square.row = (highlighted_square.row + DIRECTIONS[direction][0] + BOARD_SIZE) % BOARD_SIZE;
        highlighted_square.col = (highlighted_square.col + DIRECTIONS[direction][1] + BOARD_SIZE) % BOARD_SIZE;        
    }
    else if (WasButtonPressed(BUTTON1)) {
        ButtonAcknowledge(BUTTON1);
        // Select highlighted piece
        if (IS_SQUARE_NULL(selected_square)) {
            selected_square.row = highlighted_square.row;
            selected_square.col = highlighted_square.col;
        }
        else {
            // Attempt move
            if (1) {
                move_piece(selected_square, highlighted_square);
                if (turn == WHITE) {
                    turn = BLACK;
                }
                else {
                    turn = WHITE;
                }
                draw_player_symbols();
            }

            // Deselect selected square
            draw_square(selected_square);
            selected_square.row = -1;
        }
    }
    else if (IsButtonHeld(BUTTON0, 500)) {
        // Open direction selection menu
        UserApp1_pfStateMachine = Chess_Game_Selecting_Direction;
    }
}

static void UserApp1SM_Error(void) {
  
}
