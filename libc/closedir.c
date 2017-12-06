#include <dirent.h>
#include <unistd.h>

int closedir(DIR *dirp) {
    uint64_t stream_addr = dirp->stream;
    free(dirp);
    return syscall2(SYSCALL_CLOSEDIR, stream_addr);
}
