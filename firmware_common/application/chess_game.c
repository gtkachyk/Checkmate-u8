#include "chess_app.h"

static GameResult previous_result = RESULT_NONE;
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

// static u8 board[8][8] = {
//     {10, 8, 9, 11, 12, 9, 8, 10}, 
//     {0, 7, 7, 7, 7, 7, 7, 7}, 
//     {7, 0, 0, 0, 0, 0, 0, 0}, 
//     {0, 0, 0, 1, 0, 0, 0, 0}, 
//     {0, 0, 0, 0, 0, 0, 0, 0}, 
//     {0, 0, 0, 0, 0, 0, 0, 0}, 
//     {1, 1, 1, 0, 1, 1, 1, 1}, 
//     {4, 2, 3, 5, 6, 3, 2, 4}
// };

static Colour turn = WHITE;
static uint8_t white_pawns_special_move_status = 0xFF;
static uint8_t black_pawns_special_move_status = 0xFF;
static bool white_has_short_castle_privileges = TRUE;
static bool black_has_short_castle_privileges = TRUE;
static bool white_has_long_castle_privileges = TRUE;
static bool black_has_long_castle_privileges = TRUE;
static Square en_passantable_square = {-1, -1};
static Square white_king_square = {ONE, E}; // Tracks the location of the white king
static Square black_king_square = {EIGHT, E}; // Tracks the location of the black king
static bool king_in_check = FALSE; // Tracks if the moving player's king is in check
static bool attempting_special_pawn_move = FALSE;
static bool attempting_short_castle = FALSE;
static bool attempting_long_castle = FALSE;
static bool attempting_en_passant = FALSE;
static bool attempting_promotion = FALSE;
const int8_t DIRECTIONS[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
static MovementDirection direction = UP;
static Square highlighted_square = {7, 0};
static Square selected_square = {-1, -1};
static uint8_t piece_cache;
static Move move_to_make;
static bool has_valid_move = FALSE;
static Move last_eight_moves[8];

static bool debug = FALSE;

void set_attempting_promotion(bool new_status) {
    attempting_promotion = new_status;
}

bool get_attempting_promotion() {
    return attempting_promotion;
}

bool are_moves_equal(Move move_1, Move move_2) {
    return move_1.start.col == move_2.start.col && move_1.start.row == move_2.start.row && move_1.end.col == move_2.end.col && move_1.end.row == move_2.end.row;
}

Move create_move(int8_t start_col, int8_t start_row, int8_t end_col, int8_t end_row) {
    return (Move){.start = (Square){.col = start_col, .row = start_row}, .end = (Square){.col = end_col, .row = end_row}};
}

void push_to_last_eight_moves(Move move) {
    for (int i = 7; i <= 0; i--) {
        if (last_eight_moves[i].start.row == -1) {
            last_eight_moves[i] = move;
            return;
        }
    }
    last_eight_moves[7] = last_eight_moves[6];
    last_eight_moves[6] = last_eight_moves[5];
    last_eight_moves[5] = last_eight_moves[4];
    last_eight_moves[4] = last_eight_moves[3];
    last_eight_moves[3] = last_eight_moves[2];
    last_eight_moves[2] = last_eight_moves[1];
    last_eight_moves[1] = last_eight_moves[0];
    last_eight_moves[0] = move;
}

void initialize_last_eight_moves() {
    for (int i = 0; i < 8; i++) {
        last_eight_moves[i] = create_move(-1, -1, -1, -1);
    }
}

bool get_has_valid_move() {
    return has_valid_move;
}

void set_has_valid_move(bool status) {
    has_valid_move = status;
}

bool get_king_in_check() {
    return king_in_check;
}

void set_king_in_check(bool status) {
    king_in_check = status;
}

Move get_move_to_make() {
    return move_to_make;
}

void set_move_to_make(Move move) {
    move_to_make = move;
}

Square get_white_king_square() {
    return white_king_square;
}

void set_white_king_square(Square new_square) {
    white_king_square = new_square;
}

Square get_black_king_square() {
    return black_king_square;
}

void set_black_king_square(Square new_square) {
    black_king_square = new_square;
}

int8_t* get_direction_option(MovementDirection direction_option) {
    return DIRECTIONS[direction_option];
}

void set_attempting_special_pawn_move(bool new_status) {
    attempting_special_pawn_move = new_status;
}

bool get_attempting_special_pawn_move() {
    return attempting_special_pawn_move;
}

void set_attempting_short_castle(bool new_status) {
    attempting_short_castle = new_status;
}

bool get_attempting_short_castle() {
    return attempting_short_castle;
}

void set_attempting_long_castle(bool new_status) {
    attempting_long_castle = new_status;
}

bool get_attempting_long_castle() {
    return attempting_long_castle;
}

void set_attempting_en_passant(bool new_status) {
    attempting_en_passant = new_status;
}

bool get_attempting_en_passant() {
    return attempting_en_passant;
}

void set_white_pawn_special_move_status(uint8_t file, bool new_status) {
    white_pawns_special_move_status = white_pawns_special_move_status ^ (1 << file);
}

bool get_white_pawn_special_move_status(uint8_t file) {
    return (white_pawns_special_move_status >> file) & 1;
}

void set_black_pawn_special_move_status(uint8_t file, bool new_status) {
    black_pawns_special_move_status = black_pawns_special_move_status ^ (1 << file);
}

bool get_black_pawn_special_move_status(uint8_t file) {
    return (black_pawns_special_move_status >> file) & 1;
}

Square get_en_passantable_square() {
    return en_passantable_square;
}

void set_en_passantable_square(Square new_en_passantable_square) {
    en_passantable_square = new_en_passantable_square;
}

MovementDirection get_direction() {
    return direction;
}

void set_direction(MovementDirection new_direction) {
    direction = new_direction;
}

Square get_highlighted_square() {
    return highlighted_square;
}

void set_highlighted_square(Square new_highlighted_square) {
    highlighted_square = new_highlighted_square;
}

Square get_selected_square() {
    return selected_square;
}

void set_selected_square(Square new_selected_square) {
    selected_square = new_selected_square;
}

GameResult get_previous_result() {
    return previous_result;
}

void set_previous_result(GameResult new_previous_result) {
    previous_result = new_previous_result;
}

uint8_t get_square_content(uint8_t file, uint8_t rank) {
    return board[rank][file];
}

void set_square_content(uint8_t file, uint8_t rank, uint8_t content) {
    board[rank][file] = content;
}

Colour get_turn() {
    return turn;
}

void set_turn(Colour new_turn) {
    turn = new_turn;
}

bool get_white_short_castle_privileges() {
    return white_has_short_castle_privileges;
}

void set_white_short_castle_privileges(bool new_privileges) {
    white_has_short_castle_privileges = new_privileges;
}

bool get_white_long_castle_privileges() {
    return white_has_long_castle_privileges;
}

void set_white_long_castle_privileges(bool new_privileges) {
    white_has_long_castle_privileges = new_privileges;
}

bool get_black_short_castle_privileges() {
    return black_has_short_castle_privileges;
}

void set_black_short_castle_privileges(bool new_privileges) {
    black_has_short_castle_privileges = new_privileges;
}

bool get_black_long_castle_privileges() {
    return black_has_long_castle_privileges;
}

void set_black_long_castle_privileges(bool new_privileges) {
    black_has_long_castle_privileges = new_privileges;
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

Colour get_square_colour (int row, int col) {
    return ((row ^ col) % 2 == 0) ? WHITE : BLACK;
}

bool coordinates_are_valid(uint8_t row, uint8_t col) {
    if (row > 0 || row < 7 || col > 0 || col < 7) {
        return TRUE;
    }
    return FALSE;
}

void change_turn() {
    if (turn == WHITE) {
        turn = BLACK;
    }
    else {
        turn = WHITE;
    }
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

bool squares_have_opposite_colour_pieces(Square square_1, Square square_2) {
    return get_piece_colour(get_square_content(square_1.col, square_1.row)) == !get_piece_colour(get_square_content(square_2.col, square_2.row));
}

bool is_path_clear(uint8_t row_1, uint8_t col_1, uint8_t row_2, uint8_t col_2) {
    int8_t row_dir = (row_2 > row_1) - (row_2 < row_1); // -1, 0, or 1
    int8_t col_dir = (col_2 > col_1) - (col_2 < col_1);

    uint8_t i = row_1 + row_dir;
    uint8_t j = col_1 + col_dir;
    
    while (i != row_2 || j != col_2) {
        if (get_square_content(j, i) != EMPTY_SQUARE) return FALSE;
        i += row_dir;
        j += col_dir;
    }
    return TRUE;
}

bool piece_can_move(uint8_t row_1, uint8_t col_1, uint8_t row_2, uint8_t col_2) {
    // printf("piece_can_move called with row_1 = %u, col_1 = %u\n", row_1, col_1);
    if (!coordinates_are_valid(row_1, col_1) || !coordinates_are_valid(row_2, col_2)) return FALSE;
    uint8_t piece = get_square_content(col_1, row_1);
    uint8_t destination_square = get_square_content(col_2, row_2);
    if (get_piece_colour(piece) != turn) return FALSE;
    if (get_piece_colour(piece) == get_piece_colour(destination_square)) return FALSE;
    if (piece == EMPTY_SQUARE) return FALSE;

    int8_t row_change = row_2 - row_1;
    int8_t col_change = col_2 - col_1;
    if (piece == WHITE_PAWN || piece == BLACK_PAWN) {
        int8_t pawn_direction = (piece == WHITE_PAWN) ? -1 : 1;
        if (row_2 == row_1 + pawn_direction && col_2 == col_1 && destination_square == EMPTY_SQUARE) {
            // Standard move
            return TRUE;
        }
        else if (row_2 == row_1 + pawn_direction && abs(col_2 - col_1) == 1) {
            // Capture
            if (squares_have_opposite_colour_pieces((Square){.col = col_1, .row = row_1}, (Square){.col = col_2, .row = row_2})) {
                // Standard
                return TRUE;
            }
            else if (destination_square == EMPTY_SQUARE && ARE_SQUARES_EQUAL(((Square){.col = col_2, .row = row_1}), get_en_passantable_square()) && squares_have_opposite_colour_pieces((Square){.col = col_1, .row = row_1}, (Square){.col = col_2, .row = row_1})) {
                // En passant
                attempting_en_passant = TRUE;
                return TRUE;
            }
        }
        else if (row_2 == row_1 + pawn_direction*2 && col_2 == col_1 && destination_square == EMPTY_SQUARE && get_square_content(col_1, row_1 + pawn_direction) == EMPTY_SQUARE && ((turn == WHITE && get_white_pawn_special_move_status(col_1)) || (turn == BLACK && get_black_pawn_special_move_status(col_1)))) {
            // Special move
            set_en_passantable_square((Square){.row = row_2, .col = col_2});
            attempting_special_pawn_move = TRUE;
            return TRUE;
        }
        // print_int_var("condition_1", row_2 == row_1 + pawn_direction);
        // print_int_var("condition_2", col_2 == col_1);
        // print_int_var("condition_3", destination_square == EMPTY_SQUARE);
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
            if (get_white_short_castle_privileges() && col_2 == G && get_square_content(F, ONE) == EMPTY_SQUARE && get_square_content(G, ONE) == EMPTY_SQUARE && get_square_content(H, ONE) == WHITE_ROOK) {
                attempting_short_castle = TRUE;
                return TRUE;
            }
            else if (get_white_long_castle_privileges() && col_2 == C && get_square_content(D, ONE) == EMPTY_SQUARE && get_square_content(C, ONE) == EMPTY_SQUARE && get_square_content(B, ONE) == EMPTY_SQUARE && get_square_content(A, ONE) == WHITE_ROOK) {
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
            if (get_black_short_castle_privileges() && col_2 == G && get_square_content(F, EIGHT) == EMPTY_SQUARE && get_square_content(G, EIGHT) == EMPTY_SQUARE && get_square_content(H, EIGHT) == BLACK_ROOK) {
                attempting_short_castle = TRUE;
                return TRUE;
            }
            else if (get_black_long_castle_privileges() && col_2 == C && get_square_content(D, EIGHT) == EMPTY_SQUARE && get_square_content(C, EIGHT) == EMPTY_SQUARE && get_square_content(B, EIGHT) == EMPTY_SQUARE && get_square_content(A, EIGHT) == BLACK_ROOK) {
                attempting_long_castle = TRUE;
                return TRUE;
            }
        }
    }
    return FALSE;
}

bool is_valid_move() {
    if (piece_can_move(get_selected_square().row, get_selected_square().col, get_highlighted_square().row, get_highlighted_square().col)) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

void reset_special_move_attempt_flags() {
    attempting_special_pawn_move = FALSE;
    attempting_short_castle = FALSE;
    attempting_long_castle = FALSE;
    attempting_en_passant = FALSE;
}

void move_start_to_end() {
    piece_cache = board[move_to_make.end.row][move_to_make.end.col];
    board[move_to_make.end.row][move_to_make.end.col] = board[move_to_make.start.row][move_to_make.start.col];
    board[move_to_make.start.row][move_to_make.start.col] = EMPTY_SQUARE;

    // Update king location
    if (board[move_to_make.end.row][move_to_make.end.col] == WHITE_KING) {
        white_king_square = (Square){.col = move_to_make.end.col, .row = move_to_make.end.row};
    }
    else if (board[move_to_make.end.row][move_to_make.end.col] == BLACK_KING) {
        black_king_square = (Square){.col = move_to_make.end.col, .row = move_to_make.end.row};
    }
    
    // Special moves
    if (attempting_special_pawn_move) {
        if (turn == WHITE) {
            set_white_pawn_special_move_status(move_to_make.end.col, FALSE);
        }
        else {
            set_black_pawn_special_move_status(move_to_make.end.col, FALSE);
        }
    }
    else if (attempting_short_castle) {
        if (turn == WHITE) {
            board[ONE][F] = board[ONE][H];
            board[ONE][H] = EMPTY_SQUARE;
            white_has_short_castle_privileges = FALSE;
        }
        else {
            board[EIGHT][F] = board[EIGHT][H];
            board[EIGHT][H] = EMPTY_SQUARE;
            black_has_short_castle_privileges = FALSE;
        }
    }
    else if (attempting_long_castle) {
        if (turn == WHITE) {
            board[ONE][D] = board[ONE][A];
            board[ONE][A] = EMPTY_SQUARE;
            white_has_long_castle_privileges = FALSE;
        }
        else {
            board[EIGHT][D] = board[EIGHT][A];
            board[EIGHT][A] = EMPTY_SQUARE;
            black_has_long_castle_privileges = FALSE;
        }
    }
    else if (attempting_en_passant) {
        board[move_to_make.start.row][move_to_make.end.col] = EMPTY_SQUARE;
        set_en_passantable_square((Square){.col = -1, .row = -1});
    }
}
void undo_move_start_to_end() {
    board[move_to_make.start.row][move_to_make.start.col] = board[move_to_make.end.row][move_to_make.end.col];
    board[move_to_make.end.row][move_to_make.end.col] = piece_cache;
    if (board[move_to_make.start.row][move_to_make.start.col] == WHITE_KING) {
        white_king_square = (Square){.col = move_to_make.start.col, .row = move_to_make.start.row};
    }
    else if (board[move_to_make.start.row][move_to_make.start.col] == BLACK_KING) {
        black_king_square = (Square){.col = move_to_make.start.col, .row = move_to_make.start.row};
    }

    // Special moves
    if (attempting_special_pawn_move) {
        if (turn == WHITE) {
            set_white_pawn_special_move_status(move_to_make.end.col, TRUE);
        }
        else {
            set_black_pawn_special_move_status(move_to_make.end.col, TRUE);
        }
    }
    else if (attempting_short_castle) {
        if (turn == WHITE) {
            board[ONE][H] = board[ONE][F];
            board[ONE][F] = EMPTY_SQUARE;
            white_has_short_castle_privileges = TRUE;
        }
        else {
            board[EIGHT][H] = board[EIGHT][F];
            board[EIGHT][F] = EMPTY_SQUARE;
            black_has_short_castle_privileges = TRUE;
        }
    }
    else if (attempting_long_castle) {
        if (turn == WHITE) {
            board[ONE][A] = board[ONE][D];
            board[ONE][D] = EMPTY_SQUARE;
            white_has_long_castle_privileges = TRUE;
        }
        else {
            board[EIGHT][A] = board[EIGHT][D];
            board[EIGHT][D] = EMPTY_SQUARE;
            black_has_long_castle_privileges = TRUE;
        }
    }
    else if (attempting_en_passant) {
        if (turn == WHITE) {
            board[move_to_make.start.row][move_to_make.end.col] = BLACK_PAWN;
        }
        else {
            board[move_to_make.start.row][move_to_make.end.col] = WHITE_PAWN;
        }
        set_en_passantable_square((Square){.col = move_to_make.end.col, .row = move_to_make.start.row});
    }
}

bool insufficient_material() {
    uint8_t white_bishop_count = 0;
    Square white_bishop_square = {-1, -1};
    uint8_t white_knight_count = 0;
    uint8_t black_bishop_count = 0;
    Square black_bishop_square = {-1, -1};
    uint8_t black_knight_count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            uint8_t piece = board[i][j];
            if (piece != EMPTY_SQUARE) {
                if (piece == WHITE_PAWN || piece == BLACK_PAWN || piece == WHITE_ROOK || piece == BLACK_ROOK || piece == WHITE_QUEEN || piece == BLACK_QUEEN) {
                    return FALSE;
                }
                else if (piece == WHITE_BISHOP) {
                    white_bishop_count++;
                    white_bishop_square = (Square){.col = j, .row = i};
                }
                else if (piece == WHITE_KNIGHT) {
                    white_knight_count++;
                }
                else if (piece == BLACK_BISHOP) {
                    black_bishop_count++;
                    black_bishop_square = (Square){.col = j, .row = i};
                }
                else if (piece == BLACK_KNIGHT) {
                    black_knight_count++;
                }
            }
        }
    }
    if ((white_knight_count > 1 || black_knight_count > 1) || 
        (white_bishop_count + white_knight_count > 1 || black_bishop_count + black_knight_count > 1) ||
        (white_bishop_count == 1 && black_bishop_count == 1 && get_square_colour(white_bishop_square.row, white_bishop_square.col) != get_square_colour(black_bishop_square.row, black_bishop_square.col))) {
        return FALSE;
    }
    return TRUE;
}

bool draw_by_repetition() {
    return are_moves_equal(last_eight_moves[0], last_eight_moves[4]) && are_moves_equal(last_eight_moves[1], last_eight_moves[5]) && are_moves_equal(last_eight_moves[2], last_eight_moves[6]) && are_moves_equal(last_eight_moves[3], last_eight_moves[7]);
}

void reset_game_data() {
    if (!debug) {
        previous_result = RESULT_NONE;
        reset_board();
        turn = WHITE;
        white_pawns_special_move_status = 0xFF;
        black_pawns_special_move_status = 0xFF;
        white_has_short_castle_privileges = TRUE;
        black_has_short_castle_privileges = TRUE;
        white_has_long_castle_privileges = TRUE;
        black_has_long_castle_privileges = TRUE;
        en_passantable_square = (Square){.row = -1, .col = -1};
        white_king_square = (Square){.row = 7, .col = 4};
        black_king_square = (Square){.row = 0, .col = 4};
        king_in_check = FALSE; // Tracks if the moving player's king is in check
        attempting_special_pawn_move = FALSE;
        attempting_en_passant = FALSE;
        attempting_short_castle = FALSE;
        attempting_long_castle = FALSE;
        attempting_promotion = FALSE;
        direction = UP;
        highlighted_square = (Square){.row = 7, .col = 0};
        selected_square = (Square){.row = -1, .col = -1};
        initialize_last_eight_moves();
    }
    else {
        previous_result = RESULT_NONE;
        turn = BLACK;
        white_pawns_special_move_status = 0xFF;
        black_pawns_special_move_status = 0xFF;
        white_has_short_castle_privileges = TRUE;
        black_has_short_castle_privileges = TRUE;
        white_has_long_castle_privileges = TRUE;
        black_has_long_castle_privileges = TRUE;
        en_passantable_square = (Square){.row = -1, .col = -1};
        white_king_square = (Square){.row = 7, .col = 4};
        black_king_square = (Square){.row = 0, .col = 4};
        king_in_check = FALSE;
        attempting_special_pawn_move = FALSE;
        attempting_en_passant = FALSE;
        attempting_short_castle = FALSE;
        attempting_long_castle = FALSE;
        attempting_promotion = FALSE;
        direction = DOWN;
        highlighted_square = (Square){.col = E, .row = SEVEN};
        selected_square = (Square){.row = -1, .col = -1};
        initialize_last_eight_moves();
    }
}