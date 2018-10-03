#ifndef CHIP8_H
#define CHIP8_H
#define START 0x200

struct chip8 {
    unsigned short opcode; /* 35 opcodes - two bytes long  */
    unsigned char memory[4096]; /* 4k memory */
    unsigned char V[16]; /* he Chip 8 has 15 8-bit general purpose registers named V0,V1 up to VE. The 16th register is used  for the ‘carry flag’.  */
    unsigned short I;  /*  Index register I and a program counter (pc) which can have a value from 0x000 to 0xFFF */
    unsigned short pc; 
    uint32_t gfx[2048]; /* 2048 pixels (64 x 32). Holds pixel state (1 or 0): */
    unsigned char delay_timer; 
    unsigned char sound_timer;  /* Interupts and hardware registers. The Chip 8 has none, but there are two timer registers that count at 60 Hz. When set above zero they will count down to zero */
    unsigned short stack[16]; /* used to remember the current location before a jump is performed */
    unsigned short sp;
    unsigned char key[16]; /*  HEX based keypad (0x0-0xF) */
    int drawFlag;
};


static unsigned char font[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, 
    0x20, 0x60, 0x20, 0x20, 0x70, 
    0xF0, 0x10, 0xF0, 0x80, 0xF0, 
    0xF0, 0x10, 0xF0, 0x10, 0xF0, 
    0x90, 0x90, 0xF0, 0x10, 0x10, 
    0xF0, 0x80, 0xF0, 0x10, 0xF0, 
    0xF0, 0x80, 0xF0, 0x90, 0xF0, 
    0xF0, 0x10, 0x20, 0x40, 0x40, 
    0xF0, 0x90, 0xF0, 0x90, 0xF0, 
    0xF0, 0x90, 0xF0, 0x10, 0xF0, 
    0xF0, 0x90, 0xF0, 0x90, 0x90, 
    0xE0, 0x90, 0xE0, 0x90, 0xE0, 
    0xF0, 0x80, 0x80, 0x80, 0xF0, 
    0xE0, 0x90, 0x90, 0x90, 0xE0, 
    0xF0, 0x80, 0xF0, 0x80, 0xF0, 
    0xF0, 0x80, 0xF0, 0x80, 0x80  
};


int done = 0;
const int SCREEN_WIDTH = 64;
const int SCREEN_HEIGHT = 32;
uint8_t key[16];
uint8_t key_down; /* Stores the key pressed in this cycle. can be none. can be checked once (and then it will be reset) */
SDL_Event e;

void init(struct chip8 *cpu);
int loadrom(struct chip8 *cpu, const char *path);
void docycle(struct chip8 *cpu);
uint8_t handle_key_down(SDL_Keycode keycode);
uint8_t handle_key_up(SDL_Keycode keycode);

#endif /* CHIP8_H */
