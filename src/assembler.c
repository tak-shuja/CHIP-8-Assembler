#include "include/assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Globals
Instruction instructions[MAX_INSTRUCTION_SIZE];
int instruction_count = 0;

Label label_table[MAX_LABEL_SIZE];
int label_count = 0;

void str_to_upper(char *str)
{
  while (*str)
  {
    *str = toupper((unsigned char)*str);
    str++;
  }
}

void trim(char *str)
{
  char *start = str;
  while (isspace(*start))
    start++;

  if (*start == 0)
  {
    *str = 0;
    return;
  }

  char *end = start + strlen(start) - 1;
  while (end > start && isspace(*end))
    end--;

  *(end + 1) = '\0';

  if (start != str)
    memmove(str, start, end - start + 2);
}

int resolve_label(const char *label_name)
{
  for (int i = 0; i < label_count; i++)
  {
    if (!strcmp(label_table[i].name, label_name))
      return label_table[i].address;
  }
  printf("Error: Unknown label `%s`\n", label_name);
  return -1;
}

int parse_instruction(char *line, int current_address)
{
  trim(line);
  str_to_upper(line);

  if (line[0] == ';' || line[0] == '\0')
    return 0;

  Instruction instr = {0};
  instr.address = current_address;

  char *colon_pos = strchr(line, ':');
  if (colon_pos)
  {
    int label_len = colon_pos - line;
    strncpy(instr.label, line, label_len);
    instr.label[label_len] = '\0';

    strcpy(label_table[label_count].name, instr.label);
    label_table[label_count].address = instr.address;
    label_count++;

    line = colon_pos + 1;
    trim(line);
  }

  char *token = strtok(line, " ,\n\t");
  if (!token)
  {
    instructions[instruction_count++] = instr;
    return 1;
  }

  strcpy(instr.mnemonic, token);
  token = strtok(NULL, " ,\n\t");
  if (token)
    strcpy(instr.operand1, token);

  token = strtok(NULL, " ,\n\t");
  if (token)
    strcpy(instr.operand2, token);

  token = strtok(NULL, " ,\n\t");
  if (token)
    strcpy(instr.operand3, token);

  instructions[instruction_count++] = instr;
  return 1;
}

void first_pass(FILE *input)
{
  char line[256];
  int current_address = 0x200;
  while (fgets(line, sizeof(line), input))
  {
    int added = parse_instruction(line, current_address);
    current_address += 2 * added;
  }
}

uint8_t get_register_index(char *reg)
{
  return (uint8_t)strtol(reg + 1, NULL, 16);
}

uint16_t perform_LDXVX(char *operand, int return_mask)
{
  uint8_t vx = strtol(operand + 1, NULL, 16);
  return return_mask | (vx << 8);
}


