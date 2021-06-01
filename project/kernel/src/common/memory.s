.text
.global detect_memory
// Detects the size of physical memory. Returns it's answer in bytes in r1
detect_memory:
    push {r0, r2, r3, r4, r5, lr}

    // Load 0 in R0. R0 is used as a counter which we increase with
    // a mebibyte at a time to search the address space.
    // We skip addresses in the `mmio regions` array.
    ldr r0, =0x00000000

    // r3 represents the increment that's taken every iteration.
    // It will go down as we reach the end until it's 0 and we found the absolute end.
    ldr r3, =0x10000000

detect_memory_loop:
    // Add the increment
    add r0, r0, r3

    // Is this address in one of the forbidden regions?
    ldr r4, =mmio_regions_start
detect_memory_forbidden_loop:
    // Load the start value
    ldr r5, [r4]
    // Add 4 to get to the end address of the region
    add r4, r4, #4

    // If the current address is greater than this it might be in the region
    cmp r0, r5
    bge detect_memory_forbidden_loop_maybe_forbidden
detect_memory_forbidden_loop_not_forbidden:
    // Else just skip the end
    add r4, r4, #4

    // Check if we're done first
    ldr r5, =mmio_regions_end
    cmp r4, r5
    bge detect_memory_forbidden_loop_done
    // if not, continue with the next entry in the forbidden regions array
    b detect_memory_forbidden_loop

detect_memory_forbidden_loop_maybe_forbidden:
    ldr r5, [r4]

    // If the current address is greater than the end address, it's not forbidden and we can continue.
    cmp r0, r5
    bge detect_memory_forbidden_loop_not_forbidden

    // The region is forbidden.
    // set the current address to the end address which is guaranteed not to be forbidden anymore
    mov r0, r5
    /* FallThrough */

detect_memory_forbidden_loop_done:

    // Write the value 42 at the current address
    mov r1, #42
    str r1, [r0]
    // Read it again
    ldr r2, [r0]
    // Is it still 42? then continue.
    // Else decrease the step size and retry
    cmp r2, r1
    beq detect_memory_continue_loop

    // First subtract the previous stepsize
    sub r0, r0, r3
    // Divide the increment by 2
    asr r3, r3, #1

    // Is the increment zero? then stop. We found the end of ram
    cmp r3, #0
    beq detect_memory_loop_done

    // retry
    b detect_memory_loop

detect_memory_continue_loop:
    ldr r1, =max_memory_size
    ldr r1, [r1]
    cmp r0, r1

    blt detect_memory_loop
detect_memory_loop_done:

    // Put the result in r1
    mov r1, r0

    pop {r0, r2, r3, r4, r5, pc}


.data
max_memory_size: .long 0x40000000
// 2-tuples representing forbidden memory regions.
// First long is the start address (inclusive)
// Second long is the end address (exclusive)
mmio_regions_start:
    // On qemu the second-to-last megabyte seems to be unusable
    // TODO: why?
    .long 0x3fe00000
    .long 0x3ff00000

    // BCM2836 regions

mmio_regions_end:
