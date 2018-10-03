#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "chip8.h"

#define screen_idx(x, y) (uint32_t)((x) + ((y) * SCREEN_WIDTH))
#define WHITE 0xFFFFFFFF


void init(struct chip8 *cpu) {
	/* make sure not working with uninitialized stuff */
	srand((unsigned int)time(NULL));

	memset(cpu, 0, sizeof(struct chip8));

	cpu->pc = START;

	memcpy(cpu->memory + 0x50, font, sizeof(font)); /* load font into memory */

}
int loadrom(struct chip8 *cpu, const char *path){
    FILE *fp;
    long len;


    len = 0;
    fp = fopen(path, "rb");

	if(fp) {
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		fread(cpu->memory + START, (size_t)len, 1, fp);
		fclose(fp);
		return(EXIT_SUCCESS);
    }
    else {
		perror("failed to open rom ");
		exit(EXIT_FAILURE);
	}
}

void docycle(struct chip8 *cpu) {
    unsigned short X, Y, pixel;
    uint8_t row, col; 
    uint32_t index, timeout;

	cpu->opcode = (short unsigned int)(cpu->memory[cpu->pc] << 8 | cpu->memory[cpu->pc + 1]);
    col = row = 0;
    X = (cpu->opcode & 0x0F00) >> 8;
    Y = (cpu->opcode & 0x00F0) >> 4;
	timeout = SDL_GetTicks() + 16;



    while (!SDL_TICKS_PASSED(SDL_GetTicks(), timeout)) {
    }
    
	switch(cpu->opcode & 0xF000) {
		case 0x0000:
			switch(cpu->opcode) {
				case 0x00E0: /* 00E0: Clears the screen */
					memset(cpu->gfx, 0, sizeof(uint32_t)*2048);
					cpu->drawFlag = 1;
				break;
				case 0x00EE: /* 00EE: Returns from subroutine */
					cpu->pc = cpu->stack[--cpu->sp];
				break;

		                default:
                		    ;
                		break;
			}

		break;

		case 0xA000: /* ANNN: Sets I to the address NNN */
			cpu->I = cpu->opcode & 0x0FFF;
		break;

		case 0xB000: /* BNNN: Jump to address NNN plus V0. */			
			cpu->pc = (short unsigned int)((cpu->opcode & 0x0FFF) + cpu->V[0]);
		break;

		case 0xC000: /* CXNN: Sets VX to the result of bitwise and on a random number and NN. */
			cpu->V[X] = (unsigned char)((rand() % 256) & (cpu->opcode & 0x00FF));
		break;

		case 0xD000: /* graphics */
			cpu->V[0xF] = 0;
			index = 0;
			for (row = 0; row < (cpu->opcode & 0x000F); ++row) {
				pixel = cpu->memory[cpu->I + row];
				for(col = 0; col < 8; ++col) {
					if((pixel & (0x80 >> col)) != 0) {
						index = screen_idx(cpu->V[X] + col, cpu->V[Y] + row);
						if(cpu->gfx[index] == WHITE) {
							cpu->V[0xF] = 1;
						}
					}
					cpu->gfx[index] ^= WHITE;
				}
			}

			cpu->drawFlag = 1;
		break;

		case 0x1000: /* 1NNN: Jumps to address NNN */
			cpu->pc = (cpu->opcode & 0x0FFF);
		break;

		case 0x2000: /* 2NNN: Calls subroutine at address NNN. */
			cpu->stack[cpu->sp++] = cpu->pc;
			cpu->pc = (cpu->opcode & 0x0FFF);
		break;

		case 0x3000: /* 3XNN: Skips the next instruction if VX equals NN. */
			if((cpu->opcode & 0x00FF) == cpu->V[X]) {
				cpu->pc = (short unsigned int) (cpu->pc + 2);
			}
		break;

		case 0x4000: /* 4XNN: Skips the next instruction if VX does not equal NN. */
			if(cpu->V[X] != (cpu->opcode & 0x00FF)) {
				cpu->pc = (short unsigned int) (cpu->pc + 2);
			}
		break;

		case 0x5000: /* 5XY0: Skips the next instruction if VX equals VY. */
			if(cpu->V[X] == cpu->V[Y]) {
				cpu->pc = (short unsigned int) (cpu->pc + 2);
			}
		break;

		case 0x6000: /* 6XNN: Sets VX to NN */
			cpu->V[X] = (unsigned char)(cpu->opcode & 0x00FF);
		break;

		case 0x7000: /* 7XNN: Adds NN to VX. */
			cpu->V[X] = (unsigned char)((cpu->opcode & 0x00FF) + cpu->V[X]);
		break;

		case 0x8000:
			switch(cpu->opcode & 0x000F) {
				case 0x0000: /* 8XY0: Sets VX to the value of VY. */
					cpu->V[X] = cpu->V[Y];
				break;

				case 0x0001: /* Sets VX to VX or VY. */
					cpu->V[X] = (cpu->V[X] | cpu->V[Y]);
				break;

				case 0x0002: /* 8XY2: Sets VX to VX and VY */
					cpu->V[X] = (cpu->V[X] & cpu->V[Y]);
				break;

				case 0x0003: /* 8XY3: Sets VX to VX xor VY. */
					cpu->V[X] = (cpu->V[X] ^ cpu->V[Y]);
				break;

				case 0x0004: /* 8XY4: Adds VY to VX. VF is set to 1 when there is a carry. */ 
					cpu->V[0xF] = cpu->V[Y] > (0xFF - cpu->V[X]); /* carry */
					cpu->V[X] = (unsigned char)(cpu->V[X] + cpu->V[Y]);
				break;

				case 0x0005: /* 8XY5: VY is subtracted from VX. VF is set to 0 when borrow. */
					cpu->V[0xF] = cpu->V[X] < cpu->V[Y];
					cpu->V[X] = (unsigned char)(cpu->V[X] - cpu->V[Y]);
				break;

				case 0x0006: /* 8XY6: Shifts VX right by one. VF is set to value of least sig bit before shift */
					cpu->V[0xF] = (cpu->V[X] & 1); /* get least sig bit */
					cpu->V[X] = (cpu->V[X] >> 1);
				break;

				case 0x0007: /* 8XY7: Sets VX to VY minus VX. VF is set to 0 when borrow. */
                    cpu->V[0xF] = cpu->V[Y] < cpu->V[X]; /* borrow */
                    cpu->V[X] = (unsigned char)(cpu->V[Y] - cpu->V[X]);
				break;

				case 0x000E: /* Shifts VX left by one. VF is set to value of most sig bit before shift. */
					cpu->V[0xF] = (cpu->V[X] & 1); /* get least sig bit */
					cpu->V[X] = (unsigned char)(cpu->V[X] << 1);
				break;

                default:
                    ;
                break;
			}
		break;

		case 0x9000: /*  5XY0: Skips the next instruction if VX does not equal VY. */
			if(cpu->V[X] != cpu->V[Y]) {
				cpu->pc = (short unsigned int) (cpu->pc + 2);
			}
		break;

		case 0xE000:
        	switch(cpu->opcode & 0x00FF) {
	            case 0x009E: /*0xEX9E: Skips the next instruction if the key stored in VX is pressed.*/
	                if (key_down == cpu->V[X]) {
	                    cpu->pc = (short unsigned int) (cpu->pc + 2);
	                }
	                break;

	            case 0x00A1: /*0xEXA1: Skips the next instruction if the key stored in VX is not pressed. */
	                if (key_down != cpu->V[X]) {
	                    cpu->pc = (short unsigned int) (cpu->pc + 2);
	                }
	                break;

                default:
                    ;
                break;
	        }
        key_down = 0;
        break;

        case 0xF000:
        	switch(cpu->opcode & 0x00FF) {
        		case 0x0007: /* 0xFX07: Sets VX to the value of the delay timer */
        			cpu->delay_timer = cpu->V[X];
        		break;

        		case 0x000A: /* 0xFX0A: Wait for a keypress, halting operation, store in VX */
        			do {
        				SDL_WaitEvent(&e);
        			} while(e.type != SDL_KEYDOWN);
        			cpu->V[X] = handle_key_down(e.key.keysym.sym);
        		break;

        		case 0x0015: /* 0xFX15: Sets delay timer to VX */
        			cpu->delay_timer = cpu->V[X];
        		break;

        		case 0x0018: /* 0xFX18 sound timer  = VX */
        			cpu->sound_timer = cpu->V[X];
        		break;

        		case 0x001E: /* 0xFX1E add VX to I */
        			cpu->I = (short unsigned int)(cpu->I + cpu->V[X]);
        		break;

        		case 0x0029: /* 0xFX29 Sets I to the location of the sprite in character VX (4x5 font).  */
        			cpu->I = (short unsigned int)((cpu->opcode & 0x0F00) >> 8) ;
        		break;

        		case 0x0033: /* Stores BCD representation of VX, with 3 digits, in memory at location I. */
        			cpu->memory[cpu->I] = cpu->V[X] / 100;
        			cpu->memory[cpu->I+1] = (cpu->V[X] / 10) % 10;
        			cpu->memory[cpu->I+2] = cpu->V[X] % 10;
        		break;

        		case 0x0055: /* 0xFX55: Store registers V0 through VX in memory at location I. */
        			memcpy(cpu->memory + cpu->I, cpu->V, sizeof(X+1));
        		break;

        		case 0x0065: /* Read registers V0 through VX from memory at location I.  */
        			memcpy(cpu->V, cpu->memory + cpu->I, sizeof(X+1));
        		break;

                default:
                    ;
                break;
        	}
        break;

		default:
			printf("Unknown opcode, skipping: %d\n", cpu->opcode);
		break;
	}
	if(cpu->delay_timer > 0){
		cpu->delay_timer--;
	}
	if(cpu->sound_timer > 0) {
		if(cpu->sound_timer  == 1) {
			printf("sound");
			cpu->sound_timer--;
		}
	}
	cpu->pc = (short unsigned int) (cpu->pc + 2);
}


