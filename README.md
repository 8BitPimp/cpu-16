# CPU-16
## Description

CPU-16 is a fake 16 bit processor written as part of my desire to better learn assembly language, CPU design, and VM design.
Since CPU-16 is not a real architecture I was able to make the instruction set as simple to learn as possible.
A RISC load/store architecture is followed, so allowing memory to be accessed only via specific instructions.
This processor also follows a Von-Neumann architecture; with program instructions occupy the same memory as the working data of a program.

## Status

* An assembler has been written in python
* The emulator is still very much a work in progress
* A address/data bus abstraction has been added to the emulator
* Bytes written to 0x8000 to 0x8400 will be output to the console (crappy serial)

## Todo

* Loads of scope to improve the emulator
* Interrupts not implemented
* Only tested with hello world example
* More interrupt vectors can be implemented
* Add timer peripheral
* Add basic VGA graphics peripheral
* Add sound synth peripheral such as (YM2612, SN76489)
* Add keyboard controller peripheral
* Add disk controller
* Write basic OS

## Usage

The Assembler:
```
python cpu16asm.py <input.asm> <output.img>
```

The emulator:
```
cpu16 <output.img>
```

## Registers

All registers are 16 bits wide.

```
R00 (ZR)     Zero (always 0)
R01 (PC)     PC
R02 (SP)     stack pointer
R03
...          general purpose
R16
```

## Memory map

Since CPU-16 has a 16bit address bus, 0xffff is the highest location that can be addressed.
words are stored in memory as little endian.

```
{ 0x0000 .. 0x7fff }  - SRAM
{ 0x8000 .. 0x83ff }  - serial interface
{ 0x8400 .. 0xfffd }  - SRAM
{ 0xfffe }            - reset vector
```

## Power on

At power-on the program counter is set to the address of the reset vector; 0xFFFE.

## Assembly directives

* Operand types:
  * A symbolic address can be either a function name or a label.
```
#xxxx           - hexadecimal literal
%name           - symbolic address
```

* Symbolic address:
  * Functions will impose their scope on any labels declared within them.
```
FUNCTION name   - declare function 'name'
.label          - mark address with label
END             - mark end of function
```

* Address control:
  * All following output will follow from address #XXXX.
```
AT #XXXX        - set assembler output address
```

* Encode data:
  * String data is not zero terminated and may not contain spaces
  * Spaces must be encoded as the ascii #20 char
  * Data can contain many types on one line
```
DATA #ff        - output byte 0xff
DATA $HELLO     - output ascii string 'HELLO'
DATA %label     - output address of label
DATA $HI #20 #1234
```

## Instructions

* Instruction format
  * Instructions are either 16bits in size or 32bits if it has an immediate value.
  * The CC byte encodes the specific opcode to execute.
  * RX and RY are specified in the XY register byte of the instruction.
  * IMM is a little endian 16 bit immediate value following the instruction.
```
0xCC 0xYX [0xIIII]
```

* Instruction map
  * <LD/ST>W denotes a memory operation for WORD values.
  * <LD/ST>B denotes a memory operator for BYTE values.
  * <LDx/STx>+ denotes that the address register will be incremented by the data size after execution.

```
| 0xCC   | 0x0_          | 0x1_        | 0x2_      | 0x3_       | 0x4_    | 0x5_          | 0x6_     | 0x7_   |
| ------ | ------------- | ----------- | --------- | ---------- | ------- | ------------- | -------- | ------ |
| 0x_0   | LDW   RY  RX  | LDW IMM RX  | ADD RY RX | ADD IMM RX | PUSH RX | JMP       IMM | CALL IMM | RETI   |
| 0x_1   | LDB   RY  RX  | LDB IMM RX  | MUL RY RX | MUL IMM RX | POP  RX | JNE RY RX IMM | INT  IMM | CLI    |
| 0x_2   | LDW+  RY  RX  |             | SHL RY RX | SHL IMM RX |         | JEQ RY RX IMM |          | STI    |
| 0x_3   | LDB+  RY  RX  |             | SHR RY RX | SHR IMM RX |         | JL  RY RX IMM |          | RET    |
| 0x_4   | STW   RY  RX  | STW RX IMM  | SUB RY RX | SUB IMM RX |         | JG  RY RX IMM |          | BRK    |
| 0x_5   | STB   RY  RX  | STB RX IMM  | DIV RY RX | DIV IMM RX |         | JLE RY RX IMM |          |        |
| 0x_6   | STW+  RY  RX  |             | MOD RY RX | MOD IMM RX |         | JGE RY RX IMM |          |        |
| 0x_7   | STB+  RY  RX  |             | AND RY RX | AND IMM RX |         |               |          |        |
| 0x_8   |               |             | OR  RY RX | OR  IMM RX |         |               |          |        |
| 0x_9   |               |             | XOR RY RX | XOR IMM RX |         |               |          |        |
| 0x_A   |               |             | MOV RY RX | MOV IMM RX |         |               |          |        |
| 0x_B   |               |             | MLH RY RX | MLH IMM RX |         |               |          |        |
```

  * LDW  - load word
  * LDB  - load byte
  * LDW+ - load word and increment
  * LDB+ - load byte and increment
  * STW  - store word
  * STB  - store byte
  * STW+ - store word and increment
  * SDB+ - store byte and increment
  * CALL - call a subroutine (PUSH PC+4)
  * INT  - raise a specific interrupt
  * MLH  - multiply, keep high word (good for fixed point)
  * RETI - return from interrupt
  * CLI  - disable interrupts
  * STI  - enable interrupts
  * RET  - return from subroutine

## Pseudo instructions

  * NOP (MOV ZR ZR)
  * NOT (XOR RX #FFFF)
  * SWP (XOR RX RY, XOR RY RX, XOR RX RY)
