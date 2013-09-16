global SafeCallAgent

_SafeCallAgent:
push ebp
mov ebp,esp
mov eax,dword[ebp+8]
mov ecx,dword[eax+4]
lea edx,[eax+8]
arg_loop:
push dword[edx+ecx]
loop arg_loop
call [eax]
pop ebp
ret