#include <sys/defs.h>

struct idtr_struct {
    uint16_t limit;
    uint64_t offset;
} __attribute__((packed));

struct idt_struct {
    uint16_t offset_1;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_2;
    uint32_t offset_3;
    uint32_t reserved;
} __attribute__((packed));

void init_idt();
void set_irq(uint8_t int_n, uint64_t addr, uint16_t selector, uint8_t type_attr);

void enable_interrupts();
void disable_interrupts();
// void config_intr(uint8_t int_n, void* callback);
// void itr_handler(uint8_t int_n);
