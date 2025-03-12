#include "chess_app.h"

// Constants
const int8_t DIRECTIONS[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
const uint8_t PROMOTIONS[4][2] = {{BLACK_KNIGHT, WHITE_KNIGHT}, {BLACK_BISHOP, WHITE_BISHOP}, {BLACK_ROOK, WHITE_ROOK}, {BLACK_QUEEN, WHITE_QUEEN}};

// Testing data
static bool debug = FALSE;
// static u8 board[8][8] = {
//     {10, 8, 9, 0, 12, 9, 8, 10}, 
//     {7, 7, 7, 7, 0, 0, 7, 7}, 
//     {0, 0, 0, 0, 1, 11, 0, 0}, 
//     {0, 0, 0, 0, 0, 1, 0, 0}, 
//     {0, 0, 0, 0, 0, 0, 0, 0}, 
//     {0, 0, 0, 0, 0, 0, 0, 2}, 
//     {1, 1, 1, 1, 1, 0, 3, 1}, 
//     {4, 2, 3, 5, 0, 6, 0, 4}
// };

// Game state
static bool puzzle_mode = FALSE;
static uint8_t current_puzzle = 0;
static GameResult previous_result = RESULT_NONE;
static Colour turn = WHITE;
static Square en_passantable_square = NULL_SQUARE;
static Square white_king_square = {ONE, E}; // Tracks the location of the white king
static Square black_king_square = {EIGHT, E}; // Tracks the location of the black king
static bool king_in_check = FALSE; // Tracks if the moving player's king is in check
static bool has_valid_move = FALSE;
static Move last_eight_moves[8];
static int8_t fifty_move_rule_counter = 0;
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

// Interface state
static MovementDirection direction = UP;
static PromotionOption promotion = KNIGHT;
static Square highlighted_square = {7, 0};
static Square selected_square = {-1, -1};

// Piece statuses
static uint8_t white_pawns_special_move_privileges = 0xFF; // Doesn't work when a pawn moves into another col
static uint8_t black_pawns_special_move_privileges = 0xFF;
static bool white_has_short_castle_privileges = TRUE;
static bool black_has_short_castle_privileges = TRUE;
static bool white_has_long_castle_privileges = TRUE;
static bool black_has_long_castle_privileges = TRUE;

// Movement validation flags
static bool attempting_special_pawn_move = FALSE; // Only needed to prevent en_passantable_square from being reset after a pawn moves two squares
static bool attempting_promotion = FALSE;
static bool attempting_en_passant = FALSE;
static bool attempting_short_castle = FALSE;
static bool attempting_long_castle = FALSE;

static bool pawn_loses_special_move_privilege = FALSE;
static bool king_loses_short_castle_privileges = FALSE;
static bool king_loses_long_castle_privileges = FALSE;

// Temp data
static uint8_t piece_cache;
static Move move_to_make;
static Move move_to_make_part_two;
static bool preserve_validation_flags = FALSE;

void set_fifty_move_rule_counter(int8_t new_val) {
    fifty_move_rule_counter = new_val;
}

int8_t get_fifty_move_rule_counter() {
    return fifty_move_rule_counter;
}

void set_preserve_validation_flags(bool state) {
    preserve_validation_flags = state;
}

bool get_preserve_validation_flags() {
    return preserve_validation_flags;
}

void set_current_puzzle(uint8_t puzzle) {
    current_puzzle = puzzle;
}

uint8_t get_current_puzzle() {
    return current_puzzle;
}

void set_puzzle_mode(bool state) {
    puzzle_mode = state;
}

bool get_puzzle_mode() {
    return puzzle_mode;
}

void set_move_to_make_part_two(Move move) {
    move_to_make_part_two = move;
}

Move get_move_to_make_part_two() {
    return move_to_make_part_two;
}

PromotionOption get_promotion() {
    return promotion;
}

void set_promotion(PromotionOption new_promotion) {
    promotion = new_promotion;
}

uint8_t* get_promotion_option(PromotionOption promotion_option, Colour turn) {
    return PROMOTIONS[promotion_option][turn];
}

void set_king_loses_short_castle_privileges(bool new_status) {
    king_loses_short_castle_privileges = new_status;
}

bool get_king_loses_short_castle_privileges() {
    return king_loses_short_castle_privileges;
}

void set_king_loses_long_castle_privileges(bool new_status) {
    king_loses_long_castle_privileges = new_status;
}

bool get_king_loses_long_castle_privileges() {
    return king_loses_long_castle_privileges;
}

void set_pawn_loses_special_move_privilege(bool new_status) {
    pawn_loses_special_move_privilege = new_status;
}

bool get_pawn_loses_special_move_privilege() {
    return pawn_loses_special_move_privilege;
}

void set_attempting_promotion(bool new_status) {
    attempting_promotion = new_status;
}

bool get_attempting_promotion() {
    return attempting_promotion;
}

bool are_moves_equal(Move move_1, Move move_2) {
    return move_1.start.col == move_2.start.col && move_1.start.row == move_2.start.row && move_1.end.col == move_2.end.col && move_1.end.row == move_2.end.row;
}

Move create_move(int8_t start_col, int8_t start_row, int8_t end_col, int8_t end_row, PromotionOption promotion_option) {
    return (Move){.start = (Square){.col = start_col, .row = start_row}, .end = (Square){.col = end_col, .row = end_row}, .promotion_selection = promotion_option};
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
        last_eight_moves[i] = create_move(-1, -1, -1, -1, NO_PROMOTION);
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

void remove_white_pawn_special_move_privileges(uint8_t file) {
    if (get_white_pawn_special_move_privileges(file)) {
        white_pawns_special_move_privileges = white_pawns_special_move_privileges ^ (1 << file);
    }
}

bool get_white_pawn_special_move_privileges(uint8_t file) {
    return (white_pawns_special_move_privileges >> file) & 1;
}

void remove_black_pawn_special_move_privileges(uint8_t file) {
    if (get_black_pawn_special_move_privileges(file)) {
        black_pawns_special_move_privileges = black_pawns_special_move_privileges ^ (1 << file);
    }
}

bool get_black_pawn_special_move_privileges(uint8_t file) {
    return (black_pawns_special_move_privileges >> file) & 1;
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
        return (uint8_t)WHITE;
    }
    else if (piece > 6 && piece < 13) {
        return (uint8_t)BLACK;
    }
    else {
        return UNDEFINED_COLOUR;
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

void set_rank (uint8_t rank, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h) {
    board[rank][0] = a;
    board[rank][1] = b;
    board[rank][2] = c;
    board[rank][3] = d;
    board[rank][4] = e;
    board[rank][5] = f;
    board[rank][6] = g;
    board[rank][7] = h;
}

void load_puzzle_0() {
    set_rank(EIGHT, BLACK_ROOK,     EMPTY_SQUARE,   BLACK_BISHOP,   EMPTY_SQUARE,   EMPTY_SQUARE,   BLACK_BISHOP,   EMPTY_SQUARE,   BLACK_ROOK);
    set_rank(SEVEN, BLACK_PAWN,     BLACK_PAWN,     EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   WHITE_QUEEN,    BLACK_PAWN,     EMPTY_SQUARE);
    set_rank(SIX,   EMPTY_SQUARE,   EMPTY_SQUARE,   BLACK_KNIGHT,   BLACK_KING,     BLACK_KNIGHT,   EMPTY_SQUARE,   EMPTY_SQUARE,   BLACK_PAWN);
    set_rank(FIVE,  EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   BLACK_PAWN,     BLACK_PAWN,     WHITE_PAWN,     EMPTY_SQUARE,   BLACK_PAWN);
    set_rank(FOUR,  WHITE_PAWN,     EMPTY_SQUARE,   BLACK_PAWN,     EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE);
    set_rank(THREE, EMPTY_SQUARE,   WHITE_KNIGHT,   WHITE_PAWN,     EMPTY_SQUARE,   WHITE_KNIGHT,   WHITE_BISHOP,   EMPTY_SQUARE,   EMPTY_SQUARE);
    set_rank(TWO,   EMPTY_SQUARE,   WHITE_PAWN,     WHITE_PAWN,     EMPTY_SQUARE,   WHITE_PAWN,     WHITE_PAWN,     WHITE_ROOK,     EMPTY_SQUARE);
    set_rank(ONE,   EMPTY_SQUARE,   WHITE_KING,     EMPTY_SQUARE,   WHITE_ROOK,     EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   BLACK_QUEEN);
    white_pawns_special_move_privileges = 0x6C;
    black_pawns_special_move_privileges = 0xC2;
    white_has_short_castle_privileges = FALSE;
    black_has_short_castle_privileges = FALSE;
    white_has_long_castle_privileges = FALSE;
    black_has_long_castle_privileges = FALSE;
    en_passantable_square = NULL_SQUARE;
    white_king_square = (Square){.col = B, .row = ONE};
    black_king_square = (Square){.col = D, .row = SIX};
    turn = WHITE;
}

void load_puzzle_1() {
    set_rank(EIGHT, EMPTY_SQUARE,   EMPTY_SQUARE,   WHITE_KING,     EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE);
    set_rank(SEVEN, WHITE_QUEEN,    EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE);
    set_rank(SIX,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE);
    set_rank(FIVE,  EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE);
    set_rank(FOUR,  EMPTY_SQUARE,   WHITE_KNIGHT,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE);
    set_rank(THREE, EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE);
    set_rank(TWO,   BLACK_PAWN,     BLACK_PAWN,     BLACK_PAWN,     BLACK_PAWN,     BLACK_PAWN,     BLACK_PAWN,     BLACK_PAWN,     BLACK_PAWN);
    set_rank(ONE,   BLACK_ROOK,     BLACK_KNIGHT,   BLACK_BISHOP,   BLACK_QUEEN,    BLACK_KING,     BLACK_BISHOP,   BLACK_KNIGHT,   BLACK_ROOK);
    white_pawns_special_move_privileges = 0x00;
    black_pawns_special_move_privileges = 0x00;
    white_has_short_castle_privileges = FALSE;
    black_has_short_castle_privileges = FALSE;
    white_has_long_castle_privileges = FALSE;
    black_has_long_castle_privileges = FALSE;
    en_passantable_square = NULL_SQUARE;
    white_king_square = (Square){.col = C, .row = EIGHT};
    black_king_square = (Square){.col = E, .row = ONE};
    turn = WHITE;
}

void load_puzzle_2() {
    set_rank(EIGHT, EMPTY_SQUARE,   BLACK_KING,     EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE);
    set_rank(SEVEN, EMPTY_SQUARE,   EMPTY_SQUARE,   BLACK_PAWN,     EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   BLACK_PAWN,     EMPTY_SQUARE);
    set_rank(SIX,   EMPTY_SQUARE,   BLACK_PAWN,     EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   WHITE_PAWN,     EMPTY_SQUARE);
    set_rank(FIVE,  EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE);
    set_rank(FOUR,  EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   WHITE_KING);
    set_rank(THREE, EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   BLACK_BISHOP,   BLACK_QUEEN,    EMPTY_SQUARE,   EMPTY_SQUARE);
    set_rank(TWO,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   BLACK_PAWN,     EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE);
    set_rank(ONE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE);
    white_pawns_special_move_privileges = 0x00;
    black_pawns_special_move_privileges = 0x22;
    white_has_short_castle_privileges = FALSE;
    black_has_short_castle_privileges = FALSE;
    white_has_long_castle_privileges = FALSE;
    black_has_long_castle_privileges = FALSE;
    en_passantable_square = NULL_SQUARE;
    white_king_square = (Square){.col = H, .row = FOUR};
    black_king_square = (Square){.col = B, .row = EIGHT};
    turn = BLACK;
}

void reset_board() {
    set_rank(EIGHT, BLACK_ROOK,     BLACK_KNIGHT,   BLACK_BISHOP,   BLACK_QUEEN,    BLACK_KING,     BLACK_BISHOP,   BLACK_KNIGHT,   BLACK_ROOK);
    set_rank(SEVEN, BLACK_PAWN,     BLACK_PAWN,     BLACK_PAWN,     BLACK_PAWN,     BLACK_PAWN,     BLACK_PAWN,     BLACK_PAWN,     BLACK_PAWN);
    set_rank(SIX,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE);
    set_rank(FIVE,  EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE);
    set_rank(FOUR,  EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE);
    set_rank(THREE, EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE,   EMPTY_SQUARE);
    set_rank(TWO,   WHITE_PAWN,     WHITE_PAWN,     WHITE_PAWN,     WHITE_PAWN,     WHITE_PAWN,     WHITE_PAWN,     WHITE_PAWN,     WHITE_PAWN);
    set_rank(ONE,   WHITE_ROOK,     WHITE_KNIGHT,   WHITE_BISHOP,   WHITE_QUEEN,    WHITE_KING,     WHITE_BISHOP,   WHITE_KNIGHT,   WHITE_ROOK);
}

void set_movement_validation_flags(bool special_pawn_move, bool special_move_privilege, bool promotion, bool en_passant, bool short_castle, bool long_castle, bool loses_short_castle, bool loses_long_castle) {
    attempting_special_pawn_move = special_pawn_move;
    pawn_loses_special_move_privilege = special_move_privilege;
    attempting_promotion = promotion;
    attempting_en_passant = en_passant;
    attempting_short_castle = short_castle;
    attempting_long_castle = long_castle;
    king_loses_short_castle_privileges = loses_short_castle;
    king_loses_long_castle_privileges = loses_long_castle;
}

bool squares_have_opposite_colour_pieces(Square square_1, Square square_2) {
    if ((get_piece_colour(get_square_content(square_1.col, square_1.row)) == UNDEFINED_COLOUR) || (get_piece_colour(get_square_content(square_2.col, square_2.row)) == UNDEFINED_COLOUR)) {
        return FALSE;
    }
    if (get_piece_colour(get_square_content(square_1.col, square_1.row)) != get_piece_colour(get_square_content(square_2.col, square_2.row))) {
        return TRUE;
    }
    return FALSE;
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
            if (!preserve_validation_flags) {
                turn == WHITE ? set_movement_validation_flags(FALSE, row_1 == TWO, row_2 == EIGHT, FALSE, FALSE, FALSE, FALSE, FALSE) : set_movement_validation_flags(FALSE, row_1 == SEVEN, row_2 == ONE, FALSE, FALSE, FALSE, FALSE, FALSE);
            }
            return TRUE;
        }
        else if (row_2 == row_1 + pawn_direction && abs(col_2 - col_1) == 1) {
            // Capture
            if (squares_have_opposite_colour_pieces((Square){.col = col_1, .row = row_1}, (Square){.col = col_2, .row = row_2})) {
                // Standard
                if (!preserve_validation_flags) {
                    turn == WHITE ? set_movement_validation_flags(FALSE, row_1 == TWO, row_2 == EIGHT, FALSE, FALSE, FALSE, FALSE, FALSE) : set_movement_validation_flags(FALSE, row_1 == SEVEN, row_2 == ONE, FALSE, FALSE, FALSE, FALSE, FALSE);
                }
                return TRUE;
            }
            else if (destination_square == EMPTY_SQUARE && ARE_SQUARES_EQUAL(((Square){.col = col_2, .row = row_1}), get_en_passantable_square()) && squares_have_opposite_colour_pieces((Square){.col = col_1, .row = row_1}, (Square){.col = col_2, .row = row_1})) {
                // En passant
                if (!preserve_validation_flags) {
                    set_movement_validation_flags(FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE);
                }
                return TRUE;
            }
        }
        else if (row_2 == row_1 + pawn_direction*2 && col_2 == col_1 && destination_square == EMPTY_SQUARE && get_square_content(col_1, row_1 + pawn_direction) == EMPTY_SQUARE && ((turn == WHITE && get_white_pawn_special_move_privileges(col_1) && row_1 == TWO) || (turn == BLACK && get_black_pawn_special_move_privileges(col_1) && row_1 == SEVEN))) {
            // Special move
            if (!preserve_validation_flags) {
                set_movement_validation_flags(TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
            }
            return TRUE;
        }
    }
    else if (piece == WHITE_KNIGHT || piece == BLACK_KNIGHT) {
        if (((abs(row_2 - row_1) == 2 && abs(col_2 - col_1) == 1) || (abs(row_2 - row_1) == 1 && abs(col_2 - col_1) == 2))) {
            if (!preserve_validation_flags) {
                set_movement_validation_flags(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
            }
            return TRUE;
        }
    }
    else if (piece == WHITE_BISHOP || piece == BLACK_BISHOP) {
        if ((abs(row_change) == abs(col_change) && is_path_clear(row_1, col_1, row_2, col_2))) {
            if (!preserve_validation_flags) {
                set_movement_validation_flags(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
            }
            return TRUE;
        }
    }
    else if (piece == WHITE_ROOK || piece == BLACK_ROOK) {
        if (((row_1 == row_2 || col_1 == col_2) && is_path_clear(row_1, col_1, row_2, col_2))) {
            if (col_1 == H && ((turn == WHITE && white_has_short_castle_privileges) || (turn == BLACK && black_has_short_castle_privileges))) {
                if (!preserve_validation_flags) {
                    set_movement_validation_flags(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE);
                }
            }
            else if (col_1 == A && ((turn == WHITE && white_has_long_castle_privileges) || (turn == BLACK && black_has_long_castle_privileges))) {
                if (!preserve_validation_flags) {
                    set_movement_validation_flags(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE);
                }
            }
            else {
                if (!preserve_validation_flags) {
                    set_movement_validation_flags(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
                }
            }
            return TRUE;
        }
    }
    else if (piece == WHITE_QUEEN || piece == BLACK_QUEEN) {
        if (((abs(row_change) == abs(col_change) || row_1 == row_2 || col_1 == col_2) && is_path_clear(row_1, col_1, row_2, col_2))) {
            if (!preserve_validation_flags) {
                set_movement_validation_flags(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
            }
            return TRUE;
        }
    }
    else if (piece == WHITE_KING) {
        // Standard movement
        if ((row_change == 0 || abs(row_change) == 1) && (col_change == 0 || abs(col_change) == 1)) {
            if (!preserve_validation_flags) {
                set_movement_validation_flags(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE);
            }
            return TRUE;
        }
        else if (row_1 == ONE && col_1 == E && row_2 == ONE) {
            if (get_white_short_castle_privileges() && col_2 == G && get_square_content(F, ONE) == EMPTY_SQUARE && get_square_content(G, ONE) == EMPTY_SQUARE && get_square_content(H, ONE) == WHITE_ROOK) {
                if (!preserve_validation_flags) {
                    set_movement_validation_flags(FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, TRUE, TRUE);
                }
                return TRUE;
            }
            else if (get_white_long_castle_privileges() && col_2 == C && get_square_content(D, ONE) == EMPTY_SQUARE && get_square_content(C, ONE) == EMPTY_SQUARE && get_square_content(B, ONE) == EMPTY_SQUARE && get_square_content(A, ONE) == WHITE_ROOK) {
                if (!preserve_validation_flags) {
                    set_movement_validation_flags(FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE);
                }
                return TRUE;
            }
        }
    }
    else if (piece == BLACK_KING) {
        // Standard movement
        if ((row_change == 0 || abs(row_change) == 1) && (col_change == 0 || abs(col_change) == 1)) {
            if (!preserve_validation_flags) {
                set_movement_validation_flags(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE);
            }
            return TRUE;
        }
        else if (row_1 == EIGHT && col_1 == E && row_2 == EIGHT) {
            if (get_black_short_castle_privileges() && col_2 == G && get_square_content(F, EIGHT) == EMPTY_SQUARE && get_square_content(G, EIGHT) == EMPTY_SQUARE && get_square_content(H, EIGHT) == BLACK_ROOK) {
                if (!preserve_validation_flags) {
                    set_movement_validation_flags(FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, TRUE, TRUE);
                }
                return TRUE;
            }
            else if (get_black_long_castle_privileges() && col_2 == C && get_square_content(D, EIGHT) == EMPTY_SQUARE && get_square_content(C, EIGHT) == EMPTY_SQUARE && get_square_content(B, EIGHT) == EMPTY_SQUARE && get_square_content(A, EIGHT) == BLACK_ROOK) {
                if (!preserve_validation_flags) {
                    set_movement_validation_flags(FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE);
                }
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

void reset_movement_validation_flags() {
    attempting_en_passant = FALSE;
    attempting_special_pawn_move = FALSE;
    pawn_loses_special_move_privilege = FALSE;
    attempting_promotion = FALSE;

    attempting_short_castle = FALSE;
    attempting_long_castle = FALSE;
    king_loses_short_castle_privileges = FALSE;
    king_loses_long_castle_privileges = FALSE;
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
        turn = WHITE;
        white_pawns_special_move_privileges = 0xFF;
        black_pawns_special_move_privileges = 0xFF;
        white_has_short_castle_privileges = TRUE;
        black_has_short_castle_privileges = TRUE;
        white_has_long_castle_privileges = TRUE;
        black_has_long_castle_privileges = TRUE;
        en_passantable_square = (Square){.row = -1, .col = -1};
        white_king_square = (Square){.row = 7, .col = 4};
        black_king_square = (Square){.row = 0, .col = 4};
        king_in_check = FALSE;
        reset_movement_validation_flags();
        direction = UP;
        promotion = KNIGHT;
        highlighted_square = (Square){.row = 7, .col = 0};
        selected_square = (Square){.row = -1, .col = -1};
        initialize_last_eight_moves();
        fifty_move_rule_counter = 0;

        if (puzzle_mode) {
            if (current_puzzle == 0) {
                load_puzzle_0();
            }
            else if (current_puzzle == 1) {
                load_puzzle_1();
            }
            else if (current_puzzle == 2) {
                load_puzzle_2();
            }
        }
        else {
            if (!get_skip_reset()) {
                reset_board();
            }
        }
    }
    else {
        previous_result = RESULT_NONE;
        turn = BLACK;
        white_pawns_special_move_privileges = 0b11111001;
        black_pawns_special_move_privileges = 0b11110001;
        white_has_short_castle_privileges = FALSE;
        black_has_short_castle_privileges = TRUE;
        white_has_long_castle_privileges = FALSE;
        black_has_long_castle_privileges = TRUE;
        en_passantable_square = (Square){.row = -1, .col = -1};
        white_king_square = (Square){.row = ONE, .col = F};
        black_king_square = (Square){.row = EIGHT, .col = E};
        king_in_check = FALSE;
        reset_movement_validation_flags();
        direction = UP;
        promotion = KNIGHT;
        highlighted_square = (Square){.col = A, .row = ONE};
        selected_square = (Square){.row = -1, .col = -1};
        initialize_last_eight_moves();
        fifty_move_rule_counter = 0;
    }
}