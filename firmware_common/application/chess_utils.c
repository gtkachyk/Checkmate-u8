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