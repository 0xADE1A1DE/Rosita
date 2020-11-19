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

class Replacement:
    def __init__(self, src_groups, src_length, src_lineno, rep_insts, file, matcher):
        self.src_groups = src_groups
        self.src_length = src_length
        self.src_lineno = src_lineno
        self.rep_insts = rep_insts
        self.file = file
        self.from_matcher = matcher
    def __str__(self):
        return self.from_matcher.__str__() +":"+ str(self.rep_insts)
    def __repr__(self):
        return self.__str__()
class Matcher:
    rot_val = 0
    def _get_slidingw_size(self):
        raise NotImplementedError()
    def _get_fix(self, list, ptvalmarkers):
        raise NotImplementedError()
    def match(self, insts, at_idx, ptmarkers) -> (Replacement, int):
        j = 0
        i = 0
        sw = self._get_slidingw_size()
    
        m_list =[]
        if not insts[at_idx].is_inst():
            print (insts[at_idx])
            raise RuntimeError()
        
        while i<sw and at_idx+j < len(insts):
            if insts[at_idx+j].is_inst():
                m = self._get_all()[i].match(insts[at_idx+j].content)
                #print(insts[at_idx+j].content, self.__str__(), self._get_all()[i])
                if m:
                    m_list.append(m.groups())
                else:
                    m_list = []
                    break
                i += 1 
            j += 1
        
        if m_list != [] and self._validate(m_list):
            insts_list, prio = self._get_fix(m_list, ptmarkers)
            who = self.__str__()
            if insts_list == None or insts_list == []:
                raise RuntimeError
            #print(insts[at_idx])
            print(insts[at_idx], ptmarkers, m_list, insts_list, who)
            return (Replacement(m_list, sw, insts[at_idx].get_line_no(),
                               insts_list,
                               insts[at_idx].asmfile, self), prio)
        return (None, -99999)
    def _validate(self, list):
        raise NotImplementedError()
    def _get_all(self):
        raise NotImplementedError()
    def _all_match(self, slidingw):
        idx=0
        m_list=[]
        for inst in slidingw:
            m = self._get_all()[idx].match(inst.content)
            if not m:
                return None
            else:
                m_list.append(m.groups())
            idx+=1
        return m_list
    def get_priority(self):
        raise NotImplementedError()
    def __str__(self):
        raise NotImplementedError()

