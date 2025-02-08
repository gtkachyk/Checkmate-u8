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
/*** My Variables ***/
/***** Constants *****/
const int LIGHT_SQUARE_COLOUR = 0;
const int DARK_SQUARE_COLOUR = 1;
const int DEFAULT_SQUARE_FLASH_RATE = 500;
const int DEFAULT_ARROW_FLASH_RATE = 500;
const int PLAYER_SYMBOL_START_COL = 5;
const int WHITE_PLAYER_SYMBOL_WIDTH = 23;
const int BLACK_PLAYER_SYMBOL_WIDTH = 25;
const int WHITE_PLAYER_SYMBOL_START_ROW = 56;
const int BLACK_PLAYER_SYMBOL_START_ROW = 4;
const int CUSTOM_FONT_HEIGHT = 5;
const int ARROW_SIZE = 10;
const int ARROW_START_ROW = 28;
const int ARROW_START_COL = 102;

/******* Arrow Bitmaps *******/
extern const u8 default_display_up_arrow[10][2];
extern const u8 default_display_down_arrow[10][2];
extern const u8 default_display_left_arrow[10][2];
extern const u8 default_display_right_arrow[10][2];
extern const u8 default_display_invisible_arrow[10][2];

/******* Letter Bitmaps *******/
extern const u8 default_display_WHITE[7][3];
extern const u8 default_display_BLACK[7][4];
extern const u8 default_display_TURN[18][1];
extern const u8 default_display_MOVEMENT[40][1];

/******* Piece Bitmaps *******/
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

/***** Structs *****/
struct display_info {
    int square_size;
    int bottom_left_pixel_row;
    int bottom_left_pixel_col;
    int row_write_direction;
    int col_write_direction;
};

/***** Game Engine Data *****/
struct display_info default_display = {7, 59, 35, -1, 1};
static int game_state = 0;
static int previous_result = 0;
static int turn = 1;
static int highlighted_square[2] = {7, 0};
static int highlighted_square_highlight_colour = 0;
static int highlighted_square_flash_timer = 0;
static int movement_arrow_flash_timer = DEFAULT_ARROW_FLASH_RATE / 2;
static int movement_arrow_visible = 1;
int direction_options[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}}; // 0 = rank 1 -> 8, 1 = file a -> h, 2 = rank 8 -> 1, 3 = file h -> a
static int movement_direction = 0; // Index of an element in direction_options
static int selecting_direction = 1; // Boolean 
static int selected_square[2] = {-1, -1}; // -1, -1 = no selected piece
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
u8* movement_direction_sprites[4];
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

void highlight_square(int rank, int file, struct display_info display) {
    PixelAddressType pixel_coords = get_square_pixel_coordinates(rank, file, display);
    PixelBlockType image;
    image.u16RowStart = pixel_coords.u16PixelRowAddress;
    image.u16ColumnStart = pixel_coords.u16PixelColumnAddress;
    image.u16RowSize = display.square_size;
    image.u16ColumnSize = display.square_size;

    if (highlighted_square_highlight_colour == LIGHT_SQUARE_COLOUR) {
        LcdLoadInverseBitmap(piece_sprites[board[rank][file]], &image);
        highlighted_square_highlight_colour = DARK_SQUARE_COLOUR;
    }
    else {
        LcdLoadBitmap(piece_sprites[board[rank][file]], &image);
        highlighted_square_highlight_colour = LIGHT_SQUARE_COLOUR;
    }
}

void flash_arrow() {
    PixelBlockType image;
    image.u16RowStart = ARROW_START_ROW;
    image.u16ColumnStart = ARROW_START_COL;
    image.u16RowSize = ARROW_SIZE;
    image.u16ColumnSize = ARROW_SIZE;

    if (movement_arrow_visible) {
        LcdLoadBitmap(&default_display_invisible_arrow, &image);
        movement_arrow_visible = 0;
    }
    else {
        LcdLoadBitmap(movement_direction_sprites[movement_direction], &image);
        movement_arrow_visible = 1;
    }
}

void draw_square(int rank, int file, struct display_info display) {
    int square_colour = get_square_colour(rank, file);
    PixelAddressType pixel_coords = get_square_pixel_coordinates(rank, file, display);
    PixelBlockType image;
    image.u16RowStart = pixel_coords.u16PixelRowAddress;
    image.u16ColumnStart = pixel_coords.u16PixelColumnAddress;
    image.u16RowSize = display.square_size;
    image.u16ColumnSize = display.square_size;

    if (square_colour == LIGHT_SQUARE_COLOUR) {
        LcdLoadBitmap(piece_sprites[board[rank][file]], &image);
    }
    else {
        LcdLoadInverseBitmap(piece_sprites[board[rank][file]], &image);
    }
}

void draw_movement_direction() {
    PixelBlockType image;
    image.u16RowStart = ARROW_START_ROW;
    image.u16ColumnStart = ARROW_START_COL;
    image.u16RowSize = ARROW_SIZE;
    image.u16ColumnSize = ARROW_SIZE;

    LcdLoadBitmap(movement_direction_sprites[movement_direction], &image);
}

