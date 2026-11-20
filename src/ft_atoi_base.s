section .text
	global ft_atoi_base
	extern strlen

	ft_atoi_base:
		; validate_base
		mov r10, 0
		movzx ebx, byte [rsi + r10]
		.base_loop:
			; Check for forbidden chars
			cmp bl, 43 ; '+'
			je .error
			cmp bl, 45 ; '-'
			je .error
			cmp bl, 32 ; ' '
			je .error
			cmp bl, 9 ; '\t'
			je .error
			cmp bl, 10 ; '\n'
			je .error
			cmp bl, 11 ; '\v'
			je .error
			cmp bl, 12 ; '\f'
			je .error
			cmp bl, 13 ; '\r'
			je .error

			; check for ducplicate chars
			mov r11, r10
			.diff_loop:
				inc r11
				movzx ecx, byte [rsi + r11]
				cmp bl, cl
				je .error

				cmp cl, 0
				jne .diff_loop

			inc r10
			movzx ebx, byte [rsi + r10]
			cmp bl, 0
			jne .base_loop

		; Check if base size is greater than 1
		cmp r10, 1
		jle .error

		; atoi
		push rdi
		mov rdi, rsi
		call strlen
		mov r12, rax
		mov rax, 0
		pop rdi

		mov r13, 0
		mov r10, 0
		movzx ebx, byte [rdi + r10]
		cmp bl, 0
		je .ret

		cmp bl, 43
		je .plus_sign

		cmp bl, 45
		je .minus_sign

		jmp .atoi_loop

		.plus_sign:
			inc r10
			movzx ebx, byte [rdi + r10]
			jmp .atoi_loop

		
		.minus_sign:
			inc r10
			movzx ebx, byte [rdi + r10]
			mov r13, 1
			jmp .atoi_loop

		.atoi_loop:
			mov r11, 0
			.find_loop:
				movzx ecx, byte [rsi + r11]

				cmp bl, cl
				je .atoi_add

				inc r11
				cmp cl, 0
				jne .find_loop
				je .end
				
			.atoi_add:
				imul rax, r12
				add rax, r11
			
			inc r10
			movzx ebx, byte [rdi + r10]
			cmp bl, 0
			jne .atoi_loop

		.end:
			cmp r13, 1
			je .neg

			jmp .ret

			.neg:
				neg rax
				jo .error
				jmp .ret


		.error:
			mov rax, 0

		.ret:
			ret

		
