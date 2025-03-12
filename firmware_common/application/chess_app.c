#include "chess_app.h"

// System
extern volatile u32 G_u32SystemTime1ms;
extern volatile u32 G_u32SystemTime1s;
extern volatile u32 G_u32SystemFlags;
extern volatile u32 G_u32ApplicationFlags;
volatile u32 G_u32ChessAppFlags;
static fnCode_type ChessApp_pfStateMachine;
extern u8 G_au8DebugScanBuffer[DEBUG_SCANF_BUFFER_SIZE];
extern u8 G_u8DebugScanfCharCount;

// State management
static fnCode_type return_state;
static int queue_timer = QUEUE_TIMER_MAX;
static Queue state_queue;

// Modes
static bool controls_locked = FALSE;
static bool analysis_mode = FALSE; // Analysis mode is when the program is checking if the current player has a valid move
static bool debug = FALSE;

// Testing
static uint8_t debug_action = NO_DEBUG_ACTION;
static uint8_t debug_inputs_index = 0;
static uint8_t debug_moves_index = 0;
uint8_t debug_inputs_size;
uint8_t debug_moves_size;
static Move* debug_moves;
static uint8_t* debug_inputs; // Max inputs is 50 with current implementation
static uint8_t debug_timer = DEBUG_TIMER_MAX;
bool skip_reset = FALSE; // Skips board reset upon starting a new game

// Inter-state communication
static uint8_t move_to_make_index = NO_MOVE;
static uint8_t i = 0;
static uint8_t j = 0;
static uint8_t k = 0;
static uint8_t l = 0;
static uint8_t x = 0;
static uint8_t y = 0;

/************************************************************************************************/
/************************************************************************************************/
/************************************** Helper Functions ****************************************/
/************************************************************************************************/
/************************************************************************************************/

/************************************************************************************************/
/***************************************** Test Setup *******************************************/
/************************************************************************************************/
bool get_skip_reset() {
    return skip_reset;
}

void create_game_1_moves() {
}

void create_debug_moves() {
    debug_moves_size = 1;
    debug_moves = (Move*)malloc(debug_moves_size * sizeof(Move));
    if (debug_moves == NULL) {
        draw_error_message();
        ChessApp_pfStateMachine = ChessAppSM_Error;
        return;
    }
    debug_inputs = (uint8_t*)malloc(64 * sizeof(uint8_t));
    if (debug_inputs == NULL) {
        draw_error_message();
        ChessApp_pfStateMachine = ChessAppSM_Error;
        return;
    }
    create_game_1_moves();
}

