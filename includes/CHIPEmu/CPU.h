/*
    This is the core components of CHIP-8, one single file.
    
    I have followed some tutorials to understand how does a CHIP-8
    work. However the codes below belongs to me.

    I have followed tutorials from Austin Morlan(1) and Tobias
    V. I. Langhoff(2)

    1: https://austinmorlan.com/posts/chip8_emulator/ (C++ visual examples)
    2: https://tobiasvl.github.io/blog/write-a-chip-8-emulator/ (Explaination)

    I have mostly used these websites to learn how does the CHIP-8
    work on paper. Also since I am a beginner to emulating, Austin
    Morlan's codes really helped me alot. You can see most codes are
    just identical to his but tweaked by me as I am still learning.

    This is my first time coding an emulator so if anything's wrong
    please submit an issue and tell me to improve it.

    Since this is COMPLETELY independent from the other rest of the codes here,
    you can just take this code and kinda use your own desired libraries like
    swap GLFW with native API's like Win32 API, change the audio library, EVEN
    change the OpenGL to something else.
*/

#ifndef CPU_H
#define CPU_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PROG_ST_ADD 0x200 // Program Start Address
#define FONT_ST_ADD 0x50 // Fontset Start Address

#define MEMORY_SIZE 4096 // Exactly 4KB of memory.
#define REG_AMOUNT 16 // V0 to VF registers (exactly 16)
#define VIDEO_SIZE 2048 // It's just 64 * 32 but no need to leave the calculating to compiler.
#define VIDEO_WIDTH_SIZE 64 // Width of Video Size
#define VIDEO_HEIGHT_SIZE 32 // Height of Video Size

#define FONT_SIZE 80 // Font size

/*
    8-Bit Fontset, see Tobias' blog for more explaination:
    https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
*/
const uint8_t FONT[FONT_SIZE] =
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

/*
    CHIP-8 CPU
    Includes: 
    -> 8-bit Memory of 4KB
    -> 8-bit 16 Registers
    -> 32-bit Video Buffer
    -> 16-bit Index
    -> 16-bit Program Counter
    -> 16-bit 16 Stacks
    -> 8-bit Stack Pointers
    -> 8-bit 16 Keys (from keypad)
    -> 8-bit Delay Timer
    -> 8-bit Sound Timer
    -> 16-bit opcode

    Please do not tweak anything inside unless
    you know what you are doing!
*/
struct CPU
{
    uint8_t MEMORY[MEMORY_SIZE]; // Memory
    uint8_t V_REG[REG_AMOUNT]; // Register
    uint32_t VIDEO_BUFFER[VIDEO_SIZE]; // Video Buffer
    uint16_t I; // Index
    uint16_t PC; // Program Counter
    uint16_t S[16]; // Stack
    uint8_t SP; // Stack Pointer
    uint8_t keypad[16]; // Keypad
    uint8_t delayTimer; // Delay Timer
    uint8_t soundTimer; // Sound Timer
    uint16_t opcode; // OPCODE
};

/*
    CHIP-8 Functionss
*/
typedef void (*CH8F)();

CH8F table[0xF + 1];
CH8F table0[0xF + 1];
CH8F table8[0xF + 1];
CH8F tableE[0xF + 1];
CH8F tableF[0x65 + 1];

static struct CPU CHIP8; // CHIP-8

/*
    Reads a CH8 file then loads to memory.

    Returns 0 if it can't find or open the file
    Returns 0 if it can't allocate memory to the buffer
    Returns 1 if finishes without any problem

    DOES NOT RETURN THE FILE CONTENTS! FILE CONTENTS
    ARE DIRECTLY WRITTEN TO THE CHIP8 STRUCT'S MEMORY.
*/
int loadFromCH8File(const char* filePath)
{
    FILE* file = fopen(filePath, "rb");
    if (file == NULL) return 0;

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize * sizeof(char));
    if (buffer == NULL) { fclose(file); return 0; };

    fread(buffer, sizeof(char), fileSize, file);
    fclose(file);

    for (size_t i = 0; i < fileSize; i++) { CHIP8.MEMORY[PROG_ST_ADD + i] = (unsigned char)buffer[i]; };

    free(buffer);

    return 1;
};

/*
    Instruct to clean the screen completely,
    we are simply doing this with setting
    every number on VIDEO_BUFFER in the CHIP8
    to create a blank screen. It does not
    involve the OpenGL calls, it's independent.
*/
void OP_00E0() { memset(CHIP8.VIDEO_BUFFER, 0, sizeof(CHIP8.VIDEO_BUFFER)); };

/*
    Instructs to return back.
*/
void OP_00EE() { CHIP8.PC = CHIP8.S[--CHIP8.SP]; };

/*
    Jumps to an address.
*/
void OP_1NNN() { CHIP8.PC = (CHIP8.opcode & 0x0FFFu); };

