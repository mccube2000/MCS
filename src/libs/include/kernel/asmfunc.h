#ifndef _ASMFUNC_H_
#define _ASMFUNC_H_

#include "resource/executor.h"
#include "types.h"

#define hlt() __asm__("hlt" ::)
#define cli() __asm__("cli" ::)
#define sti() __asm__("sti" ::)
#define nop() __asm__("nop" ::)
#define pause() __asm__("pause" ::)

#define iret() __asm__("iret" ::)

extern void load_gdtr(int32_t limit, int32_t base);
extern void load_idtr(int32_t limit, int32_t base);

extern uint8_t io_in8(uint32_t port);
extern uint16_t io_in16(uint32_t port);
extern uint32_t io_in32(uint32_t port);

extern void io_out(uint32_t port, uint32_t data);

extern void asm_inthandler0c(void);
extern void asm_inthandler0d(void);
extern void asm_page_fault(void);
extern void asm_int_pit(void);
extern void asm_int_keyboard(void);
extern void asm_inthandler27(void);
extern void asm_int_mouse(void);

extern int32_t io_load_eflags(void);
extern void io_store_eflags(int32_t eflags);
extern uint32_t load_cr0(void);
extern void *load_cr2(void);
extern void store_cr0(uint32_t cr0);
extern void load_tr(int32_t tr);

extern exe_reg32_s *save_context(exe_reg32_s *);
extern void load_context(exe_reg32_s);

// extern void farjmp(int32_t eip, int32_t cs);
// extern void farcall(int32_t eip, int32_t cs);
// extern void asm_hrb_ap(void);
// extern void start_app(int32_t eip, int32_t cs, int32_t esp, int32_t ds, int32_t *tss_esp0);
// extern void asm_end_app(int32_t eip);

#endif
