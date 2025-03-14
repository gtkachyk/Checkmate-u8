# Misc
void create_test_moves() {
    debug_moves[0] = create_move(B, TWO, B, FOUR, NO_PROMOTION);
    debug_moves[1] = create_move(E, SEVEN, E, FIVE, NO_PROMOTION);

    debug_moves[2] = create_move(B, FOUR, B, FIVE, NO_PROMOTION);
    debug_moves[3] = create_move(E, FIVE, E, FOUR, NO_PROMOTION);

    debug_moves[4] = create_move(B, FIVE, B, SIX, NO_PROMOTION);
    debug_moves[5] = create_move(E, FOUR, E, THREE, NO_PROMOTION);

    debug_moves[6] = create_move(D, TWO, E, THREE, NO_PROMOTION);
    debug_moves[7] = create_move(C, SEVEN, A, FIVE, NO_PROMOTION);
    debug_moves[8] = create_move(C, SEVEN, D, SIX, NO_PROMOTION);
    debug_moves[9] = create_move(C, SEVEN, B, SIX, NO_PROMOTION);
}

# Pawn standard movement
void create_test_moves() {
    skip_reset = TRUE;
    set_square_content(C, TWO, EMPTY_SQUARE);
    set_square_content(C, ONE, EMPTY_SQUARE);
    set_square_content(D, ONE, EMPTY_SQUARE);
    set_square_content(E, ONE, EMPTY_SQUARE);
    set_square_content(E, TWO, EMPTY_SQUARE);
    set_square_content(D, SEVEN, EMPTY_SQUARE);
    set_square_content(D, EIGHT, EMPTY_SQUARE);
    set_square_content(E, EIGHT, EMPTY_SQUARE);
    set_square_content(F, EIGHT, EMPTY_SQUARE);
    set_square_content(F, SEVEN, EMPTY_SQUARE);

    debug_moves[0] = create_move(D, TWO, C, TWO, NO_PROMOTION);
    debug_moves[1] = create_move(D, TWO, C, THREE, NO_PROMOTION);
    debug_moves[2] = create_move(D, TWO, E, THREE, NO_PROMOTION);
    debug_moves[3] = create_move(D, TWO, E, TWO, NO_PROMOTION);
    debug_moves[4] = create_move(D, TWO, D, ONE, NO_PROMOTION);
    debug_moves[5] = create_move(D, TWO, D, TWO, NO_PROMOTION);
    debug_moves[6] = create_move(D, TWO, C, FOUR, NO_PROMOTION);
    debug_moves[7] = create_move(D, TWO, D, FIVE, NO_PROMOTION);
    debug_moves[8] = create_move(D, TWO, E, FOUR, NO_PROMOTION);
    debug_moves[9] = create_move(D, TWO, D, THREE, NO_PROMOTION);

    debug_moves[10] = create_move(E, SEVEN, D, SEVEN, NO_PROMOTION);
    debug_moves[11] = create_move(E, SEVEN, D, SIX, NO_PROMOTION);
    debug_moves[12] = create_move(E, SEVEN, F, SIX, NO_PROMOTION);
    debug_moves[13] = create_move(E, SEVEN, F, SEVEN, NO_PROMOTION);
    debug_moves[14] = create_move(E, SEVEN, E, EIGHT, NO_PROMOTION);
    debug_moves[15] = create_move(E, SEVEN, E, SEVEN, NO_PROMOTION);
    debug_moves[16] = create_move(E, SEVEN, D, FIVE, NO_PROMOTION);
    debug_moves[17] = create_move(E, SEVEN, E, FOUR, NO_PROMOTION);
    debug_moves[18] = create_move(E, SEVEN, F, FIVE, NO_PROMOTION);
}

# Pawn standard movement 2
void create_test_moves() {
    skip_reset = TRUE;
    set_square_content(C, TWO, BLACK_PAWN);
    set_square_content(C, ONE, BLACK_PAWN);
    set_square_content(D, ONE, BLACK_PAWN);
    set_square_content(E, ONE, BLACK_PAWN);
    set_square_content(E, TWO, BLACK_PAWN);
    set_square_content(C, FOUR, BLACK_PAWN);
    set_square_content(C, FIVE, BLACK_PAWN);
    set_square_content(E, FOUR, BLACK_PAWN);
    set_square_content(E, FIVE, BLACK_PAWN);

    set_square_content(D, SEVEN, WHITE_PAWN);
    set_square_content(D, EIGHT, WHITE_PAWN);
    set_square_content(E, EIGHT, WHITE_PAWN);
    set_square_content(F, EIGHT, WHITE_PAWN);
    set_square_content(F, SEVEN, WHITE_PAWN);
    set_square_content(D, FIVE, WHITE_PAWN);
    set_square_content(D, FOUR, WHITE_PAWN);
    set_square_content(F, FIVE, WHITE_PAWN);
    set_square_content(F, FOUR, WHITE_PAWN);
    
    debug_moves[0] = create_move(D, TWO, C, TWO, NO_PROMOTION);
    debug_moves[1] = create_move(D, TWO, C, THREE, NO_PROMOTION);
    debug_moves[2] = create_move(D, TWO, E, THREE, NO_PROMOTION);
    debug_moves[3] = create_move(D, TWO, E, TWO, NO_PROMOTION);
    debug_moves[4] = create_move(D, TWO, D, ONE, NO_PROMOTION);
    debug_moves[5] = create_move(D, TWO, D, TWO, NO_PROMOTION);
    debug_moves[6] = create_move(D, TWO, C, FOUR, NO_PROMOTION);
    debug_moves[7] = create_move(D, TWO, D, FIVE, NO_PROMOTION);
    debug_moves[8] = create_move(D, TWO, E, FOUR, NO_PROMOTION);
    debug_moves[9] = create_move(D, TWO, D, THREE, NO_PROMOTION);

    debug_moves[10] = create_move(E, SEVEN, D, SEVEN, NO_PROMOTION);
    debug_moves[11] = create_move(E, SEVEN, D, SIX, NO_PROMOTION);
    debug_moves[12] = create_move(E, SEVEN, F, SIX, NO_PROMOTION);
    debug_moves[13] = create_move(E, SEVEN, F, SEVEN, NO_PROMOTION);
    debug_moves[14] = create_move(E, SEVEN, E, EIGHT, NO_PROMOTION);
    debug_moves[15] = create_move(E, SEVEN, E, SEVEN, NO_PROMOTION);
    debug_moves[16] = create_move(E, SEVEN, D, FIVE, NO_PROMOTION);
    debug_moves[17] = create_move(E, SEVEN, E, FOUR, NO_PROMOTION);
    debug_moves[18] = create_move(E, SEVEN, F, FIVE, NO_PROMOTION);
}