void add_debug_input(uint8_t* index, uint8_t value) {
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

/************************************************************************************************/
/*************************************** Miscellaneous ******************************************/
/************************************************************************************************/

// Games must only end by calling this function
void end_game(GameResult result) {
    // If puzzle mode and the current puzzle is passed (currently, all puzzles are mate in one)
    if (get_puzzle_mode() && result == RESULT_CHECKMATE) {
        if (get_current_puzzle() == 2) {
            set_current_puzzle(0);
        }
        else {
            set_current_puzzle(get_current_puzzle() + 1);
        }
    }
    set_king_in_check(FALSE);
    set_previous_result(result);
    change_turn();
    ChessApp_pfStateMachine = Chess_Menu;
}

// Precondition: move_to_make has been validated but not reflected in the board
void update_fifty_move_counter() {
    // If the player moves a pawn or captures material
    uint8_t piece_to_move = get_square_content(get_move_to_make().start.col, get_move_to_make().start.row); // get move to make is wrong
    if (piece_to_move == WHITE_PAWN || piece_to_move == BLACK_PAWN || get_attempting_en_passant() || get_square_content(get_move_to_make().end.col, get_move_to_make().end.row) != EMPTY_SQUARE) {
        if (get_turn() == WHITE) {
            set_fifty_move_rule_counter(-1);
        }
        else {
            set_fifty_move_rule_counter(0);
        }
    }
    else {
        set_fifty_move_rule_counter(get_fifty_move_rule_counter() + 1);
    }
}

void queue_post_move_analysis() {
    analysis_mode = TRUE;
    enqueue(&state_queue, &Post_Move_Check_Is_Draw_By_Fifty_Move_Rule);
    enqueue(&state_queue, &Post_Move_Check_Is_Draw_By_Repetition); // Check if the 2nd and 1st most recent moves are identical to the 4th and 3rd most recent moves
    enqueue(&state_queue, &Post_Move_Check_Is_In_Check); // Check if king is in check
    enqueue(&state_queue, &Post_Move_Check_Is_Sufficient_Material); // Check if there is sufficient material
    enqueue(&state_queue, &Post_Move_Check_Has_Valid_Move); // Check if moving player has any valid moves
}

void invalidate_castling_attempt(void) {
    change_turn();
    reset_movement_validation_flags();
    if (!analysis_mode) {
        enqueue(&state_queue, &Deselect_Selected_Square);
    }
    i = 0;
    j = 0;
    ChessApp_pfStateMachine = return_state;
}

void queue_move_to_make() {
    enqueue(&state_queue, &Lift_Piece);
    enqueue(&state_queue, &Place_Piece);
}

void set_next_debug_action() {
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
}

void set_state_from_queue() {
    ChessApp_pfStateMachine = get_front(&state_queue);
    dequeue(&state_queue);
    queue_timer = QUEUE_TIMER_MAX;
}

void ChessAppRunActiveState(void) {
    ChessApp_pfStateMachine();
}

/************************************************************************************************/
/***************************************** App Setup ********************************************/
/************************************************************************************************/
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

void ChessAppInitialize(void) {
    if (TRUE) {
        if (debug) {
            set_puzzle_mode(FALSE);
            reset_game_data();
            create_debug_moves();
            debug_inputs[0] = BUTTON1;
            generate_button_input(debug_moves[debug_moves_index], 1);
            debug_moves_index++;
        }
        state_queue = *(create_queue());
        ChessApp_pfStateMachine = Chess_Menu;
    }
    else {
        draw_error_message();
        ChessApp_pfStateMachine = ChessAppSM_Error;
    }
}

/************************************************************************************************/
/************************************************************************************************/
/*************************************** State Machines *****************************************/
/************************************************************************************************/
/************************************************************************************************/

/************************************************************************************************/
/************************************** Drawing Functions ***************************************/
/************************************************************************************************/

/******************************************** Menu **********************************************/
static void Chess_Menu(void) {
    if (get_menu_parts_drawn() != MENU_PARTS) {
        draw_menu();
    }
    else if (WasButtonPressed(BUTTON0)) {
        ButtonAcknowledge(BUTTON0);
        set_menu_parts_drawn(0);
        set_puzzle_mode(FALSE);
        reset_app_data();
        reset_game_data();
        enqueue(&state_queue, &Flash_Highlighted_Square);
        enqueue(&state_queue, &Flash_Movement_Indicator);
        return_state = Selecting_Direction;
        ChessApp_pfStateMachine = Draw_Board;
    }
    else if (WasButtonPressed(BUTTON1)) {
        ButtonAcknowledge(BUTTON1);
        set_menu_parts_drawn(0);
        set_puzzle_mode(TRUE);
        reset_app_data();
        reset_game_data();
        enqueue(&state_queue, &Flash_Highlighted_Square);
        enqueue(&state_queue, &Flash_Movement_Indicator);
        return_state = Selecting_Direction;
        ChessApp_pfStateMachine = Draw_Board;
    }
}

/***************************************** Interface ********************************************/
static void Draw_Board(void) {
    if (get_board_parts_drawn() != BOARD_PARTS) {
        draw_board();
    }
    else {
        set_board_parts_drawn(0);
        ChessApp_pfStateMachine = Draw_Interface;
    }
}

static void Draw_Interface(void) {
    if (get_interface_parts_drawn() != INTERFACE_PARTS) {
        draw_game_interface();
    }
    else {
        set_interface_parts_drawn(0);
        ChessApp_pfStateMachine = return_state;
    }
}

/*********************************** Moving Side Indicators *************************************/
static void Update_Player_Symbols(void) {
    draw_player_symbols(get_turn());
    ChessApp_pfStateMachine = return_state;
}

static void Change_Movement_Direction(void) {
    set_direction((get_direction() + 1) % DIRECTION_OPTIONS);
    draw_movement_direction(get_direction());
    ChessApp_pfStateMachine = return_state;
    controls_locked = FALSE;
}

/*************************************** Piece Movement *****************************************/
static void Set_Promotion_Square(void) {
    set_square_content(get_move_to_make().end.col, get_move_to_make().end.row, get_promotion_option(get_promotion(), get_turn()));
    draw_piece(get_square_content(get_move_to_make().end.col, get_move_to_make().end.row), get_move_to_make().end.row, get_move_to_make().end.col);
    return_state = Selecting_Promotion;
    ChessApp_pfStateMachine = return_state;
    controls_locked = FALSE;
}

static void Remove_EnPassantable_Piece(void) {
    draw_piece(EMPTY_SQUARE, get_en_passantable_square().row, get_en_passantable_square().col);
    set_square_content(get_en_passantable_square().col, get_en_passantable_square().row, EMPTY_SQUARE);
    ChessApp_pfStateMachine = return_state;
}

static void Lift_Piece(void) { // Use inline function to reduce redundancy?
    if (!IS_MOVE_NULL(get_move_to_make()) && move_to_make_index == PART_ONE){
        draw_piece(EMPTY_SQUARE, get_move_to_make().start.row, get_move_to_make().start.col);
    }
    else if (!IS_MOVE_NULL(get_move_to_make_part_two()) && move_to_make_index == PART_TWO) {
        draw_piece(EMPTY_SQUARE, get_move_to_make_part_two().start.row, get_move_to_make_part_two().start.col);
    }
    ChessApp_pfStateMachine = return_state;
}

static void Place_Piece(void) { // Use inline function to reduce redundancy?
    if (!IS_MOVE_NULL(get_move_to_make()) && move_to_make_index == PART_ONE){
        // Draw the move
        draw_piece(get_square_content(get_move_to_make().start.col, get_move_to_make().start.row), get_move_to_make().end.row, get_move_to_make().end.col);

        // Update the board
        set_square_content(get_move_to_make().end.col, get_move_to_make().end.row, get_square_content(get_move_to_make().start.col, get_move_to_make().start.row));
        set_square_content(get_move_to_make().start.col, get_move_to_make().start.row, EMPTY_SQUARE);
        if (!IS_MOVE_NULL(get_move_to_make_part_two())) move_to_make_index = PART_TWO;
    }
    else if (!IS_MOVE_NULL(get_move_to_make_part_two()) && move_to_make_index == PART_TWO) {
        // Draw the move
        draw_piece(get_square_content(get_move_to_make_part_two().start.col, get_move_to_make_part_two().start.row), get_move_to_make_part_two().end.row, get_move_to_make_part_two().end.col);

        // Update the board
        set_square_content(get_move_to_make_part_two().end.col, get_move_to_make_part_two().end.row, get_square_content(get_move_to_make_part_two().start.col, get_move_to_make_part_two().start.row));
        set_square_content(get_move_to_make_part_two().start.col, get_move_to_make_part_two().start.row, EMPTY_SQUARE);
    }
    ChessApp_pfStateMachine = return_state;
}

/************************************* Flashing Indicators **************************************/
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

/*********************************** Moving Board Indicators ************************************/
static void Move_Highlighted_Square_Part_1(void) {
    // New workaround method of accessing the change direction menu, only used by real players
    if (IsButtonHeld(BUTTON0, 200)) {
        debug_action = NO_DEBUG_ACTION;
        dequeue(&state_queue); // Remove Move_Highlighted_Square_Part_2 from queue

        // Open direction selection menu
        ChessApp_pfStateMachine = Selecting_Direction;
        return_state = Selecting_Direction; // Update the return state so queued states do not switch back to this one
        enqueue(&state_queue, &Flash_Movement_Indicator); // Queue direction indicator flashing
        controls_locked = FALSE;
        return;
    }

    // The main purpose of this state
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

static void Deselect_Selected_Square(void) {
    draw_square(get_selected_square(), get_square_content(get_selected_square().col, get_selected_square().row));
    set_selected_square((Square){.row = -1, .col = -1});
    ChessApp_pfStateMachine = return_state;

    if (!analysis_mode) {
        controls_locked = FALSE;
    }
}

static void Change_Promotion_Option(void) {
    set_promotion((get_promotion() + 1) % PROMOTION_OPTIONS);
    set_square_content(get_highlighted_square().col, get_highlighted_square().row, get_promotion_option(get_promotion(), get_turn()));
    draw_piece(get_square_content(get_highlighted_square().col, get_highlighted_square().row), get_highlighted_square().row, get_highlighted_square().col);
    ChessApp_pfStateMachine = return_state;
    controls_locked = FALSE;
}

/************************************************************************************************/
/************************************** Move Invalidation ***************************************/
/************************************************************************************************/
// This state is entered after a move is invalidated
static void Process_Invalid_Move(void) { // Currently unused and untested
    if (!analysis_mode) {
        enqueue(&state_queue, &Deselect_Selected_Square);
    }
    reset_movement_validation_flags();
    ChessApp_pfStateMachine = return_state;
}

/************************************************************************************************/
/**************************************** Move Validation ***************************************/
/************************************************************************************************/
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
            // Lock the movement validation flags
            set_preserve_validation_flags(TRUE);

            // Move the piece
            move_start_to_end();

            // Change the turn to analyze the opponents possible moves
            change_turn();

            // Go directly to check validation
            ChessApp_pfStateMachine = Validate_Move_Self_Inflicted_Check;
        }
    }
    else {
        // enqueue(&state_queue, &Process_Invalid_Move);
        if (!analysis_mode) {
            enqueue(&state_queue, &Deselect_Selected_Square);
        }
        reset_movement_validation_flags();
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
    set_preserve_validation_flags(TRUE);
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

                    // Unlock the movement validation flags
                    set_preserve_validation_flags(FALSE);

                    reset_movement_validation_flags();
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

    // Unlock the movement validation flags
    set_preserve_validation_flags(FALSE);
    
    if (analysis_mode) {
        set_has_valid_move(TRUE);
    }
    else {
        enqueue(&state_queue, &Process_Valid_Move);
    }
    i = 0;
    j = 0;
    ChessApp_pfStateMachine = return_state;
}

