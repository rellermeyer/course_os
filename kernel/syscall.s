.global syscall

; int syscall(int number)
; assume number passed in r0 for now
syscall:
    PUSH {r4-r11} ; save work registers, others should be banked
    SVC [r0]
    POP {r4-r11} ; restore work registers
    MOV pc, lr ; resume execution
    
