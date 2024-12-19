#ifndef _BOOTSECTOR_CHAP02_H_
#define _BOOTSECTOR_CHAR02_H_

#include "../../ch01_bootsector/c/bootsector.h"

#define ASM_EPILOG_TIMER_INTERUPT \
__asm__(\
    ".code16\n\t"\
    ".global _start\n\t"\
"_start:\n\t"\
    "xorw %ax, %ax\n\t"\
    "movw %ax, %ds\n\t"\
    "movw %ax, %es\n\t"\
    "movw %ax, %ss\n\t"\
    "movw $0x7C00, %sp\n\t"\
\
    "call asm_clear_screen\n\t"\
    "movb $12, _asm_char_1_\n\t"\
    "movb $27, _asm_char_2_\n\t"\
    "call asm_set_cursor_pos__row_col\n\t"\
    "movb $10, _asm_char_1_\n\t"\
    "movw $ADV_MSG, _asm_msg_\n\t"\
    "call asm_print_string__msg_color\n\t"\
\
    "cli\n\t"\
\
        /* ; Remap PIC */\
        "movb $0x11, %al\n\t"           /* ; Initialize command */\
        "outb %al, $0x20\n\t"           /* ; Send to PIC1 */\
        "movb $0x20, %al\n\t"           /* ; New offset for IRQ0 (32) */\
        "outb %al, $0x21\n\t"\
        "movb $0x04, %al\n\t"           /* ; Tell PIC1 there is a slave at IRQ2 */\
        "outb %al, $0x21\n\t"\
        "movb $0x01, %al\n\t"            /* ; 8086 mode */\
        "outb %al, $0x21\n\t"\
\
        "movb $0x36, %al\n\t"       /* // Channel 0, square wave mode */\
        "outb %al, $0x43\n\t"       /* // Send control word to PIT */\
        "movw $0x04AF, %ax\n\t"     /* // Load value for 1000 Hz (1193 in decimal) */\
        "outb %al, $0x40\n\t"       /* // Send low byte to PIT channel 0 */\
        "movb %ah, %al\n\t"\
        "outb %al, $0x40\n\t"       /* // Send high byte to PIT channel 0 */\
\
        "xorw %ax, %ax\n\t"          /* // Clear AX (set to zero) */\
        "movw %ax, %es\n\t"          /* // Set ES to zero (flat segment) */\
        "movw %ax, %es:0x20*4+2\n\t" /* // Set ISR segment */\
        /* "leaw .inline_isr_, %dx\n\t" */       /* // Load the address of _isr_ into DX */\
        "movw $.inline_isr_, %ax\n\t"\
        "movw %ax, %es:0x20*4\n\t"  /* // Set ISR offset for IRQ0 */\
        "movw %es:0x20*4, %ax\n\t"\
        /* "movw %ax, _address_\n\t"\ */\
        "movb $0xFE, %al\n\t"        /* // Enable IRQ0 only */\
        "outb %al, $0x21\n\t"        /* // Send command to PIC */\
\
    "sti\n\t"\
".halt:\n\t"\
    "hlt\n\t"\
    "jmp .halt\n\t"\
\
".inline_isr_:\n\t"\
        "pushal\n\t"            /* //PUSH_REGISTERS */\
        "movw %cs, %ax\n\t"      /* // Setup data segment */\
        "movw %ax, %ds\n\t"\
        "movw %ax, %es\n\t"\
\
        "call timer_handler\n\t"\
\
        "movb $0x20, %al\n\t"   /* // ACK ISR */\
        "outb %al, $0x20\n\t"\
\
        "popal\n\t"     /* //POP_REGISTERS */\
"iret\n\t"\
)

#define BEGIN_TIMER_HANDLER \
void timer_hadler() {\
    __asm__ (\
        ".global timer_handler\n\t"\
        "timer_handler:\n\t"\
    )

#define END_TIMER_HANDLER \
    __asm__ (\
        "ret\n\t"\
    );\
}

#endif
