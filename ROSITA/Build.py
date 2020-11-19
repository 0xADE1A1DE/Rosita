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

import Utils
import CTemplate

from subprocess import call
from subprocess import Popen, PIPE
from os import listdir
from os.path import isfile, join, splitext, splitext
from os import devnull

import json
OPT_SINGLE = "-c"
OPT_ASM_OUT = "-S"
OPT_OBJ_OUT = "-o"

EXT_ASM = ".s"
EXT_OBJ = ".o"
class Builder():
    def __init__(self,src_config={},cc="",asm="",ld="",cxx="",cxxflags="",cflags="",lflags="",ldflags="",ldendflags="",cwd=".",objcopy="",buildfile=""):
        self.cc = cc
        self.cxx = cxx
        self.cflags = cflags
        self.cxxflags = cxxflags
        self.lflags = lflags
        self.ldflags = ldflags
        self.ldendflags = ldendflags
        self.asm_files = []
        self.cwd = cwd
        self.asm = asm
        self.ld = ld
        self.objcopy = objcopy
        self.c_files_preconfig = []

        if buildfile == "":
            buildfile = "build.json"
        buildjson = join(cwd, buildfile)
        if isfile(buildjson):
            A = json.loads(open(buildjson).read())
            self.c_tmpl_files = A['c_tmpl_files']
            self.c_files_preconfig = A['c_files']
            self.c_files = A['c_files']
            
            if src_config != {}:
                tc = open(join(cwd,'tracecount.txt'),'w')
                tc.write(src_config['NTRACES'])
                tc.close()
            
            for tf in self.c_tmpl_files:
                tc = splitext(join(cwd,tf))[0] + '_tmpl.c'
                if src_config == {}:
                    raise Exception('Empty source config')
                CTemplate.CTemplate(src_config, join(cwd,tf), tc)
                self.c_files.append(splitext(tf)[0] + '_tmpl.c')
            
            self.cflags = A['cflags']
            self.res_reg_files = A['reserve_register_in_files']
            self.res_reg_cflags = A['reserve_register_cflags']
            
            self.res_reg_cxxflags = A['reserve_register_cxxflags']
            self.cxxflags = A['cxxflags']
            self.ldflags = A['ldflags']
            self.cxx_files = A['cxx_files']
            self.obj_files = A['obj_files']
            self.asm_files = A['asm_files']
        else:
            raise FileNotFoundError(buildfile)
        
    def source_cfg_update(self, source_cfg):
        tc = open(join(self.cwd,'tracecount.txt'),'w')
        tc.write(source_cfg['NTRACES'])
        tc.close()
        
        for tf in self.c_tmpl_files:
            tc = splitext(join(self.cwd,tf))[0] + '_tmpl.c'
            if source_cfg == {}:
                raise Exception('Empty source config')
            CTemplate.CTemplate(source_cfg, join(self.cwd,tf), tc)

            
    def build_asm_for_tmpl(self):
        for tf in self.c_tmpl_files:
            tc = splitext(join(self.cwd,tf))[0] + '_tmpl.c'
            self._build_asm(tc)
            
    def build_asm(self):
        # todo: add logic to check modification time and 
        # not replace asm files that are modified after
        # c file mod time
        onlyfiles = list(self.c_files)
        onlyfiles.extend(self.cxx_files)

        for file in onlyfiles:
            self._build_asm(file)

    def build_obj(self):
        for file in self.get_asm_files():
            self._build_obj_from_asm(file)

    def get_asm_files(self, fullpath=False):
        onlyfiles = list(self.c_files)
        onlyfiles.extend(self.cxx_files)

        asmfiles = self._switchext(onlyfiles, EXT_ASM)
        asmfiles.extend(self.asm_files)

        return self._switchext(asmfiles,EXT_ASM,fullpath)
    def build_obj_from_sources(self):
        onlyfiles = list(self.c_files)
        onlyfiles.extend(self.cxx_files)

        for file in onlyfiles:
            if Utils.iscxxfile(file):
                self._run_command([self.cxx, self.cxxflags.replace('$file', file), self._obj_out(file) ])
            elif Utils.iscfile(file):
                self._run_command([self.cc, self.cflags.replace('$file', file), self._obj_out(file) ])

    def _build_asm(self,file):
        res_reg_cflag = ""
        res_reg_cxxflag = ""

        if file in set(self.res_reg_files):
            res_reg_cflag = self.res_reg_cflags
            res_reg_cxxflag = self.res_reg_cxxflags
        if Utils.iscxxfile(file):
            self._run_command([self.cxx, self.cxxflags.replace('$file', file), res_reg_cflag, self._asm_out(file) ])
            #self._run_command([self.cxx, self.cxxflags, OPT_SINGLE, file, self._obj_out(file) ])
        elif Utils.iscfile(file):
            self._run_command([self.cc, self.cflags.replace('$file', file), res_reg_cxxflag, self._asm_out(file) ])
            #self._run_command([self.cc, self.cflags, OPT_SINGLE, file, self._obj_out(file) ])

    def _build_obj_from_asm(self,file):
        if Utils.isasfile(file):
            self._run_command([self.asm, file, self._obj_out(file)])
            #self._run_command([self.cxx, self.cxxflags, OPT_SINGLE, file, self._obj_out(file) ])
    def _switchext(self, files, ext, fullpath=False):
        nfiles = []
        for file in files:
            if fullpath:
                nfiles.append(join(self.cwd, splitext(file)[0]) + ext)
            else:
                nfiles.append(splitext(file)[0] + ext)
        return nfiles
    def link(self, final):
        onlyfiles = list(self.c_files)
        onlyfiles.extend(self.cxx_files)
        onlyfiles.extend(self.asm_files)
        
        objs = list(self.obj_files)
        
        objs.extend(self._switchext(onlyfiles, EXT_OBJ))
        
        self._run_command([self.ld, self.ldflags.replace("$files", " ".join(objs)), OPT_OBJ_OUT, final])

    def makeflatbin(self, final):
        self._run_command([self.objcopy, "-Obinary", final, final+".bin"])

    def _run_command(self, command):
        args = []
        for cmd in command:
            args.extend(cmd.split())
        print(" ".join(args))
        #w = open('cmdout','w')
        #ret = call(args, cwd = self.cwd, stdout = w.fileno())
        p = Popen(args, cwd = self.cwd, stdin=PIPE, stdout=PIPE, stderr=PIPE)
        output, err = p.communicate(b"")
        
        if p.returncode != 0:
            print(output,err)
            raise OSError(command)
        #call(args, cwd=self.cwd)
    def _asm_out(self,file):
        return OPT_ASM_OUT
    def _obj_out(self,file):
        return OPT_OBJ_OUT + " " + file.split(".")[0]+'.o'