/*
    Calls a function.
*/
void OP_2NNN() { CHIP8.S[CHIP8.SP++] = CHIP8.PC; CHIP8.PC = (CHIP8.opcode & 0x0FFFu); };

/*
    Skips to next instruction if Vx is equal to byte.
*/
void OP_3XKK()
{
    if (CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u] == (CHIP8.opcode & 0x00FFu)) CHIP8.PC += 2;
};

/*
    Skipts to next instruction if Vx is NOT equal to byte.
*/
void OP_4XKK()
{
    if (CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u] != (CHIP8.opcode & 0x00FFu)) CHIP8.PC += 2;
};

/*
    Skips to next instruction if Vx is equal to Vy.
*/
void OP_5XY0()
{
    if (CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u] == CHIP8.V_REG[(CHIP8.opcode & 0x00F0u) >> 4u]) CHIP8.PC += 2;
};

/*
    Assigns a byte to Vx.
*/
void OP_6XKK()
{
    CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u] = (CHIP8.opcode & 0x00FFu);
};

/*
    Adds byte to Vx.
*/
void OP_7XKK()
{
    CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u] += (CHIP8.opcode & 0x00FFu);
};

/*
    Sets Vx to Vy.
*/
void OP_8XY0()
{
    CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u] = CHIP8.V_REG[(CHIP8.opcode & 0x00F0u) >> 4u];
};

/*
    Sets Vx to Vx OR Vy.
*/
void OP_8XY1()
{
    CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u] |= CHIP8.V_REG[(CHIP8.opcode & 0x00F0u) >> 4u];
};

/*
    Sets Vx to Vx AND Vy.
*/
void OP_8XY2()
{
    CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u] &= CHIP8.V_REG[(CHIP8.opcode & 0x00F0u) >> 4u];
};

/*
    Sets Vx to Vx XOR Vy.
*/
void OP_8XY3()
{
    CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u] ^= CHIP8.V_REG[(CHIP8.opcode & 0x00F0u) >> 4u];
};

/*
    Sums up Vx and Vy and returns 1 if
    higher than 255(8-bit maximum), 0 if not.
*/
void OP_8XY4()
{
    uint8_t Vx = (CHIP8.opcode & 0x0F00u) >> 8u;
    uint16_t Vxy = CHIP8.V_REG[Vx] + CHIP8.V_REG[(CHIP8.opcode & 0x00F0u) >> 4u];
    CHIP8.V_REG[0xF] = Vxy > 255U;

    CHIP8.V_REG[Vx] = Vxy & 0xFFu;
};

/*
    Substracts Vx and Vy depending on which
    one is bigger in value.
*/
void OP_8XY5()
{
    uint8_t Vx = (CHIP8.opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (CHIP8.opcode & 0X00F0u) >> 4u;
    
    CHIP8.V_REG[0xF] = (CHIP8.V_REG[Vx] >= CHIP8.V_REG[Vy]);
    CHIP8.V_REG[Vx] -= CHIP8.V_REG[Vy];
};

/*
    Division by 2.
*/
void OP_8XY6()
{
    uint8_t Vx = (CHIP8.opcode & 0x0F00u) >> 8u;

    CHIP8.V_REG[0xF] = (CHIP8.V_REG[Vx] & 0x1u);
    CHIP8.V_REG[Vx] = CHIP8.V_REG[CHIP8.opcode & 0x00F0u >> 4u] >>  1;
};

/*
    Substracts Vx and Vy.
*/
void OP_8XY7()
{
    uint8_t Vx = (CHIP8.opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (CHIP8.opcode & 0x00F0u) >> 4u;

    CHIP8.V_REG[0xF] = (CHIP8.V_REG[Vx] >= CHIP8.V_REG[Vy]);
    CHIP8.V_REG[Vx] = CHIP8.V_REG[Vy] - CHIP8.V_REG[Vx];
};

/*
    Multiplication by 2.
*/
void OP_8XYE()
{
    uint8_t Vx = (CHIP8.opcode & 0x0F00u) >> 8u;

    CHIP8.V_REG[0xF] = (CHIP8.V_REG[Vx] & 0x80u) >> 7u;
    CHIP8.V_REG[Vx] = CHIP8.V_REG[CHIP8.opcode & 0x00F0u >> 4u] << 1;
};

/*
    Skips to the next instruction if Vx and Vy are NOT equal.
*/
void OP_9XY0()
{
    if (CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u] != CHIP8.V_REG[(CHIP8.opcode & 0x00F0u) >> 4u]) CHIP8.PC += 2;
};

/*
    Sets Index to NNN.
*/
void OP_ANNN()
{
    CHIP8.I = (CHIP8.opcode & 0x0FFFu);
};

/*
    Jumps to V0 + NNN.
*/
void OP_BNNN()
{
    CHIP8.PC = CHIP8.V_REG[0] + (CHIP8.opcode & 0x0FFFu);
};

/*
    Random number generation AND KK.
*/
void OP_CXKK()
{
    uint8_t randomNumber = rand() % 256;
    CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u] = (randomNumber & (CHIP8.opcode & 0x00FFu));
};

