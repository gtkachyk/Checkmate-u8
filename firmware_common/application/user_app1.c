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
static fnCode_type return_state;
int UserApp1_au8Name[];
static GameResult previous_result = RESULT_NONE;
static Colour turn = WHITE;
static MovementDirection direction = UP;
static Square highlighted_square = {7, 0};
static Square selected_square = {-1, -1};
static uint16_t highlighted_square_flash_timer = 0;
static uint16_t movement_arrow_flash_timer = ARROW_FLASH_RATE / 2;
static Colour highlighted_square_highlight_colour = BLACK;
static bool movement_arrow_visible = TRUE;

static uint8_t white_pawns_special_move_status = 0xFF;
static uint8_t black_pawns_special_move_status = 0xFF;
static Square en_passantable = {-1, -1};
static Square white_king_square = {7, 4}; // Tracks the location of the white king
static Square black_king_square = {0, 4}; // Tracks the location of the black king
static bool king_in_check = FALSE; // Tracks if the moving player's king is in check

static u8 board[8][8] = {
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
extern u8 G_au8DebugScanBuffer[DEBUG_SCANF_BUFFER_SIZE];
extern u8 G_u8DebugScanfCharCount;

static int draw_timer = DRAW_TIMER_MAX;
static bool attempting_en_passant = FALSE;
static bool attempting_short_castle = FALSE;
static bool attempting_long_castle = FALSE;
static bool white_has_short_castle_privileges = TRUE;
static bool black_has_short_castle_privileges = TRUE;
static bool white_has_long_castle_privileges = TRUE;
static bool black_has_long_castle_privileges = TRUE;
static Queue state_queue;

/* Functions */
Node* createNode(int new_data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = new_data;
    new_node->next = NULL;
    return new_node;
}

Queue* createQueue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    return q;
}

int isEmpty(Queue* q) {
    if (q->front == NULL && q->rear == NULL) {
        return TRUE;
    }
    return FALSE;
}

void enqueue(Queue* q, FuncPtr new_data) {
    Node* new_node = createNode(new_data);
    if (q->rear == NULL) {
        q->front = q->rear = new_node;
        return;
    }
    q->rear->next = new_node;
    q->rear = new_node;
}

void dequeue(Queue* q) {
    if (isEmpty(q)) {
        return;
    }
    Node* temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL)
        q->rear = NULL;
    free(temp);
}

FuncPtr getFront(Queue* q) {
    if (isEmpty(q)) {
        return NULL;
    }
    return q->front->data;
}

FuncPtr getRear(Queue* q) {
    if (isEmpty(q)) {
        return NULL;
    }
    return q->rear->data;
}

void set_square(uint8_t file, uint8_t rank, uint8_t piece) {
    board[rank][file] = piece;
}

PixelAddressType get_square_pixel_coordinates(uint8_t rank_index, uint8_t file_index) {
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
}

void print_int_var (u8 name[], u32 val) {
    u8 equals_sign[] = " = ";
    DebugPrintf(name);
    DebugPrintf(equals_sign);
    DebugPrintNumber(val);
    DebugLineFeed();
}

void print_str_var (u8 name[], u8 val[]) {
    u8 equals_sign[] = " = ";
    DebugPrintf(name);
    DebugPrintf(equals_sign);
    DebugPrintf(val);
    DebugLineFeed();
}

void print_str (u8 str[]) {
    DebugPrintf(str);
    DebugLineFeed();
}

void update_white_pawn_special_move_status(uint8_t file) {
    white_pawns_special_move_status = white_pawns_special_move_status | (0 << file);
}

bool get_white_pawn_special_move_status(uint8_t file) {
    return (white_pawns_special_move_status >> file) & 1;
}

void update_black_pawn_special_move_status(uint8_t file) {
    black_pawns_special_move_status = black_pawns_special_move_status | (0 << file);
}

bool get_black_pawn_special_move_status(uint8_t file) {
    return (black_pawns_special_move_status >> file) & 1;
}

bool coordinates_are_valid(uint8_t row, uint8_t col) {
    if (row > 0 || row < 7 || col > 0 || col < 7) {
        return TRUE;
    }
    return FALSE;
}