uint16_t assemble_instructions(Instruction *instr)
{

  // CLS
  if (!strcmp(instr->mnemonic, "CLS"))
  {
    return 0x00E0;
  }

  // RET
  else if (!strcmp(instr->mnemonic, "RET"))
  {
    return 0x00EE;
  }

  // compare LD
  else if (!strcmp(instr->mnemonic, "LD"))
  {
    // LD Vx, ...
    if (instr->operand1[0] == 'V')
    {
      uint8_t vx = get_register_index(instr->operand1);

      // LD Vx, byte
      if (isdigit(instr->operand2[0]))
      {
        uint8_t byte = (uint8_t)strtol(instr->operand2, NULL, 16);
        return 0x6000 | (vx << 8) | byte;
      }
      else if (instr->operand2[0] == 'V')
      {
        uint8_t vy = get_register_index(instr->operand2);
        return 0x8000 | (vx << 8) | (vy << 4);
      }
      else if (!strcmp(instr->operand2, "DT"))
      {
        return 0xF007 | (vx << 8);
      }
      else if (!strcmp(instr->operand2, "K"))
      {
        return 0xF00A | (vx << 8);
      }
      else if (!strcmp(instr->operand2, "[I]"))
      {
        return 0xF065 | (vx << 8);
      }
    }

    // LD I, addr
    else if (!strcmp(instr->operand1, "I"))
    {
      // LD I, addr
      uint16_t address = strtol(instr->operand2, NULL, 0);
      return 0xA000 | (address & 0x0FFF);
    }

    // LD F, Vx
    else if (!strcmp(instr->operand1, "F"))
    {
      return perform_LDXVX(instr->operand2, 0xF029);
    }

    // LD B, Vx
    else if (!strcmp(instr->operand1, "B"))
    {

      return perform_LDXVX(instr->operand2, 0xF033);
    }

    // LD DT, ...
    else if (!strcmp(instr->operand1, "DT"))
    {

      return perform_LDXVX(instr->operand2, 0xF015);
    }

    // LD ST, ...
    else if (!strcmp(instr->operand1, "ST"))
    {
      return perform_LDXVX(instr->operand2, 0xF018);
    }

    // LD [I], ...
    else if (!strcmp(instr->operand1, "[I]"))
    {
      return perform_LDXVX(instr->operand2, 0xF055);
    }
  }

  // JP
  else if (!strcmp(instr->mnemonic, "JP"))
  {
    uint16_t address;
    if (isdigit(instr->operand1[0]))
    {
      address = strtol(instr->operand1, NULL, 16);
      return 0x1000 | (address & 0x0FFF);
    }
    else if (!strcmp(instr->operand1, "V0"))
    {
      address = strtol(instr->operand2, NULL, 16);
      return 0xB000 | (address & 0x0FFF);
    }
    else
    {
      address = resolve_label(instr->operand1);
      return 0x1000 | (address & 0x0FFF);
    }
  }

  // CALL
  else if (!strcmp(instr->mnemonic, "CALL"))
  {
    // uint16_t address = strtol(instr->operand2, NULL, 16);
    uint16_t label_address = resolve_label(instr->operand1);
    return 0x2000 | (label_address & 0x0FFF);
  }

  // CONDITIONALS

  // SE Vx, ...
  else if (!strcmp(instr->mnemonic, "SE"))
  {
    uint8_t vx = get_register_index(instr->operand1);

    if (isdigit(instr->operand2[0]))
    {
      uint8_t byte = (uint8_t)strtol(instr->operand2, NULL, 16);
      return 0x3000 | (vx << 8) | byte;
    }
    else if (instr->operand2[0] == 'V')
    {
      uint8_t vy = get_register_index(instr->operand2);

      return 0x5000 | (vx << 8) | (vy << 4);
    }
  }

  // SNE Vx, ...
  else if (!strcmp(instr->mnemonic, "SNE"))
  {
    uint8_t vx = get_register_index(instr->operand1);

    if (isdigit(instr->operand2[0]))
    {
      uint8_t byte = (uint8_t)strtol(instr->operand2, NULL, 0);
      return 0x4000 | (vx << 8) | byte;
    }
    else if (instr->operand2[0] == 'V')
    {
      uint8_t vy = get_register_index(instr->operand2);

      return 0x9000 | (vx << 8) | (vy << 4);
    }
  }

  // ARITHMETIC & BITWISE

  // ADD
  else if (!strcmp(instr->mnemonic, "ADD"))
  {

    uint8_t vx = get_register_index(instr->operand1);

    if (isdigit(instr->operand2[0]))
    {
      uint8_t byte = (uint8_t)strtol(instr->operand2, NULL, 16);
      return 0x7000 | (vx << 8) | byte;
    }
    else if (instr->operand1[0] == 'V' && instr->operand2[0] == 'V')
    {
      uint8_t vy = get_register_index(instr->operand2);

      return 0x8004 | (vx << 8) | (vy << 4);
    }
    else if (!strcmp(instr->operand1, "I"))
    {
      vx = get_register_index(instr->operand2);
      return 0xF01E | (vx << 8);
    }
  }

  // OR
  else if (!strcmp(instr->mnemonic, "OR"))
  {
    uint8_t vx, vy;
    vx = get_register_index(instr->operand1);

    vy = get_register_index(instr->operand2);

    return 0x8001 | (vx << 8) | (vy << 4);
  }

  // AND
  else if (!strcmp(instr->mnemonic, "AND"))
  {
    uint8_t vx, vy;
    vx = get_register_index(instr->operand1);

    vy = get_register_index(instr->operand2);

    return 0x8002 | (vx << 8) | (vy << 4);
  }

  // XOR
  else if (!strcmp(instr->mnemonic, "XOR"))
  {
    uint8_t vx, vy;
    vx = get_register_index(instr->operand1);

    vy = get_register_index(instr->operand2);

    return 0x8003 | (vx << 8) | (vy << 4);
  }

  // SUB
  else if (!strcmp(instr->mnemonic, "SUB"))
  {
    uint8_t vx, vy;
    vx = get_register_index(instr->operand1);

    vy = get_register_index(instr->operand2);

    return 0x8005 | (vx << 8) | (vy << 4);
  }

  // SUBN
  else if (!strcmp(instr->mnemonic, "SUBN"))
  {
    uint8_t vx, vy;
    vx = get_register_index(instr->operand1);

    vy = get_register_index(instr->operand2);

    return 0x8007 | (vx << 8) | (vy << 4);
  }

  // BIT SHIFTS
  // SHR Vx, Vy [vy is ignored]
  else if (!strcmp(instr->mnemonic, "SHR"))
  {
    uint8_t vx, vy;
    vx = get_register_index(instr->operand1);
    vy = get_register_index(instr->operand2);

    return 0x8006 | (vx << 8) | (vy << 4);
  }
  // SHL Vx, Vy [vy is ignored]
  else if (!strcmp(instr->mnemonic, "SHL"))
  {
    uint8_t vx, vy;
    vx = get_register_index(instr->operand1);

    vy = get_register_index(instr->operand2);

    return 0x800E | (vx << 8) | (vy << 4);
  }

  // KEY INPUT
  // SKP Vx
  else if (!strcmp(instr->mnemonic, "SKP"))
  {
    uint8_t vx;
    vx = get_register_index(instr->operand1);

    return 0xE09E | (vx << 8);
  }
  // SKNP Vx
  else if (!strcmp(instr->mnemonic, "SKNP"))
  {
    uint8_t vx;
    vx = get_register_index(instr->operand1);

    return 0xE091 | (vx << 8);
  }

  // DRAWING
  // DRW Vx, Vy, N
  else if (!strcmp(instr->mnemonic, "DRW"))
  {
    uint8_t vx, vy, n;
    vx = get_register_index(instr->operand1);

    vy = get_register_index(instr->operand2);

    n = strtol(instr->operand3, NULL, 16);
    return 0xD000 | (vx << 8) | (vy << 4) | n;
  }
  // printf("Invalid mnemonic `%s` at address `0x%03X`\n", instr->mnemonic, instr->address);
  return 0x0000;
}

void write_binary(const char *filename)
{
  FILE *file = fopen(filename, "wb");
  if (!file)
  {
    perror("Failed to open output file");
    exit(1);
  }

  for (int i = 0; i < instruction_count; i++)
  {
    uint16_t opcode = assemble_instructions(&instructions[i]);
    uint8_t byte1 = (opcode >> 8) & 0xFF;
    uint8_t byte2 = opcode & 0xFF;

    fwrite(&byte1, 1, 1, file);
    fwrite(&byte2, 1, 1, file);
  }
  fclose(file);
  printf("Written %d instructions to %s\n", instruction_count, filename);
}
