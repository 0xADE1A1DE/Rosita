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

from ASMParser import DefaultMode
import re

class X86Mode(DefaultMode):
    ret = re.compile(r"ret[q]?")
    call = re.compile(r"call[q]?\s+([_a-zA-Z0-9]+)")
    jumps = []
    def strip_comments(self, line):
        return line.split('#')[0]
    def __str__(self):
        return "X86"
    def get_op(self, line: str):
        return None

