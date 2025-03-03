#include "chess_app.h"

/* Variables */
extern volatile u32 G_u32SystemTime1ms;
extern volatile u32 G_u32SystemTime1s;
extern volatile u32 G_u32SystemFlags;
extern volatile u32 G_u32ApplicationFlags;
volatile u32 G_u32ChessAppFlags;
static fnCode_type ChessApp_pfStateMachine;
static fnCode_type return_state;
int ChessApp_au8Name[];
extern u8 G_au8DebugScanBuffer[DEBUG_SCANF_BUFFER_SIZE];
extern u8 G_u8DebugScanfCharCount;
static int queue_timer = QUEUE_TIMER_MAX;
static Queue state_queue;
static uint8_t i = 0;
static uint8_t j = 0;
static uint8_t k = 0;
static uint8_t l = 0;
static uint8_t x = 0;
static uint8_t y = 0;
static bool controls_locked = FALSE;
static bool analysis_mode = FALSE; // Analysis mode is when the program is checking if the current player has a valid move
static bool debug = TRUE;
static uint8_t debug_action = 5;
static uint8_t debug_inputs_index = 0;
static uint8_t debug_moves_index = 0;
uint8_t debug_inputs_size;
uint8_t debug_moves_size;
static Move* debug_moves;
static uint8_t debug_inputs[64]; // Max inputs is 50 with current implementation
static uint8_t debug_timer = 500;

