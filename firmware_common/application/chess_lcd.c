#include "chess_app.h"

static bool movement_arrow_visible = TRUE;
static Colour highlighted_square_highlight_colour = BLACK;
static const u8* movement_direction_sprites[4] = {&up_arrow[0][0], &right_arrow[0][0], &down_arrow[0][0], &left_arrow[0][0]};
static const u8* piece_sprites[13] = {
    &empty_square[0][0], 
    &white_pawn[0][0], &white_knight[0][0], &white_bishop[0][0], &white_rook[0][0], &white_queen[0][0], &white_king[0][0], 
    &black_pawn[0][0], &black_knight[0][0], &black_bishop[0][0], &black_rook[0][0], &black_queen[0][0], &black_king[0][0]
};

void draw_piece(uint8_t piece, uint8_t rank, uint8_t file) {
    PixelAddressType pixel_coords = get_square_pixel_coordinates(rank, file);
    PixelBlockType image = {
        .u16RowStart = pixel_coords.u16PixelRowAddress,
        .u16ColumnStart = pixel_coords.u16PixelColumnAddress,
        .u16RowSize = SQUARE_PIXEL_SIZE,
        .u16ColumnSize = SQUARE_PIXEL_SIZE
    };

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

void draw_pieces() {
    for (int rank = 0; rank < BOARD_SIZE; rank++) {
        for (int file = 0; file < BOARD_SIZE; file++) {
            draw_piece(get_square_content(file, rank), rank, file);
        }
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

void draw_error_message() {
    LcdClearScreen();
    PixelAddressType sTargetPixel = {25, 52};
    LcdLoadString("ERROR", LCD_FONT_SMALL, &sTargetPixel);
}

void draw_menu(GameResult previous_result, Colour turn) {
    LcdClearScreen();
    PixelAddressType sTargetPixel = {5, 52};
    LcdLoadString("Menu", LCD_FONT_SMALL, &sTargetPixel);

    sTargetPixel.u16PixelRowAddress = 15;
    sTargetPixel.u16PixelColumnAddress = 10;
    LcdLoadString("Button 0 - New Game", LCD_FONT_SMALL, &sTargetPixel);

    sTargetPixel.u16PixelRowAddress = 28;
    sTargetPixel.u16PixelColumnAddress = 10;
    unsigned char puzzle_str[20];
    sprintf(puzzle_str, "Button 1 - Puzzle %d", get_current_puzzle() + 1);
    LcdLoadString(puzzle_str, LCD_FONT_SMALL, &sTargetPixel);

    sTargetPixel.u16PixelRowAddress = 40;
    sTargetPixel.u16PixelColumnAddress = 25;
    if (previous_result == RESULT_CHECKMATE) {
        if (turn == BLACK) {
            LcdLoadString("Black wins by", LCD_FONT_SMALL, &sTargetPixel);
        }
        else {
            LcdLoadString("White wins by", LCD_FONT_SMALL, &sTargetPixel);
        }
        sTargetPixel.u16PixelRowAddress = 50;
        sTargetPixel.u16PixelColumnAddress = 35;
        LcdLoadString("checkmate!", LCD_FONT_SMALL, &sTargetPixel);
    }
    else if (previous_result == RESULT_RESIGNATION) {
        if (turn == BLACK) {
            LcdLoadString("White wins by", LCD_FONT_SMALL, &sTargetPixel);
        }
        else {
            LcdLoadString("Black wins by", LCD_FONT_SMALL, &sTargetPixel);
        }
        sTargetPixel.u16PixelRowAddress = 50;
        sTargetPixel.u16PixelColumnAddress = 30;
        LcdLoadString("resignation!", LCD_FONT_SMALL, &sTargetPixel);
    }
    else if (previous_result == RESULT_DRAW) {
        sTargetPixel.u16PixelColumnAddress = 52;
        LcdLoadString("Draw", LCD_FONT_SMALL, &sTargetPixel);
    }
    else if (previous_result == RESULT_PUZZLE_FAILED) {
        sTargetPixel.u16PixelRowAddress = 45;
        sTargetPixel.u16PixelColumnAddress = 20;
        LcdLoadString("Puzzle failed :(", LCD_FONT_SMALL, &sTargetPixel);
    }
    set_check_indicator();
}

void draw_player_symbols(Colour turn) {
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

void flash_arrow(MovementDirection direction) {
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

void draw_movement_direction(MovementDirection direction) {
    PixelBlockType image = {
        .u16RowStart = ARROW_START_ROW,
        .u16ColumnStart = ARROW_START_COL,
        .u16RowSize = ARROW_SIZE,
        .u16ColumnSize = ARROW_SIZE
    };

    LcdLoadBitmap(movement_direction_sprites[direction], &image);
}

PixelAddressType get_square_pixel_coordinates(uint8_t rank_index, uint8_t file_index) {
    return (PixelAddressType) {
        .u16PixelRowAddress = (BOARD_BOTTOM_LEFT_PIXEL_ROW + ((DISPLAY_ROW_DIRECTION * SQUARE_PIXEL_SIZE) * (BOARD_SIZE - rank_index))) - (1 * DISPLAY_ROW_DIRECTION),
        .u16PixelColumnAddress = BOARD_BOTTOM_LEFT_PIXEL_COL + ((DISPLAY_COL_DIRECTION * SQUARE_PIXEL_SIZE) * (file_index))
    };
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

void draw_square(Square square, uint8_t content) {
    PixelAddressType pixel_coords = get_square_pixel_coordinates(square.row, square.col);
    PixelBlockType image = {
        .u16RowStart = pixel_coords.u16PixelRowAddress,
        .u16ColumnStart = pixel_coords.u16PixelColumnAddress,
        .u16RowSize = SQUARE_PIXEL_SIZE,
        .u16ColumnSize = SQUARE_PIXEL_SIZE
    };

    if (get_square_colour(square.row, square.col) == WHITE) {
        LcdLoadBitmap(piece_sprites[content], &image);
    }
    else {
        LcdLoadInverseBitmap(piece_sprites[content], &image);
    }
}

void highlight_square(Square square, uint8_t content, bool highlight_selected) {
    PixelAddressType pixel_coords = get_square_pixel_coordinates(square.row, square.col);
    PixelBlockType image = {
        .u16RowStart = pixel_coords.u16PixelRowAddress,
        .u16ColumnStart = pixel_coords.u16PixelColumnAddress,
        .u16RowSize = SQUARE_PIXEL_SIZE,
        .u16ColumnSize = SQUARE_PIXEL_SIZE
    };

    void (*drawFunc)(const u8*, const PixelBlockType*) = (highlighted_square_highlight_colour == WHITE) ? LcdLoadInverseBitmap : LcdLoadBitmap;

    drawFunc(piece_sprites[content], &image);

    highlighted_square_highlight_colour ^= WHITE ^ BLACK;

    if (highlight_selected) {
        void (*overlayFunc)(const u8*, const PixelBlockType*) = (highlighted_square_highlight_colour == WHITE) ? LcdLoadBitmapSetOnly : LcdLoadInverseBitmapSetOnly;
        overlayFunc(&selected_square_overlay, &image);
    }
}

void draw_game_interface() {
    draw_board();
    draw_movement_direction(get_direction());
    draw_player_symbols(get_turn());
    draw_turn_symbol();
    draw_movement_symbol();
    set_check_indicator();
}

void set_check_indicator() {
    if (get_king_in_check()) {
        LedOn(RED3);
    }
    else {
        LedOff(RED3);
    }
}