# Pawn standard movement 3
void create_test_moves() {
    skip_reset = TRUE;
    set_square_content(C, TWO, BLACK_PAWN);
    set_square_content(C, ONE, BLACK_PAWN);
    set_square_content(D, ONE, BLACK_PAWN);
    set_square_content(E, ONE, BLACK_PAWN);
    set_square_content(E, TWO, BLACK_PAWN);
    set_square_content(C, FOUR, BLACK_PAWN);
    set_square_content(C, FIVE, BLACK_PAWN);
    set_square_content(E, FOUR, BLACK_PAWN);
    set_square_content(E, FIVE, BLACK_PAWN);

    set_square_content(D, SEVEN, BLACK_PAWN);
    set_square_content(D, EIGHT, BLACK_PAWN);
    set_square_content(E, EIGHT, BLACK_PAWN);
    set_square_content(F, EIGHT, BLACK_PAWN);
    set_square_content(F, SEVEN, BLACK_PAWN);
    set_square_content(D, FIVE, BLACK_PAWN);
    set_square_content(D, FOUR, BLACK_PAWN);
    set_square_content(F, FIVE, BLACK_PAWN);
    set_square_content(F, FOUR, BLACK_PAWN);
    
    debug_moves[0] = create_move(D, TWO, C, TWO, NO_PROMOTION);
    debug_moves[1] = create_move(D, TWO, C, THREE, NO_PROMOTION);
    debug_moves[2] = create_move(D, TWO, E, THREE, NO_PROMOTION);
    debug_moves[3] = create_move(D, TWO, E, TWO, NO_PROMOTION);
    debug_moves[4] = create_move(D, TWO, D, ONE, NO_PROMOTION);
    debug_moves[5] = create_move(D, TWO, D, TWO, NO_PROMOTION);
    debug_moves[6] = create_move(D, TWO, C, FOUR, NO_PROMOTION);
    debug_moves[7] = create_move(D, TWO, D, FIVE, NO_PROMOTION);
    debug_moves[8] = create_move(D, TWO, E, FOUR, NO_PROMOTION);
    debug_moves[9] = create_move(D, TWO, D, THREE, NO_PROMOTION);

    debug_moves[10] = create_move(E, SEVEN, D, SEVEN, NO_PROMOTION);
    debug_moves[11] = create_move(E, SEVEN, D, SIX, NO_PROMOTION);
    debug_moves[12] = create_move(E, SEVEN, F, SIX, NO_PROMOTION);
    debug_moves[13] = create_move(E, SEVEN, F, SEVEN, NO_PROMOTION);
    debug_moves[14] = create_move(E, SEVEN, E, EIGHT, NO_PROMOTION);
    debug_moves[15] = create_move(E, SEVEN, E, SEVEN, NO_PROMOTION);
    debug_moves[16] = create_move(E, SEVEN, D, FIVE, NO_PROMOTION);
    debug_moves[17] = create_move(E, SEVEN, E, FOUR, NO_PROMOTION);
    debug_moves[18] = create_move(E, SEVEN, F, FIVE, NO_PROMOTION);
}

# Pawn standard movement 4
void create_test_moves() {
    skip_reset = TRUE;
    set_square_content(C, TWO, WHITE_PAWN);
    set_square_content(C, ONE, WHITE_PAWN);
    set_square_content(D, ONE, WHITE_PAWN);
    set_square_content(E, ONE, WHITE_PAWN);
    set_square_content(E, TWO, WHITE_PAWN);
    set_square_content(C, FOUR, WHITE_PAWN);
    set_square_content(C, FIVE, WHITE_PAWN);
    set_square_content(E, FOUR, WHITE_PAWN);
    set_square_content(E, FIVE, WHITE_PAWN);

    set_square_content(D, SEVEN, WHITE_PAWN);
    set_square_content(D, EIGHT, WHITE_PAWN);
    set_square_content(E, EIGHT, WHITE_PAWN);
    set_square_content(F, EIGHT, WHITE_PAWN);
    set_square_content(F, SEVEN, WHITE_PAWN);
    set_square_content(D, FIVE, WHITE_PAWN);
    set_square_content(D, FOUR, WHITE_PAWN);
    set_square_content(F, FIVE, WHITE_PAWN);
    set_square_content(F, FOUR, WHITE_PAWN);
    
    debug_moves[0] = create_move(D, TWO, C, TWO, NO_PROMOTION);
    debug_moves[1] = create_move(D, TWO, C, THREE, NO_PROMOTION);
    debug_moves[2] = create_move(D, TWO, E, THREE, NO_PROMOTION);
    debug_moves[3] = create_move(D, TWO, E, TWO, NO_PROMOTION);
    debug_moves[4] = create_move(D, TWO, D, ONE, NO_PROMOTION);
    debug_moves[5] = create_move(D, TWO, D, TWO, NO_PROMOTION);
    debug_moves[6] = create_move(D, TWO, C, FOUR, NO_PROMOTION);
    debug_moves[7] = create_move(D, TWO, D, FIVE, NO_PROMOTION);
    debug_moves[8] = create_move(D, TWO, E, FOUR, NO_PROMOTION);
    debug_moves[9] = create_move(D, TWO, D, THREE, NO_PROMOTION);

    debug_moves[10] = create_move(E, SEVEN, D, SEVEN, NO_PROMOTION);
    debug_moves[11] = create_move(E, SEVEN, D, SIX, NO_PROMOTION);
    debug_moves[12] = create_move(E, SEVEN, F, SIX, NO_PROMOTION);
    debug_moves[13] = create_move(E, SEVEN, F, SEVEN, NO_PROMOTION);
    debug_moves[14] = create_move(E, SEVEN, E, EIGHT, NO_PROMOTION);
    debug_moves[15] = create_move(E, SEVEN, E, SEVEN, NO_PROMOTION);
    debug_moves[16] = create_move(E, SEVEN, D, FIVE, NO_PROMOTION);
    debug_moves[17] = create_move(E, SEVEN, E, FOUR, NO_PROMOTION);
    debug_moves[18] = create_move(E, SEVEN, F, FIVE, NO_PROMOTION);
}