/* Functions */
void create_game_1_moves() {
    debug_moves[0] = create_move(E, TWO, E, FOUR, NO_PROMOTION);
    debug_moves[1] = create_move(E, SEVEN, E, FIVE, NO_PROMOTION);

    debug_moves[2] = create_move(G, ONE, F, THREE, NO_PROMOTION);
    debug_moves[3] = create_move(B, EIGHT, C, SIX, NO_PROMOTION);

    debug_moves[4] = create_move(D, TWO, D, FOUR, NO_PROMOTION);
    debug_moves[5] = create_move(E, FIVE, D, FOUR, NO_PROMOTION);

    debug_moves[6] = create_move(F, ONE, C, FOUR, NO_PROMOTION);
    debug_moves[7] = create_move(F, EIGHT, C, FIVE, NO_PROMOTION);

    debug_moves[8] = create_move(C, TWO, C, THREE, NO_PROMOTION);
    debug_moves[9] = create_move(D, FOUR, C, THREE, NO_PROMOTION);

    debug_moves[10] = create_move(C, FOUR, F, SEVEN, NO_PROMOTION);
    debug_moves[11] = create_move(E, EIGHT, F, SEVEN, NO_PROMOTION);

    debug_moves[12] = create_move(D, ONE, D, FIVE, NO_PROMOTION);
    debug_moves[13] = create_move(F, SEVEN, E, EIGHT, NO_PROMOTION);

    debug_moves[14] = create_move(D, FIVE, H, FIVE, NO_PROMOTION);
    debug_moves[15] = create_move(G, SEVEN, G, SIX, NO_PROMOTION);

    debug_moves[16] = create_move(H, FIVE, C, FIVE, NO_PROMOTION);
    debug_moves[17] = create_move(D, SEVEN, D, SIX, NO_PROMOTION);

    debug_moves[18] = create_move(C, FIVE, C, THREE, NO_PROMOTION);
    debug_moves[19] = create_move(C, EIGHT, G, FOUR, NO_PROMOTION);

    debug_moves[20] = create_move(C, THREE, H, EIGHT, NO_PROMOTION);
    debug_moves[21] = create_move(G, FOUR, F, THREE, NO_PROMOTION);

    debug_moves[22] = create_move(H, EIGHT, G, EIGHT, NO_PROMOTION);
    debug_moves[23] = create_move(E, EIGHT, D, SEVEN, NO_PROMOTION);

    debug_moves[24] = create_move(G, EIGHT, F, SEVEN, NO_PROMOTION);
    debug_moves[25] = create_move(D, EIGHT, E, SEVEN, NO_PROMOTION);

    debug_moves[26] = create_move(F, SEVEN, F, THREE, NO_PROMOTION);
    debug_moves[27] = create_move(A, EIGHT, F, EIGHT, NO_PROMOTION);

    debug_moves[28] = create_move(F, THREE, G, FOUR, NO_PROMOTION);
    debug_moves[29] = create_move(D, SEVEN, D, EIGHT, NO_PROMOTION);

    debug_moves[30] = create_move(E, ONE, G, ONE, NO_PROMOTION);
    debug_moves[31] = create_move(C, SIX, D, FOUR, NO_PROMOTION);

    debug_moves[32] = create_move(G, FOUR, D, ONE, NO_PROMOTION);
    debug_moves[33] = create_move(E, SEVEN, E, FOUR, NO_PROMOTION);

    debug_moves[34] = create_move(C, ONE, G, FIVE, NO_PROMOTION);
    debug_moves[35] = create_move(D, EIGHT, C, EIGHT, NO_PROMOTION);

    debug_moves[36] = create_move(F, ONE, E, ONE, NO_PROMOTION);
    debug_moves[37] = create_move(E, FOUR, F, FIVE, NO_PROMOTION);

    debug_moves[38] = create_move(G, FIVE, E, THREE, NO_PROMOTION);
    debug_moves[39] = create_move(D, FOUR, C, SIX, NO_PROMOTION);

    debug_moves[40] = create_move(B, ONE, C, THREE, NO_PROMOTION);
    debug_moves[41] = create_move(C, SIX, E, FIVE, NO_PROMOTION);

    debug_moves[42] = create_move(H, TWO, H, THREE, NO_PROMOTION);
    debug_moves[43] = create_move(H, SEVEN, H, FIVE, NO_PROMOTION);

    debug_moves[44] = create_move(C, THREE, D, FIVE, NO_PROMOTION);
    debug_moves[45] = create_move(F, EIGHT, F, SEVEN, NO_PROMOTION);

    debug_moves[46] = create_move(E, THREE, F, FOUR, NO_PROMOTION);
    debug_moves[47] = create_move(G, SIX, G, FIVE, NO_PROMOTION);

    debug_moves[48] = create_move(F, FOUR, E, FIVE, NO_PROMOTION);
    debug_moves[49] = create_move(D, SIX, E, FIVE, NO_PROMOTION);

    debug_moves[50] = create_move(D, ONE, E, TWO, NO_PROMOTION);
    debug_moves[51] = create_move(G, FIVE, G, FOUR, NO_PROMOTION);

    debug_moves[52] = create_move(E, TWO, E, FIVE, NO_PROMOTION);
    debug_moves[53] = create_move(F, FIVE, F, TWO, NO_PROMOTION);

    debug_moves[54] = create_move(G, ONE, H, ONE, NO_PROMOTION);
    debug_moves[55] = create_move(B, SEVEN, B, SIX, NO_PROMOTION);

    debug_moves[56] = create_move(D, FIVE, E, SEVEN, NO_PROMOTION);
    debug_moves[57] = create_move(C, EIGHT, B, EIGHT, NO_PROMOTION);

    debug_moves[58] = create_move(E, SEVEN, C, SIX, NO_PROMOTION);
    debug_moves[59] = create_move(B, EIGHT, C, EIGHT, NO_PROMOTION);

    debug_moves[60] = create_move(A, ONE, D, ONE, NO_PROMOTION);
    debug_moves[61] = create_move(G, FOUR, H, THREE, NO_PROMOTION);

    debug_moves[62] = create_move(E, FIVE, E, SIX, NO_PROMOTION);
    debug_moves[63] = create_move(C, EIGHT, B, SEVEN, NO_PROMOTION);

    debug_moves[64] = create_move(C, SIX, D, EIGHT, NO_PROMOTION);
    debug_moves[65] = create_move(B, SEVEN, B, EIGHT, NO_PROMOTION);

    debug_moves[66] = create_move(E, SIX, E, TWO, NO_PROMOTION);
    debug_moves[67] = create_move(H, THREE, G, TWO, NO_PROMOTION);

    debug_moves[68] = create_move(H, ONE, H, TWO, NO_PROMOTION);
    debug_moves[69] = create_move(G, TWO, G, ONE, QUEEN);

    debug_moves[70] = create_move(H, TWO, H, THREE, NO_PROMOTION);
    debug_moves[71] = create_move(G, ONE, G, THREE, NO_PROMOTION);
}

void create_test_moves() {
    debug_moves[0] = create_move(H, TWO, H, THREE, NO_PROMOTION);
    debug_moves[1] = create_move(G, ONE, G, THREE, NO_PROMOTION);
}

void create_debug_moves() {
    debug_moves_size = 72;
    debug_moves = (Move*)malloc(debug_moves_size * sizeof(Move));
    if (debug_moves == NULL) {
        ChessApp_pfStateMachine = ChessAppSM_Error;
        return;
    }
    // create_test_moves();
    create_game_1_moves(); // set size to 72
}

void add_debug_input(uint8_t* index, uint8_t value) {
    // print_int_var("value", value);
    debug_inputs[*index] = value;
    *index = *index + 1;
}