uint8_t handle_key_down(SDL_Keycode keycode) 
{
    switch(keycode) {
        case SDLK_1:
            key[0x1] = 1;
            return 0x1;
        case SDLK_2:
            key[0x2] = 1;
            return 0x2;
        case SDLK_3:
            key[0x3] = 1;
            return 0x3;
        case SDLK_4:
            key[0xC] = 1;
            return 0xC;
        case SDLK_q:
            key[0x4] = 1;
            return 0x4;
        case SDLK_w:
            key[0x5] = 1;
            return 0x5;
        case SDLK_e:
            key[0x6] = 1;
            return 0x6;
        case SDLK_r:
            key[0xD] = 1;
            return 0xD;
        case SDLK_a:
            key[0x7] = 1;
            return 0x7;
        case SDLK_s:
            key[0x8] = 1;
            return 0x8;
        case SDLK_d:
            key[0x9] = 1;
            return 0x9;
        case SDLK_f:
            key[0xE] = 1;
            return 0xE;
        case SDLK_z:
            key[0xA] = 1;
            return 0xA;
        case SDLK_x:
            key[0x0] = 1;
            return 0x0;
        case SDLK_c:
            key[0xB] = 1;
            return 0xB;
        case SDLK_v:
            key[0xF] = 1;
            return 0xF;
        default:
            return(EXIT_FAILURE);
    }
}

