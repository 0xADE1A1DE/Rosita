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


# link across files
class Mapper:
    def __init__(self):
        self.funcs = {}
        self.trigs = []
    def add_trigs(self, trigs):
        self.trigs.extend(trigs)
    def add_funcs(self, asmfuncs):
        for f in asmfuncs:
            self.add_func(f)
    def add_func(self, asmfunc):
        self.funcs[asmfunc.get_name()] = asmfunc
    def get(self):
        return self.funcs
    def link(self):
        for trig in self.trigs:
            trig.link_calls(self.funcs)
        for func in self.funcs.items():
            func[1].link_calls(self.funcs)

    