void generate_input_select_promotion(uint8_t* index, PromotionOption* current_promotion, PromotionOption new_promotion) {
    while (*current_promotion != new_promotion) {
        add_debug_input(index, BUTTON0);
        *current_promotion = (*current_promotion + 1) % PROMOTION_OPTIONS;
    }
    add_debug_input(index, BUTTON1);
}

void generate_input_select_direction(uint8_t* index, MovementDirection* current_direction, MovementDirection new_direction) {
    while (*current_direction != new_direction) {
        add_debug_input(index, BUTTON0);
        *current_direction = (*current_direction + 1) % DIRECTION_OPTIONS;
    }
    add_debug_input(index, BUTTON1);
}

void generate_input_select_square(uint8_t* index, MovementDirection* current_direction, PromotionOption* current_promotion, Square square_1, Square square_2) {
    int8_t row_change = square_2.row - square_1.row;
    if (row_change > 0 && *current_direction != DOWN) {
        add_debug_input(index, DIRECTION_SELECTION_BUTTON);
        generate_input_select_direction(index, current_direction, DOWN);
    }
    else if (row_change < 0 && *current_direction != UP) {
        add_debug_input(index, DIRECTION_SELECTION_BUTTON);
        generate_input_select_direction(index, current_direction, UP);
    }

    // Move to the correct row
    int8_t row_dir = (square_2.row > square_1.row) - (square_2.row < square_1.row);
    uint8_t current_row = square_1.row;
    while (current_row != square_2.row) {
        add_debug_input(index, BUTTON0);
        current_row += row_dir;
    }

    // Open the direction selection menu if needed
    int8_t col_change = square_2.col - square_1.col;
    if (col_change > 0 && *current_direction != RIGHT) {
        add_debug_input(index, DIRECTION_SELECTION_BUTTON);
        generate_input_select_direction(index, current_direction, RIGHT);
    }
    else if (col_change < 0 && *current_direction != LEFT) {
        add_debug_input(index, DIRECTION_SELECTION_BUTTON);
        generate_input_select_direction(index, current_direction, LEFT);
    }

    // Move to the correct col
    int8_t col_dir = (square_2.col > square_1.col) - (square_2.col < square_1.col);
    uint8_t current_col = square_1.col;
    while (current_col != square_2.col) {
        add_debug_input(index, BUTTON0);
        current_col += col_dir;
    }

    // Select the square
    add_debug_input(index, BUTTON1);
}

void generate_button_input(Move move, uint8_t index) {
    Square highlighted_square = get_highlighted_square();
    MovementDirection direction = get_direction();
    PromotionOption promotion = get_promotion();
    generate_input_select_square(&index, &direction, &promotion, highlighted_square, move.start);
    generate_input_select_square(&index, &direction, &promotion, move.start, move.end);
    if (move.promotion_selection != NO_PROMOTION) {
        generate_input_select_promotion(&index, &promotion, move.promotion_selection);
    }
    debug_inputs_size = index;
}

Node* create_node(fnCode_type new_data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = new_data;
    new_node->next = NULL;
    return new_node;
}

Queue* create_queue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    return q;
}

bool is_empty(Queue* q) {
    if (q->front == NULL && q->rear == NULL) {
        return TRUE;
    }
    return FALSE;
}

void enqueue(Queue* q, fnCode_type new_data) {
    Node* new_node = create_node(new_data);
    if (q->rear == NULL) {
        q->front = q->rear = new_node;
        return;
    }
    q->rear->next = new_node;
    q->rear = new_node;
}

void dequeue(Queue* q) {
    if (is_empty(q)) {
        return;
    }
    Node* temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL)
        q->rear = NULL;
    free(temp);
}

fnCode_type get_front(Queue* q) {
    if (is_empty(q)) {
        return NULL;
    }
    return q->front->data;
}

fnCode_type get_rear(Queue* q) {
    if (is_empty(q)) {
        return NULL;
    }
    return q->rear->data;
}

void resign() {
    set_king_in_check(FALSE);
    set_check_indicator();
    set_previous_result(RESULT_RESIGNATION);
    draw_menu(get_previous_result(), get_turn());
    ChessApp_pfStateMachine = Chess_Menu;
}

void reset_app_data() {
    controls_locked = FALSE;
    analysis_mode = FALSE;
    queue_timer = QUEUE_TIMER_MAX;
    return_state = NULL;
    i = 0;
    j = 0;
    k = 0;
    l = 0;
    x = 0;
    y = 0;

    // Clear the state queue
    while (get_front(&state_queue) != NULL) {
        dequeue(&state_queue);
    }
}

void setup_new_game() {
    reset_app_data();
    reset_game_data();
    draw_game_interface();
}

