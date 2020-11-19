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

class ARMMode(DefaultMode):
    ret = re.compile(r"bx\s+lr|pop\s+{[r0-9, ]*pc}|pop\s+pc")
    call = re.compile(r"bl\s+([_a-zA-Z0-9]+)")
    jmp_b = re.compile(r"b\s+(\.[_a-zA-Z0-9]+)")
    jmp_ble = re.compile(r"ble\s+(\.[_a-zA-Z0-9]+)")
    jmp_bls = re.compile(r"bls\s+(\.[_a-zA-Z0-9]+)")
    jmp_bgt = re.compile(r"bgt\s+(\.[_a-zA-Z0-9]+)")
    jmp_bne = re.compile(r"bne\s+(\.[_a-zA-Z0-9]+)")
    jml_bhi = re.compile(r"bhi\s+(\.[_a-zA-Z0-9]+)")
    jumps = [jmp_b, jmp_ble, jmp_bls, jmp_bne, jmp_bgt, jml_bhi]
    oper = re.compile(r"(ldr|ldrb|str|strb|movs|movb|ror|rors|lsls|lsl|eors|eor|orrs|bl|bx|push|pop|asrs|asr|and|ands|cmp|b)")

    def strip_comments(self, line):
        return line.split("@")[0]
    def __str__(self):
        return "ARM"
    def get_op(self, line: str):
        res = self.oper.match(line.split(' ')[0])
        if res:
            return res[1]
        return None

