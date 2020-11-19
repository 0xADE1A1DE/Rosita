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
from Matcher import Matcher

class ARMMatcher(Matcher):
    random_regs = ['r7']
    temp_vars = ['T32']
    temp_regs = []

    def get_random_reg(self):
        return self.random_regs[0]
    def get_temp_var(self):
        return self.temp_vars[0]
""" match only labels """
class LDRMatcherARM2(ARMMatcher): 
    def __init__(self):
        #self.ldr = re.compile(r"ldrb\s+(r[0-9]+), \[(r[0-9]+|sp), (r[0-9]+|#[0-9]+)\]")
        self.ldr = re.compile(r"(ldrb?)\s+(r[0-9]+),\s*(\.L[0-9]+(\+[0-9]+)*)")

        self.all = [self.ldr]
    def _validate(self, groups):
        return True
    def _get_all(self):
        return self.all
    def _get_fix(self, groups, ptvalmarkers):
        inst = groups[0][0]
        rd = groups[0][1]
        r1 = groups[0][2]
        rt = self.get_random_reg()
        tv = self.get_temp_var()
        inst_list = []
        prio = 1
        used_regs = [rd,r1]
        
        rp1 = PickRegister().pick_not(used_regs,1)[0]
        r1X = lambda x: x
        #if r1.startswith('.L'):
        #    r1X = lambda x: x
        #else:
        #    r1X = lambda x: '[' + x + ']'

        if ptvalmarkers.find('L') >= 0 or ptvalmarkers.find('S') >= 0:
            
            #it was confirmed by experiment that using str after str after ldr
            #is stronger than ldr after ldr
            if ptvalmarkers.find('Op1Op2') >= 0:
                inst_list += [
                    #'movs {}, {}'.format(rd, rt),
                    "push {{{}}}".format(rt),
                    "pop {{{}}}".format(rd),
                    
            #   inst_list = ["ldr {}, =T32".format(rd),
            #          "str {}, [{}]".format(rd, rd), 
                    "{} {}, {}".format(inst, rd, r1X(r1))]
            else:
                inst_list += [
                    "push {{{}}}".format(rt),
                    "pop {{{}}}".format(rt),
                    
            #   inst_list = ["ldr {}, =T32".format(rd),
            #          "str {}, [{}]".format(rd, rd), 
                    "{} {}, {}".format(inst, rd, r1X(r1))]
            
            prio = 30
        else:
            # pointers can't leak information, but this is 
            # here for completeness
            inst_list = ["movs {}, {}".format(rt, rt), 
                 "{} {}, {}".format(inst, rd, r1X(r1))]

        return (inst_list, prio)
    def _get_slidingw_size(self):
        return 1
    def __str__(self):
        return "LDRMatcherARM2"
    def get_priority(self):
        return 1

class LDRMatcherARM1(ARMMatcher):
    def __init__(self):
        #self.ldr = re.compile(r"ldrb\s+(r[0-9]+), \[(r[0-9]+|sp), (r[0-9]+|#[0-9]+)\]")
        self.ldr = re.compile(r"(ldrb?)\s+(r[0-9]+),\s*\[\s*(r[0-9]+|sp)\s*\]")

        self.all = [self.ldr]
    def _validate(self, groups):
        return True
    def _get_all(self):
        return self.all
    def _get_fix(self, groups, ptvalmarkers):
        inst = groups[0][0]
        rd = groups[0][1]
        r1 = groups[0][2]
        rt = self.get_random_reg()
        tv = self.get_temp_var()
        inst_list = []
        prio = 1
        used_regs = [rd,r1]
        
        rp1 = PickRegister().pick_not(used_regs,1)[0]
        r1X = lambda x: x
        #if r1.startswith('.L'):
        #    r1X = lambda x: x
        #else:
        #    r1X = lambda x: '[' + x + ']'

        if ptvalmarkers.find('L') >= 0 or ptvalmarkers.find('S') >= 0:
            
            #it was confirmed by experiment that using str after str after ldr
            #is stronger than ldr after ldr
            if rd not in set([r1]):
                if ptvalmarkers.find('Op1Op2') >= 0:
                    inst_list += [
                        #'movs {}, {}'.format(rd, rt),
                        "push {{{}}}".format(rt),
                        "pop {{{}}}".format(rd),
                        
            #   inst_list = ["ldr {}, =T32".format(rd),
            #          "str {}, [{}]".format(rd, rd), 
                        "{} {}, [{}]".format(inst, rd, r1X(r1))]
                else:
                    inst_list += [
                        "push {{{}}}".format(rt),
                        "pop {{{}}}".format(rt),
                        
            #   inst_list = ["ldr {}, =T32".format(rd),
            #          "str {}, [{}]".format(rd, rd), 
                        "{} {}, [{}]".format(inst, rd, r1X(r1))]
            else:
                inst_list = [
                        #"push {{{}}}".format(rp1),
                        #"ldr {}, =T32".format(rp1),
                        #"str {}, [{}]".format(rp1, rp1), 
                        "push {{{}}}".format(rt),
                        "pop {{{}}}".format(rt),
                        "{} {}, [{}]".format(inst, rd, r1X(r1)),
                        #"pop {{{}}}".format(rp1)
                        ]
            
            prio = 30
        elif ptvalmarkers.find('E') >=0:
            inst_list = ["ror {}, {}".format(rt, rt), 
                     "{} {}, [{}]".format(inst, rd, r1X(r1))]

            prio = 30
        else:
            if rd not in set([r1]):
                inst_list = ["movs {}, {}".format(rd, rt),
                     "{} {}, [{}]".format(inst, rd, r1X(r1))]
            else:
                # pointers can't leak information, but this is 
                # here for completeness
                inst_list = ["movs {}, {}".format(rt, rt), 
                     "{} {}, [{}]".format(inst, rd, r1X(r1))]
            prio = 30
        return (inst_list, prio)
    def _get_slidingw_size(self):
        return 1
    def __str__(self):
        return "LDRMatcherARM1"
    def get_priority(self):
        return 1

