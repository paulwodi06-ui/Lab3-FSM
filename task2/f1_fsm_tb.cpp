#include "Vf1_fsm.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#include <iostream>
#include <string>

#include "vbuddy.cpp"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    // Create the FSM and waveform tracer
    Vf1_fsm* top = new Vf1_fsm;
    VerilatedVcdC* tfp = new VerilatedVcdC;

    Verilated::traceEverOn(true);
    top->trace(tfp, 99);
    tfp->open("f1_fsm.vcd");

    // Connect to Vbuddy
    if (vbdOpen() < 0) {
        std::cerr << "Error opening Vbuddy port" << std::endl;
        delete top;
        delete tfp;
        return 1;
    }

    vbdSetMode(1); // This configures the rotary encoder button to operate in one-shot mode
    vbdHeader("Lab 3: F1 Lights");

    // Initialize inputs
    top->clk = 0;
    top->rst = 1;
    top->en = 0;
    top->eval();

    int simcyc = 0;

    // Generate one clock cycle
    auto tick = [&]() {
        top->clk = 0;
        top->eval();
        tfp->dump(2 * simcyc);

        top->clk = 1;
        top->eval();
        tfp->dump(2 * simcyc + 1);

        simcyc++;
    };

    // Reset FSM to S0
    tick();
    top->rst = 0;

    // Main simulation loop
    while (vbdGetkey() != 'q') {
        // Advance one state per button press
        top->en = vbdFlag();

        tick();

        // Display current state on Vbuddy
        vbdBar(top->data_out & 0xFF); // This sends the eight-bit output of your FSM to the NeoPixel LED bar
        vbdHex(1, top->data_out & 0xF);
        vbdHex(2, (top->data_out >> 4) & 0xF);
        vbdCycle(simcyc);
    }

    // Clean up
    top->final();
    tfp->close();
    vbdClose();

    delete top;
    delete tfp;

    return 0;
}
