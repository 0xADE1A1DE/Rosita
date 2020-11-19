# Copyright 2020 University of Adelaide
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#    http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


def check_inst(inst, mask, match):
    return inst & mask == match
def cycles_push(inst):
    if check_inst(inst, 0b1111111000000000, 0b1011010000000000):
        return 1 + bin(inst & 0x00ff).count('1')
    return -1

def cycles_pop(inst):
    # Format 14: push/pop registers
    if check_inst(inst, 0b1111111000000000, 0b1011110000000000):
        return 1 + bin(inst & 0x00ff).count('1')
    return -1

def cycles_pop_pc(inst):
    # Format 14: push/pop registers
    if check_inst(inst, 0b1111111100000000, 0b1011110100000000):
        return 4 + bin(inst & 0x00ff).count('1')
    return -1

def cycles_add(inst):
    # Format 2: add/subtract
    if check_inst(inst, 0b1111101000000000, 0b0001100000000000):
        return 1
    return -1

def cycles_add_pc(inst):
    return -1

def cycles_rot(inst):
    # Format 4
    if check_inst(inst, 0b1111111111000000, 0b0100000111000000):
        return 1
    return -1

def cycles_ldr(inst):
    # Format 7
    # Format 9
    if check_inst(inst, 0b1111101000000000, 0b0101100000000000) or \
        check_inst(inst, 0b1110100000000000, 0b0110100000000000):
        return 2
    return -1

def cycles_str(inst):
    # Format 7
    # Format 9
    if check_inst(inst, 0b1111101000000000, 0b0101000000000000) or \
        check_inst(inst, 0b1110100000000000, 0b0110000000000000):
        return 2
    return -1

def cycles_mov(inst):
    # Format 1: move shifted register
    # Format 3: move/compare/add/subtract immediate
    # Format 5: Hi register operations/branch exchange
    if check_inst(inst, 0b1111111111000000, 0b0000000000000000) or \
        check_inst(inst, 0b1111100000000000, 0b0010000000000000) or \
        check_inst(inst, 0b1111111100000000, 0b0100011000000000):
        return 1
    return -1


def cycles_mov_pc(inst):
    # Format 5: dest = pc
    if check_inst(inst, 0b1111111101000111, 0b0100011001000111):
        return 3
    return -1


__cycle_counts = [ 
    [ cycles_mov, cycles_mov_pc ],
    [ cycles_add, cycles_add_pc ],
    [ cycles_ldr ],
    [ cycles_str ],
    [ cycles_rot ],
    [ cycles_pop, cycles_pop_pc ],
    [ cycles_push ]
]

def get_cycle_counts():
    return __cycle_counts