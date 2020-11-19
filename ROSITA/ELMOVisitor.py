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

import Config
import DeviceContext
import PropTvalAnalyzer
import CycleCounter
import InstBuilder
import sys

class LeakageModelVisitor:
    def on_visit(self, lm, exec_cycles, tval, ptvals, asm_inst_lines, asm_inst_idx, asm_trace_lines, asm_trace_idx):
        raise NotImplemented

class Printer(LeakageModelVisitor):
    PR_C_NORMAL = "\033[0;37;40m"
    PR_C_RED = "\033[1;31;40m"
    def __init__(self):
        self.ptvala = PropTvalAnalyzer.PropTvalAnalyzer()
    
    def on_visit(self, lm, exec_cycles, tval, ptvals, asm_inst_lines, asm_inst_idx, asm_trace_lines, asm_trace_idx):
        startmark = ""
        endmark = ""
        if abs(tval) > 4.5:
            if Config.get_config().cfg_leak_mark:
                startmark = Config.get_config().cfg_leak_mark
                endmark = ""
            else:
                startmark = self.PR_C_RED
                endmark = self.PR_C_NORMAL
        
        dbg = lm._get_debug_info(asm_trace_lines[asm_trace_idx])
        if dbg:
            dbg = str(dbg)
        else:
            dbg = ""

        #print(ptvals)
        #exit(-1)
        #print(asm_inst_lines[asm_inst_idx])
        #print(len(asm_inst_lines), asm_inst_idx)
        #print(asm_inst_lines[asm_inst_idx])

        if ptvals != []:
            print("%s"%(self.ptvala.get_markers(ptvals[asm_trace_idx])),"%2s"%startmark,"%15.4f" % tval, " %4d" % asm_inst_lines[asm_inst_idx].get_line_no(),
                " %-30s" % asm_inst_lines[asm_inst_idx].get_line().replace('\t', ' '),
                " %4d" % exec_cycles,
                " %4d" % asm_trace_idx, " %-50s"% asm_trace_lines[asm_trace_idx], " ", dbg ,endmark, sep="")
        else:
            print("XXX","%2s"%startmark,"%15.4f" % tval, " %4d" % asm_inst_lines[asm_inst_idx].get_line_no(),
                " %-30s" % asm_inst_lines[asm_inst_idx].get_line().replace('\t', ' '), 
                " %4d" % exec_cycles,
                " %4d" % asm_trace_idx, " %-50s"% asm_trace_lines[asm_trace_idx], " ", dbg ,endmark, sep="")
        

        
class InstMatcher(LeakageModelVisitor): 
    def __init__(self, matchers):
        self.matchers = matchers
        self.leakages = {}
        self.ptvala = PropTvalAnalyzer.PropTvalAnalyzer()
    def on_visit(self, lm, exec_cycles, tval, ptvals, asm_inst_lines, asm_inst_idx, asm_trace_lines, asm_trace_idx):
        # gather each leaky instruction so that we can operate on them later
        
        if abs(tval) > 4.5:
            tup = (asm_inst_lines[asm_inst_idx].get_file().file, asm_inst_lines[asm_inst_idx].get_line_no())
            self.leakages[tup] = (tval, asm_inst_lines, asm_inst_idx, self.ptvala.get_markers(ptvals[asm_trace_idx]))
            
    def match(self):
        rps_list = []
        for file, line_no in self.leakages.keys():
            tup = (file, line_no)
            tv, asm, idx, ptmarkers = self.leakages[tup]
            print ("lll ", idx)
            rep_list = []
            for m in self.matchers:
                r, prio = m.match(asm, idx, ptmarkers)
                if r:
                    rep_list.append((r, prio))

            rep_list = sorted(rep_list, key=lambda x:x[1])
            rep = rep_list[-1][0]
            if rep:
                rps_list.append(rep)

        return rps_list

class CycleAccurateTValEmitter(LeakageModelVisitor):
    def __init__(self, dc: DeviceContext.DeviceContext):
        self.cc = dc.get_cyclecounter() 
        self.ib = dc.get_instbuilder()
    def on_visit(self, lm, exec_cycles, tval, ptvals, asm_inst_lines, asm_inst_idx, asm_trace_lines, asm_trace_idx):
        inst = self.ib.build_from_elmoout(asm_trace_lines[asm_trace_idx])
        if inst is None:
            print(0)
        else:
            for i in range(0, self.cc.get_cycle_count(inst)):
                print(tval)
