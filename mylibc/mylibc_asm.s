.intel_syntax noprefix

# int write(int fd, char *ptr, int count);
write:
# prologue
  push rbp
  mov rbp, rsp

# syscall number
  mov rax, 4

# setup argments
  mov rdi, rsi
  mov rsi, rdx
  mov rdx, rcx

  syscall

# epilogue
  mov rsp, rbp
  pop rbp
  ret
