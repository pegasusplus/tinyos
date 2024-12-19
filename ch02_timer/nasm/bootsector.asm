[BITS 16]
[ORG 0x7C00]

%include "../../ch01_bootsector/nasm/common.asm"
%include "common.asm"

start:
    INIT_SEGMENTS
    CLEAR_SCREEN

    PRINT_STRING_COLOR [hello_msg_row], [hello_msg_col], [hello_msg_color], hello_msg
    PRINT_STRING_COLOR [turn_on_msg_row], [turn_on_msg_col], [turn_on_msg_color], turn_on_msg

    cli                     ; Disable interrupts

    ;; Set up the stack
    ;xor ax, ax
    ;mov ss, ax
    ;mov sp, 0x7C00

    ; Remap PIC
    mov al, 0x11           ; Initialize command
    out 0x20, al           ; Send to PIC1
    mov al, 0x20           ; New offset for IRQ0 (32)
    out 0x21, al
    mov al, 0x04           ; Tell PIC1 there is a slave at IRQ2
    out 0x21, al
    mov al, 0x01           ; 8086 mode
    out 0x21, al

    SETUP_TIMER_INTERRUPT _isr_timer
    ENABLE_TIMER_INTERRUPT_ONLY

    ; Enable interrupts
    sti

    ; Infinite loop
loop:
    hlt                    ; Halt until interrupt
    jmp loop

; Timer Interrupt Service Routine (ISR)
BEGIN_ISR_TIMER _isr_timer
    ; Set up data segment
    mov ax, cs
    mov ds, ax
    mov es, ax

    ; Display time
    CALL_INC_MICRO_SEC_AND_TO_ASCII
    CALL_QUERY_AND_PRINT_TIME
    ;CALL_PRINT_ADV_SCROLL

.done:
    ACK_ISR
END_ISR_TIMER


FN_BCD_TO_ASCII
FN_PRINT_STRING
;FN_PRINT_ADV_SCROLL
FN_INC_MICRO_SEC_AND_TO_ASCII
FN_QUERY_AND_PRINT_TIME

; Data

hello_msg db 'Hello, bootsector by ASM in timer driving world!', 0
hello_msg_row db 0
hello_msg_col db 0
hello_msg_color db 0x0E

DATA_TIME_STR_HR
DATA_SAFE_POWER_OFF
;DATA_ADV

times 510-($-$$) db 0
dw 0xAA55