// This state is entered after a move is validated
static void Process_Valid_Move(void) {
    Colour player_colour = get_piece_colour(get_square_content(get_move_to_make().start.col, get_move_to_make().start.row));

    // Update fifty move counter before updating board
    update_fifty_move_counter();

    // Queue the states to update the lcd and board with the first part of the move
    move_to_make_index = PART_ONE;
    queue_move_to_make();

    // Queue the states to update the lcd and board with the second part of the move if needed
    if (get_attempting_short_castle()) {
        player_colour == WHITE ? set_move_to_make_part_two((Move){.start = (Square){.col = H, .row = ONE}, .end = (Square){.col = F, .row = ONE}}) : set_move_to_make_part_two((Move){.start = (Square){.col = H, .row = EIGHT}, .end = (Square){.col = F, .row = EIGHT}});
        queue_move_to_make();
        enqueue(&state_queue, &Setup_Post_Move_Analysis);
    }
    else if (get_attempting_long_castle()) {
        player_colour == WHITE ? set_move_to_make_part_two((Move){.start = (Square){.col = A, .row = ONE}, .end = (Square){.col = D, .row = ONE}}) : set_move_to_make_part_two((Move){.start = (Square){.col = A, .row = EIGHT}, .end = (Square){.col = D, .row = EIGHT}});
        queue_move_to_make();
        enqueue(&state_queue, &Setup_Post_Move_Analysis);
    }
    else if (get_attempting_en_passant()) {
        enqueue(&state_queue, &Remove_EnPassantable_Piece);
        enqueue(&state_queue, &Setup_Post_Move_Analysis);
    }
    else if (get_attempting_promotion()) {
        enqueue(&state_queue, &Set_Promotion_Square);
    }
    else {
        enqueue(&state_queue, &Setup_Post_Move_Analysis);
    }
    ChessApp_pfStateMachine = return_state;
}