# Pawn standard capture
void create_test_moves() {
    debug_moves[0] = create_move(B, TWO, B, FOUR, NO_PROMOTION);
    debug_moves[1] = create_move(E, SEVEN, E, FIVE, NO_PROMOTION);

    debug_moves[2] = create_move(B, FOUR, B, FIVE, NO_PROMOTION);
    debug_moves[3] = create_move(E, FIVE, E, FOUR, NO_PROMOTION);

    debug_moves[4] = create_move(B, FIVE, B, SIX, NO_PROMOTION);
    debug_moves[5] = create_move(E, FOUR, E, THREE, NO_PROMOTION);

    debug_moves[6] = create_move(D, TWO, F, FOUR, NO_PROMOTION);
    debug_moves[7] = create_move(D, TWO, C, THREE, NO_PROMOTION);
    debug_moves[8] = create_move(D, TWO, E, THREE, NO_PROMOTION);
    debug_moves[9] = create_move(C, SEVEN, A, FIVE, NO_PROMOTION);
    debug_moves[10] = create_move(C, SEVEN, D, SIX, NO_PROMOTION);
    debug_moves[11] = create_move(C, SEVEN, B, SIX, NO_PROMOTION);
}

# Pawn standard capture 2
void create_test_moves() {
    skip_reset = TRUE;
    set_square_content(C, THREE, BLACK_PAWN);
    set_square_content(E, THREE, BLACK_PAWN);

    set_square_content(D, SIX, WHITE_PAWN);
    set_square_content(F, SIX, WHITE_PAWN);

    debug_moves[0] = create_move(D, TWO, B, FOUR, NO_PROMOTION);
    debug_moves[1] = create_move(D, TWO, F, FOUR, NO_PROMOTION);
    debug_moves[2] = create_move(D, TWO, D, THREE, NO_PROMOTION);

    debug_moves[3] = create_move(E, SEVEN, C, FIVE, NO_PROMOTION);
    debug_moves[4] = create_move(E, SEVEN, G, FIVE, NO_PROMOTION);
}

# Pawn standard capture 3
void create_test_moves() {
    skip_reset = TRUE;
    set_square_content(B, FOUR, BLACK_PAWN);
    set_square_content(F, FOUR, BLACK_PAWN);

    set_square_content(C, FIVE, WHITE_PAWN);
    set_square_content(G, FIVE, WHITE_PAWN);

    debug_moves[0] = create_move(D, TWO, B, FOUR, NO_PROMOTION);
    debug_moves[1] = create_move(D, TWO, F, FOUR, NO_PROMOTION);

    debug_moves[2] = create_move(E, SEVEN, C, FIVE, NO_PROMOTION);
    debug_moves[3] = create_move(E, SEVEN, G, FIVE, NO_PROMOTION);
}