class LDRMatcherARM(ARMMatcher):
    def __init__(self):
        #self.ldr = re.compile(r"ldrb\s+(r[0-9]+), \[(r[0-9]+|sp), (r[0-9]+|#[0-9]+)\]")
        self.ldr = re.compile(r"(ldrb?)\s+(r[0-9]+),\s*\[\s*(r[0-9]+|sp)\s*,\s*(r[0-9]+|#(0x)?[0-9A-F]+)\s*\]")

        self.all = [self.ldr]
    def _validate(self, groups):
        return True
    def _get_all(self):
        return self.all
    def _get_fix(self, groups, ptvalmarkers):
        inst = groups[0][0]
        rd = groups[0][1]
        r1 = groups[0][2]
        r2 = groups[0][3]
        rt = self.get_random_reg()
        tv = self.get_temp_var()
        inst_list = []
        prio = 1
        used_regs = [rd,r1]
        
        if r2.startswith('r'):
            used_regs.append(r2)
        
        rp2 = PickRegister().pick_not(used_regs,1)[0]
        
        if ptvalmarkers.find('L') >= 0 or ptvalmarkers.find('S') >=0:
            
            #it was confirmed by experiment that using str after str after ldr
            #is stronger than ldr after ldr 
            if rd not in set([r1,r2]):
                if ptvalmarkers.find('Op1Op2') >= 0:
                    inst_list += [
                        #'movs {}, {}'.format(rd, rt),
                        "push {{{}}}".format(rt),
                        "pop {{{}}}".format(rd),
                        
            #   inst_list = ["ldr {}, =T32".format(rd),
            #          "str {}, [{}]".format(rd, rd), 
                        "{} {}, [{}, {}]".format(inst, rd, r1, r2)]
                else:
                    inst_list += [
                        "push {{{}}}".format(rt),
                        "pop {{{}}}".format(rt),
                        
            #   inst_list = ["ldr {}, =T32".format(rd),
            #          "str {}, [{}]".format(rd, rd), 
                        "{} {}, [{}, {}]".format(inst, rd, r1, r2)]
            else:
                if ptvalmarkers.find('Op1Op2') >= 0:
                    inst_list += [
                            "push {{{}}}".format(rp2),
                            "movs {}, {}".format(rp2, rd),
                            "push {{{}}}".format(rt),
                            "pop {{{}}}".format(rt)]
                            
                    if rd == r1:
                        inst_list += ["{} {}, [{}, {}]".format(inst, rd, rp2, r2)]
                    elif rd == r2:
                        inst_list += ["{} {}, [{}, {}]".format(inst, rd, r1, rp2)]
                    else:
                        raise Exception('Unsupported')
                    
                    inst_list += ["pop {{{}}}".format(rp2)]
                    
                else:
                    inst_list = [
                            "push {{{}}}".format(rt),
                            "pop {{{}}}".format(rt),
                            "{} {}, [{}, {}]".format(inst, rd, r1, r2)
                            ]
                
            prio = 30
        elif ptvalmarkers.find('E') >=0:
            inst_list += ["ror {}, {}".format(rt, rt)]
            if ptvalmarkers.find('Op1Op2') >=0:
                if rd not in set([r1,r2]):
                    inst_list += ["movs {}, {}".format(rd, rt),
                        "{} {}, [{}, {}]".format(inst, rd, r1, r2)]
                else:
                    # sbox leakage todo:
                    #if ptvalmarkers.find('Op1Op2') >= 0:
                    #    inst_list = ['push {}'] # pick re
                    inst_list += ["push {{{}}}".format(rp2)]
                    
                    inst_list += ["movs {}, {}".format(rp2, rd),
                                "movs {}, {}".format(rd, rt)]
        
                    if rd == r1:
                        inst_list += ["{} {}, [{}, {}]".format(inst, rd, rp2, r2)]
                    elif rd == r2:
                        inst_list += ["{} {}, [{}, {}]".format(inst, rd, r1, rp2)]
                    else:
                        raise Exception('Unsupported')
                    
                    inst_list += ["pop {{{}}}".format(rp2)]
            else:
                 inst_list += ["{} {}, [{}, {}]".format(inst, rd, r1, r2)]
        else:
            if ptvalmarkers.find('Op1Op2') >= 0:
                if rd not in set([r1,r2]):
                    inst_list += ["movs {}, {}".format(rd, rt),
                        "{} {}, [{}, {}]".format(inst, rd, r1, r2)]
                else:
                    # sbox leakage todo:
                    #if ptvalmarkers.find('Op1Op2') >= 0:
                    #    inst_list = ['push {}'] # pick re
                    inst_list += ["push {{{}}}".format(rp2)]
                    
                    inst_list += ["movs {}, {}".format(rp2, rd),
                                "movs {}, {}".format(rd, rt)]
        
                    if rd == r1:
                        inst_list += ["{} {}, [{}, {}]".format(inst, rd, rp2, r2)]
                    elif rd == r2:
                        inst_list += ["{} {}, [{}, {}]".format(inst, rd, r1, rp2)]
                    else:
                        raise Exception('Unsupported')
                    
                    inst_list += ["pop {{{}}}".format(rp2)]
            else:
                 inst_list += ["movs {}, {}".format(rt,rt),
                            "{} {}, [{}, {}]".format(inst, rd, r1, r2)]
                
        return (inst_list, prio)
    def _get_slidingw_size(self):
        return 1
    def __str__(self):
        return "LDRMatcherARM"
    def get_priority(self):
        return 1

