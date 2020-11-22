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

class CFG:
    cfg_leak_mark = "*"
    cfg_keep_orginal = False
    cfg_mod_line_end_marker = "\t\t\t@ edited"

    cwd = ""
    
    ELMO = os.getenv('ELMO_DIR')
    if ELMO is None:
        print('Set $ELMO_DIR!')
        exit(-1)

    TOOLCHAIN = os.getenv('TOOLCHAIN_DIR')
    if TOOLCHAIN is None:
        print('Set $TOOLCHAIN_DIR!')
        exit(-1)

    elmo_asm_output = ELMO+"/test/output/asmoutput/asmtrace00001.txt"
    elmo_tvalues = ELMO+"/test/output/fixedvsrandomtstatistics.txt"
    elmo_ptvalues = ELMO+"/test/output/fixedvsrandompropttest.txt"
    elmo_cwd = ELMO+"/test"
    elmo_command = ELMO+"/elmo"

    cc = TOOLCHAIN+"/arm-none-eabi-gcc"
    asm = TOOLCHAIN+"/arm-none-eabi-as"
    objdump = TOOLCHAIN+"/arm-none-eabi-objdump"
    objcopy = TOOLCHAIN+"/arm-none-eabi-objcopy"
    readelf = TOOLCHAIN+"/arm-none-eabi-readelf"
    ld = TOOLCHAIN+"/arm-none-eabi-gcc"

    def __init__(self):
        if os.getenv("ASM_EDITOR_CWD"):
            self.cwd = os.getenv("ASM_EDITOR_CWD")
            if not os.path.exists(self.cwd):
                raise FileNotFoundError(self.cwd)
            self.elmo_tvalues = self.cwd + "/../ttest/ttesttestvectors.txt"
            self.elmo_asm_output = self.cwd + "/../asmoutput/asmtrace00001.txt"

__g_cfg_inst = CFG()

def get_config():
    return __g_cfg_inst
