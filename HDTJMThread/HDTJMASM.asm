.code

TASBoolean PROC

	mov eax,1
	xchg DWORD PTR [rcx], eax ; atomic�� ���
	and eax,eax
	ret
TASBoolean ENDP
end