class MOVOverwriteMatcherARM(ARMMatcher):
    def __init__(self):
        self.mov = re.compile(r"(movs?)\s+(r[0-9]+|ip),\s*(r[0-9]+|ip|#(0x)?[0-9A-F]+)")

        self.all = [self.mov]
    def _validate(self, groups):
        return True
    def _get_all(self):
        return self.all
    def _get_fix(self, groups, ptvalmarkers):

        inst = groups[0][0]
        r1 = groups[0][1]
        r2 = groups[0][2]
        rt = self.get_random_reg()
        insts_list = []
        prio = 1
        if ptvalmarkers.find('S') >= 0 or ptvalmarkers.find('L') >= 0:
            insts_list += [ "push {{{}}}".format(rt),
                           "pop {{{}}}".format(rt)]

            if ptvalmarkers.find('Op1Op2') >= 0:
                insts_list += ["{} {}, {}".format(inst, r1, rt)]
            prio = 30
        #elif ptvalmarkers.find('Op1') >= 0:
        #    insts_list += ["{} {}, {}".format(inst, r1, rt)]
        #    prio = 30
        elif ptvalmarkers.find('E') >= 0:
            insts_list += ['ror {}, {}'.format(rt, rt)]

            if ptvalmarkers.find('Op1Op2') >= 0:
                insts_list += ["{} {}, {}".format(inst, r1, rt)]
            prio = 30
        elif ptvalmarkers.find('Op1Op2') >= 0:
            insts_list += [ "{} {}, {}".format(inst, r1, rt) ]
            prio = 30
        else:
            insts_list += [ "movs {}, {}".format(rt, rt) ]

        insts_list +=[ "{} {}, {}".format(inst, r1, r2)]
        return (insts_list,prio)

    def _get_slidingw_size(self):
        return 1

    def __str__(self):
        return "MOVOverwriteMatcherARM"

    def get_priority(self):
        return 1
