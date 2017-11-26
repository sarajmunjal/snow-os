#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/physical_memory.h>
#include <sys/paging.h>

#define PAGE_SIZE 4096
#define PHYS_VIDEO_MEM 0xB8000
#define ENTRIES 512

uint64_t *pml4_t, kernel_virtual_base;

void update_page_tables(uint64_t virt_addr, uint64_t physical_addr, uint16_t flags) {
    uint64_t *pdpe_t = NULL, *pde_t = NULL, *pte_t = NULL;
    uint64_t pdpe, pde, pte;
    uint32_t
    offset_pte = 0x1FF & (virt_addr >> 12),
    offset_pde = 0x1FF & (virt_addr >> 21),
    offset_pdpe = 0x1FF & (virt_addr >> 30),
    offset_pml4 = 0x1FF & (virt_addr >> 39);

    if ((pml4_t[offset_pml4] & 1UL) != 1UL) {
        pdpe = get_free_page();
    } else {
        pdpe = pml4_t[offset_pml4] ^ flags;
    }
    pml4_t[offset_pml4] = pdpe | flags; // page is present and writable
    pdpe_t = (uint64_t * )(kernel_virtual_base + pdpe);

    if ((pdpe_t[offset_pdpe] & 1UL) != 1UL) {
        pde = get_free_page();
    } else {
        pde = pdpe_t[offset_pdpe] ^ flags;
    }
    pdpe_t[offset_pdpe] = pde | flags;
    pde_t = (uint64_t * )(kernel_virtual_base + pde);

    if ((pde_t[offset_pde] & 1UL) != 1UL) {
        pte = get_free_page();
    } else {
        pte = pde_t[offset_pde] ^ flags;
    }
    pde_t[offset_pde] = pte | flags;
    pte_t = (uint64_t * )(kernel_virtual_base + pte);
    pte_t[offset_pte] = physical_addr | flags;
}

uint64_t setup_user_page_tables() {
    uint64_t *user_pml4 = (uint64_t *) kmalloc(4096);
    uint64_t *current_pml4 = (uint64_t *)(get_cr3() + USER_VADDR);
    user_pml4[511] = current_pml4[511]; // cannot access current_pml4 data?
    return ((uint64_t) user_pml4 - USER_VADDR);
}

uint64_t get_cr3() {
    uint64_t cr3;
    __asm__ __volatile__("movq %%cr3, %0;":"=r"(cr3));
    return cr3;
}

void set_new_cr3(uint64_t cr3_addr) {
    __asm__ __volatile__ ("movq %0, %%cr3;"::"r"(cr3_addr));
}

void init_paging(uint64_t kernmem, uint64_t physbase, uint64_t physfree) {
    uint64_t cr3_addr = get_free_page(), v_i = kernmem, p_i = physbase;
    kernel_virtual_base = kernmem - physbase;
    pml4_t = (uint64_t * )(kernel_virtual_base + cr3_addr);
    // map all physical memory to virtual memory (identity mapping)
    // better choice for 64 bit systems (used by Linux)
    while (p_i < (physfree + get_free_pages_count() * PAGE_SIZE)) {
        update_page_tables(v_i, p_i, PAGING_KERNEL_R_W_FLAGS);
        v_i += PAGE_SIZE;
        p_i += PAGE_SIZE;
    }
    update_page_tables(kernel_virtual_base + PHYS_VIDEO_MEM, PHYS_VIDEO_MEM, PAGING_KERNEL_R_W_FLAGS);
    set_new_cr3(cr3_addr);
    /* this is causing bootloop, can be debugged later, not important at this stage
    // map physical memory from 0x0 to physbase
    p_i = (uint64_t) 0x0;
    v_i = kernel_virtual_base + physfree + (get_free_pages_count() + 1) * PAGE_SIZE;
    while (p_i < physbase) {
        setup_page_tables(v_i, p_i);
        v_i += PAGE_SIZE;
        p_i += PAGE_SIZE;
    }
    // add physical memory earlier used for bootloader page tables to free list
    add_back_free_pages((uint64_t) 0x0, physbase / PAGE_SIZE);
    update_max_pages(physbase / PAGE_SIZE);
    */
}

void *kmalloc(uint32_t bytes_required) {
    uint32_t no_of_pages;

    if (bytes_required % PAGE_SIZE) {
        no_of_pages = (bytes_required / PAGE_SIZE) + 1;
    } else {
        no_of_pages = (bytes_required / PAGE_SIZE);
    }
    uint64_t allocated_addr = 0x0;
    for (uint32_t i = 0; i < no_of_pages; i++) {
        uint64_t ph_addr = get_free_page();
        if (i == 0) {
            allocated_addr = ph_addr;
        }
        // give contiguous virtual space
        // this might lead to overwriting virtual address used by other process
        // check page bits to avoid that later on
        update_page_tables(kernel_virtual_base + allocated_addr + i * PAGE_SIZE, ph_addr, PAGING_KERNEL_R_W_FLAGS);
    }
    return (void *) (kernel_virtual_base + allocated_addr);
}