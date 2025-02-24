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
static bool analysis_mode = FALSE;

/* Functions */
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

fnCode_type getRear(Queue* q) {
    if (is_empty(q)) {
        return NULL;
    }
    return q->rear->data;
}

void resign() {
    set_previous_result(RESULT_RESIGNATION);
    draw_menu(get_previous_result(), get_turn());
    ChessApp_pfStateMachine = Chess_Menu;
}

void setup_new_game() {
    reset_game_data();
    draw_game_interface();
}

void queue_post_move_analysis() {
    analysis_mode = TRUE;
    enqueue(&state_queue, &Chess_Game_Find_Valid_Move); // Check if moving player has any valid moves
    // Check if there is sufficient material
    // Check if the position has been encountered twice before
}

void ChessAppInitialize(void) {
  if (1) {
    draw_menu(get_previous_result(), get_turn());
    state_queue = *(create_queue());
    enqueue(&state_queue, &Chess_Game_Flash_Highlighted_Square);
    enqueue(&state_queue, &Chess_Game_Flash_Movement_Indicator);
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
        return_state = Chess_Game_Selecting_Direction;
        ChessApp_pfStateMachine = Chess_Game_Selecting_Direction;
    }
}

static void Chess_Game_Lift_Piece(void) {
    draw_piece(EMPTY_SQUARE, get_move_to_make().start.row, get_move_to_make().start.col);
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Place_Piece(void) {
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
    if (get_attempting_special_pawn_move()) {
        player_colour == WHITE ? set_white_pawn_special_move_status(get_move_to_make().end.col, FALSE) : set_black_pawn_special_move_status(get_move_to_make().end.col, FALSE);
    }
    else if (get_attempting_short_castle()) {
        player_colour == WHITE ? set_move_to_make((Move){.start = (Square){.col = H, .row = ONE}, .end = (Square){.col = F, .row = ONE}}) : set_move_to_make((Move){.start = (Square){.col = H, .row = EIGHT}, .end = (Square){.col = F, .row = EIGHT}});
        player_colour == WHITE ? set_white_short_castle_privileges(FALSE) : set_black_short_castle_privileges(FALSE);
        enqueue(&state_queue, &Chess_Game_Lift_Piece);
        enqueue(&state_queue, &Chess_Game_Place_Piece);
    }
    else if (get_attempting_long_castle()) {
        player_colour == WHITE ? set_move_to_make((Move){.start = (Square){.col = A, .row = ONE}, .end = (Square){.col = D, .row = ONE}}) : set_move_to_make((Move){.start = (Square){.col = A, .row = EIGHT}, .end = (Square){.col = D, .row = EIGHT}});
        player_colour == WHITE ? set_white_long_castle_privileges(FALSE) : set_black_long_castle_privileges(FALSE);
        enqueue(&state_queue, &Chess_Game_Lift_Piece);
        enqueue(&state_queue, &Chess_Game_Place_Piece);
    }
    else if (get_attempting_en_passant()) { // untested
        set_move_to_make((Move){.start = (Square){.col = get_move_to_make().start.col, .row = get_move_to_make().start.row}, .end = (Square){.col = get_en_passantable_square().col, .row = get_en_passantable_square().row}});
        enqueue(&state_queue, &Chess_Game_Lift_Piece);
        enqueue(&state_queue, &Chess_Game_Place_Piece);
    }
    change_turn();
    enqueue(&state_queue, &Chess_Game_Update_Player_Symbols);
    enqueue(&state_queue, &Chess_Game_Deselect_Selected_Square);
    reset_special_move_attempt_flags();
    queue_post_move_analysis();
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Update_Player_Symbols(void) {
    draw_player_symbols(get_turn());
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Flash_Highlighted_Square(void) {
    highlight_square(get_highlighted_square(), get_square_content(get_highlighted_square().col, get_highlighted_square().row), ARE_SQUARES_EQUAL(get_selected_square(), get_highlighted_square()));
    enqueue(&state_queue, &Chess_Game_Flash_Highlighted_Square);
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Flash_Movement_Indicator(void) {
    flash_arrow(get_direction());
    if (return_state == Chess_Game_Selecting_Direction) {
        enqueue(&state_queue, &Chess_Game_Flash_Movement_Indicator);
    }
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Move_Highlighted_Square_Part_1(void) {
    draw_square(get_highlighted_square(), get_square_content(get_highlighted_square().col, get_highlighted_square().row));
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Move_Highlighted_Square_Part_2(void) {
    if (ARE_SQUARES_EQUAL(get_selected_square(), get_highlighted_square())) {
        draw_selected_square_indicator(get_selected_square());
    }
    set_highlighted_square((Square){.row = (get_highlighted_square().row + get_direction_option(get_direction())[0] + BOARD_SIZE) % BOARD_SIZE, .col = (get_highlighted_square().col + get_direction_option(get_direction())[1] + BOARD_SIZE) % BOARD_SIZE});
    ChessApp_pfStateMachine = return_state;
    controls_locked = FALSE;
}

// When a move is attempted, this is the last state executed regardless of the result.
static void Chess_Game_Deselect_Selected_Square(void) {
    draw_square(get_selected_square(), get_square_content(get_selected_square().col, get_selected_square().row));
    set_selected_square((Square){.row = -1, .col = -1});
    ChessApp_pfStateMachine = return_state;

    if (!analysis_mode) {
        controls_locked = FALSE;
    }
}

// Validate that the move is physically possible
static void Chess_Game_Validate_Piece_Movement(void) {
    if (piece_can_move(get_move_to_make().start.row, get_move_to_make().start.col, get_move_to_make().end.row, get_move_to_make().end.col)) {
        // Move the piece
        move_start_to_end();

        // Go to next part of validation process
        ChessApp_pfStateMachine = Chess_Game_Validate_Check_Status;
    }
    else {
        if (!analysis_mode) {
            enqueue(&state_queue, &Chess_Game_Deselect_Selected_Square);
        }
        ChessApp_pfStateMachine = return_state;
    }
}

// Validate that the player is not in check after the move
static void Chess_Game_Validate_Check_Status(void) {
    while (i < BOARD_SIZE) {
        while (j < BOARD_SIZE) {
            // Get the current piece's color
            Colour piece_colour = get_piece_colour(get_square_content(j, i));

            // If an opponent's piece is found
            if (piece_colour != get_turn()) {
                // Get the king's position based on the turn
                Square king_square = (get_turn() == WHITE) ? get_white_king_square() : get_black_king_square();

                // Check if the opponent's piece can move to the king's square
                if (piece_can_move(i, j, king_square.row, king_square.col)) {
                    // Invalidate move and reset state
                    undo_move_start_to_end();
                    reset_special_move_attempt_flags();
                    if (!analysis_mode) {
                        enqueue(&state_queue, &Chess_Game_Deselect_Selected_Square);
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
    undo_move_start_to_end();
    if (analysis_mode) {
        set_has_valid_move(TRUE);
    }
    else {
        enqueue(&state_queue, &Chess_Game_Lift_Piece);
        enqueue(&state_queue, &Chess_Game_Place_Piece);
    }
    i = 0;
    j = 0;
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Find_Valid_Move(void) {
    if (!get_has_valid_move()) {
        ChessApp_pfStateMachine = Chess_Game_Validate_Piece_Movement;
        return_state = Chess_Game_Find_Valid_Move;
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
        draw_menu(get_previous_result(), !get_turn());
        ChessApp_pfStateMachine = Chess_Menu;
        return;
    }
    print_int_var("move_to_make.start.col", get_move_to_make().start.col);
    print_int_var("move_to_make.start.row", get_move_to_make().start.row);
    print_int_var("move_to_make.end.col", get_move_to_make().end.col);
    print_int_var("move_to_make.end.row", get_move_to_make().end.row);
    x = 0;
    y = 0;
    k = 0;
    l = 0;
    analysis_mode = FALSE;
    controls_locked = FALSE;
    return_state = Chess_Game_Selecting_Square;
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Selecting_Square(void) {
    queue_timer--;
    if (queue_timer == 0) {
        ChessApp_pfStateMachine = get_front(&state_queue);
        dequeue(&state_queue);
        queue_timer = QUEUE_TIMER_MAX;
        return;
    }
    if (controls_locked) return;

    // Process button intput
    if (WasButtonPressed(BUTTON0)) {
        ButtonAcknowledge(BUTTON0);

        // Move highlighted square in the selected direction
        controls_locked = TRUE;
        enqueue(&state_queue, &Chess_Game_Move_Highlighted_Square_Part_1);
        enqueue(&state_queue, &Chess_Game_Move_Highlighted_Square_Part_2);      
    }
    else if (WasButtonPressed(BUTTON1)) {
        ButtonAcknowledge(BUTTON1);
        // Select highlighted piece
        if (IS_SQUARE_NULL(get_selected_square())) {
            set_selected_square(get_highlighted_square()); // Indicator is rendered next time the square is flashed
        }
        else {
            // Attempt move
            controls_locked = TRUE;
            set_move_to_make((Move){.start = (Square){.col = get_selected_square().col, .row = get_selected_square().row}, .end = (Square){.col = get_highlighted_square().col, .row = get_highlighted_square().row}});
            ChessApp_pfStateMachine = Chess_Game_Validate_Piece_Movement;
        }
    }
    else if (IsButtonHeld(BUTTON0, 500)) {
        // Open direction selection menu
        ChessApp_pfStateMachine = Chess_Game_Selecting_Direction;
        return_state = Chess_Game_Selecting_Direction; // Update the return state so queued states do not switch back to this one
        enqueue(&state_queue, &Chess_Game_Flash_Movement_Indicator); // Queue direction indicator flashing
    }
}

static void Chess_Game_Change_Movement_Direction(void) {
    set_direction((get_direction() + 1) % DIRECTION_OPTIONS);
    draw_movement_direction(get_direction());
    ChessApp_pfStateMachine = return_state;
    controls_locked = FALSE;
}

static void Chess_Game_Close_Movement_Direction_Menu(void) {
    draw_movement_direction(get_direction());
    ChessApp_pfStateMachine = return_state;
    controls_locked = FALSE;
}

static void Chess_Game_Selecting_Direction(void) {
    queue_timer--;
    if (queue_timer == 0) {
        ChessApp_pfStateMachine = get_front(&state_queue);
        dequeue(&state_queue);
        queue_timer = QUEUE_TIMER_MAX;
        return;
    }
    if (controls_locked) return;

    // Process button intput
    if (WasButtonPressed(BUTTON0)) {
        ButtonAcknowledge(BUTTON0);

        // Select next direction option
        controls_locked = TRUE;
        enqueue(&state_queue, &Chess_Game_Change_Movement_Direction);
    }
    else if (WasButtonPressed(BUTTON1)) {
        ButtonAcknowledge(BUTTON1);

        // Close direction selection menu
        controls_locked = TRUE;
        return_state = Chess_Game_Selecting_Square; // Update the return state so queued states do not switch back to this one
        enqueue(&state_queue, &Chess_Game_Close_Movement_Direction_Menu); // Queue state to redraw the direction indicator incase it is invisible
    }
    else if (IsButtonHeld(BUTTON0, 2000)) {
        // Resign game
        resign();
    }
}

static void ChessAppSM_Error(void) {
  
}