# White and black full short castle test
void create_game_1_moves() {
    debug_moves[0] = create_move(E, TWO, E, FOUR, NO_PROMOTION);
    debug_moves[1] = create_move(D, SEVEN, D, FIVE, NO_PROMOTION);

    debug_moves[2] = create_move(E, FOUR, D, FIVE, NO_PROMOTION);
    debug_moves[3] = create_move(D, EIGHT, D, SIX, NO_PROMOTION);

    debug_moves[4] = create_move(F, ONE, C, FOUR, NO_PROMOTION);
    debug_moves[5] = create_move(C, EIGHT, G, FOUR, NO_PROMOTION);

    debug_moves[6] = create_move(G, ONE, F, THREE, NO_PROMOTION);
    debug_moves[7] = create_move(D, SIX, E, FIVE, NO_PROMOTION); 

    debug_moves[8] = create_move(E, ONE, G, ONE, NO_PROMOTION); // White attempts short castle in check
    debug_moves[9] = create_move(D, ONE, E, TWO, NO_PROMOTION);
    debug_moves[10] = create_move(G, FOUR, F, THREE, NO_PROMOTION);

    debug_moves[11] = create_move(E, TWO, E, FOUR, NO_PROMOTION);
    debug_moves[12] = create_move(E, FIVE, G, FIVE, NO_PROMOTION);

    debug_moves[13] = create_move(E, FOUR, G, FOUR, NO_PROMOTION);
    debug_moves[14] = create_move(F, THREE, G, TWO, NO_PROMOTION);

    debug_moves[15] = create_move(E, ONE, G, ONE, NO_PROMOTION); // White attempts short through check on f1
    debug_moves[16] = create_move(G, FOUR, G, TWO, NO_PROMOTION);
    debug_moves[17] = create_move(G, FIVE, G, TWO, NO_PROMOTION);

    debug_moves[18] = create_move(D, TWO, D, THREE, NO_PROMOTION);
    debug_moves[19] = create_move(G, TWO, G, THREE, NO_PROMOTION);

    debug_moves[20] = create_move(E, ONE, G, ONE, NO_PROMOTION); // White attempts short through check on g1
    debug_moves[21] = create_move(C, ONE, E, THREE, NO_PROMOTION);
    debug_moves[22] = create_move(G, THREE, F, THREE, NO_PROMOTION);

    debug_moves[23] = create_move(E, ONE, G, ONE, NO_PROMOTION); // White short castles successfully with h1 attacked
    debug_moves[24] = create_move(F, THREE, F, TWO, NO_PROMOTION);

    debug_moves[25] = create_move(G, ONE, F, TWO, NO_PROMOTION);
    debug_moves[26] = create_move(B, EIGHT, C, SIX, NO_PROMOTION);

    debug_moves[27] = create_move(F, TWO, E, ONE, NO_PROMOTION);
    debug_moves[28] = create_move(C, SIX, D, FOUR, NO_PROMOTION);

    debug_moves[29] = create_move(F, ONE, H, ONE, NO_PROMOTION);
    debug_moves[30] = create_move(E, SEVEN, E, SIX, NO_PROMOTION);

    debug_moves[31] = create_move(E, ONE, G, ONE, NO_PROMOTION); // White attempts short castle with no privileges
    debug_moves[32] = create_move(B, ONE, C, THREE, NO_PROMOTION);
    debug_moves[33] = create_move(F, EIGHT, D, SIX, NO_PROMOTION);

    debug_moves[34] = create_move(E, ONE, C, ONE, NO_PROMOTION); // White attempts long castle with no privileges
    debug_moves[35] = create_move(D, FIVE, E, SIX, NO_PROMOTION);
    debug_moves[36] = create_move(G, EIGHT, H, SIX, NO_PROMOTION);

    debug_moves[37] = create_move(E, SIX, F, SEVEN, NO_PROMOTION);
    debug_moves[38] = create_move(E, EIGHT, G, EIGHT, NO_PROMOTION); // Black attempts short castle in check
    debug_moves[39] = create_move(H, SIX, F, SEVEN, NO_PROMOTION);

    debug_moves[40] = create_move(H, ONE, F, ONE, NO_PROMOTION);
    debug_moves[41] = create_move(G, SEVEN, G, SIX, NO_PROMOTION);

    debug_moves[42] = create_move(F, ONE, F, SEVEN, NO_PROMOTION);
    debug_moves[43] = create_move(E, EIGHT, G, EIGHT, NO_PROMOTION); // Black attempts short castle through check on f8
    debug_moves[44] = create_move(C, SEVEN, C, SIX, NO_PROMOTION);

    debug_moves[45] = create_move(F, SEVEN, B, SEVEN, NO_PROMOTION);
    debug_moves[46] = create_move(E, EIGHT, G, EIGHT, NO_PROMOTION); // Black attempts short castle through check on g8
    debug_moves[47] = create_move(D, FOUR, E, SIX, NO_PROMOTION);

    debug_moves[48] = create_move(E, THREE, D, FOUR, NO_PROMOTION);
    debug_moves[49] = create_move(E, EIGHT, G, EIGHT, NO_PROMOTION); // Black short castles successfully with h8 attacked

    debug_moves[50] = create_move(B, SEVEN, B, THREE, NO_PROMOTION);
    debug_moves[51] = create_move(G, EIGHT, F, SEVEN, NO_PROMOTION);

    debug_moves[52] = create_move(C, FOUR, B, FIVE, NO_PROMOTION);
    debug_moves[53] = create_move(F, SEVEN, E, EIGHT, NO_PROMOTION);

    debug_moves[54] = create_move(D, FOUR, C, FIVE, NO_PROMOTION);
    debug_moves[55] = create_move(F, EIGHT, H, EIGHT, NO_PROMOTION);

    debug_moves[56] = create_move(B, THREE, A, THREE, NO_PROMOTION);
    debug_moves[57] = create_move(E, EIGHT, G, EIGHT, NO_PROMOTION); // Black attempts short castle with no privileges
    debug_moves[58] = create_move(A, SEVEN, A, FIVE, NO_PROMOTION);

    debug_moves[59] = create_move(A, THREE, A, FOUR, NO_PROMOTION);
    debug_moves[60] = create_move(E, EIGHT, C, EIGHT, NO_PROMOTION); // Black attempts long castle with no privileges
    debug_moves[61] = create_move(E, EIGHT, D, SEVEN, NO_PROMOTION);

    debug_moves[62] = create_move(E, ONE, D, TWO, NO_PROMOTION);
}

# White and black full long castle test
void create_game_1_moves() {
    debug_moves[0] = create_move(D, TWO, D, FOUR, NO_PROMOTION);
    debug_moves[1] = create_move(D, SEVEN, D, FIVE, NO_PROMOTION);

    debug_moves[2] = create_move(D, ONE, D, THREE, NO_PROMOTION);
    debug_moves[3] = create_move(D, EIGHT, D, SIX, NO_PROMOTION);

    debug_moves[4] = create_move(C, ONE, F, FOUR, NO_PROMOTION);
    debug_moves[5] = create_move(C, EIGHT, F, FIVE, NO_PROMOTION);

    debug_moves[6] = create_move(B, ONE, A, THREE, NO_PROMOTION);
    debug_moves[7] = create_move(B, EIGHT, A, SIX, NO_PROMOTION);

    debug_moves[8] = create_move(G, ONE, H, THREE, NO_PROMOTION);
    debug_moves[9] = create_move(G, EIGHT, H, SIX, NO_PROMOTION);

    debug_moves[10] = create_move(G, TWO, G, THREE, NO_PROMOTION);
    debug_moves[11] = create_move(G, SEVEN, G, SIX, NO_PROMOTION);

    debug_moves[12] = create_move(F, ONE, G, TWO, NO_PROMOTION);
    debug_moves[13] = create_move(F, EIGHT, G, SEVEN, NO_PROMOTION);

    debug_moves[14] = create_move(D, THREE, B, FIVE, NO_PROMOTION);
    debug_moves[15] = create_move(E, EIGHT, C, EIGHT, NO_PROMOTION); // Black attempts long castle in check
    debug_moves[16] = create_move(F, FIVE, D, SEVEN, NO_PROMOTION);

    debug_moves[17] = create_move(B, FIVE, B, SIX, NO_PROMOTION);
    debug_moves[18] = create_move(D, SIX, B, FOUR, NO_PROMOTION);

    debug_moves[19] = create_move(E, ONE, C, ONE, NO_PROMOTION); // White attempts long castle in check
    debug_moves[20] = create_move(F, FOUR, D, TWO, NO_PROMOTION);
    debug_moves[21] = create_move(B, FOUR, B, THREE, NO_PROMOTION);

    debug_moves[22] = create_move(B, SIX, C, SEVEN, NO_PROMOTION);
    debug_moves[23] = create_move(B, THREE, C, TWO, NO_PROMOTION);

    debug_moves[24] = create_move(C, SEVEN, C, SIX, NO_PROMOTION);
    debug_moves[25] = create_move(C, TWO, C, THREE, NO_PROMOTION);

    debug_moves[26] = create_move(E, ONE, C, ONE, NO_PROMOTION); // White attempts to long castle through check on c1
    debug_moves[27] = create_move(C, SIX, C, FIVE, NO_PROMOTION);
    debug_moves[28] = create_move(E, EIGHT, C, EIGHT, NO_PROMOTION); // Black attempts to long castle through check on c8
    debug_moves[29] = create_move(C, THREE, C, FOUR, NO_PROMOTION);

    debug_moves[30] = create_move(C, FIVE, B, SIX, NO_PROMOTION);
    debug_moves[31] = create_move(E, EIGHT, C, EIGHT, NO_PROMOTION); // Black attempts to long castle through check on d8
    debug_moves[32] = create_move(C, FOUR, B, THREE, NO_PROMOTION);

    debug_moves[33] = create_move(E, ONE, C, ONE, NO_PROMOTION); // White attempts to long castle through check on d1
    debug_moves[34] = create_move(B, SIX, A, SEVEN, NO_PROMOTION);
    debug_moves[35] = create_move(B, THREE, A, TWO, NO_PROMOTION);

    debug_moves[36] = create_move(D, TWO, F, FOUR, NO_PROMOTION);
    debug_moves[37] = create_move(D, SEVEN, F, FIVE, NO_PROMOTION);

    debug_moves[38] = create_move(E, ONE, C, ONE, NO_PROMOTION); // White successfully long castles with a1 and b1 attacked
    debug_moves[39] = create_move(E, EIGHT, C, EIGHT, NO_PROMOTION); // Black successfully long castles with a8 and b8 attacked

    debug_moves[40] = create_move(C, ONE, D, TWO, NO_PROMOTION);
    debug_moves[41] = create_move(C, EIGHT, D, SEVEN, NO_PROMOTION);

    debug_moves[42] = create_move(D, TWO, E, ONE, NO_PROMOTION);
    debug_moves[43] = create_move(D, SEVEN, E, EIGHT, NO_PROMOTION);

    debug_moves[44] = create_move(D, ONE, A, ONE, NO_PROMOTION);
    debug_moves[45] = create_move(D, EIGHT, A, EIGHT, NO_PROMOTION);

    debug_moves[46] = create_move(E, ONE, C, ONE, NO_PROMOTION); // White attempts to long castle without privileges
    debug_moves[47] = create_move(E, ONE, G, ONE, NO_PROMOTION); // White attempts to short castle without privileges
    debug_moves[48] = create_move(A, ONE, A, TWO, NO_PROMOTION);
    debug_moves[49] = create_move(E, EIGHT, C, EIGHT, NO_PROMOTION); // Black attempts to long castle without privileges
    debug_moves[50] = create_move(E, EIGHT, G, EIGHT, NO_PROMOTION); // Black attempts to short castle without privileges
    debug_moves[51] = create_move(A, EIGHT, A, SEVEN, NO_PROMOTION);
}

