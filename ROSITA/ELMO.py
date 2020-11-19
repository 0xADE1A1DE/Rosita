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

import os
import re
from subprocess import check_call

import Config
import DeviceContext
import CycleCounter
import InstBuilder
import ELMOVisitor
import ASMParser
import ARMParserMode

class CoverageModelException(BaseException):
    pass
class Interop:
    def run(self, binfile):
        check_call([Config.CFG().elmo_command, binfile], cwd=Config.CFG().elmo_cwd)

class ExecutionTracer:
    asm_trace_lines = None
    asm_inst_lines = None
    def _get_debug_info(self, elmo_asm_line):
        ret = self.ib.build_from_elmoout(elmo_asm_line)
        if ret:
            return self.bin_info.find_source_line_at(int(ret.get_addr(), 16))
        return None
    def find_inst_idx(self, insts, inst):
        idx = 0
        for z in insts:
            if z == inst:
                return idx
            idx += 1
        return None
    def clean_lines(self, lilist):
        i = len(lilist) -1
        while i>=0:
            lilist[i] = lilist[i].strip()
            i -= 1
    def run(self, visitor, trigs):               
        # match starttrigger point with binary's start
        addrs = -1
        idx = 0
        exec_cycles = 0
        for line in self.asm_trace_lines:
            if self.ib.isstarttrig(line):
                gg = self.ib.build_from_elmoout(line)
                addrs = int(gg.get_addr(), 16)
                idx +=1
                break
            idx += 1
        if addrs == -1:
            raise CoverageModelException()
        # get func addrs and name
        #faddrs, fname = self.bin_info.find_func_called_from(addrs)
        asm_trace_idx = idx
        
        # get corresponding function from assembly file lines

        asm_inst_idx = -1
        # fname = ":"
        # for trig in trigs:
        #     idx = 0
        #     for line in trig.get_lines():
        #         func_name = line.get_func_name()
        #         print(line)
        #         if func_name == fname:
        #             asm_inst_idx = idx
        #             self.asm_inst_lines = trig.get_lines()
        #             break;
        #         idx += 1
        asm_inst_idx = 2
        self.asm_inst_lines = trigs[0].get_lines()
        if asm_inst_idx == -1:
            raise CoverageModelException()
        i = asm_trace_idx  
        j = asm_inst_idx - 2
        #cycles_left = 0

        for tlin in self.tval_lines:
            tlin = tlin.strip()
        for tlin in self.asm_trace_lines:
            tlin = tlin.strip()
            
        stack = []
        while i < len(self.asm_trace_lines) -1:
            if self.ib.iscall(self.asm_trace_lines[i]):
                call_addr = self.ib.build_from_elmoout(self.asm_trace_lines[i]).get_addr()
                # get function name and addrs from the binary
                faddrs, fname = self.bin_info.find_func_called_from(int(call_addr, 16))
                # if endtrigger is reached stop

                if fname == 'endtrigger':
                    break
                if ASMParser.ASMFuncDefs.islibrary(fname):
                    stack.append((j-1,self.asm_inst_lines))
                    # set new source
                    asmfile = ASMParser.ASMFile("",ARMParserMode.ARMMode())
                    self.asm_inst_lines = [ASMParser.ASMInst("",0,asmfile)]*1024
                    # jump to function start
                    j = 0
                    # elmo outputs ttwo lines per func
                    i += 1 
                else: 
                    # get fsunction source
                    func = self.fm.get()[fname]
                    stack.append((j,self.asm_inst_lines))
                    # set new source
                    self.asm_inst_lines = func.lines 
                    # jump to function start
                    j = func.start - 1
                    # elmo outputs ttwo lines per func
                    i += 1 
            
            # the source inst list also includes labels within it, ignore them
            while j < len(self.asm_inst_lines) and self.asm_inst_lines[j].is_label():
                j += 1
            
            tval = float(self.tval_lines[i])
            visitor.on_visit(self, exec_cycles, tval, self.ptval_lines, self.asm_inst_lines, j, self.asm_trace_lines, i)

            # check for jumps
            jmploc = self.ib.isjump(self.asm_trace_lines[i], self.asm_trace_lines[i+1])
            
            # cycle accuracy is turned off in ELMO because of discrepancies
            # http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0432c/CHDCICDF.html
            
            if jmploc:
                file = self.asm_inst_lines[j].get_file()
                jump_name = self.asm_inst_lines[j].get_jump_name()
                
                inst = self.labels[file][jump_name]
                # jump
                temp = self.find_inst_idx(self.insts_by_file[file], inst)
                # assuming that the next line is not a jump too
                next_inst = self.insts_by_file[file][temp+1]
                # -1 is compensation for j+=1 below, which would happen every loop
                j = self.find_inst_idx(self.asm_inst_lines, next_inst) - 1


            # remove multiple instructions after push and pop (elmo adds these)
            # to show individual push and pop operations
            if self.ib.ispush(self.asm_trace_lines[i]):
                exec_cycles += 2
                if self.asm_trace_lines[i+1].startswith('r'):
                    i += 1
                    while self.asm_trace_lines[i].startswith('r'):
                        i += 1
                        exec_cycles += 2
                    # recover last line
                    i -= 2 
                else:
                    j += 1
            elif self.ib.ispop(self.asm_trace_lines[i]):
                exec_cycles += 2
                if self.asm_trace_lines[i+1].startswith('r'):
                    i += 1
                    while self.asm_trace_lines[i].startswith('r'):
                        i += 1
                        exec_cycles += 2
                    # recover last line 
                    i -= 2 
                else:
                    j += 1
            else:
                if not self.asm_trace_lines[i].startswith('r'):
                    inst = self.ib.build_from_elmoout(self.asm_trace_lines[i])
                    if inst is None:
                        print(self.asm_trace_lines[i])
                    # calc executed cycles
                    exec_cycles += self.dc.get_cyclecounter().get_cycle_count(inst)

                j += 1

            if len(stack) > 0:
                F = self.asm_trace_lines[i+1].find("pop") != -1 and self.asm_trace_lines[i+1].find("pc") != -1
                if self.asm_inst_lines[j].is_func_end() or F:
                    self.asm_inst_lines = stack[-1][1]
                    j = stack[-1][0]
                    stack.pop(-1)

            i += 1
            
    def __init__(self, dc: DeviceContext, elmo_asm_output, elmo_tvalues, elmo_ptvalues, bin_info, fm, trigs, labels, insts_by_file, xprint=False):
        fasm = open(elmo_asm_output, "r")
        ftvals = open(elmo_tvalues, "r")
        fptvals = open(elmo_ptvalues, "r")
        tvals = list(ftvals.readlines())
        self.asm_trace_lines = list(fasm.readlines())
        fasm.close()
        ftvals.close()
        self.tval_lines = tvals
        self.ptval_lines = [x.strip().split(' ') for x in fptvals.readlines()]
        self.ptval_lines = list(map(lambda x:list(map(lambda s: float(s), x)),self.ptval_lines))
        self.insts_by_file = insts_by_file
        fptvals.close()
        self.clean_lines(self.tval_lines)
        self.clean_lines(self.asm_trace_lines)
        self.bin_info = bin_info
        self.fm = fm
        self.labels = labels
        self.dc = dc 
        self.ib = dc.get_instbuilder()
        if xprint:
            self.run(ELMOVisitor.Printer(), trigs)