/*
    Drawing to the VIDEO_BUFFER
*/
void OP_DXYN()
{
    uint8_t x = CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u] % VIDEO_WIDTH_SIZE;
    uint8_t y = CHIP8.V_REG[(CHIP8.opcode & 0x00F0u) >> 4u] % VIDEO_HEIGHT_SIZE;
    uint8_t height = (CHIP8.opcode & 0x000Fu);
    CHIP8.V_REG[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row)
    {
        //if (y + row >= VIDEO_HEIGHT_SIZE) break;
        uint8_t byte = CHIP8.MEMORY[CHIP8.I + row];

        for (unsigned int col = 0; col < 8; ++col)
        {
            //if (x + col >= VIDEO_WIDTH_SIZE) break;
            
            uint32_t* pixel = &CHIP8.VIDEO_BUFFER[(y + row) * VIDEO_WIDTH_SIZE + (x + col)];
            if (byte & (0x80u >> col)) { if (*pixel == 0xFFFFFFFF) { CHIP8.V_REG[0xF] = 1; }; *pixel ^= 0xFFFFFFFF; };
        };
    };
};

/*
    Check if a key is pressed. Skip to next instruction if yes.
*/
void OP_EX9E()
{
    if (CHIP8.keypad[CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u]]) CHIP8.PC += 2;
};

/*
    Check if a key is NOT pressed. Skip to next instruction if yes.
*/
void OP_EXA1()
{
    if (!CHIP8.keypad[CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u]]) CHIP8.PC += 2;
};

/*
    Set Vx to the delayTimer value.
*/
void OP_FX07()
{
    CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u] = CHIP8.delayTimer;
};

/*
    Listens to the key states. Stores the value of key to the Vx.
*/
void OP_FX0A()
{
    uint8_t Vx = (CHIP8.opcode & 0x0F00u) >> 8u;

    if (CHIP8.keypad[0]) CHIP8.V_REG[Vx] = 0;
    else if (CHIP8.keypad[1]) CHIP8.V_REG[Vx] = 1;
    else if (CHIP8.keypad[2]) CHIP8.V_REG[Vx] = 2;
    else if (CHIP8.keypad[3]) CHIP8.V_REG[Vx] = 3;
    else if (CHIP8.keypad[4]) CHIP8.V_REG[Vx] = 4;
    else if (CHIP8.keypad[5]) CHIP8.V_REG[Vx] = 5;
    else if (CHIP8.keypad[6]) CHIP8.V_REG[Vx] = 6;
    else if (CHIP8.keypad[7]) CHIP8.V_REG[Vx] = 7;
    else if (CHIP8.keypad[8]) CHIP8.V_REG[Vx] = 8;
    else if (CHIP8.keypad[9]) CHIP8.V_REG[Vx] = 9;
    else if (CHIP8.keypad[10]) CHIP8.V_REG[Vx] = 10;
    else if (CHIP8.keypad[11]) CHIP8.V_REG[Vx] = 11;
    else if (CHIP8.keypad[12]) CHIP8.V_REG[Vx] = 12;
    else if (CHIP8.keypad[13]) CHIP8.V_REG[Vx] = 13;
    else if (CHIP8.keypad[14]) CHIP8.V_REG[Vx] = 14;
    else if (CHIP8.keypad[15]) CHIP8.V_REG[Vx] = 15;
    else CHIP8.PC -= 2;
};

/*
    Sets delayTimer value to Vx.
*/
void OP_FX15()
{
    CHIP8.delayTimer = CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u];
};

/*
    Sets soundTimer value to Vx.
*/
void OP_FX18()
{
    CHIP8.soundTimer = CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u];
};

/*
    Adds Vx to the Index.
*/
void OP_FX1E()
{
    CHIP8.I += CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u];
};

/*
    Sets the Index location for the sprite.
*/
void OP_FX29()
{
    CHIP8.I = FONT_ST_ADD + (5 * CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u]);
};

/*
    Storing of value based on the decimals.

    EG:
        345 is just
        300 (3 hundreds)
         40 (4 tens)
          5 (5 ones)

    A detailed example can be actually found in Tobias V. I. Langholff's CHIP-8 Guide:
    https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#fx33-binary-coded-decimal-conversion
*/
void OP_FX33()
{
    uint8_t val = CHIP8.V_REG[(CHIP8.opcode & 0x0F00u) >> 8u];

    CHIP8.MEMORY[CHIP8.I + 2] = val % 10; val /= 10;
    CHIP8.MEMORY[CHIP8.I + 1] = val % 10; val /= 10;
    CHIP8.MEMORY[CHIP8.I] = val % 10;
};

