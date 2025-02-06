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
int LIGHT_SQUARE_COLOUR = 0;
int DARK_SQUARE_COLOUR = 1;
int DEFAULT_DISPLAY_A1_PIXEL_ROW = 53;
int DEFAULT_DISPLAY_A1_PIXEL_COL = 35;

extern const u8 default_display_empty_square[7][1];

extern const u8 default_display_white_pawn[7][1];
extern const u8 default_display_white_knight[7][1];
extern const u8 default_display_white_bishop[7][1];
extern const u8 default_display_white_rook[7][1];
extern const u8 default_display_white_queen[7][1];
extern const u8 default_display_white_king[7][1];

extern const u8 default_display_black_pawn[7][1];
extern const u8 default_display_black_knight[7][1];
extern const u8 default_display_black_bishop[7][1];
extern const u8 default_display_black_rook[7][1];
extern const u8 default_display_black_queen[7][1];
extern const u8 default_display_black_king[7][1];

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
u8* piece_sprites[13];

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

void draw_pieces(struct display_info display) {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            draw_piece(board[rank][file], rank, file, display);
        }
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

    // Draw pieces
    draw_pieces(display);
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

PixelAddressType get_square_pixel_coordinates(int rank_index, int file_index, struct display_info display) {
    PixelAddressType coords;
    coords.u16PixelRowAddress = (display.bottom_left_pixel_row + ((display.row_write_direction * display.square_size) * (8 - rank_index))) - (1 * display.row_write_direction);
    coords.u16PixelColumnAddress = display.bottom_left_pixel_col + ((display.col_write_direction * display.square_size) * (file_index));
    return coords;
}

int get_square_colour (int rank_index, int file_index) {
    if ((rank_index + 1) % 2 == 0) {
        if ((file_index + 1) % 2 != 0) {
            return 0;
        }
        else {
            return 1;
        }
    }
    else {
        if ((file_index + 1) % 2 == 0) {
            return 0;
        }
        else {
            return 1;
        }
    }
}

void draw_piece(int piece, int rank, int file, struct display_info display) {
    PixelAddressType pixel_coords = get_square_pixel_coordinates(rank, file, display);
    PixelBlockType image;
    image.u16RowStart = pixel_coords.u16PixelRowAddress;
    image.u16ColumnStart = pixel_coords.u16PixelColumnAddress;
    image.u16RowSize = display.square_size;
    image.u16ColumnSize = display.square_size;

    if (get_square_colour(rank, file) == LIGHT_SQUARE_COLOUR) {
        LcdLoadBitmap(piece_sprites[piece], &image);
    }
    else {
        LcdLoadInverseBitmap(piece_sprites[piece], &image);
    }
}

void UserApp1Initialize(void) {
  /* If good initialization, set state to Idle */
  if(1) {
    if (game_state == 0) {
        piece_sprites[0] = &default_display_empty_square[0][0];
        piece_sprites[1] = &default_display_white_pawn[0][0];
        piece_sprites[2] = &default_display_white_knight[0][0];
        piece_sprites[3] = &default_display_white_bishop[0][0];
        piece_sprites[4] = &default_display_white_rook[0][0];
        piece_sprites[5] = &default_display_white_queen[0][0];
        piece_sprites[6] = &default_display_white_king[0][0];
        piece_sprites[7] = &default_display_black_pawn[0][0];
        piece_sprites[8] = &default_display_black_knight[0][0];
        piece_sprites[9] = &default_display_black_bishop[0][0];
        piece_sprites[10] = &default_display_black_rook[0][0];
        piece_sprites[11] = &default_display_black_queen[0][0];
        piece_sprites[12] = &default_display_black_king[0][0];

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

// Issue: can't set pixels in different loop iterations without clearing screen
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
        // draw_piece(1, 6, 7, default_display);
        // draw_piece(1, 6, 6, default_display);
        // draw_piece(1, 6, 5, default_display);
        // draw_piece(1, 6, 4, default_display);
        // draw_piece(1, 6, 3, default_display);
        // draw_piece(1, 6, 2, default_display);
        // draw_piece(1, 6, 1, default_display);
        // draw_piece(1, 6, 0, default_display);
    }
    else if (IsButtonHeld(BUTTON0, 2000)) {
        if (game_state == 1) {
            resign();
        }
    }  
}

static void UserApp1SM_Error(void) {
  
}
