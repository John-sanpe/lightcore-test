#include <fs.h>
#include <system/syscall.h>

state kernel_execve(const char *file, const char *const *argv, const char *const *envp)
{
    
    return -ENOERR;
}


void execve()
{
    
    
    
}
SYSCALL_ENTRY(syscall_execve, execve, 5);