uint8_t handle_key_up(SDL_Keycode keycode)
{
    switch(keycode) {
        case SDLK_1:
            key[0x1] = 0;
            return 0x1;
        case SDLK_2:
            key[0x2] = 0;
            return 0x2;
        case SDLK_3:
            key[0x3] = 0;
            return 0x3;
        case SDLK_4:
            key[0xC] = 0;
            return 0xC;
        case SDLK_q:
            key[0x4] = 0;
            return 0x4;
        case SDLK_w:
            key[0x5] = 0;
            return 0x5;
        case SDLK_e:
            key[0x6] = 0;
            return 0x6;
        case SDLK_r:
            key[0xD] = 0;
            return 0xD;
        case SDLK_a:
            key[0x7] = 0;
            return 0x7;
        case SDLK_s:
            key[0x8] = 0;
            return 0x8;
        case SDLK_d:
            key[0x9] = 0;
            return 0x9;
        case SDLK_f:
            key[0xE] = 0;
            return 0xE;
        case SDLK_z:
            key[0xA] = 0;
            return 0xA;
        case SDLK_x:
            key[0x0] = 0;
            return 0x0;
        case SDLK_c:
            key[0xB] = 0;
            return 0xB;
        case SDLK_v:
            key[0xF] = 0;
            return 0xF;
        default:
            return(EXIT_FAILURE);
    }
}