uint8_t get_piece_colour(uint8_t piece) {
    if (piece > 0 && piece < 7) {
        return WHITE;
    }
    else if (piece > 6 && piece < 13) {
        return BLACK;
    }
    else {
        return -1;
    }
}

bool is_path_clear(uint8_t row_1, uint8_t col_1, uint8_t row_2, uint8_t col_2) {
    int8_t row_dir = (row_2 > row_1) - (row_2 < row_1); // -1, 0, or 1
    int8_t col_dir = (col_2 > col_1) - (col_2 < col_1);

    uint8_t i = row_1 + row_dir;
    uint8_t j = col_1 + col_dir;
    
    while (i != row_2 || j != col_2) {
        if (board[i][j] != EMPTY_SQUARE) return FALSE;
        i += row_dir;
        j += col_dir;
    }
    return TRUE;
}

// Debug message template: printf("Attempting to move <piece_name> on %c%c to %c%c...\n", file_index_to_notation(col_1), rank_index_to_notation(row_1), file_index_to_notation(col_2), rank_index_to_notation(row_2));
bool piece_can_move(uint8_t row_1, uint8_t col_1, uint8_t row_2, uint8_t col_2) {
    // printf("piece_can_move called with row_1 = %u, col_1 = %u\n", row_1, col_1);
    if (!coordinates_are_valid(row_1, col_1) || !coordinates_are_valid(row_2, col_2)) return FALSE;
    uint8_t piece = board[row_1][col_1];
    uint8_t destination_square = board[row_2][col_2];
    if (get_piece_colour(piece) == get_piece_colour(destination_square)) return FALSE;
    if (piece == EMPTY_SQUARE) return FALSE;

    int8_t row_change = row_2 - row_1;
    int8_t col_change = col_2 - col_1;
    if (piece == WHITE_PAWN || piece == BLACK_PAWN) {
        int direction = (piece == WHITE_PAWN) ? -1 : 1;
        if (row_2 == row_1 + direction && col_2 == col_1 && destination_square == EMPTY_SQUARE) {
            // Standard move
            return TRUE;
        }
        else if (row_2 == row_1 + direction && abs(col_2 - col_1) == 1) {
            // Capture
            if (get_piece_colour(destination_square) == BLACK) {
                // Standard
                return TRUE;
            }
            else if (destination_square == EMPTY_SQUARE && en_passantable.row == row_1 && en_passantable.col == col_2 && board[row_1][col_2] == BLACK_PAWN) {
                // En passant
                attempting_en_passant = TRUE;
                return TRUE;
            }
        }
        else if (row_2 == row_1 + direction*2 && col_2 == col_1 && destination_square == EMPTY_SQUARE && board[row_1 + direction][col_1] == EMPTY_SQUARE && get_white_pawn_special_move_status(col_1)) {
            // Special move
            return TRUE;
        }
    }
    else if (piece == WHITE_KNIGHT || piece == BLACK_KNIGHT) {
        if (((abs(row_2 - row_1) == 2 && abs(col_2 - col_1) == 1) || (abs(row_2 - row_1) == 1 && abs(col_2 - col_1) == 2))) {
            return TRUE;
        }
    }
    else if (piece == WHITE_BISHOP || piece == BLACK_BISHOP) {
        return (abs(row_change) == abs(col_change) && is_path_clear(row_1, col_1, row_2, col_2));
    }
    else if (piece == WHITE_ROOK || piece == BLACK_ROOK) {
        return ((row_1 == row_2 || col_1 == col_2) && is_path_clear(row_1, col_1, row_2, col_2));
    }
    else if (piece == WHITE_QUEEN || piece == BLACK_QUEEN) {
        return ((abs(row_change) == abs(col_change) || row_1 == row_2 || col_1 == col_2) && is_path_clear(row_1, col_1, row_2, col_2));
    }
    else if (piece == WHITE_KING) {
        // Standard movement
        if ((row_change == 0 || abs(row_change) == 1) && (col_change == 0 || abs(col_change) == 1)) {
            return TRUE;
        }
        else if (row_1 == ONE && col_1 == E && row_2 == ONE) {
            if (white_has_short_castle_privileges && col_2 == G && board[ONE][F] == EMPTY_SQUARE && board[ONE][G] == EMPTY_SQUARE && board[ONE][H] == WHITE_ROOK) {
                attempting_short_castle = TRUE;
                return TRUE;
            }
            else if (white_has_long_castle_privileges && col_2 == C && board[ONE][D] == EMPTY_SQUARE && board[ONE][C] == EMPTY_SQUARE && board[ONE][B] == EMPTY_SQUARE && board[ONE][A] == WHITE_ROOK) {
                attempting_long_castle = TRUE;
                return TRUE;
            }
        }
    }
    else if (piece == BLACK_KING) {
        // Standard movement
        if ((row_change == 0 || abs(row_change) == 1) && (col_change == 0 || abs(col_change) == 1)) {
            return TRUE;
        }
        else if (row_1 == EIGHT && col_1 == E && row_2 == EIGHT) {
            if (black_has_short_castle_privileges && col_2 == G && board[EIGHT][F] == EMPTY_SQUARE && board[EIGHT][G] == EMPTY_SQUARE && board[EIGHT][H] == BLACK_ROOK) {
                attempting_short_castle = TRUE;
                return TRUE;
            }
            else if (black_has_long_castle_privileges && col_2 == C && board[EIGHT][D] == EMPTY_SQUARE && board[EIGHT][C] == EMPTY_SQUARE && board[EIGHT][B] == EMPTY_SQUARE && board[EIGHT][A] == BLACK_ROOK) {
                attempting_long_castle = TRUE;
                return TRUE;
            }
        }
    }
    return FALSE;
}

