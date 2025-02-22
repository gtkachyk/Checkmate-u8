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
    draw_piece(EMPTY_SQUARE, get_selected_square().row, get_selected_square().col);
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Place_Piece(void) {
    // Draw the selected square piece on the highlighted square
    draw_piece(get_square_content(get_selected_square().col, get_selected_square().row), get_highlighted_square().row, get_highlighted_square().col);

    // Move the selected square piece to the highlighted square
    set_square_content(get_highlighted_square().col, get_highlighted_square().row, get_square_content(get_selected_square().col, get_selected_square().row));
    set_square_content(get_selected_square().col, get_selected_square().row, EMPTY_SQUARE);

    if (get_attempting_special_pawn_move()) {
        if (get_piece_colour(get_square_content(get_highlighted_square().col, get_highlighted_square().row)) == WHITE) {
            set_white_pawn_special_move_status(get_highlighted_square().col, FALSE);
        }
        else {
            set_black_pawn_special_move_status(get_highlighted_square().col, FALSE);
        }
    }
    else if (get_attempting_short_castle()) {
        if (get_highlighted_square().col == G && get_highlighted_square().row == ONE) {
            enqueue(&state_queue, &Chess_Game_Lift_H1_Rook_To_Castle);
            enqueue(&state_queue, &Chess_Game_Place_H1_Rook_To_Castle);
        }
        else {
            enqueue(&state_queue, &Chess_Game_Lift_H8_Rook_To_Castle);
            enqueue(&state_queue, &Chess_Game_Place_H8_Rook_To_Castle);
        }
    }
    else if (get_attempting_long_castle()) {
        if (get_highlighted_square().col == C && get_highlighted_square().row == ONE) {
            enqueue(&state_queue, &Chess_Game_Lift_A1_Rook_To_Castle);
            enqueue(&state_queue, &Chess_Game_Place_A1_Rook_To_Castle);
        }
        else {
            enqueue(&state_queue, &Chess_Game_Lift_A8_Rook_To_Castle);
            enqueue(&state_queue, &Chess_Game_Place_A8_Rook_To_Castle);
        }
    }
    else if (get_attempting_en_passant()) {
        enqueue(&state_queue, &Chess_Game_Remove_En_Passant_Pawn);
    }
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Lift_A1_Rook_To_Castle(void) {
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Place_A1_Rook_To_Castle(void) {
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Lift_H1_Rook_To_Castle(void) {
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Place_H1_Rook_To_Castle(void) {
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Lift_A8_Rook_To_Castle(void) {
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Place_A8_Rook_To_Castle(void) {
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Lift_H8_Rook_To_Castle(void) {
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Place_H8_Rook_To_Castle(void) {
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Remove_En_Passant_Pawn(void) {
    // Clear the captured pawn
    draw_piece(EMPTY_SQUARE, get_en_passantable_square().row, get_en_passantable_square().col);
    set_square_content(get_en_passantable_square().col, get_en_passantable_square().row, EMPTY_SQUARE);
    set_en_passantable_square((Square){.row = -1, .col = -1});
    set_attempting_en_passant(FALSE);
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
}

static void Chess_Game_Deselect_Selected_Square(void) {
    draw_square(get_selected_square(), get_square_content(get_selected_square().col, get_selected_square().row));
    set_selected_square((Square){.row = -1, .col = -1});
    ChessApp_pfStateMachine = return_state;
}

// Validate that the move is physically possible
static void Chess_Game_Validate_Move_Part_1(void) {
    if (piece_can_move(get_selected_square().row, get_selected_square().col, get_highlighted_square().row, get_highlighted_square().col)) {
        // Move the piece
        move_selected_to_highlighted();

        // Go to next part of validation process
        ChessApp_pfStateMachine = Chess_Game_Validate_Move_Part_2;
    }
    else {
        enqueue(&state_queue, &Chess_Game_Deselect_Selected_Square);
        ChessApp_pfStateMachine = return_state;
    }
}

// Validate that the player is not in check after the move
static void Chess_Game_Validate_Move_Part_2(void) {
    while (i < BOARD_SIZE) {
        while (j < BOARD_SIZE) {
            // Get the current piece's color
            Colour pieceColor = get_piece_colour(get_square_content(j, i));

            // If an opponent's piece is found
            if (pieceColor != get_turn()) {
                // Get the king's position based on the turn
                Square kingSquare = (get_turn() == WHITE) ? get_white_king_square() : get_black_king_square();

                // Check if the opponent's piece can move to the king's square
                if (piece_can_move(i, j, kingSquare.row, kingSquare.col)) {
                    // Invalidate move and reset state
                    undo_move_selected_to_highlighted();
                    enqueue(&state_queue, &Chess_Game_Deselect_Selected_Square);
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
    undo_move_selected_to_highlighted();
    i = 0;
    j = 0;
    change_turn();
    enqueue(&state_queue, &Chess_Game_Lift_Piece);
    enqueue(&state_queue, &Chess_Game_Place_Piece);
    enqueue(&state_queue, &Chess_Game_Update_Player_Symbols);
    enqueue(&state_queue, &Chess_Game_Deselect_Selected_Square);
    ChessApp_pfStateMachine = return_state;
}


static void Chess_Game_Selecting_Square(void) {
    queue_timer--;
    if (queue_timer == 0) {
        // Create queue of state function pointers
        ChessApp_pfStateMachine = get_front(&state_queue);
        dequeue(&state_queue);
        queue_timer = QUEUE_TIMER_MAX;
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
        if (IS_SQUARE_NULL(get_selected_square())) {
            set_selected_square(get_highlighted_square());
        }
        else {
            // Attempt move
            // ChessApp_pfStateMachine = Chess_Game_Attempt_Move;
            ChessApp_pfStateMachine = Chess_Game_Validate_Move_Part_1;
        }
    }
    else if (IsButtonHeld(BUTTON0, 500)) {
        // Open direction selection menu
        ChessApp_pfStateMachine = Chess_Game_Selecting_Direction; // Change state instantly to prevent further square selection input
        return_state = Chess_Game_Selecting_Direction; // Update the return state so queued states do not switch back to this one
        enqueue(&state_queue, &Chess_Game_Flash_Movement_Indicator); // Queue direction indicator flashing
        queue_timer = QUEUE_TIMER_MAX; // Reset timer to prevent rapid lcd changes when instantly changing to a new state
    }
}

static void Chess_Game_Change_Movement_Direction(void) {
    set_direction((get_direction() + 1) % DIRECTION_OPTIONS);
    draw_movement_direction(get_direction());
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Close_Movement_Direction_Menu(void) {
    draw_movement_direction(get_direction());
    ChessApp_pfStateMachine = return_state;
}

static void Chess_Game_Selecting_Direction(void) {
    queue_timer--;
    if (queue_timer == 0) {
        // Create queue of state function pointers
        ChessApp_pfStateMachine = get_front(&state_queue);
        dequeue(&state_queue);
        queue_timer = QUEUE_TIMER_MAX;
    }
    else if (WasButtonPressed(BUTTON0)) {
        ButtonAcknowledge(BUTTON0);

        // Select next direction option
        enqueue(&state_queue, &Chess_Game_Change_Movement_Direction);
    }
    else if (WasButtonPressed(BUTTON1)) {
        ButtonAcknowledge(BUTTON1);

        // Close direction selection menu
        ChessApp_pfStateMachine = Chess_Game_Selecting_Square; // Change state instantly to prevent further direction selection menu input
        return_state = Chess_Game_Selecting_Square; // Update the return state so queued states do not switch back to this one
        enqueue(&state_queue, &Chess_Game_Close_Movement_Direction_Menu); // Queue state to redraw the direction indicator incase it is invisible
        queue_timer = QUEUE_TIMER_MAX; // Reset timer to prevent rapid lcd changes when instantly changing to a new state
    }
    else if (IsButtonHeld(BUTTON0, 2000)) {
        // Resign game
        resign();
    }
}

static void ChessAppSM_Error(void) {
  
}
