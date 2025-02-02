#include "configuration.h"
#include <lcd_NHD-C12864LZ.h>
#include <mpgl2-ehdw-02.h>

/* Not My Variables */
// Global variable definitions with scope across entire project. All Global variable names shall start with "G_<type>UserApp1".
// New Variables
volatile u32 G_u32UserApp1Flags;

// Existing variables (defined in other files -- should all contain the "extern" keyword).
extern volatile u32 G_u32SystemTime1ms;
extern volatile u32 G_u32SystemTime1s;
extern volatile u32 G_u32SystemFlags;
extern volatile u32 G_u32ApplicationFlags;

// Global variable definitions with scope limited to this local application. Variable names shall start with "UserApp1_<type>" and be declared as static.
static fnCode_type UserApp1_pfStateMachine;
int UserApp1_au8Name[];

/* My Variables */
// Constants
int DEFAULT_DISPLAY_BOARD_SQUARE_SIZE = 7;
int DEFAULT_DISPLAY_BOARD_BOTTOM_LEFT_PIXEL_ROW = 59;
int DEFAULT_DISPLAY_BOARD_BOTTOM_LEFT_PIXEL_COL = 35;
int DEFAULT_DISPLAY_BOARD_ROW_WRITE_DIRECTION = -1;
int DEFAULT_DISPLAY_BOARD_COL_WRITE_DIRECTION = 1;

// Structs
struct display_info {
    int square_size;
    int bottom_left_pixel_row;
    int bottom_left_pixel_col;
    int row_write_direction;
    int col_write_direction;
};

// Game engine data
struct display_info default_display = {7, 59, 35, -1, 1};
static int game_state = 0;
static int previous_result = 0;
static int turn = 1;
static int board[8][8] = {
    {10, 8, 9, 11, 12, 9, 8, 10}, 
    {7, 7, 7, 7, 7, 7, 7, 7}, 
    {0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0}, 
    {1, 1, 1, 1, 1, 1, 1, 1}, 
    {4, 2, 3, 5, 6, 3, 2, 4}
};
int (*draw_piece_functions[13]) (int rank_pixel_coords, int file_pixel_coords, struct display_info display);

/* Functions */
void draw_line(int start_row, int start_col, int length, int row_increment, int col_increment) {
    PixelAddressType sTargetPixel = {start_row, start_col};
    for (int i = 0; i < length; i++) {
        LcdSetPixel(&sTargetPixel);
        sTargetPixel.u16PixelRowAddress += row_increment;
        sTargetPixel.u16PixelColumnAddress += col_increment;
    }
}

void colour_square(int square_row, int square_col, struct display_info display) {
    for (int i = 0; i < display.square_size; i++) {
        int row = square_row + (i * display.row_write_direction);
        draw_line(row, square_col, display.square_size, 0, display.col_write_direction);
    }
}

