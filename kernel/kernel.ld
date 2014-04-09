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
	. = 0x00100000;

	_kernstart = .;

	.startup . : {
		startup.o(.text)
	}

	.text BLOCK(8) : {
		_ktextstart = ABSOLUTE(.);
		*(.text)
		_ktextend = ABSOLUTE(.);
	}

	.data BLOCK(8) : {
		_kdatastart = ABSOLUTE(.);
		*(.data)
		_kdataend = ABSOLUTE(.);
	}

	.bss BLOCK(8) : {
		_kbssstart = ABSOLUTE(.);
		*(.bss COMMON)
		_kbssend = ABSOLUTE(.);
	} 
	
	_kernend = .;

	. = ALIGN(4k);

	_transtable = .;

	. = . + 16K;

	_lowused = .;

}
