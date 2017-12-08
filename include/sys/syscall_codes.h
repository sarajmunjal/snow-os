#ifndef SYSCALL_CODES_H
#define SYSCALL_CODES_H

typedef enum syscall_code {
    SYSCALL_READ = 0x1,
    SYSCALL_WRITE = 0x2,
    SYSCALL_OPEN = 0x3,
    SYSCALL_CLOSE = 0x4,
    SYSCALL_UNLINK = 0x5,
    SYSCALL_CHDIR = 0x6,
    SYSCALL_GETCWD = 0x7,
    SYSCALL_FORK = 0x8,
    SYSCALL_EXECVPE = 0x9,
    SYSCALL_WAIT = 0xA,
    SYSCALL_YIELD = 0xB,
    SYSCALL_PID = 0xC,
    SYSCALL_PPID = 0xD,
    SYSCALL_MALLOC = 0xE,
    SYSCALL_FREE = 0xF,
    SYSCALL_OPENDIR = 0x10,
    SYSCALL_READDIR = 0x11,
    SYSCALL_CLOSEDIR = 0x12,
    SYSCALL_SLEEP = 0x13,
    SYSCALL_PS = 0x14,
    SYSCALL_WAITPID = 0x15,
    SYSCALL_EXIT = 0x16,
    SYSCALL_KILL = 0x17
} syscall_code_t;
#endif