void queue_post_move_analysis() {
    analysis_mode = TRUE;
    set_has_valid_move(FALSE);
    set_king_in_check(FALSE);
    set_check_indicator();
    enqueue(&state_queue, &Post_Move_Check_Is_Draw_By_Repetition); // Check if the 2nd and 1st most recent moves are identical to the 4th and 3rd most recent moves
    enqueue(&state_queue, &Post_Move_Check_Is_In_Check); // Check if king is in check
    enqueue(&state_queue, &Post_Move_Check_Is_Sufficient_Material); // Check if there is sufficient material
    enqueue(&state_queue, &Post_Move_Check_Has_Valid_Move); // Check if moving player has any valid moves
}

void invalidate_castling_attempt(void) {
    change_turn();
    reset_special_move_attempt_flags();
    if (!analysis_mode) {
        enqueue(&state_queue, &Deselect_Selected_Square);
    }
    i = 0;
    j = 0;
    ChessApp_pfStateMachine = return_state;
}

void ChessAppInitialize(void) {
  if (1) {
    if (debug) {
        reset_game_data();
        create_debug_moves();
        debug_inputs[0] = BUTTON1;
        generate_button_input(debug_moves[debug_moves_index], 1);
        debug_moves_index++;
    }
    draw_menu(get_previous_result(), get_turn());
    state_queue = *(create_queue());
    ChessApp_pfStateMachine = Chess_Menu;
  }
  else {
    ChessApp_pfStateMachine = ChessAppSM_Error;
  }
}

void ChessAppRunActiveState(void) {
  ChessApp_pfStateMachine();
}

// State Machine Function Definitions
static void Chess_Menu(void) {
    if (WasButtonPressed(BUTTON0)) {
        ButtonAcknowledge(BUTTON0);
        setup_new_game();
        enqueue(&state_queue, &Flash_Highlighted_Square);
        enqueue(&state_queue, &Flash_Movement_Indicator);
        return_state = Selecting_Direction;
        ChessApp_pfStateMachine = Selecting_Direction;
    }
}
static void Set_Promotion_Square(void) {
    set_square_content(get_highlighted_square().col, get_highlighted_square().row, get_promotion_option(get_promotion(), get_turn()));
    draw_piece(get_square_content(get_highlighted_square().col, get_highlighted_square().row), get_highlighted_square().row, get_highlighted_square().col);
    ChessApp_pfStateMachine = return_state;
    controls_locked = FALSE;
}

static void Remove_EnPassantable_Piece(void) {
    draw_piece(EMPTY_SQUARE, get_en_passantable_square().row, get_en_passantable_square().col);
    set_square_content(get_en_passantable_square().col, get_en_passantable_square().row, EMPTY_SQUARE);
    set_en_passantable_square((Square){.row = -1, .col = -1});
    change_turn();
    push_to_last_eight_moves(get_move_to_make());
    enqueue(&state_queue, &Update_Player_Symbols);
    enqueue(&state_queue, &Deselect_Selected_Square);
    queue_post_move_analysis();
    ChessApp_pfStateMachine = return_state;
}

static void Lift_Piece(void) {
    draw_piece(EMPTY_SQUARE, get_move_to_make().start.row, get_move_to_make().start.col);
    ChessApp_pfStateMachine = return_state;
}

