#include "chess_app.h"

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

void print_square(u8 name[], Square square) {
    print_str(name);
    print_int_var("col", square.col);
    print_int_var("row", square.row);
}

void print_move(u8 name[], Move move) {
    print_str(name);
    print_int_var("start col", move.start.col);
    print_int_var("start row", move.start.row);
    print_int_var("end col", move.end.col);
    print_int_var("end row", move.end.row);
}

void print_state() {
    print_str("State:");
    print_square("highlighted_square", get_highlighted_square());
    print_square("selected_square", get_selected_square());
    DebugLineFeed();
    DebugLineFeed();
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