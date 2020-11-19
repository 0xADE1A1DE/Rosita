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

import InstBuilder
class CycleCounter:
    def get_cycle_count(self, inst_type):
        raise NotImplemented

class ARMCycleCounter(CycleCounter):
    def __init__(self):
        self.cycles = {'str':2,
                       'ldr':2,
                       'strb':2,
                       'ldrb':2,
                       'lsls':1,
                       'lsrs':1,
                       'muls':1,
                       'bics':1,
                       'bne':2,
                       'b':1,
                       'bl':1,
                       'bx':1,
                       'cmp':1,
                       'cmps':1,
                       'ands':1,
                       'movs':1,
                       'mov':1,
                       'cpy':1, # same as mov
                       'rors':1,
                       'adds':1,
                       'add':1,
                       'sub':1,
                       'subs':1,
                       'eors':1,
                       'push':1,
                       'pop':1,
                       'orrs':1}
    def get_cycle_count(self, inst: InstBuilder.Inst):
        inst_type = inst.get_inst()
        if inst_type in self.cycles:
            return self.cycles[inst_type]
        else:
            raise NotImplementedError(inst_type)