static void Place_Piece(void) {
    // Draw the selected square piece on the highlighted square
    draw_piece(get_square_content(get_move_to_make().start.col, get_move_to_make().start.row), get_move_to_make().end.row, get_move_to_make().end.col);

    // Move the selected square piece to the highlighted square
    set_square_content(get_move_to_make().end.col, get_move_to_make().end.row, get_square_content(get_move_to_make().start.col, get_move_to_make().start.row));
    set_square_content(get_move_to_make().start.col, get_move_to_make().start.row, EMPTY_SQUARE);

    Colour player_colour = get_piece_colour(get_square_content(get_move_to_make().end.col, get_move_to_make().end.row));

    // Update king location
    if (player_colour == WHITE && get_square_content(get_move_to_make().end.col, get_move_to_make().end.row) == WHITE_KING) {
        set_white_king_square((Square){.col = get_move_to_make().end.col, .row = get_move_to_make().end.row});
    }
    else if (player_colour == BLACK && get_square_content(get_move_to_make().end.col, get_move_to_make().end.row) == BLACK_KING) {
        set_black_king_square((Square){.col = get_move_to_make().end.col, .row = get_move_to_make().end.row});
    }

    // Check special move flags
    if (get_attempting_short_castle()) {
        player_colour == WHITE ? set_move_to_make((Move){.start = (Square){.col = H, .row = ONE}, .end = (Square){.col = F, .row = ONE}}) : set_move_to_make((Move){.start = (Square){.col = H, .row = EIGHT}, .end = (Square){.col = F, .row = EIGHT}});
        player_colour == WHITE ? set_white_short_castle_privileges(FALSE) : set_black_short_castle_privileges(FALSE);
        enqueue(&state_queue, &Lift_Piece);
        enqueue(&state_queue, &Place_Piece);
        set_en_passantable_square((Square){.row = -1, .col = -1});
    }
    else if (get_attempting_long_castle()) {
        player_colour == WHITE ? set_move_to_make((Move){.start = (Square){.col = A, .row = ONE}, .end = (Square){.col = D, .row = ONE}}) : set_move_to_make((Move){.start = (Square){.col = A, .row = EIGHT}, .end = (Square){.col = D, .row = EIGHT}});
        player_colour == WHITE ? set_white_long_castle_privileges(FALSE) : set_black_long_castle_privileges(FALSE);
        enqueue(&state_queue, &Lift_Piece);
        enqueue(&state_queue, &Place_Piece);
        set_en_passantable_square((Square){.row = -1, .col = -1});
    }
    else if (get_attempting_promotion()) {
        // Open promotion menu
        enqueue(&state_queue, &Set_Promotion_Square);
        set_en_passantable_square((Square){.row = -1, .col = -1});
        return_state = Selecting_Promotion;
        ChessApp_pfStateMachine = Selecting_Promotion;
    }
    else if (get_attempting_en_passant()) {
        enqueue(&state_queue, &Remove_EnPassantable_Piece);
    }
    else {
        if (get_attempting_special_pawn_move()) {
            player_colour == WHITE ? set_white_pawn_special_move_status(get_move_to_make().end.col, FALSE) : set_black_pawn_special_move_status(get_move_to_make().end.col, FALSE);
        }
        else {
            set_en_passantable_square((Square){.row = -1, .col = -1});
        }
        change_turn();
        push_to_last_eight_moves(get_move_to_make());
        enqueue(&state_queue, &Update_Player_Symbols);
        enqueue(&state_queue, &Deselect_Selected_Square);
        queue_post_move_analysis();
    }
    reset_special_move_attempt_flags();
    ChessApp_pfStateMachine = return_state;
}

static void Update_Player_Symbols(void) {
    draw_player_symbols(get_turn());
    ChessApp_pfStateMachine = return_state;
}

static void Flash_Highlighted_Square(void) {
    highlight_square(get_highlighted_square(), get_square_content(get_highlighted_square().col, get_highlighted_square().row), ARE_SQUARES_EQUAL(get_selected_square(), get_highlighted_square()));
    enqueue(&state_queue, &Flash_Highlighted_Square);
    ChessApp_pfStateMachine = return_state;
}

static void Flash_Movement_Indicator(void) {
    flash_arrow(get_direction());
    if (return_state == Selecting_Direction) {
        enqueue(&state_queue, &Flash_Movement_Indicator);
    }
    ChessApp_pfStateMachine = return_state;
}

static void Move_Highlighted_Square_Part_1(void) {
    draw_square(get_highlighted_square(), get_square_content(get_highlighted_square().col, get_highlighted_square().row));
    ChessApp_pfStateMachine = return_state;
}

static void Move_Highlighted_Square_Part_2(void) {
    if (ARE_SQUARES_EQUAL(get_selected_square(), get_highlighted_square())) {
        draw_selected_square_indicator(get_selected_square());
    }
    set_highlighted_square((Square){.row = (get_highlighted_square().row + get_direction_option(get_direction())[0] + BOARD_SIZE) % BOARD_SIZE, .col = (get_highlighted_square().col + get_direction_option(get_direction())[1] + BOARD_SIZE) % BOARD_SIZE});
    ChessApp_pfStateMachine = return_state;
    controls_locked = FALSE;
}

// When a move is attempted, this is the last state executed regardless of the result.
static void Deselect_Selected_Square(void) {
    draw_square(get_selected_square(), get_square_content(get_selected_square().col, get_selected_square().row));
    set_selected_square((Square){.row = -1, .col = -1});
    ChessApp_pfStateMachine = return_state;

    if (!analysis_mode) {
        controls_locked = FALSE;
    }
}

// Validate that the move is physically possible
static void Validate_Move_Piece_Movement(void) {
    if (piece_can_move(get_move_to_make().start.row, get_move_to_make().start.col, get_move_to_make().end.row, get_move_to_make().end.col)) {
        if (get_attempting_short_castle() || get_attempting_long_castle()) {
            // Change the turn to analyze the opponents possible moves
            change_turn();

            // Go to castle movement validation
            ChessApp_pfStateMachine = Validate_Move_Castle_Through_Check;
        }
        else {
            // Move the piece
            move_start_to_end();

            // Change the turn to analyze the opponents possible moves
            change_turn();

            // Go directly to check validation
            ChessApp_pfStateMachine = Validate_Move_Self_Inflicted_Check;
        }
    }
    else {
        if (!analysis_mode) {
            enqueue(&state_queue, &Deselect_Selected_Square);
        }
        reset_special_move_attempt_flags();
        ChessApp_pfStateMachine = return_state;
    }
}

