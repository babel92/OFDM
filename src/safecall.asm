SECTION .data

SECTION .text
global _SafeThisCallAgent
extern _printf

_SafeThisCallAgent:
push ebp
mov ebp,esp
mov eax,dword[ebp+8]
mov ecx,dword[eax+4]
lea edx,[eax+8]
dec ecx
loop1:
push dword[edx+ecx*4]
loop loop1
mov ecx,dword [eax+8]
call [eax]
pop ebp
ret

_SafeStdCallAgent:
push ebp
mov ebp,esp
mov eax,dword[ebp+8]
mov ecx,dword[eax+4]
lea edx,[eax+8]
loop2:
lea ebx,[ecx-1]
push dword[edx+ebx*4]
loop loop2
call [eax]
pop ebp
ret
