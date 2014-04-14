.global exit
.global yield
.global printf

syscall:
    PUSH {r4-r11} ; save work registers
    SVC [r0]      ; assume syscall number in r0
    POP {r4-r11}  ; restore work registers
    MOV pc, lr 

exit:
    MOV r1, r0
    LDR r0, =0x0 ; syscall(0)
    B syscall

yield:
    MOV r1, sp    ; pass old sp to handler
    PUSH {r4-r11} ; save registers
    MOV r2, sp    ; pass new sp to handler
    MOV r3, lr    ; pass RA to handler
    LDR r0, =0x1 ; syscall(1)
    B syscall

printf:
    PUSH {r2, r3} ; in case we have too many varargs
    MOV r3, r1    ; shift everything down to make room in r0, r1
    MOV r2, r0    ; handler will need to undo this shift
                  ; before calling into os_printf
    
    MOV r1, sp    ; pass stack pointer to handler
    LDR r0, =0x2  ; syscall(2)
    B syscall