// Validate that the player is not castling through a check
static void Validate_Move_Castle_Through_Check(void) {
    while (i < BOARD_SIZE) {
        while (j < BOARD_SIZE) {
            // Get the current piece's color
            Colour piece_colour = get_piece_colour(get_square_content(j, i));

            // If an opponent's piece is found
            if (piece_colour == get_turn()) {
                Square king_square = (get_turn() == WHITE) ? get_black_king_square() : get_white_king_square();

                // Check if the king is already in check
                if (piece_can_move(i, j, king_square.row, king_square.col)) {
                    invalidate_castling_attempt();
                    return;
                }
                
                // Check if the king needs to move through a check
                uint8_t row = (get_turn() == WHITE) ? EIGHT : ONE;
                if ((get_attempting_short_castle() && piece_can_move(i, j, row, F)) || (get_attempting_long_castle() && piece_can_move(i, j, row, D))) {
                    invalidate_castling_attempt();
                    return;
                }

                j++;
                return;
            }
            j++;
        }
        j = 0;
        i++;
    }

    // Castling movement is valid, check if the king ends up in check after the move
    i = 0;
    j = 0;
    change_turn();
    move_start_to_end();
    change_turn();
    ChessApp_pfStateMachine = Validate_Move_Self_Inflicted_Check;
}

// Validate that the player is not in check after the move
static void Validate_Move_Self_Inflicted_Check(void) {
    while (i < BOARD_SIZE) {
        while (j < BOARD_SIZE) {
            // Get the current piece's color
            Colour piece_colour = get_piece_colour(get_square_content(j, i));

            // If an opponent's piece is found
            if (piece_colour == get_turn()) {
                // Get the king's position based on the turn
                Square king_square = (get_turn() == WHITE) ? get_black_king_square() : get_white_king_square();

                // Check if the opponent's piece can move to the king's square
                if (piece_can_move(i, j, king_square.row, king_square.col)) {
                    // Invalidate move and reset state
                    change_turn();
                    undo_move_start_to_end();
                    reset_special_move_attempt_flags();
                    if (!analysis_mode) {
                        enqueue(&state_queue, &Deselect_Selected_Square);
                    }
                    i = 0;
                    j = 0;
                    ChessApp_pfStateMachine = return_state;
                    return;
                } 

                // Only analyze one move per call
                j++;
                return;
            }
            j++;
        }
        j = 0;
        i++;
    }

    // Move is valid, proceed with game state updates
    change_turn();
    undo_move_start_to_end();
    if (analysis_mode) {
        set_has_valid_move(TRUE);
    }
    else {
        enqueue(&state_queue, &Lift_Piece);
        enqueue(&state_queue, &Place_Piece);
    }
    i = 0;
    j = 0;
    ChessApp_pfStateMachine = return_state;
}

// Check if the result is a draw by repetition
static void Post_Move_Check_Is_Draw_By_Repetition(void) {
    if (draw_by_repetition()) {
        set_previous_result(RESULT_DRAW);
        draw_menu(get_previous_result(), get_turn());
        ChessApp_pfStateMachine = Chess_Menu;
    }
    else {
        ChessApp_pfStateMachine = return_state;
    }
}

// Check if the current player is in check
static void Post_Move_Check_Is_In_Check(void) {
    while (i < BOARD_SIZE) {
        while (j < BOARD_SIZE) {
            Colour piece_colour = get_piece_colour(get_square_content(j, i));
            if (piece_colour != get_turn() && piece_colour != NO_COLOUR) {
                Square king_square = (get_turn() == WHITE) ? get_white_king_square() : get_black_king_square();
                change_turn();
                if (piece_can_move(i, j, king_square.row, king_square.col)) {
                    set_king_in_check(TRUE);
                    i = 0;
                    j = 0;
                    ChessApp_pfStateMachine = return_state;
                    change_turn();
                    set_check_indicator();
                    reset_special_move_attempt_flags();
                    return;
                }
                // Only analyze one move per call
                change_turn();
                j++;
                return;
            }
            j++;
        }
        j = 0;
        i++;
    }
    i = 0;
    j = 0;
    reset_special_move_attempt_flags();
    ChessApp_pfStateMachine = return_state;
}

