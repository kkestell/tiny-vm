; fill up 64k of memory

; counter
li $1 0x00000000

; end
li $2 0x0000FFFF

; memory location of loop start
li $3 loop

loop:
  ; store the value of the counter in the memory location contained in the counter.
  sw $1 $1

  ; increment the counter
  inc $1

  ; loop if the counter hasn't yet reached the end
  bne $1 $2 $3

  ; end program
  halt