/************************************************************************************************/
/************************************* Post Move Validation *************************************/
/************************************************************************************************/
// All valid moves should end here
static void Setup_Post_Move_Analysis(void) {
    Colour player_colour = get_piece_colour(get_square_content(get_move_to_make().end.col, get_move_to_make().end.row));

    // Update data
    push_to_last_eight_moves(get_move_to_make());
    if (get_pawn_loses_special_move_privilege()) {
        player_colour == WHITE ? remove_white_pawn_special_move_privileges(get_move_to_make().start.col) : remove_black_pawn_special_move_privileges(get_move_to_make().start.col);
    }
    if (get_king_loses_short_castle_privileges()) {
        player_colour == WHITE ? set_white_short_castle_privileges(FALSE) : set_black_short_castle_privileges(FALSE);
    }
    if (get_king_loses_long_castle_privileges()) {
        player_colour == WHITE ? set_white_long_castle_privileges(FALSE) : set_black_long_castle_privileges(FALSE);
    }
    if (player_colour == WHITE && get_square_content(get_move_to_make().end.col, get_move_to_make().end.row) == WHITE_KING) { // King location
        set_white_king_square((Square){.col = get_move_to_make().end.col, .row = get_move_to_make().end.row});
    }
    else if (player_colour == BLACK && get_square_content(get_move_to_make().end.col, get_move_to_make().end.row) == BLACK_KING) {
        set_black_king_square((Square){.col = get_move_to_make().end.col, .row = get_move_to_make().end.row});
    }
    if (get_attempting_special_pawn_move()) { // En passant square
        set_en_passantable_square((Square){.row = get_move_to_make().end.row, .col = get_move_to_make().end.col});
    }
    else {
        set_en_passantable_square(NULL_SQUARE);
    }
    set_has_valid_move(FALSE);
    set_king_in_check(FALSE);
    reset_movement_validation_flags();
    move_to_make_index = NO_MOVE;
    set_move_to_make(NULL_MOVE);
    set_move_to_make_part_two(NULL_MOVE);
    change_turn();

    // Queue remaining ui changes
    set_check_indicator();
    enqueue(&state_queue, &Update_Player_Symbols);
    enqueue(&state_queue, &Deselect_Selected_Square);

    // Queue post move analysis
    queue_post_move_analysis();
    return_state = Selecting_Square;
    ChessApp_pfStateMachine = return_state;
}