# Pawn special move privilege revoked after standard move
void create_test_moves() {
    debug_moves[0] = create_move(E, TWO, E, THREE, NO_PROMOTION);
    debug_moves[1] = create_move(B, SEVEN, B, SIX, NO_PROMOTION);

    debug_moves[2] = create_move(E, THREE, E, FIVE, NO_PROMOTION);
    debug_moves[3] = create_move(E, THREE, E, FOUR, NO_PROMOTION);
    debug_moves[4] = create_move(B, SIX, B, FOUR, NO_PROMOTION);
    debug_moves[5] = create_move(B, SIX, B, FIVE, NO_PROMOTION);
}

# Pawn special move privilege revoked after standard capture
void create_test_moves() {
    debug_moves[0] = create_move(A, TWO, A, FOUR, NO_PROMOTION);
    debug_moves[1] = create_move(D, SEVEN, D, FIVE, NO_PROMOTION);

    debug_moves[2] = create_move(A, FOUR, A, FIVE, NO_PROMOTION);
    debug_moves[3] = create_move(D, FIVE, D, FOUR, NO_PROMOTION);

    debug_moves[4] = create_move(A, FIVE, A, SIX, NO_PROMOTION);
    debug_moves[5] = create_move(D, FOUR, D, THREE, NO_PROMOTION);

    debug_moves[6] = create_move(C, TWO, D, THREE, NO_PROMOTION);
    debug_moves[7] = create_move(B, SEVEN, A, SIX, NO_PROMOTION);

    debug_moves[8] = create_move(D, THREE, D, FIVE, NO_PROMOTION);
    debug_moves[9] = create_move(D, THREE, D, FOUR, NO_PROMOTION);
    debug_moves[10] = create_move(A, SIX, A, FOUR, NO_PROMOTION);
    debug_moves[11] = create_move(A, SIX, A, FIVE, NO_PROMOTION);
}

# Test game 1
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

# Test game 2 (en passant checkmate)
void create_game_1_moves() {
    debug_moves[0] = create_move(E, TWO, E, FOUR, NO_PROMOTION);
    debug_moves[1] = create_move(E, SEVEN, E, SIX, NO_PROMOTION);

    debug_moves[2] = create_move(D, TWO, D, FOUR, NO_PROMOTION);
    debug_moves[3] = create_move(D, SEVEN, D, FIVE, NO_PROMOTION);

    debug_moves[4] = create_move(E, FOUR, E, FIVE, NO_PROMOTION);
    debug_moves[5] = create_move(C, SEVEN, C, FIVE, NO_PROMOTION);

    debug_moves[6] = create_move(C, TWO, C, THREE, NO_PROMOTION);
    debug_moves[7] = create_move(C, FIVE, D, FOUR, NO_PROMOTION);

    debug_moves[8] = create_move(C, THREE, D, FOUR, NO_PROMOTION);
    debug_moves[9] = create_move(F, EIGHT, B, FOUR, NO_PROMOTION);

    debug_moves[10] = create_move(B, ONE, C, THREE, NO_PROMOTION);
    debug_moves[11] = create_move(B, EIGHT, C, SIX, NO_PROMOTION);

    debug_moves[12] = create_move(G, ONE, F, THREE, NO_PROMOTION);
    debug_moves[13] = create_move(G, EIGHT, E, SEVEN, NO_PROMOTION);

    debug_moves[14] = create_move(F, ONE, D, THREE, NO_PROMOTION);
    debug_moves[15] = create_move(E, EIGHT, G, EIGHT, NO_PROMOTION);

    debug_moves[16] = create_move(D, THREE, H, SEVEN, NO_PROMOTION);
    debug_moves[17] = create_move(G, EIGHT, H, SEVEN, NO_PROMOTION);

    debug_moves[18] = create_move(F, THREE, G, FIVE, NO_PROMOTION);
    debug_moves[19] = create_move(H, SEVEN, G, SIX, NO_PROMOTION);

    debug_moves[20] = create_move(H, TWO, H, FOUR, NO_PROMOTION);
    debug_moves[21] = create_move(C, SIX, D, FOUR, NO_PROMOTION);

    debug_moves[22] = create_move(D, ONE, G, FOUR, NO_PROMOTION);
    debug_moves[23] = create_move(F, SEVEN, F, FIVE, NO_PROMOTION);

    debug_moves[24] = create_move(H, FOUR, H, FIVE, NO_PROMOTION);
    debug_moves[25] = create_move(G, SIX, H, SIX, NO_PROMOTION);

    debug_moves[26] = create_move(G, FIVE, E, SIX, NO_PROMOTION);
    debug_moves[27] = create_move(G, SEVEN, G, FIVE, NO_PROMOTION);
    
    debug_moves[28] = create_move(H, FIVE, G, SIX, NO_PROMOTION);
}

