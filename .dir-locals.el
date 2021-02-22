((nil . ((eval . (let ((root (projectile-project-root)))
		   (let ((command (concat "arm-none-eabi-gdb -i=mi -ex \"target remote localhost:1234\" -ex \"symbol-file " root "src/kernel/build/kernel.sym\"")))
                     (setq-local gud-gud-gdb-command-name
				 command
				 )
		     (setq-local gud-gdb-command-name
				 command
				 )
		     )
		   )
	       )
	 )
      )
 )
