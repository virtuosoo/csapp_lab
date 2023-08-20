movq $0x5561d000, %r8
mov %r8, %r10
movq $0x6166373939623935, %r9
movq %r9, (%r8)
add $0x9, %r8
movb $0x00, (%r8)
mov %r10, %rdi
pushq $0x4018fa
ret
