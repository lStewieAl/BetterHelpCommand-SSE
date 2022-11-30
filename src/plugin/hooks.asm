filterTypeID PROTO C
SkipAddr  PROTO C
ContinueAddr  PROTO C
.code

HookCheckTypeID PROC
	jz skip

	mov rax, [filterTypeID]
	mov rax, [rax]
	test rax, rax
	je noFilter

	cmp byte ptr [rbx + 1Ah], al
	jnz skip

noFilter:
	mov rax, ContinueAddr
	jmp qword ptr [rax]

	skip:
	mov rax, SkipAddr
	jmp qword ptr [rax]
HookCheckTypeID ENDP
END