// Check if there is sufficient material to continue
static void Post_Move_Check_Is_Sufficient_Material(void) {
    if (insufficient_material()) {
        set_previous_result(RESULT_DRAW);
        set_king_in_check(FALSE);
        draw_menu(get_previous_result(), get_turn());
        ChessApp_pfStateMachine = Chess_Menu;
    }
    else {
        ChessApp_pfStateMachine = return_state;
    }
}

// Check if the current player has a valid move
static void Post_Move_Check_Has_Valid_Move(void) {
    if (!get_has_valid_move()) {
        ChessApp_pfStateMachine = Validate_Move_Piece_Movement;
        return_state = Post_Move_Check_Has_Valid_Move;
        while (x < BOARD_SIZE) {
            while (y < BOARD_SIZE) {
                Colour pieceColor = get_piece_colour(get_square_content(y, x));
                if (pieceColor == get_turn()) {
                    while (k < BOARD_SIZE) {
                        while (l < BOARD_SIZE) {
                            set_move_to_make((Move){.start = (Square){.col = y, .row = x}, .end = (Square){.col = l, .row = k}});
                            l++;
                            return;
                        }
                        l = 0;
                        k++;
                    }
                    k = 0;
                }
                y++;
            }
            y = 0;
            x++;
        }
        get_king_in_check() ? set_previous_result(RESULT_CHECKMATE) : set_previous_result(RESULT_DRAW);
        set_king_in_check(FALSE);
        draw_menu(get_previous_result(), !get_turn());
        ChessApp_pfStateMachine = Chess_Menu;
        return;
    }
    x = 0;
    y = 0;
    k = 0;
    l = 0;
    reset_special_move_attempt_flags();
    analysis_mode = FALSE;
    controls_locked = FALSE;
    return_state = Selecting_Square;
    ChessApp_pfStateMachine = return_state;
}

static void Selecting_Square(void) {
    if (debug && !controls_locked) {
        debug_timer--;
        if (debug_timer == 0) {

            // If there are no more debug inputs
            if (debug_inputs_index > debug_inputs_size - 1) {
                if (debug_moves_index > debug_moves_size - 1) {
                    // If there are no more debug moves, end debugging session
                    debug = FALSE;
                    free(debug_moves);
                    return;
                }
                else {
                    // If there are more debug moves, populate debug_inputs with input to execute the next move
                    generate_button_input(debug_moves[debug_moves_index], 0);
                    debug_moves_index++;
                    debug_inputs_index = 0;
                }
            }
            debug_action = debug_inputs[debug_inputs_index];
            debug_inputs_index++;
            debug_timer = 500;
        }
    }

    queue_timer--;
    if (queue_timer == 0) {
        ChessApp_pfStateMachine = get_front(&state_queue);
        dequeue(&state_queue);
        queue_timer = QUEUE_TIMER_MAX;
        return;
    }
    if (controls_locked) return;

    // Process button intput
    if (WasButtonPressed(BUTTON0) || (debug && debug_action == 0)) {
        ButtonAcknowledge(BUTTON0);
        debug_action = 5;

        // Move highlighted square in the selected direction
        controls_locked = TRUE;
        enqueue(&state_queue, &Move_Highlighted_Square_Part_1);
        enqueue(&state_queue, &Move_Highlighted_Square_Part_2);      
    }
    else if (WasButtonPressed(BUTTON1) || (debug && debug_action == 1)) {
        ButtonAcknowledge(BUTTON1);
        debug_action = 5;

        // Select highlighted piece
        if (IS_SQUARE_NULL(get_selected_square())) {
            set_selected_square(get_highlighted_square()); // Indicator is rendered next time the square is flashed
        }
        else {
            // Attempt move
            controls_locked = TRUE;
            set_move_to_make((Move){.start = (Square){.col = get_selected_square().col, .row = get_selected_square().row}, .end = (Square){.col = get_highlighted_square().col, .row = get_highlighted_square().row}});
            ChessApp_pfStateMachine = Validate_Move_Piece_Movement;
        }
    }
    else if (IsButtonHeld(BUTTON0, 500) || (debug && debug_action == 2)) {
        debug_action = 5;

        // Open direction selection menu
        ChessApp_pfStateMachine = Selecting_Direction;
        return_state = Selecting_Direction; // Update the return state so queued states do not switch back to this one
        enqueue(&state_queue, &Flash_Movement_Indicator); // Queue direction indicator flashing
    }
}

static void Change_Movement_Direction(void) {
    set_direction((get_direction() + 1) % DIRECTION_OPTIONS);
    draw_movement_direction(get_direction());
    ChessApp_pfStateMachine = return_state;
    controls_locked = FALSE;
}