# Test game 3 (short castles checkmate)
void create_game_1_moves() {
    debug_moves[0] = create_move(E, TWO, E, FOUR, NO_PROMOTION);
    debug_moves[1] = create_move(E, SEVEN, E, FIVE, NO_PROMOTION);

    debug_moves[2] = create_move(D, ONE, H, FIVE, NO_PROMOTION);
    debug_moves[3] = create_move(B, EIGHT, C, SIX, NO_PROMOTION);

    debug_moves[4] = create_move(F, ONE, C, FOUR, NO_PROMOTION);
    debug_moves[5] = create_move(G, SEVEN, G, SIX, NO_PROMOTION);

    debug_moves[6] = create_move(H, FIVE, F, THREE, NO_PROMOTION);
    debug_moves[7] = create_move(D, EIGHT, E, SEVEN, NO_PROMOTION);

    debug_moves[8] = create_move(B, ONE, C, THREE, NO_PROMOTION);
    debug_moves[9] = create_move(C, SIX, D, FOUR, NO_PROMOTION);

    debug_moves[10] = create_move(F, THREE, D, ONE, NO_PROMOTION);
    debug_moves[11] = create_move(C, SEVEN, C, SIX, NO_PROMOTION);

    debug_moves[12] = create_move(G, ONE, F, THREE, NO_PROMOTION);
    debug_moves[13] = create_move(F, EIGHT, G, SEVEN, NO_PROMOTION);

    debug_moves[14] = create_move(F, THREE, D, FOUR, NO_PROMOTION);
    debug_moves[15] = create_move(E, FIVE, D, FOUR, NO_PROMOTION);

    debug_moves[16] = create_move(C, THREE, E, TWO, NO_PROMOTION);
    debug_moves[17] = create_move(D, SEVEN, D, FIVE, NO_PROMOTION);

    debug_moves[18] = create_move(C, FOUR, B, THREE, NO_PROMOTION);
    debug_moves[19] = create_move(D, FIVE, E, FOUR, NO_PROMOTION);

    debug_moves[20] = create_move(E, TWO, D, FOUR, NO_PROMOTION);
    debug_moves[21] = create_move(G, SEVEN, D, FOUR, NO_PROMOTION);

    debug_moves[22] = create_move(C, TWO, C, THREE, NO_PROMOTION);
    debug_moves[23] = create_move(D, FOUR, B, SIX, NO_PROMOTION);

    debug_moves[24] = create_move(D, TWO, D, FOUR, NO_PROMOTION);
    debug_moves[25] = create_move(E, FOUR, D, THREE, NO_PROMOTION);

    debug_moves[26] = create_move(C, ONE, E, THREE, NO_PROMOTION);
    debug_moves[27] = create_move(B, SIX, E, THREE, NO_PROMOTION);

    debug_moves[28] = create_move(F, TWO, E, THREE, NO_PROMOTION);
    debug_moves[29] = create_move(E, SEVEN, E, THREE, NO_PROMOTION);

    debug_moves[30] = create_move(E, ONE, F, ONE, NO_PROMOTION);
    debug_moves[31] = create_move(C, EIGHT, E, SIX, NO_PROMOTION);

    debug_moves[32] = create_move(B, THREE, E, SIX, NO_PROMOTION);
    debug_moves[33] = create_move(F, SEVEN, E, SIX, NO_PROMOTION);

    debug_moves[34] = create_move(H, TWO, H, THREE, NO_PROMOTION);
    debug_moves[35] = create_move(G, EIGHT, H, SIX, NO_PROMOTION);

    debug_moves[36] = create_move(D, ONE, B, THREE, NO_PROMOTION);
    debug_moves[37] = create_move(E, EIGHT, G, EIGHT, NO_PROMOTION);
}

