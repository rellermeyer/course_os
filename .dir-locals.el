((nil . ((eval . (let ((root (projectile-project-root)))
  (let ((command (concat "arm-none-eabi-gdb -i=mi -ex \"target remote localhost:1234\" -ex \"symbol-file " root "src/kernel/build/kernel.sym\"")))
    (setq-local gud-gud-gdb-command-name command)
    (setq-local gud-gdb-command-name command)
    (set (make-local-variable 'compile-command)
         (concat "cd " (concat root "src") " && make test"))
    (let ((map (make-sparse-keymap))) 
      
      (set-keymap-parent map (current-local-map))
      (use-local-map map)
      (local-set-key [f5] 'compile)
      (local-set-key [f6] 'co/gdb)

      (defun co/gdb ()
        (interactive)
        (async-shell-command (concat "cd "
                                     (concat (projectile-project-root) "src")
                                     " && "
                                     "make debug") nil 0)
        (gdb gud-gdb-command-name)))))))))
