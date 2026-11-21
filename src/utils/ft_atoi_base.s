section .text
	global ft_atoi_base
	extern strlen

	ft_atoi_base:
		; validate_base
		mov r10, 0
		movzx edx, byte [rsi + r10]
		.base_loop:
			; Check for forbidden chars
			cmp dl, 43 ; '+'
			je .error
			cmp dl, 45 ; '-'
			je .error
			cmp dl, 32 ; ' '
			je .error
			cmp dl, 9 ; '\t'
			je .error
			cmp dl, 10 ; '\n'
			je .error
			cmp dl, 11 ; '\v'
			je .error
			cmp dl, 12 ; '\f'
			je .error
			cmp dl, 13 ; '\r'
			je .error

			; check for ducplicate chars
			mov r11, r10
			.diff_loop:
				inc r11
				movzx ecx, byte [rsi + r11]
				cmp dl, cl
				je .error

				cmp cl, 0
				jne .diff_loop

			inc r10
			movzx edx, byte [rsi + r10]
			cmp dl, 0
			jne .base_loop

		; Check if base size is greater than 1
		cmp r10, 1
		jle .error

		; atoi
		push rdi
		push rsi
		sub rsp, 8 ; realign stack on 16 bytes
		mov rdi, rsi
		call strlen
		add rsp, 8
		pop rsi
		pop rdi
		mov r8, rax
		mov rax, 0

		mov r9, 0
		mov r10, 0
		movzx edx, byte [rdi + r10]
		cmp dl, 0
		je .ret

		cmp dl, 43
		je .plus_sign

		cmp dl, 45
		je .minus_sign

		jmp .atoi_loop

		.plus_sign:
			inc r10
			movzx edx, byte [rdi + r10]
			jmp .atoi_loop

		
		.minus_sign:
			inc r10
			movzx edx, byte [rdi + r10]
			mov r9, 1
			jmp .atoi_loop

		.atoi_loop:
			mov r11, 0
			.find_loop:
				movzx ecx, byte [rsi + r11]

				cmp cl, 0
				je .end

				cmp dl, cl
				je .atoi_add

				inc r11
				jmp .find_loop
				
			.atoi_add:
				imul rax, r8
				add rax, r11
			
			inc r10
			movzx edx, byte [rdi + r10]
			cmp dl, 0
			jne .atoi_loop

		.end:
			cmp r9, 1
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

		
