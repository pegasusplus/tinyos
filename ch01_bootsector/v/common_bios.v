module common_bios

pub fn asm_bios_clear_screen() {}
pub fn asm_bios_set_cursor_pos(row u8, col u8) {}
pub fn asm_bios_set_print_color(c u8) {}
pub fn asm_bios_print_char(c u8) {}