static void Close_Movement_Direction_Menu(void) {
    draw_movement_direction(get_direction());
    ChessApp_pfStateMachine = return_state;
    controls_locked = FALSE;
}

static void Selecting_Direction(void) {
    if (debug && !controls_locked) {
        debug_timer--;
        if (debug_timer == 0) {

            // If there are no more debug inputs
            if (debug_inputs_index > debug_inputs_size - 1) {
                if (debug_moves_index > debug_moves_size - 1) {
                    // If there are no more debug moves, end debugging session
                    debug = FALSE;
                    free(debug_moves);
                    return;
                }
                else {
                    // If there are more debug moves, populate debug_inputs with input to execute the next move
                    generate_button_input(debug_moves[debug_moves_index], 0);
                    debug_moves_index++;
                    debug_inputs_index = 0;
                }
            }
            debug_action = debug_inputs[debug_inputs_index];
            debug_inputs_index++;
            if (debug_action != DIRECTION_SELECTION_BUTTON) debug_timer = 500;
        }
    }

    queue_timer--;
    if (queue_timer == 0) {
        ChessApp_pfStateMachine = get_front(&state_queue);
        dequeue(&state_queue);
        queue_timer = QUEUE_TIMER_MAX;
        return;
    }
    if (controls_locked) return;

    // Process button intput
    if (WasButtonPressed(BUTTON0) || (debug && debug_action == 0)) {
        ButtonAcknowledge(BUTTON0);
        debug_action = 5;

        // Select next direction option
        controls_locked = TRUE;
        enqueue(&state_queue, &Change_Movement_Direction);
    }
    else if (WasButtonPressed(BUTTON1) || (debug && debug_action == 1)) {
        ButtonAcknowledge(BUTTON1);
        debug_action = 5;

        // Close direction selection menu
        controls_locked = TRUE;
        return_state = Selecting_Square; // Update the return state so queued states do not switch back to this one
        enqueue(&state_queue, &Close_Movement_Direction_Menu); // Queue state to redraw the direction indicator incase it is invisible
    }
    else if (IsButtonHeld(BUTTON0, 2000)) {
        // Resign game
        resign();
    }
}

static void Change_Promotion_Option(void) {
    set_promotion((get_promotion() + 1) % PROMOTION_OPTIONS);
    set_square_content(get_highlighted_square().col, get_highlighted_square().row, get_promotion_option(get_promotion(), get_turn()));
    draw_piece(get_square_content(get_highlighted_square().col, get_highlighted_square().row), get_highlighted_square().row, get_highlighted_square().col);
    ChessApp_pfStateMachine = return_state;
    controls_locked = FALSE;
}

static void Selecting_Promotion(void) {
    if (debug && !controls_locked) {
        debug_timer--;
        if (debug_timer == 0) {
            // If there are no more debug inputs
            if (debug_inputs_index > debug_inputs_size - 1) {
                if (debug_moves_index > debug_moves_size - 1) {
                    // If there are no more debug moves, end debugging session
                    debug = FALSE;
                    free(debug_moves);
                    return;
                }
                else {
                    // If there are more debug moves, populate debug_inputs with input to execute the next move
                    generate_button_input(debug_moves[debug_moves_index], 0);
                    debug_moves_index++;
                    debug_inputs_index = 0;
                }
            }
            debug_action = debug_inputs[debug_inputs_index];
            debug_inputs_index++;
            if (debug_action != DIRECTION_SELECTION_BUTTON) debug_timer = 500;
        }
    }

    queue_timer--;
    if (queue_timer == 0) {
        ChessApp_pfStateMachine = get_front(&state_queue);
        dequeue(&state_queue);
        queue_timer = QUEUE_TIMER_MAX;
        return;
    }
    if (controls_locked) return;

    // Process button intput
    if (WasButtonPressed(BUTTON0) || (debug && debug_action == 0)) {
        ButtonAcknowledge(BUTTON0);
        debug_action = 5;

        // Select next promotion option
        controls_locked = TRUE;
        enqueue(&state_queue, &Change_Promotion_Option);
    }
    else if (WasButtonPressed(BUTTON1) || (debug && debug_action == 1)) {
        ButtonAcknowledge(BUTTON1);
        debug_action = 5;

        // Continue processing
        controls_locked = TRUE;
        change_turn();
        push_to_last_eight_moves(get_move_to_make());
        enqueue(&state_queue, &Update_Player_Symbols);
        enqueue(&state_queue, &Deselect_Selected_Square);
        queue_post_move_analysis();
        return_state = Selecting_Square;
        ChessApp_pfStateMachine = return_state;
    }
}

static void ChessAppSM_Error(void) {
  
}
