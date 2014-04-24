.global exit
.global yield
.global printf

exit:
    SVC 0
    MOV pc, lr 

; Need to save registers on the process stack
; since we can't rely on kernel to do it
yield:
    PUSH {r0-r12,lr}
    LDR r0, =resume ; pass resume addr to handler
    LDR r1, sp      ; pass sp to handler
                    ; these will be saved in PCB
    SVC 1
resume:
    POP {r0-r12,pc} ; jump through lr to caller

; Working state will be saved/restored by kernel
printf:
    SVC 2
    MOV pc, lr
