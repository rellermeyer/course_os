ENTRY(_Reset)

SECTIONS
{

/*
 *. = 0x8000;
 *.startup . : { startup.o(.text) }
 *.text : { *(.text) }
 *.data : { *(.data) }
 *.bss : { *(.bss COMMON) }
 *. = ALIGN(8);
*/
	. = 0x10000;

 	.startup . : { startup.o(.text) }

	.text BLOCK(4k) : {
		_kstart = ABSOLUTE(.);
		*(.text)
		_kend = ABSOLUTE(.);
	}

	.data BLOCK(4k) : {
		_datastart = ABSOLUTE(.);
		*(.data)
		_dataend = ABSOLUTE(.);
	}

	.bss BLOCK(4k) : {
		_bssstart = ABSOLUTE(.);
		*(.bss COMMON)
		_bssend = ABSOLUTE(.);
	} 
	
	system_stack = 0x3c00;
	irq_stack = 0x2c00;
	abort_stack = 0x2800;
	svc_stack = 0x2400;

}