void draw_white_player_symbol() {
    PixelBlockType image;
    image.u16RowStart = WHITE_PLAYER_SYMBOL_START_ROW;
    image.u16ColumnStart = PLAYER_SYMBOL_START_COL;
    image.u16RowSize = CUSTOM_FONT_HEIGHT + 2; // Extra 2 pixels for padding
    image.u16ColumnSize = WHITE_PLAYER_SYMBOL_WIDTH;

    if (turn == 1) {
        LcdLoadInverseBitmap(&default_display_WHITE, &image);
    }
    else {
        LcdLoadBitmap(&default_display_WHITE, &image);
    }
}

void draw_black_player_symbol() {
    PixelBlockType image;
    image.u16RowStart = BLACK_PLAYER_SYMBOL_START_ROW;
    image.u16ColumnStart = PLAYER_SYMBOL_START_COL;
    image.u16RowSize = CUSTOM_FONT_HEIGHT + 2; // Extra 2 pixels for padding
    image.u16ColumnSize = BLACK_PLAYER_SYMBOL_WIDTH;

    if (turn == 0) {
        LcdLoadInverseBitmap(&default_display_BLACK, &image);
    }
    else {
        LcdLoadBitmap(&default_display_BLACK, &image);
    }
}

void draw_turn_symbol() {
    PixelBlockType image;
    image.u16RowStart = 23;
    image.u16ColumnStart = 0;
    image.u16RowSize = 20;
    image.u16ColumnSize = CUSTOM_FONT_HEIGHT;
    LcdLoadBitmap(&default_display_TURN, &image);
}

void draw_movement_symbol() {
    PixelBlockType image;
    image.u16RowStart = 13;
    image.u16ColumnStart = 123;
    image.u16RowSize = 40;
    image.u16ColumnSize = CUSTOM_FONT_HEIGHT;
    LcdLoadBitmap(&default_display_MOVEMENT, &image);
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
    turn = 1;
    selecting_direction = 1;
    highlighted_square[0] = 7;
    highlighted_square[1] = 0;
    selected_square[0] = -1;
    selected_square[1] = -1;
    movement_direction = 0;
    reset_board();
    draw_board(default_display);
    draw_movement_direction();
    draw_white_player_symbol();
    draw_black_player_symbol();
    draw_turn_symbol();
    draw_movement_symbol();
    game_state = 1;
}

void UserApp1Initialize(void) {
  /* If good initialization, set state to Idle */
  if(1) {
    if (game_state == 0) {
        movement_direction_sprites[0] = &default_display_up_arrow[0][0];
        movement_direction_sprites[1] = &default_display_right_arrow[0][0];
        movement_direction_sprites[2] = &default_display_down_arrow[0][0];
        movement_direction_sprites[3] = &default_display_left_arrow[0][0];

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
static void UserApp1SM_Idle(void) {
    if (WasButtonPressed(BUTTON0)) {
        ButtonAcknowledge(BUTTON0);
        if (game_state == 1) {
            if (selecting_direction) {
                // Select next direction option
                if (movement_direction == 3) {
                    movement_direction = 0;
                }
                else {
                    movement_direction++;
                }
                draw_movement_direction();
            }
            else {
                // Move highlighted square in the selected direction
                draw_square(highlighted_square[0], highlighted_square[1], default_display);
                highlighted_square[0] += direction_options[movement_direction][0];
                highlighted_square[1] += direction_options[movement_direction][1];
                if (highlighted_square[0] > 7) {
                    highlighted_square[0] = 0;
                }
                if (highlighted_square[0] < 0) {
                    highlighted_square[0] = 7;
                }
                if (highlighted_square[1] > 7) {
                    highlighted_square[1] = 0;
                }
                if (highlighted_square[1] < 0) {
                    highlighted_square[1] = 7;
                }
            }
        }
        else {
            // Start new game
            setup_new_game();
        }
    }
    else if (WasButtonPressed(BUTTON1)) {
        ButtonAcknowledge(BUTTON1);
        if (game_state == 0) {
            // Do nothing
        }
        else {
            if (selecting_direction) {
                // Close direction selection menu
                selecting_direction = 0;
                draw_movement_direction();
            }
            else {
                // Select highlighted piece
                if (selected_square[0] == -1) {
                    selected_square[0] = highlighted_square[0];
                    selected_square[1] = highlighted_square[1];
                }
                else {
                    // Attempt move
                    // Deselect selected square
                    selected_square[0] = -1;
                    selected_square[1] = -1;
                }
            }
        }
    }
    else if (IsButtonHeld(BUTTON0, 2000)) {
        // Resign game
        if (game_state == 1) {
            resign();
        }
    }
    else if (IsButtonHeld(BUTTON1, 500)) {
        // Open direction selection menu
        if (!selecting_direction) {
            selecting_direction = 1;
        }
    }
    if (game_state == 1) {
        // Make sure the square and arrow don't flash too close to each other
        if (highlighted_square_flash_timer == 0) {
            highlight_square(highlighted_square[0], highlighted_square[1], default_display);
            highlighted_square_flash_timer = DEFAULT_SQUARE_FLASH_RATE;
            movement_arrow_flash_timer = DEFAULT_ARROW_FLASH_RATE / 2;
        }
        else if (selecting_direction && movement_arrow_flash_timer == 0) {
            flash_arrow();
            movement_arrow_flash_timer = DEFAULT_ARROW_FLASH_RATE;
            highlighted_square_flash_timer = DEFAULT_SQUARE_FLASH_RATE / 2;
        }
        highlighted_square_flash_timer--;
        movement_arrow_flash_timer--;
    }
}

static void UserApp1SM_Error(void) {
  
}
