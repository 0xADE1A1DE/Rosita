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
import sys
import re

class NoImpl(BaseException):
    pass
class ASMFuncDefs:
    @staticmethod
    def islibrary(func):
        return func in set(['time','rand','srand',
            'randbyte','printbyte','endtrigger',
            'starttrigger','endprogram','memset',
            '__aeabi_idivmod','__aeabi_llsr','__aeabi_llsl'])
class DefaultTrigger:
    start = re.compile(r"[a-z]+\s+starttrigger")
    end = re.compile(r"[a-z]+\s+endtrigger")

class DefaultMode:
    func = re.compile(r"([_a-zA-Z][_a-zA-Z0-9\.]+):")
    label = re.compile(r"(\.[_a-zA-Z][_a-zA-Z0-9]+):")
    jumps = []
    def strip_comments(self, line):
        raise NoImpl
    def get_op(self, line):
        raise NoImpl

class ASMError(BaseException):
    pass

class ASMFile:
    def __init__(self, file, mode):
        self.file = file
        self.mode = mode
    def __str__(self):
        return self.mode.__str__() + " " + self.file
    def is_null(self):
        return self.file == ""

class ASMInst:
    def __init__(self, line, line_num, asmfile):
        self.content = line
        self.line_num = line_num
        self.asmfile = asmfile
    def __str__(self):
        return str(self.line_num) + ":" + self.content 
    def is_call(self):
        return self.asmfile.mode.call.match(self.content) != None
    def is_label(self):
        return self.asmfile.mode.label.match(self.content) != None
    def is_func(self):
        return self.asmfile.mode.func.match(self.content) != None
    def is_inst(self):
        return self.is_func() == False and self.is_label() == False
    def is_func_end(self):
        return self.asmfile.mode.ret.match(self.content) != None
    def get_label_name(self):
        ret = self.asmfile.mode.label.match(self.content)
        if ret:
            return ret[1]
        return None
    def get_jump_name(self):
        for jmp in self.asmfile.mode.jumps:
            ret = jmp.match(self.content)
            if ret:
                return ret.groups(1)[0]
        return None
    def get_func_name(self):
        ret = self.asmfile.mode.call.match(self.content)
        if ret:
            return ret[1]
        return None
    def get_op(self):
        return self.asmfile.mode.get_op(self.content)
    def get_line_no(self):
        return self.line_num
    def get_line(self):
        return self.content
    def get_file(self):
        return self.asmfile
    def __eq__(self, other):
        return self.line_num == other.line_num
class ASMLabel:
    def __init__(self, name, start_file, start_idx):
        self.name = name
        self.start_file = start_file
        self.start_idx = start_idx

class ASMFunction:
    def __init__(self, name, lines, start, end=-1):
        self.name = name
        self.start = start
        self.end = end
        self.calls = []
        self.linked_calls = []
        self.lines = lines
    #def add_line(self, line):
    #    self.lines.append(line)
    def add_func_call(self, func_name):
        self.calls.append(func_name)
    def set_end(self, end):
        self.end = end
    def __str__(self):
        tt= self.name +":" + str(self.start) +","+ str(self.end) +"\n" 
        for line in self.lines:
            tt += str(line)
            tt += "\n"
        return tt
    def get_lines(self):
        return self.lines
    def get_calls(self):
        return self.calls
    def get_name(self):
        return self.name
    def link_calls(self, name_map):
        self.linked_calls = []
        for call in self.calls:
            if call in name_map:
                self.linked_calls.append(name_map[call])
    def get_linked_calls(self):
        return self.linked_calls

class ASMTriggerCoverage:
    def __init__(self):
        self.lines = []
        self.calls = []
    def add_line(self, line):
        self.lines.append(line)
    def add_call(self, call):
        self.calls.append(call)
class ASMTrigger:
    def __init__(self, start, end=-1):
        self.start = start
        self.end = end
        self.call_from_trigger = []
        self.linked_calls = []
        self.lines = []
    def add_line(self, line):
        self.lines.append(line)
    def set_end(self, end):
        self.end = end
    def add_func_call(self, func_name):
        self.call_from_trigger.append(func_name)
    def link_calls(self, name_map):
        self.linked_calls = []
        for call in self.call_from_trigger:
            if call in name_map:
                self.linked_calls.append(name_map[call])
    def get_linked_calls(self):
        return self.linked_calls
    def _rec_get_calls(self, func, coverage):
        fcalls = func.get_linked_calls().copy()
        for line in func.get_lines():
            if line.is_call() and not ASMFuncDefs.islibrary(line.get_func_name()):
                try:
                    call = fcalls.pop(0)
                except:
                    print(line)
                    raise ASMError()
                coverage.add_call(call)
                self._rec_get_calls(call, coverage)          
            else:
                coverage.add_line(line)
    def get_calls(self):
        cov = ASMTriggerCoverage()
        fcalls = self.linked_calls.copy()
        for line in self.lines:
            cov.add_line(line)
            if line.is_call() and not ASMFuncDefs.islibrary(line.get_func_name()):
                try:
                    call = fcalls.pop(0)
                except:
                    print(line)
                    raise ASMError()
                cov.add_call(call)
                self._rec_get_calls(call, cov)
        return cov

    def get_lines(self):
        return self.lines

class ASMParser:
    def __init__(self, file, mode, trigger=DefaultTrigger()):
        self.lines = []
        self.funcs = []
        self.trigs = []
        self.labels = {}
        of = open(file,"r")
        line_num = 1
        current_func = None
        current_trig = None
        asmfile = ASMFile(file, mode)
        self.asmfile = asmfile
        for line in of.readlines():
            line = line.strip()
            line = mode.strip_comments(line)
            if len(line) > 0:
                if line.startswith("."):
                    label_groups = mode.label.match(line)
                    if label_groups:
                        asmline = ASMInst(line, line_num, asmfile)
                        self.labels[label_groups.group(1)] = asmline
                        self.lines.append(asmline)
                elif line.strip().startswith("#"):
                    pass
                else:
                    asmline = ASMInst(line, line_num, asmfile)
                    self.lines.append(asmline)

                    func_groups = mode.func.match(line)

                    trig_start = trigger.start.match(line)
                    trig_end = trigger.end.match(line)

                    # drop all labels that are found after a label, until a ret is found
                    if func_groups:
                        current_func = ASMFunction(func_groups.group(1), self.lines, len(self.lines))
                        self.funcs.append(current_func)
                        
                    #if func_found:
                    #    current_func.add_line(asmline)

                    #if ret_groups and func_found:
                    #    if current_func == None:
                    #        raise ASMError()
                    #    current_func.set_end(line_num)
                    #
                    #    current_func = None
                    #    func_found = False

                    #if call_groups and func_found:
                    #    fname = call_groups.group(1)
                    #    current_func.add_func_call(fname)
                    #    if current_trig:
                    #        current_trig.add_func_call(fname)

                    if trig_end:
                        current_trig.set_end(line_num)                    
                        self.trigs.append(current_trig)
                        current_trig = None

                    # placement is necessary since we need to add calls after
                    # starttrigger, not including starttrigger
                    if current_trig:
                        current_trig.add_line(asmline)

                    if trig_start:
                        current_trig = ASMTrigger(line_num)
            line_num +=1
        of.close()

    def get_funcs(self):
        return self.funcs
    def get_lines(self):
        return self.lines
    def get_trigs(self):
        return self.trigs
    def get_file(self):
        return self.asmfile
    def get_labels(self):
        return self.labels
    def print(self):
        for line in self.funcs:
            print(line)
