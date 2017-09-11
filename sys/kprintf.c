#include <sys/kprintf.h>
#include <sys/stdarg.h>
#include <sys/string.h>

#define VIDEO_BASE_ADDRESS 0xb8000
#define VIDEO_MEM_ROWS 25
#define VIDEO_MEM_COLUMNS 80

long currentRow = 0;
long currentColumn = 0;
va_list args;
typedef enum format_type {
    STRING,
    INT,
    HEX,
    VOID
} format_type;

void printSpecial(int argNumber, format_type ft);

void printChar(char c);

void printString(char *c);

void printHex(long x);

void printLong(long x);

void printVoid(void *v);

char* getAddress(long row, long column) {
    return (char *) (VIDEO_BASE_ADDRESS + (2 * VIDEO_MEM_COLUMNS * row) + 2 * column);
}

void kprintf(const char *fmt, ...) {
    va_start(args, fmt);
    const char* c = fmt;
    int argNumber = 0;
    while (*c != '\0') {
        if (*c == '%') {
            //special character case
            switch (*(c + 1)) {
                case 's':
                    printSpecial(argNumber++, STRING);
                    break;
                case 'd':
                    printSpecial(argNumber++, INT);
                    break;
                case 'x':
                    printSpecial(argNumber++, HEX);
                    break;
                case 'p':
                    printSpecial(argNumber++, VOID);
                    break;
                default:
                    break;
            }
            c += 2;
        } else {
            printChar(*c);
            c++;
        }
    }
    va_end(args);
}

void printSpecial(int argNumber, format_type ft) {
    if (ft == STRING) {
        printString(va_arg(args, char*));
    }
    else if (ft == INT) {
        printLong(va_arg(args, int));
    }
    else if (ft == HEX) {
        printHex(va_arg(args, long));
    }
    else if (ft == VOID) {
        printVoid(va_arg(args, void*));
    }
}

void printChar(char c) {
    if (c == '\n') {
        if (currentRow == VIDEO_MEM_ROWS - 1) {
            currentRow = 0;
//            memcpy(getAddress(VIDEO_MEM_ROWS - 1, VIDEO_MEM_COLUMNS - 1), getAddress(0, 0),
//                   2 * VIDEO_MEM_ROWS * VIDEO_MEM_COLUMNS);
        } else {
            currentRow++;
        }
        currentColumn = 0;
        return;
    }
    char *address = getAddress(currentRow, currentColumn);
    *address = c;
    if (currentColumn == VIDEO_MEM_COLUMNS - 1) {
        if (currentRow == VIDEO_MEM_ROWS - 1) {
            currentRow = 0;
//            memcpy(getAddress(VIDEO_MEM_ROWS - 1, VIDEO_MEM_COLUMNS - 1), getAddress(0, 0),
//                   2 * VIDEO_MEM_ROWS * VIDEO_MEM_COLUMNS);
        } else {
            currentRow++;
        }
        currentColumn = 0;
    } else {
        currentColumn++;
    }
}


void printString(char *c) {
    while (*c != '\0') {
        printChar(*(c++));
    }
}

void printLong(long x) {
    char buf[66];
    int i = 65;
    buf[i--] = '\0';
    long numCopy = x;
    while(numCopy) {
        buf[i--] = 48 + numCopy % 10;
        numCopy /= 10;
    }
    if (x < 0) {
        buf[i] = '-';
    } else {
        i++;
    }
    printString(buf + i);
}

void printHex(long x) {
    char buf[19];
    int i = 18;
    buf[i--] = '\0';
    long numCopy = x;
    while(numCopy) {
        int remain = numCopy % 16;
        if (remain < 10) {
            buf[i--] = 48 + remain;
        }
        else {
            buf[i--] = 65 + (remain - 10);
        }
        numCopy = numCopy >> 4;
    }
    buf[i--] = 'x';
    buf[i] = '0';
    printString((char*)(buf) + i);
}


void printVoid(void* v) {
    printHex((long)v);
}
