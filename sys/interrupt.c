#include <sys/defs.h>
#include <sys/io.h>
#include <sys/kprintf.h>
#include <sys/interrupt.h>
#include <sys/process.h>
#include <sys/physical_memory.h>
#include <sys/paging.h>

extern void timer_isr();

extern void keyboard_isr();

extern void syscall_isr();

extern void _x86_load_idt();

extern void page_fault_isr();

struct idtr_struct idtr_t;
struct idt_struct idt_t[256];
// void* register_int[256];

extern void page_fault_handler() {
    volatile uint64_t addr;
    __asm__ volatile("mov %%cr2, %0" : "=r" (addr));
    // kprintf("Page fault at %x\n", addr);

    addr = (addr / PAGE_SIZE) * PAGE_SIZE;

    uint64_t physical_addr_flags = get_flags(addr);
    if (((physical_addr_flags >> 9) & 1UL) == 1UL) { // COW
        uint64_t physical_addr = walk_page_table(addr);
        if (get_page_ref_count(physical_addr) == 2) {
            uint64_t v_page = (uint64_t) kmalloc(PAGE_SIZE);
            uint64_t p_page = walk_page_table(v_page);
            memcpy((void *) v_page, (void *) addr, PAGE_SIZE);
            update_page_tables(addr, p_page, PAGING_USER_R_W_FLAGS);
            set_page_ref_count(physical_addr, 1);
        } else if (get_page_ref_count(physical_addr) == 1) {
            update_page_tables(addr, physical_addr, PAGING_USER_R_W_FLAGS);
        }
        flush_tlb();
        return;
    }

    kthread_t *current_process = get_current_process();
    struct vma_struct *current_vma_stack = current_process->process_mm->vma_stack;

    // Auto-growing stack
    if (addr <= current_vma_stack->end && (current_vma_stack->start - current_vma_stack->end) <= LIMIT_STACK) {
        uint64_t pages = (current_vma_stack->end - addr) / PAGE_SIZE;
        while (pages--) {
            current_vma_stack->end -= PAGE_SIZE;
            update_page_tables(current_vma_stack->end, get_free_page(), PAGING_USER_R_W_FLAGS);
        }
    } else {
        kprintf("Segmentation fault in process %d!. Exiting.\n", current_process->pid);
        exit_current_process(0);
    }
}

void init_idt() {
    idtr_t.limit = sizeof(struct idt_struct) * 256 - 1;
    idtr_t.offset = (uint64_t) idt_t;

    memset(&idt_t, 0, sizeof(struct idt_struct) * 256);
    set_irq(IRQ0, (uint64_t) timer_isr, (uint16_t) 0x8, 0x8E);
    set_irq(IRQ1, (uint64_t) keyboard_isr, (uint16_t) 0x8, 0x8E);
    set_irq(SOFT_INTR, (uint64_t) syscall_isr, (uint16_t) 0x8, 0xEE);
    set_irq(PAGE_FAULT, (uint64_t) page_fault_isr, (uint16_t) 0x8, 0x8E);
    set_irq(DOUBLE_PAGE_FAULT, (uint64_t) page_fault_isr, (uint16_t) 0x8, 0x8E);
    _x86_load_idt((uint64_t) & idtr_t);
    // kprintf("Initialized IDT.\n");
}

void set_irq(uint8_t int_n, uint64_t addr, uint16_t selector, uint8_t type_attr) {
    idt_t[int_n].offset_1 = (uint16_t) addr & 0xFFFF;
    idt_t[int_n].selector = selector;
    idt_t[int_n].zero = 0;
    idt_t[int_n].type_attr = type_attr;
    idt_t[int_n].offset_2 = (uint16_t)((addr >> 16) & 0xFFFF);
    idt_t[int_n].offset_3 = (uint32_t)((addr >> 32) & 0xFFFFFFFF);
    idt_t[int_n].reserved = 0;
}

/*
void config_intr(uint8_t int_n, void* callback) {
    register_int[int_n] = callback;
}


void itr_handler(uint8_t int_n) {
    kprintf("Interrupt received\n");
    void* callback = register_int[int_n];
    callback();
}
*/