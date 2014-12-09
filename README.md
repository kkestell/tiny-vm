# TinyVM

TinyVM is a simple register based virtual machine implemented in C (`tinyvm.c`). The bytecode assembler is written in Python (`tas.py`). TinyVM has 4 registers (`$0` - `$3`) and 64k of memory in a 32-bit address space (`0x00000000` - `0x0000FFFF`).

Each instruction is encoded in a single 64-bit word. Register count and memory are defined at compile time, but due to only having 32 bits available for addressing and 8 bits for registers, allocating more than 4GB of memory or 256 registers is pointless.

The following instructions (loosely based on MIPS) have been implemented:

    No.    | Keyword | Instruction                   | Example            | Description
    -------|---------|-------------------------------|--------------------|------------
    0x00   | `halt`  | Halt                          | `halt`             | Terminate program
    0x01   | `nop`   | No Operation                  | `nop`              | Do nothing
    0x02   | `li`    | Load Immediate                | `li $0 0x00000000` | Load `0x00000000` into `$0`
    0x03   | `lw`    | Load Word                     | `lw $0 $1`         | Load the contents of the memory location pointed to by `$1` into `$0`
    0x04   | `sw`    | Store Word                    | `sw $0 $1`         | Store the contents of `$1` in the memory location pointed to by `$0`
    0x05   | `add`   | Add                           | `add $2 $0 $1`     | Add `$0` to `$1` and store the result in `$2`
    0x06   | `sub`   | Subtract                      | `sub $2 $0 $1`     | Subtract `$1` from `$0` and store the result in `$2`
    0x07   | `mult`  | Multiply                      | `mult $2 $0 $1`    | Multiply `$0` by `$1` and store the result in `$2`
    0x08   | `div`   | Divide                        | `div $2 $0 $1`     | Divide `$0` by `$1` and store the result in `$2`
    0x09   | `j`     | Unconditional Jump            | `j 0x00000000`     | Jump to memory location `0x00000000`
    0x0A   | `jr`    | Unconditional Jump (Register) | `jr $0`            | Jump to memory location stored in `$0`
    0x0B   | `beq`   | Branch if Equal               | `bne $0 $1 $2`     | Branch to memory location stored in `$2` if `$0` and `$1` are equal
    0x0C   | `bne`   | Branch if Not Equal           | `beq $0 $1 $2`     | Branch to memory location stored in `$2` if `$0` and `$1` are not equal
    0x0D   | `inc`   | Increment Register            | `inc $0`           | Increment `$0`
    0x0E   | `dec`   | Decrement Register            | `dec $0`           | Decrement `$0`

## Bytecode Format

Each instruction is a single 64-bit word composed of eight 8-bit octets (little endian).

The first octet contains the instruction number, while the second, third, and fourth octets contain register numbers. The lower 32 bits are either an unsigned memory location or a signed immediate value.

    Instruction | Register 0 | Register 1 | Register 2 | Immediate Value
    ------------|------------|------------|------------|----------------
    0000 0000   | 0000 0000  | 0000 0000  | 0000 0000  | 0000 0000 0000 0000 0000 0000 0000 0000

    Assembly            | Hex                  | Binary
    --------------------|----------------------|-------
    `li  $2 0x7fffffff` | `0x102000007fffffff` | 0001 0000 0010 0000 0000 0000 0000 0000 0111 1111 1111 1111 1111 1111 1111 1111
    `add $2 $0 $1`      | `0x4020001000000000` | 0100 0000 0010 0000 0000 0000 0001 0000 0000 0000 0000 0000 0000 0000 0000 0000

## Assembly Language

### Comments

Lines which begin with a semicolon are treated as comments.

### Labels

Lines which end with a colon are treated as labels. The assembler works in two passes so there is no need to forward declare your labels.

### TODO

TODO

## Example Assembly

```assembly
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
```

## Example

Assuming the assembly source above...

### Assemble, generate bytecode

    $ python tas.py test.asm test.tvm
    0201000000000000 li $1 0x00000000
    020200000000ffff li $2 0x0000FFFF
    0203000000000003 li $3 loop
    0401010000000000 sw $1 $1
    0d01000000000000 inc $1
    0c01020300000000 bne $1 $2 $3
    0000000000000000 halt

### Inspect bytecode

    $ od -x test.tvm
    0000000          00000000        02010000        0000ffff        02020000
    0000020          00000003        02030000        00000000        04010100
    0000040          00000000        0d010000        00000000        0c010203
    0000060          00000000        00000000
    0000070

### Compile TinyVM

    $ make

### Execute bytecode

Each cycle, TinyVM prints the value of the program counter, the next instruction, executes that instruction, and then prints the values of the registers, with each line representing a single CPU cycle.

    $ ./tinyvm test.tvm
    00000001 02000000ffffffff ffffffff 00000000 00000000 00000000
    00000002 0201000012345678 ffffffff 12345678 00000000 00000000
    00000003 0202000000012ac0 ffffffff 12345678 00012ac0 00000000
    00000004 0503010200000000 ffffffff 12345678 00012ac0 12358138
    00000005 0600010200000000 12332bb8 12345678 00012ac0 12358138
    00000006 0000000000000000 12332bb8 12345678 00012ac0 12358138