bool is_valid_move() {
    if (piece_can_move(selected_square.row, selected_square.col, highlighted_square.row, highlighted_square.col)) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

// void move_piece() {
//     draw_piece(EMPTY_SQUARE, selected_square.row, selected_square.col);
//     draw_piece(board[selected_square.row][selected_square.col], highlighted_square.row, highlighted_square.col);
//     board[highlighted_square.row][highlighted_square.col] = board[selected_square.row][selected_square.col];
//     board[selected_square.row][selected_square.col] = EMPTY_SQUARE;
// }

void change_turn() {
    if (turn == WHITE) {
        turn = BLACK;
    }
    else {
        turn = WHITE;
    }
}

void UserApp1Initialize(void) {
  if (1) {
    draw_menu();
    enqueue(&state_queue, &Chess_Game_Flash_Highlighted_Square);
    enqueue(&state_queue, &Chess_Game_Flash_Movement_Indicator);
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
        return_state = Chess_Game_Selecting_Direction;
        UserApp1_pfStateMachine = Chess_Game_Selecting_Direction;
    }
}

static void Chess_Game_Lift_Piece(void) {
    draw_piece(EMPTY_SQUARE, selected_square.row, selected_square.col);
    UserApp1_pfStateMachine = return_state;
}

static void Chess_Game_Place_Piece(void) {
    draw_piece(board[selected_square.row][selected_square.col], highlighted_square.row, highlighted_square.col);
    if (attempting_en_passant) {
        board[highlighted_square.row][highlighted_square.col] = board[selected_square.row][selected_square.col];
        board[selected_square.row][selected_square.col] = EMPTY_SQUARE;
        board[en_passantable.row][en_passantable.col] = EMPTY_SQUARE;
        en_passantable.row = -1;
        en_passantable.col = -1;
        attempting_en_passant = FALSE;
    }
    else {
        board[highlighted_square.row][highlighted_square.col] = board[selected_square.row][selected_square.col];
        board[selected_square.row][selected_square.col] = EMPTY_SQUARE;
    }
    UserApp1_pfStateMachine = return_state;
}

static void Chess_Game_Update_Player_Symbols(void) {
    draw_player_symbols();
    UserApp1_pfStateMachine = return_state;
}

static void Chess_Game_Flash_Highlighted_Square(void) {
    highlight_square(highlighted_square);
    enqueue(&state_queue, &Chess_Game_Flash_Highlighted_Square);
    UserApp1_pfStateMachine = return_state;
}

static void Chess_Game_Flash_Movement_Indicator(void) {
    flash_arrow();
    if (return_state == Chess_Game_Selecting_Direction) {
        enqueue(&state_queue, &Chess_Game_Flash_Movement_Indicator);
    }
    UserApp1_pfStateMachine = return_state;
}

static void Chess_Game_Move_Highlighted_Square_Part_1(void) {
    draw_square(highlighted_square);
    UserApp1_pfStateMachine = return_state;
}

static void Chess_Game_Move_Highlighted_Square_Part_2(void) {
    if (ARE_SQUARES_EQUAL(selected_square, highlighted_square)) {
        draw_selected_square_indicator(selected_square);
    }
    highlighted_square.row = (highlighted_square.row + DIRECTIONS[direction][0] + BOARD_SIZE) % BOARD_SIZE;
    highlighted_square.col = (highlighted_square.col + DIRECTIONS[direction][1] + BOARD_SIZE) % BOARD_SIZE;
    UserApp1_pfStateMachine = return_state;
}

static void Chess_Game_Deselect_Selected_Square(void) {
    draw_square(selected_square);
    selected_square.row = -1;
    UserApp1_pfStateMachine = return_state;
}

static void Chess_Game_Attempt_Move(void) {
    if (is_valid_move(selected_square, highlighted_square)) {
        change_turn();
        enqueue(&state_queue, &Chess_Game_Lift_Piece);
        enqueue(&state_queue, &Chess_Game_Place_Piece);
        enqueue(&state_queue, &Chess_Game_Update_Player_Symbols);
    }
    enqueue(&state_queue, &Chess_Game_Deselect_Selected_Square);
    UserApp1_pfStateMachine = return_state;
}

static void Chess_Game_Selecting_Square(void) {
    draw_timer--;
    if (draw_timer == 0) {
        // Create queue of state function pointers
        UserApp1_pfStateMachine = getFront(&state_queue);
        dequeue(&state_queue);
        draw_timer = DRAW_TIMER_MAX;
    }
    else if (WasButtonPressed(BUTTON0)) {
        ButtonAcknowledge(BUTTON0);

        // Move highlighted square in the selected direction
        enqueue(&state_queue, &Chess_Game_Move_Highlighted_Square_Part_1);
        enqueue(&state_queue, &Chess_Game_Move_Highlighted_Square_Part_2);      
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
            UserApp1_pfStateMachine = Chess_Game_Attempt_Move;
        }
    }
    else if (IsButtonHeld(BUTTON0, 500)) {
        // Open direction selection menu
        UserApp1_pfStateMachine = Chess_Game_Selecting_Direction; // Change state instantly to prevent further square selection input
        return_state = Chess_Game_Selecting_Direction; // Update the return state so queued states do not switch back to this one
        enqueue(&state_queue, &Chess_Game_Flash_Movement_Indicator); // Queue direction indicator flashing
        draw_timer = DRAW_TIMER_MAX; // Reset timer to prevent rapid lcd changes when instantly changing to a new state
    }
}

