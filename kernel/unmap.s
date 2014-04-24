.equ vpte0, 0x7ff4000
.equ vpte127, 0x7ff41fc

 EOR	 R0, R0
 LDR 	 R1, =vpte127
 STR	 R0, [R1]

 EOR	 R0, R0
 LDR 	 R1, =vpte0
 STR	 R0, [R1]