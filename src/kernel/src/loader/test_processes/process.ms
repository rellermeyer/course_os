.text

.global main

main:
    push {fp}
    add fp, sp, #0
    swi 123
    swi 100
