#pragma once

#include <stdint.h>

/* The CPU16 address bus is divided up into 1k pages.
 * Each page can have a separate cpu16_bus_t associated with it
 * allowing easy integration of peripherals.
 */

struct cpu16_t;

struct cpu16_bus_map_t {

    typedef uint16_t (*read_word_t )(cpu16_t *, uint16_t addr, cpu16_bus_map_t * map);
    typedef uint8_t  (*read_byte_t )(cpu16_t *, uint16_t addr, cpu16_bus_map_t * map);
    read_word_t  read_word_;
    read_byte_t  read_byte_;

    typedef void (*write_word_t)(cpu16_t *, uint16_t addr, uint16_t val, cpu16_bus_map_t * map);
    typedef void (*write_byte_t)(cpu16_t *, uint16_t addr, uint8_t  val, cpu16_bus_map_t * map);
    write_word_t write_word_;
    write_byte_t write_byte_;

    void * user_;
};

struct cpu16_device_t {

    uint32_t cycles_till_preempt_;

    typedef void (*init_t)(cpu16_t *, cpu16_device_t * device);
    init_t init_;

    typedef void (*tick_t)(cpu16_t *, cpu16_device_t * device);
    tick_t tick_;

    void * user_;
};

// create a new CPU16 context
extern
cpu16_t * cpu16_new ();

// free a CPU16 context
extern
void cpu16_free(cpu16_t *);

// execute a CPU16 context for 'count' instructions
extern
void cpu16_run(cpu16_t * cpu, uint32_t count);

// load an memory image from disk into a CPU16 context
extern
bool cpu16_load_image(cpu16_t * cpu, const char * path);

// hard reset a CPU16 context
extern
void cpu16_reset(cpu16_t * cpu);

// add a device to the cpu
extern
void cpu16_add_device(
    cpu16_t * cpu,
    cpu16_device_t * device);

// add a memory page controller
extern
void cpu16_map_bus(
    cpu16_t * cpu,
    cpu16_bus_map_t * ctrl,
    uint16_t page);

// read a byte from CPU16 memory
extern
uint8_t cpu16_read_byte(cpu16_t *cpu, uint16_t addr);

// access the memory space
extern
uint8_t * cpu16_get_memory(cpu16_t *cpu);

// read a register from CPU16
extern
uint16_t cpu16_get_register(cpu16_t *cpu, uint32_t index);

//
extern
bool cpu16_add_breakpoint(cpu16_t *cpu, uint16_t addr);

//
extern
bool cpu16_remove_breakpoint(cpu16_t *cpu, uint16_t addr);

//
extern
bool cpu16_patch(cpu16_t * cpu, uint8_t * in, uint32_t in_len, uint8_t * out, uint32_t * out_len);

//
extern
void cpu16_yield(cpu16_t *cpu);

//
extern
void cpu16_interrupt(cpu16_t *cpu, uint8_t num);
