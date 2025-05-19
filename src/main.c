#include <stdio.h>
#include <stdlib.h>
#include "include/assembler.h"

int main(int argc, char **argv)
{

  // check if the args are correct
  if (argc < 3)
  {
    printf("Usage: %s <input_file.asm> <output_file.ch8>\n", argv[0]);
    return 1;
  }

  char *input_filename = argv[1];
  char *output_file_name = argv[2];

  FILE *input_file = fopen(input_filename, "rb");

  if (NULL == input_file)
  {
    perror("Failed to open input file.");
    exit(1);
  }

  first_pass(input_file);
  fclose(input_file);

  printf("\nAddr \t Label \t Mnem \t Oper1 \t Oper2 \t Oper 3\n\n");
  for (int i = 0; i < instruction_count; i++)
  {
    Instruction instruction = instructions[i];
    // if (instruction.mnemonic[0] != 0)
    printf("0x%03X \t %s \t %s \t %s \t %s\n",
           instruction.address,
           instruction.label[0] ? instruction.label : "-",
           instruction.mnemonic[0] ? instruction.mnemonic : "-",
           instruction.operand1[0] ? instruction.operand1 : "-",
           instruction.operand2[0] ? instruction.operand2 : "-",
           instruction.operand3[0] ? instruction.operand3 : "-");
  }

  printf("Compiled_instructions: \n");
  for (int i = 0; i < instruction_count; i++)
  {
    Instruction instruction = instructions[i];
    // remove empty lines
    if (instruction.mnemonic[0] != '\0')
    {
      uint16_t machine_code = assemble_instructions(&instruction);
      printf("0x%04X: 0x%04X\n", instruction.address, machine_code);
    }
  }

  write_binary(output_file_name);
}