# Test game 4 (fifty move rule draw)
void create_game_1_moves() {
    debug_moves[0] = create_move(A, TWO, A, FOUR, NO_PROMOTION);
    debug_moves[1] = create_move(A, SEVEN, A, FIVE, NO_PROMOTION);

    debug_moves[2] = create_move(D, TWO, D, THREE, NO_PROMOTION);
    debug_moves[3] = create_move(D, SEVEN, D, SIX, NO_PROMOTION);

    debug_moves[4] = create_move(E, TWO, E, THREE, NO_PROMOTION);
    debug_moves[5] = create_move(E, SEVEN, E, SIX, NO_PROMOTION);

    debug_moves[6] = create_move(A, ONE, A, TWO, NO_PROMOTION);
    debug_moves[7] = create_move(A, EIGHT, A, SEVEN, NO_PROMOTION);

    debug_moves[8] = create_move(A, TWO, A, THREE, NO_PROMOTION);
    debug_moves[9] = create_move(A, SEVEN, A, SIX, NO_PROMOTION);

    debug_moves[10] = create_move(A, THREE, B, THREE, NO_PROMOTION);
    debug_moves[11] = create_move(A, SIX, B, SIX, NO_PROMOTION);

    debug_moves[12] = create_move(B, THREE, B, FOUR, NO_PROMOTION);
    debug_moves[13] = create_move(B, SIX, B, FIVE, NO_PROMOTION);

    debug_moves[14] = create_move(B, FOUR, C, FOUR, NO_PROMOTION);
    debug_moves[15] = create_move(B, FIVE, C, FIVE, NO_PROMOTION);

    debug_moves[16] = create_move(C, FOUR, C, THREE, NO_PROMOTION);
    debug_moves[17] = create_move(C, FIVE, C, SIX, NO_PROMOTION);

    debug_moves[18] = create_move(C, THREE, C, FOUR, NO_PROMOTION);
    debug_moves[19] = create_move(C, SIX, C, FIVE, NO_PROMOTION);

    debug_moves[20] = create_move(C, FOUR, D, FOUR, NO_PROMOTION);
    debug_moves[21] = create_move(C, FIVE, D, FIVE, NO_PROMOTION);

    debug_moves[22] = create_move(D, FOUR, E, FOUR, NO_PROMOTION);
    debug_moves[23] = create_move(D, FIVE, E, FIVE, NO_PROMOTION);

    debug_moves[24] = create_move(E, FOUR, F, FOUR, NO_PROMOTION);
    debug_moves[25] = create_move(E, FIVE, F, FIVE, NO_PROMOTION);

    debug_moves[26] = create_move(F, FOUR, F, THREE, NO_PROMOTION);
    debug_moves[27] = create_move(F, FIVE, F, SIX, NO_PROMOTION);

    debug_moves[28] = create_move(F, THREE, G, THREE, NO_PROMOTION);
    debug_moves[29] = create_move(F, SIX, G, SIX, NO_PROMOTION);

    debug_moves[30] = create_move(G, THREE, G, FOUR, NO_PROMOTION);
    debug_moves[31] = create_move(G, SIX, G, FIVE, NO_PROMOTION);

    debug_moves[32] = create_move(G, FOUR, H, FOUR, NO_PROMOTION);
    debug_moves[33] = create_move(G, FIVE, H, FIVE, NO_PROMOTION);

    debug_moves[34] = create_move(H, FOUR, H, THREE, NO_PROMOTION);
    debug_moves[35] = create_move(H, FIVE, H, SIX, NO_PROMOTION);

    debug_moves[36] = create_move(C, ONE, D, TWO, NO_PROMOTION);
    debug_moves[37] = create_move(C, EIGHT, D, SEVEN, NO_PROMOTION);

    // Checkpoint 1

    debug_moves[38] = create_move(H, THREE, H, FOUR, NO_PROMOTION);
    debug_moves[39] = create_move(H, SIX, H, FIVE, NO_PROMOTION);

    debug_moves[40] = create_move(H, FOUR, G, FOUR, NO_PROMOTION);
    debug_moves[41] = create_move(H, FIVE, G, FIVE, NO_PROMOTION);

    debug_moves[42] = create_move(G, FOUR, G, THREE, NO_PROMOTION);
    debug_moves[43] = create_move(G, FIVE, G, SIX, NO_PROMOTION);

    debug_moves[44] = create_move(G, THREE, F, THREE, NO_PROMOTION);
    debug_moves[45] = create_move(G, SIX, F, SIX, NO_PROMOTION);

    debug_moves[46] = create_move(F, THREE, F, FOUR, NO_PROMOTION);
    debug_moves[47] = create_move(F, SIX, F, FIVE, NO_PROMOTION);

    debug_moves[48] = create_move(F, FOUR, E, FOUR, NO_PROMOTION);
    debug_moves[49] = create_move(F, FIVE, E, FIVE, NO_PROMOTION);

    debug_moves[50] = create_move(E, FOUR, D, FOUR, NO_PROMOTION);
    debug_moves[51] = create_move(E, FIVE, D, FIVE, NO_PROMOTION);

    debug_moves[52] = create_move(D, FOUR, C, FOUR, NO_PROMOTION);
    debug_moves[53] = create_move(D, FIVE, C, FIVE, NO_PROMOTION);

    debug_moves[54] = create_move(C, FOUR, C, THREE, NO_PROMOTION);
    debug_moves[55] = create_move(C, FIVE, C, SIX, NO_PROMOTION);

    debug_moves[56] = create_move(C, THREE, B, THREE, NO_PROMOTION);
    debug_moves[57] = create_move(C, SIX, B, SIX, NO_PROMOTION);

    debug_moves[58] = create_move(B, THREE, B, FOUR, NO_PROMOTION);
    debug_moves[59] = create_move(B, SIX, B, FIVE, NO_PROMOTION);

    debug_moves[60] = create_move(B, FOUR, B, THREE, NO_PROMOTION);
    debug_moves[61] = create_move(B, FIVE, B, SIX, NO_PROMOTION);

    debug_moves[62] = create_move(B, THREE, A, THREE, NO_PROMOTION);
    debug_moves[63] = create_move(B, SIX, A, SIX, NO_PROMOTION);

    debug_moves[64] = create_move(A, THREE, A, TWO, NO_PROMOTION);
    debug_moves[65] = create_move(A, SIX, A, SEVEN, NO_PROMOTION);

    debug_moves[66] = create_move(A, TWO, A, ONE, NO_PROMOTION);
    debug_moves[67] = create_move(A, SEVEN, A, EIGHT, NO_PROMOTION);

    debug_moves[68] = create_move(F, ONE, E, TWO, NO_PROMOTION);
    debug_moves[69] = create_move(F, EIGHT, E, SEVEN, NO_PROMOTION);

    // Checkpoint 2

    debug_moves[70] = create_move(A, ONE, A, TWO, NO_PROMOTION);
    debug_moves[71] = create_move(A, EIGHT, A, SEVEN, NO_PROMOTION);

    debug_moves[72] = create_move(A, TWO, A, THREE, NO_PROMOTION);
    debug_moves[73] = create_move(A, SEVEN, A, SIX, NO_PROMOTION);

    debug_moves[74] = create_move(A, THREE, B, THREE, NO_PROMOTION);
    debug_moves[75] = create_move(A, SIX, B, SIX, NO_PROMOTION);

    debug_moves[76] = create_move(B, THREE, B, FOUR, NO_PROMOTION);
    debug_moves[77] = create_move(B, SIX, B, FIVE, NO_PROMOTION);

    debug_moves[78] = create_move(B, FOUR, C, FOUR, NO_PROMOTION);
    debug_moves[79] = create_move(B, FIVE, C, FIVE, NO_PROMOTION);

    debug_moves[80] = create_move(C, FOUR, C, THREE, NO_PROMOTION);
    debug_moves[81] = create_move(C, FIVE, C, SIX, NO_PROMOTION);

    debug_moves[82] = create_move(C, THREE, C, FOUR, NO_PROMOTION);
    debug_moves[83] = create_move(C, SIX, C, FIVE, NO_PROMOTION);

    debug_moves[84] = create_move(C, FOUR, D, FOUR, NO_PROMOTION);
    debug_moves[85] = create_move(C, FIVE, D, FIVE, NO_PROMOTION);

    debug_moves[86] = create_move(D, FOUR, E, FOUR, NO_PROMOTION);
    debug_moves[87] = create_move(D, FIVE, E, FIVE, NO_PROMOTION);

    debug_moves[88] = create_move(E, FOUR, F, FOUR, NO_PROMOTION);
    debug_moves[89] = create_move(E, FIVE, F, FIVE, NO_PROMOTION);

    debug_moves[90] = create_move(F, FOUR, F, THREE, NO_PROMOTION);
    debug_moves[91] = create_move(F, FIVE, F, SIX, NO_PROMOTION);

    debug_moves[92] = create_move(F, THREE, G, THREE, NO_PROMOTION);
    debug_moves[93] = create_move(F, SIX, G, SIX, NO_PROMOTION);

    debug_moves[94] = create_move(G, THREE, G, FOUR, NO_PROMOTION);
    debug_moves[95] = create_move(G, SIX, G, FIVE, NO_PROMOTION);

    debug_moves[96] = create_move(G, FOUR, H, FOUR, NO_PROMOTION);
    debug_moves[97] = create_move(G, FIVE, H, FIVE, NO_PROMOTION);

    debug_moves[98] = create_move(H, FOUR, H, THREE, NO_PROMOTION);
    debug_moves[99] = create_move(H, FIVE, H, SIX, NO_PROMOTION);

    debug_moves[100] = create_move(D, ONE, C, ONE, NO_PROMOTION);
    debug_moves[101] = create_move(D, EIGHT, C, EIGHT, NO_PROMOTION);

    debug_moves[102] = create_move(H, THREE, H, FOUR, NO_PROMOTION);
    debug_moves[103] = create_move(H, SIX, H, FIVE, NO_PROMOTION);

    debug_moves[104] = create_move(H, FOUR, G, FOUR, NO_PROMOTION);
    debug_moves[105] = create_move(H, FIVE, G, FIVE, NO_PROMOTION);
}

