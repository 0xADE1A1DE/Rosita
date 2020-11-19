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
from bisect import bisect_right
from bisect import bisect_left
from subprocess import check_output
from os import listdir
from os.path import isfile, join

"""
Read symbols, relocs ..etc directly from the binary
"""
class BinInfo:
    def __init__(self, exec, objdump="objdump", readelf="readelf"):
        self.output = check_output([objdump, '-d', exec]).decode('utf-8').split('\n')
        self.r_bl = re.compile(r" ([0-9a-f]+):\t[0-9a-f ]+\tbl\t([0-9a-f]+) <([a-zA-Z0-9_]+)>")
        self.r_line = re.compile(r"(\S+)\s+(\d+)\s+0x([0-9a-f]+).*")
        
        self.readelfout = check_output([readelf, '--debug-dump=decodedline', exec]).decode('utf-8').split('\n')
        self.linemap = {}
        fd=open('dump.txt','w')
        fd.write("\n".join(self.readelfout))

        for sl in self.readelfout:
            ret = self.r_line.match(sl)
            if ret:
                func_addr = int(ret.groups()[2], 16)
                f_line = int(ret.groups()[1])
                f_name = ret.groups()[0]
                self.linemap[func_addr] = (f_name, f_line)
            #else:
            #    print(sl)
        self.sortedlinemap = sorted(self.linemap.keys())
        
    def find_func_called_from(self, addrs):
        for sl in self.output:
            ret = self.r_bl.match(sl)
            if ret:
                func_addr = int(ret.groups()[0],16)
                if addrs == func_addr:
                    return (ret.groups()[1],ret.groups()[2])
        return None
    
    def find_source_line_at(self, addrs):
        i = bisect_right(self.sortedlinemap, addrs)
        #for f in self.sortedlinemap:
        #    print(str(f), addrs)
        #print(self.sortedlinemap[i-1], addrs)
        return self.linemap[self.sortedlinemap[i-1]]
    
