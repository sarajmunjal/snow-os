#include <sys/defs.h>
#include <sys/io.h>
#include <sys/interrupt.h>
#include <sys/kprintf.h>
#include <sys/pit.h>
#include <sys/sleep.h>

uint64_t time_since_boot = 0;

extern void pit_intr_handler() {
    time_since_boot++;
    printTime(time_since_boot);
    if (time_since_boot % 100 == 0) {
        on_second_tick();
    }
    output_b(PIC_M_CR, PIC_EOI);
}

void init_pit() {
    // config_intr(IRQ0, &pit_intr_handler);

    uint8_t command_register = 0x36; // 0011 0110
    output_b(PIT_CR, command_register);
    uint16_t f = 1193182 / 100;
    uint8_t p1 = f & 0x00FF;
    output_b(PIT_0, p1);
    uint8_t p2 = (f >> 8) & 0x00FF;
    output_b(PIT_0, p2);
    // kprintf("Initialized PIT.\n");
}