# Game template
void create_game_1_moves() {
    debug_moves[0] = create_move(NO_PROMOTION);
    debug_moves[1] = create_move(NO_PROMOTION);
    debug_moves[2] = create_move(NO_PROMOTION);
    debug_moves[3] = create_move(NO_PROMOTION);
    debug_moves[4] = create_move(NO_PROMOTION);
    debug_moves[5] = create_move(NO_PROMOTION);
    debug_moves[6] = create_move(NO_PROMOTION);
    debug_moves[7] = create_move(NO_PROMOTION);
    debug_moves[8] = create_move(NO_PROMOTION);
    debug_moves[9] = create_move(NO_PROMOTION);
    debug_moves[10] = create_move(NO_PROMOTION);
    debug_moves[11] = create_move(NO_PROMOTION);
    debug_moves[12] = create_move(NO_PROMOTION);
    debug_moves[13] = create_move(NO_PROMOTION);
    debug_moves[14] = create_move(NO_PROMOTION);
    debug_moves[15] = create_move(NO_PROMOTION);
    debug_moves[16] = create_move(NO_PROMOTION);
    debug_moves[17] = create_move(NO_PROMOTION);
    debug_moves[18] = create_move(NO_PROMOTION);
    debug_moves[19] = create_move(NO_PROMOTION);
    debug_moves[20] = create_move(NO_PROMOTION);
    debug_moves[21] = create_move(NO_PROMOTION);
    debug_moves[22] = create_move(NO_PROMOTION);
    debug_moves[23] = create_move(NO_PROMOTION);
    debug_moves[24] = create_move(NO_PROMOTION);
    debug_moves[25] = create_move(NO_PROMOTION);
    debug_moves[26] = create_move(NO_PROMOTION);
    debug_moves[27] = create_move(NO_PROMOTION);
    debug_moves[28] = create_move(NO_PROMOTION);
    debug_moves[29] = create_move(NO_PROMOTION);
    debug_moves[30] = create_move(NO_PROMOTION);
    debug_moves[31] = create_move(NO_PROMOTION);
}

# Puzzle 1 correct move
void create_game_1_moves() {
    debug_moves[0] = create_move(E, THREE, C, FOUR, NO_PROMOTION);
}

# Puzzle 1 incorrect move 1
void create_game_1_moves() {
    debug_moves[0] = create_move(E, THREE, F, FIVE, NO_PROMOTION);
    debug_moves[1] = create_move(E, THREE, G, FOUR, NO_PROMOTION);
}

# Puzzle 1 incorrect move 2
void create_game_1_moves() {
    debug_moves[0] = create_move(A, FOUR, A, SIX, NO_PROMOTION);
    debug_moves[1] = create_move(A, FOUR, B, FIVE, NO_PROMOTION);
    debug_moves[2] = create_move(A, FOUR, A, FIVE, NO_PROMOTION);
}

# Puzzle 1 incorrect move 3
void create_game_1_moves() {
    debug_moves[0] = create_move(F, FIVE, F, SEVEN, NO_PROMOTION);
    debug_moves[1] = create_move(F, FIVE, G, SIX, NO_PROMOTION);
    debug_moves[2] = create_move(F, FIVE, E, SIX, NO_PROMOTION);
}

# Puzzle 1 incorrect move 4
void create_game_1_moves() {
    debug_moves[0] = create_move(D, ONE, D, FIVE, NO_PROMOTION);
    debug_moves[1] = create_move(D, ONE, H, ONE, NO_PROMOTION);
}

# Puzzle 1 incorrect move 5
void create_game_1_moves() {
    debug_moves[0] = create_move(F, SEVEN, E, SIX, NO_PROMOTION);
}

# Puzzle template
void create_game_1_moves() {
    debug_moves[0] = create_move(NO_PROMOTION);
}