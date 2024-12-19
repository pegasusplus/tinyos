#ifndef _BOOT_SECTOR_H_
#define _BOOT_SECTOR_H_

#define ASM_EPILOG \
__asm__(    \
    ".code16\n\t"           \
    ".global _start\n\t"    \
"_start:\n" \
    "xor %ax, %ax\n\t"      \
    "mov %ax, %ds\n\t"      \
    "mov %ax, %es\n\t"      \
    "mov %ax, %cs\n\t"      \
    "mov %ax, %ss\n\t"      \
    "mov $0x7BFF, %sp\n\t"  \
    "jmp $0x0000, $bootsector_main\n\t" \
)

// Colors in BIOS print char
enum {
     COLOR_WHITE = 0x0F,
     COLOR_YELLOW = 0x0E,
     COLOR_CYAN = 0x0D,
//     COLOR_RED = 0x0C,
     COLOR_BLUE = 0x0B,
     COLOR_GREEN = 0x0A,
     COLOR_PURPLE = 0x09,
     COLOR_ORANGE = 0x08,
//     COLOR_BROWN = 0x07,
//     COLOR_GREY = 0x06,
//     COLOR_LIGHT_GREY = 0x05,
//     COLOR_DARK_GREY = 0x04,
//     COLOR_GREY_BLUE = 0x03,
//     COLOR_LIGHT_GREEN = 0x02,
//     COLOR_LIGHT_BLUE = 0x01,
//     COLOR_BLACK = 0x00,
};

#define FN_BIOS_SET_CURSOR_POS__ROW_COL \
void asm_set_cursor_pos__row_col() {\
    __asm__ volatile (\
        "pushal\n\t"\
        "xor %%bh, %%bh\n\t"\
        "movb _asm_char_1_, %%dh\n\t"\
        "movb _asm_char_2_, %%dl\n\t"\
        "movb $0x02, %%ah\n\t"\
        "int $0x10\n\t"\
        "popal\n\t"\
        :\
        :\
        : "memory"\
    );\
}

#define FN_BIOS_CLEAR_SCREEN \
void asm_clear_screen() {\
    __asm__ volatile (\
        "pushal\n\t"\
        "mov $0x0003, %%ax\n\t"\
        "int $0x10\n\t"\
        "popal\n\t"\
        :\
        :\
        :\
    );\
}

#define BIOS_CLEAR_SCREEN(_) asm_clear_screen()

#define FN_BIOS_PRINT_STRING__MSG_COLOR \
void asm_print_string__msg_color() {\
    __asm__ volatile (\
        ".code16\n\t"\
        "pushal\n\t"\
        "mov _asm_msg_, %%si\n\t"\
    ".loop:\n\t"\
        "lodsb\n\t"              /* Load next byte from [SI] into AL (address of str pointed by ESI) */\
        "test %%al, %%al\n\t"    /* Test AL (check for null terminator) */\
        "jz .done\n\t"           /* Jump to done if AL is 0 (null terminator) */\
        "movb $0x09, %%ah\n\t"   /* BIOS teletype (character + attribute) */\
        "movw $1, %%cx\n\t"      /* Print 1 character */\
        "movb _asm_char_1_, %%bl\n\t"\
        "xor %%bh, %%bh\n\t"      /* Page number 0 */\
        "int $0x10\n\t"\
        /* Move cursor forward */\
        "movb $0x03, %%ah\n\t"   /* BIOS get cursor position, result col in %dl */\
        "xor %%bh, %%bh\n\t"      /* Page number 0 */\
        "int $0x10\n\t"\
        "inc %%dl\n\t"           /* Increment column (move cursor forward) */\
        "movb $0x02, %%ah\n\t"   /* BIOS set cursor position */\
        "int $0x10\n\t"\
        "jmp .loop\n\t"          /* Repeat loop to print next char */\
    ".done:\n\t"\
        "popal\n\t"\
        :\
        :\
        : "memory" /* Clobbered registers */\
    );\
}

