// It's EXTREMELY HARD to REALIABLE pass param to 16bit inline asm (GPT said and many AI struggled and failed)
// So we choose using shared data to pass params. EVEN C code function param passing is not working!!!!!! So
// we have to use macro
// CC = i686-elf-gcc
// CFLAGS = -m16 -ffreestanding -fno-pie \
//          -nostdlib -nostdinc -fno-asynchronous-unwind-tables \
//          -fno-builtin -fno-stack-protector -mno-mmx -mno-sse
////#pragma GCC optimize("O0")
#include "bootsector.h"

ASM_EPILOG_TIMER_INTERUPT;

// Time get_rtc_time(void);
// void print_time_at(Time time, char row, char col, char color);
DATA_BIOS_PARAM;
DATA_ADV_MSG;
// Messages
char HELLO_MSG[] = " C in timer mode! ";
short _scroll_pos_ = 0;

FN_BIOS_CLEAR_SCREEN;
FN_BIOS_SET_CURSOR_POS__ROW_COL;
FN_BIOS_PRINT_STRING__MSG_COLOR;
//FN_BIOS_PRINT_ADDRESS_AS_HEX;

void print_hi_msg_scroll() {
    BIOS_PRINT_STRING__MSG(HELLO_MSG + _scroll_pos_);
    _asm_char_2_ = HELLO_MSG[_scroll_pos_];
    HELLO_MSG[_scroll_pos_] = 0;
    BIOS_PRINT_STRING__MSG(HELLO_MSG);
    HELLO_MSG[_scroll_pos_] = _asm_char_2_;
    if (++_scroll_pos_ >= sizeof(HELLO_MSG)) {
        _scroll_pos_ = 0;
    }
}

volatile unsigned char delay = 0;
BEGIN_TIMER_HANDLER;

    if (++delay > 183) {
        delay = 0;
        BIOS_SET_CURSOR_POS__ROW_COL(8, 31);
        BIOS_SET_PRINT_COLOR__COLOR(COLOR_WHITE);
        print_hi_msg_scroll();
    }

END_TIMER_HANDLER;

END_BOOTSECTOR