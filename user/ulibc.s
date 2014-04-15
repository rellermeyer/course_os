.global exit
.global yield
.global printf

; Not sure, but we might not need to ever return from here
; since a different process will now run
exit:
    SVC 0

; Need to save registers on the process stack
; since we can't rely on kernel to do it
yield:
    PUSH {r0-r12,lr}
    LDR r0, =resume ; pass resume addr to handler
    LDR r1, sp      ; pass sp to handler
                    ; these will be saved in PCB
    SVC 1

printf:
    PUSH {r0-r12,lr} ; r7 will be overwritten by SVC call,
                     ; so we might as well save here...
    SVC 2
    B resume

resume:
    POP {r0-r12,pc} ; jump through lr to caller

