#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define NUM_REGS 4
#define RAM_SIZE 65535

unsigned pc = 0;
unsigned long long *program = NULL;
int *ram = NULL;
int *r = NULL;
bool running = true;

unsigned long long fetch() {
  return program[pc++];
}

void debug_registers() {
  for(int i = 0; i < NUM_REGS; i++) printf("%08x ", r[i]);
  printf("\n");
}

void debug_ram() {
  printf("ram\n");

  for(int i = 0; i <= RAM_SIZE; i++) {
    printf("%08x ", ram[i]);
  }

  printf("\n");
}

void cycle() {
  char op = 0;
  unsigned r0 = 0;
  unsigned r1 = 0;
  unsigned r2 = 0;
  unsigned im = 0;

  unsigned long long instr = fetch();

  printf("%08x %016llx ", pc, instr);

  /*
  0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
  ^^^^^^^^^ ^^^^^^^^^ ^^^^^^^^^ ^^^^^^^^^ ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    instr.     r0        r1        r2            immediate value
  */

  op = (instr & 0xFF00000000000000) >> 56;
  r0 = (instr & 0x00FF000000000000) >> 48;
  r1 = (instr & 0x0000FF0000000000) >> 40;
  r2 = (instr & 0x000000FF00000000) >> 32;
  im = (instr & 0x00000000FFFFFFFF);

  switch(op) {
    case 0x0: // halt
      running = false;
      break;
    case 0x1: // nop
      break;
    case 0x2: // li
      r[r0] = im;
      break;
    case 0x3: // lw
      r[r0] = ram[r[r1]];
      break;
    case 0x4: // sw
      ram[r[r1]] = r[r0];
      break;
    case 0x5: // add
      r[r0] = r[r1] + r[r2];
      break;
    case 0x6: // sub
      r[r0] = r[r1] - r[r2];
      break;
    case 0x7: // mult
      r[r0] = r[r1] * r[r2];
      break;
    case 0x8: // div
      r[r0] = r[r1] / r[r2];
      break;
    case 0x9: // j
      pc = im;
      break;
    case 0xA: // jr
      pc = r[r0];
      break;
    case 0xB: // beq
      if(r[r0] == r[r1]) pc = r[r2];
      break;
    case 0xC: // bne
      if(r[r0] != r[r1]) pc = r[r2];
      break;
    case 0xD: // inc
      r[r0]++;
      break;
    case 0xE: // dec
      r[r0]--;
      break;
  }

  debug_registers();
}

bool load_program(const char *filename) {
  FILE *fp;
  long length;

  fp = fopen(filename, "r");

  if(fp == NULL) return false;

  fseek(fp, 0L, SEEK_END);
  length = ftell(fp) / sizeof(int);
  fseek(fp, 0L, SEEK_SET);

  program = (unsigned long long*)calloc(length, sizeof(unsigned long long));

  if(program == NULL) return false;

  fread(program, sizeof(unsigned long long), length, fp);

  fclose(fp);

  return true;
}

bool allocate_ram() {
  ram = (int*)calloc(RAM_SIZE, sizeof(int));
  return ram == NULL ? false : true;
}

bool allocate_registers() {
  r = (int*)calloc(NUM_REGS, sizeof(int));
  return r == NULL ? false : true;
}

int main(int argc, const char *argv[]) {
  if(!allocate_registers()) {
    printf("error allocating registers\n");
    return EXIT_FAILURE;
  }

  if(!allocate_ram()) {
    printf("error allocating ram\n");
    return EXIT_FAILURE;
  }

  if(!load_program(argv[1])) {
    printf("error loading program\n");
    return EXIT_FAILURE;
  }

  while(running) {
    cycle();
  }

  //debug_ram();

  return EXIT_SUCCESS;
}