// Check if the result is a draw by fifty move rule
static void Post_Move_Check_Is_Draw_By_Fifty_Move_Rule(void) {
    if (get_fifty_move_rule_counter() == 100) {
        // Game ends
        get_puzzle_mode() ? end_game(RESULT_PUZZLE_FAILED) : end_game(RESULT_DRAW);
    }
    else {
        // Game continues
        ChessApp_pfStateMachine = return_state;
    }
}

// Check if the result is a draw by repetition
static void Post_Move_Check_Is_Draw_By_Repetition(void) {
    if (draw_by_repetition()) {
        // Game ends
        get_puzzle_mode() ? end_game(RESULT_PUZZLE_FAILED) : end_game(RESULT_DRAW);
    }
    else {
        // Game continues
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
                    reset_movement_validation_flags();
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
    reset_movement_validation_flags();
    ChessApp_pfStateMachine = return_state;
}

// Check if there is sufficient material to continue
static void Post_Move_Check_Is_Sufficient_Material(void) {
    if (insufficient_material()) {
        // Game ends
        get_puzzle_mode() ? end_game(RESULT_PUZZLE_FAILED) : end_game(RESULT_DRAW);
    }
    else {
        // Game continues
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

        // Game ends
        get_king_in_check() ? end_game(RESULT_CHECKMATE) : end_game(RESULT_DRAW);
        return;
    }

    if (get_puzzle_mode()) {
        // Game ends
        end_game(RESULT_PUZZLE_FAILED);
    }
    else {
        // Game continues
        x = 0;
        y = 0;
        k = 0;
        l = 0;
        set_move_to_make(NULL_MOVE);
        reset_movement_validation_flags();
        analysis_mode = FALSE;
        controls_locked = FALSE;
        return_state = Selecting_Square;
        ChessApp_pfStateMachine = return_state;
    }
}

/************************************************************************************************/
/************************************** Menu Helpers ********************************************/
/************************************************************************************************/
static void Close_Movement_Direction_Menu(void) {
    draw_movement_direction(get_direction());
    ChessApp_pfStateMachine = return_state;
    controls_locked = FALSE;
}

/************************************************************************************************/
/****************************************** Menus ***********************************************/
/************************************************************************************************/
static void Selecting_Square(void) {
    if (debug && !controls_locked) {
        debug_timer--;
        if (debug_timer == 0) {
            set_next_debug_action();
            debug_timer = DEBUG_TIMER_MAX;
        }
    }

    queue_timer--;
    if (queue_timer == 0) {
        set_state_from_queue();
        return;
    }
    if (controls_locked) return;

    // Process button intput
    if (WasButtonPressed(BUTTON0) || (debug && debug_action == BUTTON0)) {
        ButtonAcknowledge(BUTTON0);
        debug_action = NO_DEBUG_ACTION;

        // Move highlighted square in the selected direction
        controls_locked = TRUE;
        enqueue(&state_queue, &Move_Highlighted_Square_Part_1);
        enqueue(&state_queue, &Move_Highlighted_Square_Part_2);      
    }
    else if (WasButtonPressed(BUTTON1) || (debug && debug_action == BUTTON1)) {
        ButtonAcknowledge(BUTTON1);
        debug_action = NO_DEBUG_ACTION;

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
    else if (debug && debug_action == DIRECTION_SELECTION_BUTTON) { // Old method of accessing the change direction menu, still used by the debugger
        debug_action = NO_DEBUG_ACTION;

        // Open direction selection menu
        ChessApp_pfStateMachine = Selecting_Direction;
        return_state = Selecting_Direction; // Update the return state so queued states do not switch back to this one
        enqueue(&state_queue, &Flash_Movement_Indicator); // Queue direction indicator flashing
    }
}

static void Selecting_Direction(void) {
    if (debug && !controls_locked) {
        debug_timer--;
        if (debug_timer == 0) {
            set_next_debug_action();
            if (debug_action != DIRECTION_SELECTION_BUTTON) debug_timer = DEBUG_TIMER_MAX;
        }
    }

    queue_timer--;
    if (queue_timer == 0) {
        set_state_from_queue();
        return;
    }
    if (controls_locked) return;

    // Process button intput
    if (WasButtonPressed(BUTTON0) || (debug && debug_action == BUTTON0)) {
        ButtonAcknowledge(BUTTON0);
        debug_action = NO_DEBUG_ACTION;

        // Select next direction option
        controls_locked = TRUE;
        enqueue(&state_queue, &Change_Movement_Direction);
    }
    else if (WasButtonPressed(BUTTON1) || (debug && debug_action == BUTTON1)) {
        ButtonAcknowledge(BUTTON1);
        debug_action = NO_DEBUG_ACTION;

        // Close direction selection menu
        controls_locked = TRUE;
        return_state = Selecting_Square; // Update the return state so queued states do not switch back to this one
        enqueue(&state_queue, &Close_Movement_Direction_Menu); // Queue state to redraw the direction indicator incase it is invisible
    }
    else if (IsButtonHeld(BUTTON0, 5000)) {
        // Resign game
        get_puzzle_mode() ? end_game(RESULT_PUZZLE_FAILED) : end_game(RESULT_RESIGNATION);
    }
}

static void Selecting_Promotion(void) {
    if (debug && !controls_locked) {
        debug_timer--;
        if (debug_timer == 0) {
            set_next_debug_action();
            if (debug_action != DIRECTION_SELECTION_BUTTON) debug_timer = DEBUG_TIMER_MAX;
        }
    }

    queue_timer--;
    if (queue_timer == 0) {
        set_state_from_queue();
        return;
    }
    if (controls_locked) return;

    // Process button intput
    if (WasButtonPressed(BUTTON0) || (debug && debug_action == BUTTON0)) {
        ButtonAcknowledge(BUTTON0);
        debug_action = NO_DEBUG_ACTION;

        // Select next promotion option
        controls_locked = TRUE;
        enqueue(&state_queue, &Change_Promotion_Option);
    }
    else if (WasButtonPressed(BUTTON1) || (debug && debug_action == BUTTON1)) {
        ButtonAcknowledge(BUTTON1);
        debug_action = NO_DEBUG_ACTION;

        // Continue processing
        controls_locked = TRUE;
        enqueue(&state_queue, &Setup_Post_Move_Analysis);
    }
}

static void ChessAppSM_Error(void) {
    // Do nothing
}