class ShiftRotMatherARM3(ARMMatcher):
    def __init__(self):
        self.ror = re.compile(r"(rors?|lsls?|lsrs?)\s+(r[0-9]+),\s*(r[0-9]+),\s*(r[0-9]+)")

        self.all = [self.ror]
    def _validate(self, groups):
        return True
    def _get_all(self):
        return self.all
    def _get_fix(self, groups, ptvalmarkers):

        inst = groups[0][0]
        r1 = groups[0][2]
        r2 = groups[0][3]
        rt = self.get_random_reg()
        insts_list = []
        prio = 1
        if ptvalmarkers.find('Op1I') >= 0:
            insts_list = [  
                            "eors {}, {}".format(r1, rt),
                            "{} {}, {}".format(inst, r1, r2),
                            "{} {}, {}".format(inst, rt, r2),
                            "eors {}, {}".format(r1, rt)
                        ]
            prio = 30
        else:
            insts_list = ["movs {}, {}".format(rt,rt),
                        "{} {}, {}".format(inst, r1, r2)]
        return (insts_list,prio)

    def _get_slidingw_size(self):
        return 1

    def __str__(self):
        return "RotationMatcherARM"

    def get_priority(self):
        return 1
class ShiftRotMatherARM(ARMMatcher):
    def __init__(self):
        self.ror = re.compile(r"(rors?|lsls?|lsrs?)\s+(r[0-9]+),\s*(r[0-9]+)")

        self.all = [self.ror]
    def _validate(self, groups):
        return True
    def _get_all(self):
        return self.all
    def _get_fix(self, groups, ptvalmarkers):

        inst = groups[0][0]
        r1 = groups[0][1]
        r2 = groups[0][2]
        rt = self.get_random_reg()
        insts_list = []
        prio = 1
        if ptvalmarkers.find('Op1I') >= 0:
            insts_list = [  
                            "eors {}, {}".format(r1, rt),
                            "{} {}, {}".format(inst, r1, r2),
                            "{} {}, {}".format(inst, rt, r2),
                            "eors {}, {}".format(r1, rt)
                        ]
            prio = 30
        else:
            insts_list = ["movs {}, {}".format(rt,rt),
                        "{} {}, {}".format(inst, r1, r2)]
        return (insts_list,prio)

    def _get_slidingw_size(self):
        return 1

    def __str__(self):
        return "RotationMatcherARM"

    def get_priority(self):
        return 1
class PickRegister:
    REG_LIST_LOW = ['r0','r1','r2','r3','r4','r5','r6'
                    #,'r7' # used for random value
                    ]
    
    def pick_not(self, reg, num):
        return list(set(self.REG_LIST_LOW) - set(reg))[:num]


class STRMatcherARM1(ARMMatcher):
    def __init__(self):
        self.str = re.compile(r"(strb?)\s+(r[0-9]+),\s*\[\s*(r[0-9]+|sp)\s*\]")

        self.all = [self.str]
    def _validate(self, groups):
        return True
    def _get_all(self):
        return self.all
    def _get_fix(self, groups, ptvalmarkers):
        inst = groups[0][0]
        rd = groups[0][1]
        r1 = groups[0][2]
        rt = self.get_random_reg()
        tv = self.get_temp_var()
        inst_list = []
        prio = 1
        rp1 = PickRegister().pick_not([rd,r1,rt],3)
        rp3 = rp1[2]
        rp2 = rp1[1]
        rp1 = rp1[0]
        

        if ptvalmarkers.find('S') >= 0 or ptvalmarkers.find('L') >= 0:
            offset = 0
            inst_list += [
                    #"push {r6}",
                    #"ldr r6, ={}".format(tv),
                    #"str r7, [r6]",
                    #"ldr r6, [r6]",
                    #"pop {r6}",
                    "push {{{}}}".format(rt),
                    "pop {{{}}}".format(rt)]
            
            if ptvalmarkers.find('Op1Op2')>=0:
                # write temp val to clear state + remove 
                # dependence on whatever that is in memory
                inst_list += ["{} {}, [{}]".format(inst, rt, r1)]
            
            inst_list += ["{} {}, [{}]".format(inst, rd, r1)]
            prio = 30
        elif ptvalmarkers.find('E') >= 0:
            inst_list = ["ror {}, {}".format(rt, rt),
                     "{} {}, [{}]".format(inst, rd, r1)]
            prio = 20
        elif ptvalmarkers.find('Op1Op2')>= 0:
            inst_list = ["{} {}, [{}]".format(inst, rt, r1),
                     "{} {}, [{}]".format(inst, rd, r1)]
            prio = 20
        else:
            inst_list = ["movs {}, {}".format(rt, rt),
                     "{} {}, [{}]".format(inst, rd, r1)]
        return (inst_list, prio)
    def _get_slidingw_size(self):
        return 1
    def __str__(self):
        return "STRMatcherARM1"
    def get_priority(self):
        return 1
    
