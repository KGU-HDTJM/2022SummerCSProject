.code

TASBoolean PROC

	mov eax,1
	xchg DWORD PTR [rcx], eax ; atomic ÇÑ ¸í·É
	and eax,eax
	ret
TASBoolean ENDP
end
