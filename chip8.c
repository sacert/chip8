#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  unsigned short opcode;
  unsigned char memory[4096];

  unsigned char registers[16];

  unsigned short index_register;
  unsigned short pc;

  unsigned char graphics[64 * 32]; // could use 2d array

  unsigned char delay_timmer;
  unsigned char sound_timer;

  unsigned short stack[16];
  unsigned short sp;

  unsigned char keyboard_inputs[16];

} CPU; 

CPU cpu;

// didn't write this
unsigned char chip8_fontset[80] =
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void initialize_cpu() {
  cpu.pc              = 0x200;
  cpu.opcode          = 0;
  cpu.index_register  = 0;
  cpu.sp              = 0;

  // load font's into memory 0x050-0x0A0
  memcpy(&cpu.memory[0x050], &chip8_fontset[0x000], sizeof(chip8_fontset));
}

void load_rom(char * file)
{
  FILE *f;
  f = fopen(file, "rb");
  fseek(f, 0, SEEK_END);
  int len = ftell(f);
  unsigned char *rom = malloc(len);
  rewind(f);
  fread(rom, len, 1, f);

  // load application from memory address 0x0200 onwards
  memcpy(&cpu.memory[0x0200], &rom[0x0000], len);
}

void cpu_cycle() {

  // opcodes are 2 bytes
  cpu.opcode = cpu.memory[cpu.pc << 8] | cpu.memory[cpu.pc + 1];
  
  switch (cpu.opcode & 0xF000) {

    // bless testing in C -- so much fun ... 
    case 0x0000: // useless right now CLS, RET, SYS addr
      printf ("Opcode not implemented | %u", cpu.opcode);
      break;
    case 0x1000: // JP addr 0x1nnn
      cpu.pc = cpu.opcode & 0x0FFF;
      break;
    case 0x2000: // CALL addr 0x2nnn
      stack[cpu.sp] = cpu.pc;
      sp += 1;
      cpu.pc = cpu.opcode & 0x0FFF;
      break;
    case 0x3000: // SE Vx, byte
      (register[(cpu.opcode & 0x0F00) >> 8] == (cpu.opcode & 0x00FF)) ? cpu.pc += 4 : cpu.pc += 2;
      break;
    case 0x4000: // SNE Vx, byte
      (register[(cpu.opcode & 0x0F00) >> 8] != (cpu.opcode & 0x00FF)) ? cpu.pc += 4 : cpu.pc += 2;
      break; 
    case 0x5000: // SE Vx. Vy
      (register[(cpu.opcode & 0x0F00) >> 8] == register[(cpu.opcode & 0x00F0) >> 4]) ? cpu.pc += 4 : cpu.pc += 2;
      break;
    case 0x6000: // LD Vx, byte
      register[(cpu.opcode & 0x0F00) >> 8] = (cpu.opcode & 0x00FF);
      cpu.pc += 2;
      break;
    case 0x7000: // ADD
      printf ("Opcode not implemented | %u", cpu.opcode);
      break;
    case 0x8000: // lots
      printf ("Opcode not implemented | %u", cpu.opcode);
      break;
    case 0x9000:
      printf ("Opcode not implemented | %u", cpu.opcode);
      break;
    case 0xA000:
      printf ("Opcode not implemented | %u", cpu.opcode);
      break;
    case 0xB000:
      printf ("Opcode not implemented | %u", cpu.opcode);
      break;
    case 0xC000:
      printf ("Opcode not implemented | %u", cpu.opcode);
      break;
    case 0xD000:
      printf ("Opcode not implemented | %u", cpu.opcode);
      break;
    case 0xE000:
      printf ("Opcode not implemented | %u", cpu.opcode);
      break;
    case 0xF000:
      printf ("Opcode not implemented | %u", cpu.opcode);
      break;
    default:
      printf ("Unknown opcode -- yikes| %u", cpu.opcode);
  }

}

int main() {
  
  initialize_cpu();
  load_rom("pong.rom");

  return 0;
}