#define FN_BIOS_PRINT_ADDRESS_AS_HEX \
/* // Define a buffer to hold the hexadecimal string */\
char _hex_buffer_[5] = ":0000";\
const void *_address_=ADV_MSG;\
void asm_print_address_as_hex() {\
    asm volatile (\
        "pushal\n\t"\
        "mov _address_, %%ax\n\t"    /* // Move SP to AX */\
        "mov %%ax, %%bx\n\t"         /* // Copy AX to BX for manipulation */\
        "mov $0x0F, %%cl\n\t"        /* // Mask for lower nibble */\
        /* // Convert high byte */\
        "mov %%bh, %%al\n\t"         /* // Move high byte of BX to AL */\
        "shr $4, %%al\n\t"           /* // Shift right by 4 to get high nibble */\
        "and %%cl, %%al\n\t"         /* // Mask lower nibble */\
        "add $0x30, %%al\n\t"        /* // Convert to ASCII */\
        "cmp $0x39, %%al\n\t"        /* // Check if it's a digit */\
        "jbe 1f\n\t"\
        "add $0x07, %%al\n\t"        /* // Convert to letter if necessary */\
        "1: mov %%al, _hex_buffer_+1\n\t"    /* // Store in buffer */\
        "mov %%bh, %%al\n\t"         /* // Move high byte of BX to AL */\
        "and %%cl, %%al\n\t"         /* // Mask lower nibble */\
        "add $0x30, %%al\n\t"        /* // Convert to ASCII */\
        "cmp $0x39, %%al\n\t"        /* // Check if it's a digit */\
        "jbe 2f\n\t"\
        "add $0x07, %%al\n\t"        /* // Convert to letter if necessary */\
        "2: mov %%al, _hex_buffer_+2\n\t"  /* // Store in buffer */\
        /* // Convert low byte */\
        "mov %%bl, %%al\n\t"         /* // Move low byte of BX to AL */\
        "shr $4, %%al\n\t"           /* // Shift right by 4 to get high nibble */\
        "and %%cl, %%al\n\t"         /* // Mask lower nibble */\
        "add $0x30, %%al\n\t"        /* // Convert to ASCII */\
        "cmp $0x39, %%al\n\t"        /* // Check if it's a digit */\
        "jbe 3f\n\t"\
        "add $0x07, %%al\n\t"        /* // Convert to letter if necessary */\
        "3: mov %%al, _hex_buffer_+3\n\t"  /* // Store in buffer */\
        "mov %%bl, %%al\n\t"         /* // Move low byte of BX to AL */\
        "and %%cl, %%al\n\t"         /* // Mask lower nibble */\
        "add $0x30, %%al\n\t"        /* // Convert to ASCII */\
        "cmp $0x39, %%al\n\t"        /* // Check if it's a digit */\
        "jbe 4f\n\t"\
        "add $0x07, %%al\n\t"        /* // Convert to letter if necessary */\
        "4: mov %%al, _hex_buffer_+4\n\t"  /* // Store in buffer */\
        "movb $0, _hex_buffer_+5\n\t"      /* // Null-terminate the string */\
        "popal\n\t"\
        :\
        :\
        :\
    );\
    BIOS_PRINT_STRING__MSG(_hex_buffer_);\
}

#define BIOS_GET_ADDRESS_OF_STACK_VAR(stack_var) \
    /* // Get the address of the stack variable and assign it to _address_ */\
    asm volatile (\
        "pushal\n\t"\
        "lea %0, %%ax\n\t" /* // Load the address of tmp into AX */\
        "mov %%ax, _address_\n\t"\
        "popal\n\t"\
        :\
        : "m"(stack_var)\
        :\
        );

#define BIOS_PRINT_ADDRESS_AS_HEX(_) \
    asm_print_address_as_hex()

//void set_cursor_position(short row, short col) {
#define BIOS_SET_CURSOR_POS__ROW_COL(row, col) \
    do {\
        _asm_char_1_ = row;  _asm_char_2_ = col;\
        asm_set_cursor_pos__row_col();\
    } while (0)

#define BIOS_SET_PRINT_COLOR__COLOR(color) \
    do {\
        _asm_char_1_ = color;\
    } while(0)

#define BIOS_PRINT_STRING__MSG_COLOR(msg, color)\
    do {\
        _asm_char_1_ = color;\
        BIOS_PRINT_STRING__MSG(msg);\
    } while(0)

#define BIOS_PRINT_STRING__MSG(msg)\
    do {\
        _asm_msg_ = msg;\
        asm_print_string__msg_color();\
    } while(0)

#define DATA_BIOS_PARAM \
volatile char _asm_char_1_;\
volatile char _asm_char_2_;\
volatile const char* _asm_msg_

#define DATA_ADV_MSG \
const char ADV_MSG[] = "GitHub:PegasusPlus/tinyos"

#define END_BOOTSECTOR \
__asm__(".section .boot_signature\n"\
        ".byte 0x55, 0xAA\n");

#endif