static void Chess_Game_Change_Movement_Direction(void) {
    direction = (direction + 1) % DIRECTION_OPTIONS;
    draw_movement_direction();
    UserApp1_pfStateMachine = return_state;
}

static void Chess_Game_Close_Movement_Direction_Menu(void) {
    draw_movement_direction();
    UserApp1_pfStateMachine = return_state;
}

static void Chess_Game_Selecting_Direction(void) {
    draw_timer--;
    if (draw_timer == 0) {
        // Create queue of state function pointers
        UserApp1_pfStateMachine = getFront(&state_queue);
        dequeue(&state_queue);
        draw_timer = DRAW_TIMER_MAX;
    }
    else if (WasButtonPressed(BUTTON0)) {
        ButtonAcknowledge(BUTTON0);

        // Select next direction option
        enqueue(&state_queue, &Chess_Game_Change_Movement_Direction);
    }
    else if (WasButtonPressed(BUTTON1)) {
        ButtonAcknowledge(BUTTON1);

        // Close direction selection menu
        UserApp1_pfStateMachine = Chess_Game_Selecting_Square; // Change state instantly to prevent further direction selection menu input
        return_state = Chess_Game_Selecting_Square; // Update the return state so queued states do not switch back to this one
        enqueue(&state_queue, &Chess_Game_Close_Movement_Direction_Menu); // Queue state to redraw the direction indicator incase it is invisible
        draw_timer = DRAW_TIMER_MAX; // Reset timer to prevent rapid lcd changes when instantly changing to a new state
    }
    else if (IsButtonHeld(BUTTON0, 2000)) {
        // Resign game
        resign();
    }
}

static void UserApp1SM_Error(void) {
  
}
