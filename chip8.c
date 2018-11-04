#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
  unsigned short opcode;
  unsigned char memory[4096];

  unsigned char registers[16];
  unsigned short index_register;

  unsigned short pc;

  unsigned char graphics[64 * 32]; // could use 2d array

  unsigned char delay_timer;
  unsigned char sound_timer;

  unsigned short stack[16];
  unsigned short sp;

  unsigned char keyboard_inputs[16];

  unsigned char draw;
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
  cpu.draw            = 0;

  // load font's into memory 0x050-0x0A0
  memcpy(&cpu.memory[0x050], &chip8_fontset[0x000], sizeof(chip8_fontset));

  // should I do this here? 
  srand(time(NULL));
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
    case 0x7000: // ADD Vx, byte ||  what about overflows? maybe look out for this one
      register[(cpu.opcode 0x0F00) >> 8] += (cpu.opcode 0x00FF);
      cpu.pc += 2; 
      break;
    case 0x8000: // lots -- will get to
      switch (cpu.opcode & 0x000F) {
        case 0x0000: // LD Vx, Vy
          register[(cpu.opcode & 0x0F00) >> 8] = register[(cpu.opcode & 0x00F0) >> 4];
          break;
        case 0x0001:
          register[(cpu.opcode & 0x0F00) >> 8] |= register[(cpu.opcode & 0x00F0) >> 4];
          break;
        case 0x0002:
          register[(cpu.opcode & 0x0F00) >> 8] &= register[(cpu.opcode & 0x00F0) >> 4];
          break;
        case 0x0003:
          register[(cpu.opcode & 0x0F00) >> 8] ^= register[(cpu.opcode & 0x00F0) >> 4];
          break;
        case 0x0004:
          int add = ((cpu.opcode & 0x0F00) >> 8) + ((cpu.opcode & 0x00F0) >> 4);
          (add > 0xFFFF) ? register[0xF] = 1 : register[0xF] = 0;
          register[(cpu.opcode & 0x0F00) >> 8] = add; 
          break;
        case 0x0005:
          (((cpu.opcode & 0x0F00) >> 8) > ((cpu.opcode & 0x00F0) >> 4)) ? register[0xF] = 1 : register[0xF] = 0;
          register[(cpu.opcode & 0x0F00) >> 8] -= register[(cpu.opcode & 0x00F0) >> 4];
          break;
        case 0x0006:
          (register[(cpu.opcode & 0x0F00) >> 8] & 0x0001) ? register[0xF] = 1 : register[0xF] = 0;
          register[(cpu.opcode & 0x0F00) >> 8] /= 2;
          break;
        case 0x0007:
          (((cpu.opcode & 0x0F00) >> 8) < ((cpu.opcode & 0x00F0) >> 4)) ? register[0xF] = 1 : register[0xF] = 0;
          register[(cpu.opcode & 0x0F00) >> 8] = register[(cpu.opcode & 0x00F0) >> 4] - register[(cpu.opcode & 0x0F00) >> 8];
          break;
        case 0x000E:
          (register[(cpu.opcode & 0x0F00) >> 8] & 0x1000) ? register[0xF] = 1 : register[0xF] = 0;
          register[(cpu.opcode & 0x0F00) >> 8] *= 2;
          break;
      }
      cpu.pc += 2;
      break;
    case 0x9000: // SNE Vx, Vy
      (register[(cpu.opcode & 0x0F00) >> 8] != register[(cpu.opcode & 0x00F0) >> 4]) ? cpu.pc += 4 : cpu.pc += 2;
      break;
    case 0xA000: // LD I, addr
      cpu.index_register = (cpu.opcode 0x0FFF);
      cpu.pc += 2;
      break;
    case 0xB000: // JP V0, addr
      cpu.pc = register[0] + (cpu.opcode & 0x0FFF);
      break;
    case 0xC000:
      register[(cpu.opcode & 0x0F00) >> 8] = ((rand() % 256) & (cpu.opcode & 0x00FF));
      cpu.pc += 2;
      break;
    case 0xD000:
      unsigned short x = register[(cpu.opcode 0x0F00) >> 8];
      unsigned short y = register[(cpu.opcode 0x00F0) >> 4];
      unsigned short n = register[(cpu.opcode 0x000F)];

      unsigned short p;
      register[0xF] = 0;

      for (int yy = 0; yy < h; yy++) {
        p = cpu.memory[cpu.index_register + yy];
        for (int xx = 0; xx < 8; xx++) {
          if ((p & (0x80 >> xx)) != 0) {
            if(graphics[(x + xx + ((y + yy) * 64))] == 1)
              register[0xF] = 1;                                 
            graphics[x + xx + ((y + yy) * 64)] ^= 1;
          }
        }
      }
      cpu.draw = 1;
      cpu.pc += 2;
      break;
    case 0xE000:
      switch (cpu.opcode & 0x00FF) {
        case 0x009E:
          if(keyboard_inputs[register[(cpu.opcode & 0x0F00) >> 8]] != 0)
            cpu.pc += 4;
          else
            cpu.pc += 2;
          break;
        case 0x00A1:
          if(keyboard_inputs[register[(cpu.opcode & 0x0F00) >> 8]] == 0)
            cpu.pc += 4;
          else
            cpu.pc += 2;
          break;
      }
      break;
    case 0xF000:
      switch (cpu.opcode & 0x00FF) {
        case 0x0007:
          register[(cpu.opcode & 0x0F00) >> 8] = cpu.deplay_timer; 
          cpu.pc += 2;
          break;
        case 0x000A:
          for (int i = 0; i < 16; i++) {
            if (cpu.keyboard_inputs[i] != 0) {
              register[(cpu.opcode & 0x0F00) >> 8] = i;
              cpu.pc += 2;
              break;
            }
          }
          break;
        case 0x0015:
          cpu.deplay_timer = register[(cpu.opcode & 0x0F00) >> 8];
          cpu.pc += 2;
          break;
        case 0x0018:
          cpu.sound_timer = register[(cpu.opcode & 0x0F00) >> 8];
          cpu.pc += 2;
          break;
        case 0x001E:
          cpu.index_register += register[(cpu.opcode & 0x0F00) >> 8];
          cpu.pc += 2;
          break;
        case 0x0029:
          cpu.index_register = register[(cpu.opcode & 0x0F00) >> 8] * 0x5;
					cpu.pc += 2;
          break;
        case 0x0033:
          cpu.memory[cpu.index_register]     = register[(cpu.opcode & 0x0F00) >> 8] / 100;
					cpu.memory[cpu.index_register + 1] = (register[(cpu.opcode & 0x0F00) >> 8] / 10) % 10;
					cpu.memory[cpu.index_register + 2] = (register[(cpu.opcode & 0x0F00) >> 8]) % 10;
          cpu.pc += 2;
          break;
        case 0x0055:
          for (int i = 0; i <= ((cpu.opcode & 0x0F00) >> 8); i++)
						cpu.memory[cpu.index_register + i] = register[i];

					cpu.index_register += ((cpu.opcode & 0x0F00) >> 8) + 1;
					cpu.pc += 2;
          break;
        case 0x0065:
          for (int i = 0; i <= ((cpu.opcode & 0x0F00) >> 8); i++)
						register[i] = cpu.memory[cpu.index_register + i];

					cpu.index_register += ((cpu.opcode & 0x0F00) >> 8) + 1;
					cpu.pc += 2;
          break;
      }
      break;
    default:
      printf ("Unknown opcode -- yikes| %u", cpu.opcode);
  }

}

int main() {
  
  initialize_cpu();
  load_rom("pong.rom");

  // for loop
  // if (cpu.draw)
  //    draw -> set cpu.draw to 0
  //

  return 0;
}