void draw_board(struct display_info display) {
    LcdClearScreen();
    int board_pixel_dimensions = (display.square_size * 8) + 1;
    int outer_board_dimensions = board_pixel_dimensions + 1;
    int top_row = display.bottom_left_pixel_row + (8 * display.square_size * display.row_write_direction) + display.row_write_direction;
    int bottom_row = display.bottom_left_pixel_row - display.row_write_direction;
    int left_col = display.bottom_left_pixel_col - display.col_write_direction;
    int right_col = display.bottom_left_pixel_col + (8 * display.square_size * display.col_write_direction) + display.col_write_direction;
    
    // Draw border
    draw_line(top_row + (1 * display.col_write_direction), left_col, outer_board_dimensions, 0, display.col_write_direction); // Top border
    draw_line(bottom_row, left_col, outer_board_dimensions, 0, display.col_write_direction); // Bottom border
    draw_line(bottom_row, left_col, outer_board_dimensions, display.row_write_direction, 0); // Left border
    draw_line(bottom_row, right_col + (1 * display.row_write_direction), outer_board_dimensions, display.row_write_direction, 0); // Right border

    // Colour squares
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if ((row + col) % 2 != 1) {
                int square_row = display.bottom_left_pixel_row + (row * display.square_size * display.row_write_direction);
                int square_col = display.bottom_left_pixel_col + (col * display.square_size * display.col_write_direction);
                colour_square(square_row, square_col, display);
            }
        }
    }
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
    if (previous_result == 1) {
        if (turn == 0) {
            LcdLoadString("Black wins by", LCD_FONT_SMALL, &sTargetPixel);
        }
        else {
            LcdLoadString("White wins by", LCD_FONT_SMALL, &sTargetPixel);
        }
        sTargetPixel.u16PixelRowAddress = 45;
        sTargetPixel.u16PixelColumnAddress = 32;
        LcdLoadString("checkmate!", LCD_FONT_SMALL, &sTargetPixel);
    }
    else if (previous_result == 2) {
        if (turn == 0) {
            LcdLoadString("White wins by", LCD_FONT_SMALL, &sTargetPixel);
        }
        else {
            LcdLoadString("Black wins by", LCD_FONT_SMALL, &sTargetPixel);
        }
        sTargetPixel.u16PixelRowAddress = 45;
        sTargetPixel.u16PixelColumnAddress = 30;
        LcdLoadString("resignation!", LCD_FONT_SMALL, &sTargetPixel);
    }
    else if (previous_result == 3) {
        LcdLoadString("Draw accepted by", LCD_FONT_SMALL, &sTargetPixel);
        sTargetPixel.u16PixelRowAddress = 45;
        sTargetPixel.u16PixelColumnAddress = 35;
        if (turn == 0) {
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
    game_state = 0;
    previous_result = 2;
    draw_menu();
}

PixelAddressType get_square_pixel_coordinates(int rank, int file, struct display_info display) {

}

void clear_square(int rank_pixel_coords, int file_pixel_coords, struct display_info display) {

}

void draw_white_pawn(int rank_pixel_coords, int file_pixel_coords, struct display_info display) {

}

void draw_white_knight(int rank_pixel_coords, int file_pixel_coords, struct display_info display) {

}

void draw_white_bishop(int rank_pixel_coords, int file_pixel_coords, struct display_info display) {

}

void draw_white_rook(int rank_pixel_coords, int file_pixel_coords, struct display_info display) {

}

void draw_white_queen(int rank_pixel_coords, int file_pixel_coords, struct display_info display) {

}

void draw_white_king(int rank_pixel_coords, int file_pixel_coords, struct display_info display) {

}

void draw_black_pawn(int rank_pixel_coords, int file_pixel_coords, struct display_info display) {

}

void draw_black_knight(int rank_pixel_coords, int file_pixel_coords, struct display_info display) {

}

void draw_black_bishop(int rank_pixel_coords, int file_pixel_coords, struct display_info display) {

}

void draw_black_rook(int rank_pixel_coords, int file_pixel_coords, struct display_info display) {

}

void draw_black_queen(int rank_pixel_coords, int file_pixel_coords, struct display_info display) {

}

void draw_black_king(int rank_pixel_coords, int file_pixel_coords, struct display_info display) {

}

void draw_piece(int piece, int rank, int file, int square_pixel_dimensions, struct display_info display) {
    PixelAddressType pixel_coords = get_square_pixel_coordinates(rank, file, display);
    draw_piece_functions[piece](pixel_coords.u16PixelRowAddress, pixel_coords.u16PixelColumnAddress, display);
}

void UserApp1Initialize(void) {
  /* If good initialization, set state to Idle */
  if(1) {
    if (game_state == 0) {
        draw_piece_functions[0] = clear_square;
        draw_piece_functions[1] = draw_white_pawn;
        draw_piece_functions[2] = draw_white_knight;
        draw_piece_functions[3] = draw_white_bishop;
        draw_piece_functions[4] = draw_white_rook;
        draw_piece_functions[5] = draw_white_queen;
        draw_piece_functions[6] = draw_white_king;
        draw_piece_functions[7] = draw_black_pawn;
        draw_piece_functions[8] = draw_black_knight;
        draw_piece_functions[9] = draw_black_bishop;
        draw_piece_functions[10] = draw_black_rook;
        draw_piece_functions[11] = draw_black_queen;
        draw_piece_functions[12] = draw_black_king;

        draw_menu();
    }
    else {
        print_game_error();
    }
    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }
  else {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_pfStateMachine = UserApp1SM_Error;
  }

}

void UserApp1RunActiveState(void) {
  UserApp1_pfStateMachine();

}

// State Machine Function Definitions
static void UserApp1SM_Idle(void) {
    if (WasButtonPressed(BUTTON0)) {
        ButtonAcknowledge(BUTTON0);
        if (game_state == 0) {
            draw_board(default_display);
            game_state = 1;
        }
    }
    else if (WasButtonPressed(BUTTON1)) {
        ButtonAcknowledge(BUTTON1);
    }
    else if (IsButtonHeld(BUTTON0, 2000)) {
        if (game_state == 1) {
            resign();
        }
    }  
}

static void UserApp1SM_Error(void) {
  
}
