; int abi_call_atoi_base(char *str, const char *base, int *clobbered)
;
; Appelle ft_atoi_base en ayant mis une valeur sentinelle dans chaque registre
; callee-saved (System V AMD64 : rbx, rbp, r12-r15, rsp), puis verifie qu'ils
; sont intacts au retour. *clobbered recoit un masque des registres abimes :
;   bit 0 rbx, 1 rbp, 2 r12, 3 r13, 4 r14, 5 r15, 6 rsp
section .text
	global abi_call_atoi_base
	extern ft_atoi_base

%macro CHECK 2 ; registre, bit
	mov r8, SENT_%1
	cmp %1, r8
	je %%ok
	or ecx, 1 << %2
%%ok:
%endmacro

%define SENT_rbx 0x1111111111111111
%define SENT_rbp 0x2222222222222222
%define SENT_r12 0x3333333333333333
%define SENT_r13 0x4444444444444444
%define SENT_r14 0x5555555555555555
%define SENT_r15 0x6666666666666666

abi_call_atoi_base:
	push rbx
	push rbp
	push r12
	push r13
	push r14
	push r15
	push rdx                ; 7 push + adresse de retour : rsp aligne sur 16
	mov [rel saved_rsp], rsp
	mov rbx, SENT_rbx
	mov rbp, SENT_rbp
	mov r12, SENT_r12
	mov r13, SENT_r13
	mov r14, SENT_r14
	mov r15, SENT_r15
	call ft_atoi_base
	xor ecx, ecx
	CHECK rbx, 0
	CHECK rbp, 1
	CHECK r12, 2
	CHECK r13, 3
	CHECK r14, 4
	CHECK r15, 5
	cmp rsp, [rel saved_rsp]
	je .rsp_ok
	or ecx, 1 << 6
	mov rsp, [rel saved_rsp]
.rsp_ok:
	pop rdx
	mov [rdx], ecx
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	pop rbx
	ret

section .bss
saved_rsp: resq 1

section .note.GNU-stack noalloc noexec nowrite progbits