/*
    Storing every register to memory from the starting index.
*/
void OP_FX55()
{
    uint8_t X = (CHIP8.opcode & 0x0F00u) >> 8u;
    for (uint8_t i = 0; i <= X; ++i) { CHIP8.MEMORY[CHIP8.I + i] = CHIP8.V_REG[i]; };
    
    CHIP8.I += X + 1;
};

/*
    Reads every register to memory from the starting index.
*/
void OP_FX65()
{
    uint8_t X = (CHIP8.opcode & 0x0F00u) >> 8u;
    for (uint8_t i = 0; i <= X; ++i) { CHIP8.V_REG[i] = CHIP8.MEMORY[CHIP8.I + i]; };

    CHIP8.I += X + 1;
};

void OP_NULL() { }; // Dummy instructor, does nothing. Don't delete.
void Table0() { table0[CHIP8.opcode & 0x000Fu](); };
void Table8() { table8[CHIP8.opcode & 0x000Fu](); };
void TableE() { tableE[CHIP8.opcode & 0x000Fu](); };
void TableF() { tableF[CHIP8.opcode & 0x00FFu](); };

/*
    Sets up the CHIP-8 CPU.
    Generates a random seed, sets the PROGRAM COUNTER
    to PROGRAM START ADRESS (0x200), copies fonts to
    the memory. Sets up the opcode table.

    You can see that 0x0, 0x8, 0xE and 0xF are unique when
    it comes to the tables. It's because instead of the others
    they are handling more than one opcodes.
*/
void initCHIP8()
{
    memset(&CHIP8, 0, sizeof(struct CPU)); // Just in case, setting everything to 0 in the CHIP8.

    srand(time(NULL)); // Seed
    CHIP8.PC = PROG_ST_ADD; // 0x200
    memcpy(&CHIP8.MEMORY[FONT_ST_ADD], FONT, FONT_SIZE); // 0x50

    for (size_t i = 0; i <= 0xF; i++) { table0[i] = &OP_NULL; table8[i] = &OP_NULL; tableE[i] = &OP_NULL; };
    for (size_t i = 0; i <= 0x65; i++) { tableF[i] = &OP_NULL; };

    table[0x0] = &Table0;
    table[0x1] = &OP_1NNN;
    table[0x2] = &OP_2NNN;
    table[0x3] = &OP_3XKK;
    table[0x4] = &OP_4XKK;
    table[0x5] = &OP_5XY0;
    table[0x6] = &OP_6XKK;
    table[0x7] = &OP_7XKK;
    table[0x8] = &Table8;
    table[0x9] = &OP_9XY0;
    table[0xA] = &OP_ANNN;
    table[0xB] = &OP_BNNN;
    table[0xC] = &OP_CXKK;
    table[0xD] = &OP_DXYN;
    table[0xE] = &TableE;
    table[0xF] = &TableF;
    
    table0[0x0] = &OP_00E0;
    table0[0xE] = &OP_00EE;

    table8[0x0] = &OP_8XY0;
    table8[0x1] = &OP_8XY1;
    table8[0x2] = &OP_8XY2;
    table8[0x3] = &OP_8XY3;
    table8[0x4] = &OP_8XY4;
    table8[0x5] = &OP_8XY5;
    table8[0x6] = &OP_8XY6;
    table8[0x7] = &OP_8XY7;
    table8[0xE] = &OP_8XYE;

    tableE[0x1] = &OP_EXA1;
    tableE[0xE] = &OP_EX9E;

    tableF[0x07] = &OP_FX07;
    tableF[0x0A] = &OP_FX0A;
    tableF[0x15] = &OP_FX15;
    tableF[0x18] = &OP_FX18;
    tableF[0x1E] = &OP_FX1E;
    tableF[0x29] = &OP_FX29;
    tableF[0x33] = &OP_FX33;
    tableF[0x55] = &OP_FX55;
    tableF[0x65] = &OP_FX65;
};

/*
    CPU Cycle.

    It emulates the CPU's ONE cycle. Ought to use
    this function in the while(shouldRender()) loop.
*/
void CHIP8CYCLE()
{
    CHIP8.opcode = (CHIP8.MEMORY[CHIP8.PC] << 8u) | CHIP8.MEMORY[CHIP8.PC + 1];
    CHIP8.PC += 2;

    table[(CHIP8.opcode & 0xF000u) >> 12u]();
};

/*
    Updates CHIP-8 timers.
*/
void CHIP8TIMERCYCLE()
{
    if (CHIP8.delayTimer > 0) --CHIP8.delayTimer;
    if (CHIP8.soundTimer > 0) --CHIP8.soundTimer;
};

#endif