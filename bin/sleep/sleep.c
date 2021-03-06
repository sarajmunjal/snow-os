#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[], char *envp[]) {
    if (argc < 2) {
        char *err_msg = "Sleep must have at least 1 argument\n";
        write(1, err_msg, strlen(err_msg));
        return -1;
    }
    int32_t vals[argc - 1];
    for (int i = 1; i < argc; i++) {
        uint32_t val = atoi(argv[i]);
        if (val == -1) {
            char *err_msg = "Arguments for sleep are not correct positive numbers\n";
            write(1, err_msg, strlen(err_msg));
            return -1;
        }
        vals[i - 1] = val;
    }
    for (int i = 0; i < argc - 1; i++) {
        sleep(vals[i]);
    }
    return 0;
}
