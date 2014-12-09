from struct import *
import sys

if len(sys.argv) < 3:
  print 'USAGE: as.py INFILE OUTFILE'
  sys.exit(1)

lines = [line.strip() for line in open(sys.argv[1]) if line.strip() != '' and line.strip()[0] != ';']
executable_lines = [line for line in lines if line[-1] != ':']

jump_table = {}

pc = 0
for line in lines:
  if line[-1] == ':':
    jump_table[line[:-1]] = pc
  pc += 1

instructions = {
  'halt': ['xxxx', 0x0],
  'nop' : ['xxxx', 0x1],
  'li'  : ['rxxi', 0x2],
  'lw'  : ['rrxx', 0x3],
  'sw'  : ['rrxx', 0x4],
  'add' : ['rrrx', 0x5],
  'sub' : ['rrrx', 0x6],
  'mult': ['rrrx', 0x7],
  'div' : ['rrrx', 0x8],
  'j'   : ['xxxi', 0x9],
  'jr'  : ['rxxx', 0xA],
  'beq' : ['rrrx', 0xB],
  'bne' : ['rrrx', 0xC],
  'inc' : ['rxxx', 0xD],
  'dec' : ['rxxx', 0xE]
}

def resolve(atom):
  if atom in jump_table:
    return jump_table[atom]
  else:
    return int(atom, 16)

of = open(sys.argv[2], 'wb')

for line in executable_lines:
  tokens = [token.strip() for token in line.split(' ')]

  if tokens[0] not in instructions:
    print 'undefined instruction'
    continue

  fmt, instr = instructions[tokens[0]]

  opcode = instr << 56

  if fmt == 'rxxx':
    opcode += (int(tokens[1][1]) << 48)
  elif fmt == 'rxxi':
    opcode += (int(tokens[1][1]) << 48) + resolve(tokens[2])
  elif fmt == 'rrxx':
    opcode += (int(tokens[1][1]) << 48) + (int(tokens[2][1]) << 40)
  elif fmt == 'rrrx':
    opcode += (int(tokens[1][1]) << 48) + (int(tokens[2][1]) << 40) + (int(tokens[3][1]) << 32)
  elif fmt == 'xxxi':
    opcode += resolve(tokens[1])

  print format(opcode, '016x'), line

  of.write(pack('<Q', opcode))

of.close
