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

import re

class Inst:
    def __init__(self):
        raise NotImplemented  
    def get_inst(self):
        raise NotImplemented
    def __str__(self):
        raise NotImplemented
class InstMatcher:
    def __init__(self):
        raise NotImplemented
    def get_regex(self):
        raise NotImplemented

class InstBuilder:
    def build_from_asmsrc(self, inststr):
        raise NotImplemented
    def build_from_elmoout(self, inststr):
        raise NotImplemented
class ARMInstASM(Inst):
    ROR='ror'
    r_register = r'(r[0-9]+)'
    r_inst = r'(%ss?)'
    def build(self, inststr):
        pass

class ARMInstELMO(Inst):
    r_bl = re.compile("--- 0x([A-F0-9]+): 0x[A-F0-9]+ bl")
    r_push = re.compile("--- 0x([A-F0-9]+): 0x[A-F0-9]+ push .+")
    r_pop = re.compile("--- 0x([A-F0-9]+): 0x[A-F0-9]+ pop .+")
    r_bls = re.compile("--- 0x([A-F0-9]+): 0x[A-F0-9]+ bls .+")
    r_bne = re.compile("--- 0x([A-F0-9]+): 0x[A-F0-9]+ bne .+")
    r_bgt = re.compile("--- 0x([A-F0-9]+): 0x[A-F0-9]+ bgt .+")
    r_bhi = re.compile("--- 0x([A-F0-9]+): 0x[A-F0-9]+ bhi .+")
    r_b = re.compile("--- 0x([A-F0-9]+): 0x[A-F0-9]+ b .+")
    r_any = re.compile("--- 0x([A-F0-9]+): .+")
    r_inst_type = re.compile("--- 0x([A-F0-9]+): 0x[A-F0-9]+ ([a-z]+) ?.*")
    r_start_trig = re.compile("--- 0x([A-F0-9]+): 0x[A-F0-9]+ bx r14")
    jmps = [(r_bls,2),(r_bne,2),(r_b,0),(r_bgt,2),(r_bhi,2)]
    
    def __init__(self, inststr):
        self.inststr = inststr
    def get_addr(self):
        g = re.match(self.r_inst_type, self.inststr)
        if g is None:
            return None
        return g[1]
    def get_inst(self):
        g = re.match(self.r_inst_type, self.inststr)
        if g is None:
            return None
        return g[2]
    def __str__(self):
        return "ARMInstELMO("+self.inststr+")"
class ARMInstBuilder(InstBuilder):
    def build_from_elmoout(self, inststr):
        new_inst = ARMInstELMO(inststr)
        if new_inst.get_inst() is None:
            return None
        return new_inst

    def build_from_asmsrc(self, inststr):
        pass

    def iscall(self, elmo_asm_line):
        ret = ARMInstELMO.r_bl.match(elmo_asm_line)
        if ret:
            return int(ret.groups()[0], 16)
        return None

    def isjump(self, elmo_asm_line, elmo_asm_line_after):
        for jmp in ARMInstELMO.jmps:
            ret = jmp[0].match(elmo_asm_line)

            if ret:
                if jmp[0] == ARMInstELMO.r_b:
                    return int(ret.groups(1)[0],16)
                ret1 = ARMInstELMO.r_any.match(elmo_asm_line_after)
                if int(ret.groups(1)[0], 16) + jmp[1] != int(ret1.groups(1)[0], 16):
                    return int(ret.groups(1)[0],16)
        return None
    
    def ispop(self, elmo_asm_line):
        return ARMInstELMO.r_pop.match(elmo_asm_line)
    
    def ispush(self, elmo_asm_line):
        return ARMInstELMO.r_push.match(elmo_asm_line)

    def isstarttrig(self, elmo_asm_line):
        return ARMInstELMO.r_start_trig.match(elmo_asm_line)


