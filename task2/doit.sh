
#!/bin/bash
set -e

rm -rf obj_dir

verilator -Wall --cc --trace \
    --top-module f1_fsm \
    f1_fsm.sv \
    --exe f1_fsm_tb.cpp

make -j -C obj_dir -f Vf1_fsm.mk Vf1_fsm

./obj_dir/Vf1_fsm