# add strb rX, [rK], ldrb str ldr
class STRMatcherARM(ARMMatcher):
    def __init__(self):
        self.str = re.compile(r"(strb?)\s+(r[0-9]+),\s*\[\s*(r[0-9]+|sp)\s*,\s*(#(0x)?[0-9A-F]+|r[0-9]+)\s*\]")

        self.all = [self.str]
    def _validate(self, groups):
        return True
    def _get_all(self):
        return self.all
    def _get_fix(self, groups, ptvalmarkers):
        inst = groups[0][0]
        rd = groups[0][1]
        r1 = groups[0][2]
        offset = groups[0][3]
        rt = self.get_random_reg()
        tv = self.get_temp_var()
        inst_list = []
        prio = 1
        rp1 = PickRegister().pick_not([rd,r1,rt],3)
        rp3 = rp1[2]
        rp2 = rp1[1]
        rp1 = rp1[0]
        

#        if ptvalmarkers.find('S') >= 0:
#            inst_list = []
#            inst_list += [
#                    "{} {}, [{}, {}]".format(inst, rt, r1, offset),
#                    "{} {}, [{}, {}]".format(inst, rd, r1, offset)
#                    ]
#            prio = 30
        if ptvalmarkers.find('L') >=0 or ptvalmarkers.find('S') >= 0:
            inst_list = []
            inst_list += [
                    #"push {r6}",
                    #"ldr r6, ={}".format(tv),
                    #"str r7, [r6]",
                    #"ldr r6, [r6]",
                    #"pop {r6}",
                    "push {{{}}}".format(rt),
                    "pop {{{}}}".format(rt)]
            if ptvalmarkers.find('Op1Op2')>=0:
                # write temp val to clear state + remove 
                # dependence on whatever that is in memory
                inst_list += ["{} {}, [{}, {}]".format(inst, rt, r1, offset)]
            
            inst_list += ["{} {}, [{}, {}]".format(inst, rd, r1, offset)]
            
            prio = 30
        elif ptvalmarkers.find('Op1Op2')>= 0:
            inst_list = ["{} {}, [{}, {}]".format(inst, rt, r1, offset),
                     "{} {}, [{}, {}]".format(inst, rd, r1, offset)]
            prio = 20
        else:
            inst_list = ["movs {}, {}".format(rt, rt),
                     "{} {}, [{}, {}]".format(inst, rd, r1, offset)]
        return (inst_list, prio)
    def _get_slidingw_size(self):
        return 1
    def __str__(self):
        return "STRMatcherARM"
    def get_priority(self):
        return 1

class EORSClassMatcherARM(ARMMatcher):
    def __init__(self):
        self.any = re.compile(r"(lsls?|lsrs?|muls?|bics?|ands?|cpy|rors?|adds?|subs?|eors?|orrs?)\s+(r[0-9]+|ip)\s*,\s*(.*)")

        self.all = [self.any]
    def _validate(self, groups):
        return True
    def _get_all(self):
        return self.all
    def _get_fix(self, groups, ptvalmarkers):
        rt = self.get_random_reg()
        inst_list = []
        inst = groups[0][0]
        if ptvalmarkers.find('S') >= 0 or ptvalmarkers.find('L') >= 0:
            inst_list = ["push {{{}}}".format(rt),
                    "pop {{{}}}".format(rt)]
        elif ptvalmarkers.find('E') >= 0:
            inst_list = ["ror {}, {}".format(rt, rt)]
        else:
            inst_list = ["movs {}, {}".format(rt,rt)]
        inst_list += ["{} {}, {}".format(groups[0][0], groups[0][1], groups[0][2])]
        return (inst_list,5)
    def _get_slidingw_size(self):
        return 1
    def __str__(self):
        return "EORSClassMatcherARM"

    def get_priority(self):
        return -9999

class ANYMatcherARM(ARMMatcher):
    def __init__(self):
        self.any = re.compile(r"(.*)")
        self.all = [self.any]
    def _validate(self, groups):
        return True
    def _get_all(self):
        return self.all
    def _get_fix(self, groups, ptvalmarkers):
        rt = self.get_random_reg()
        inst_list = ["movs {}, {}".format(rt,rt),
                    "{}".format(groups[0][0])]
        return (inst_list,-9999)
    def _get_slidingw_size(self):
        return 1
    def __str__(self):
        return "ANYMatcherARM"

    def get_priority(self):
        return -9999

