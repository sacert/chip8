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

void initialize_cpu() {
  cpu.pc              = 0x200;
  cpu.opcode          = 0;
  cpu.index_register  = 0;
  cpu.sp              = 0;
}

void read_rom(char * file) //Don't forget to free retval after use
{
  FILE *f;
  f = fopen(file, "rb");
  fseek(f, 0, SEEK_END);
  int len = ftell(f);
  unsigned char *rom = malloc(len);
  rewind(f);
  fread(rom, len, 1, f);

  memcpy(&cpu.memory[0x0000], &rom[0x0000], len);
}

void cpu_cycle() {
  cpu.opcode = cpu.memory[cpu.pc << 8] | cpu.memory[cpu.pc + 1];
  
  switch (cpu.opcode & 0xF000) {
  
    case 0xA000:
    default:
      print ("Unknown opcode -- yikes| %u", cpu.opcode);
  }

}

int main() {
  
  initialize_cpu();
  read_rom("pong.rom");

  return 0;
}
