#pragma once

#include <stdint.h>
#include <stdio.h>

#define MAX_INSTRUCTION_SIZE 1000
#define MAX_LABEL_SIZE 100

typedef struct
{
  int address;
  char label[32];
  char mnemonic[8];
  char operand1[16];
  char operand2[16];
  char operand3[16];
} Instruction;

typedef struct
{
  char name[32];
  int address;
} Label;

extern Instruction instructions[MAX_INSTRUCTION_SIZE];
extern int instruction_count;

extern Label label_table[MAX_LABEL_SIZE];
extern int label_count;

void first_pass(FILE *input);
uint16_t assemble_instructions(Instruction *instr);
void write_binary(const char *filename);
