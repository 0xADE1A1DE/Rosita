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

import numpy as np
import mmap
import struct
import os

class Cache:
    def __init__(self, file_tmpl, base_path, limit):
        self.base_path = base_path
        self.limit = limit
        self.file_tmpl = file_tmpl

        self.mmaps = {}

        self.last_mmap = None
    def get_mmap(self, file_id):
        if file_id in self.mmaps:
            return self.mmaps[file_id]
        else:
            if len(self.mmaps) > self.limit:
                with open(self.file_tmpl % (self.base_path, file_id), "rb") as f:
                    mm = mmap.mmap(f.fileno(), 0, prot=mmap.PROT_READ)
                    self.last_mmap = mm
                    return mm

            with open(self.file_tmpl % (self.base_path, file_id), "rb") as f:
                mm = mmap.mmap(f.fileno(), 0, prot=mmap.PROT_READ)
                self.mmaps[file_id] = mm

            return mm
    def put_mmap(self, mmap):
        if self.last_mmap == mmap:
            mmap.close()

# do not use to do full range evaluation(all tests vs. all instructions), take values from 
# ELMO's traces
class ElmoBinInsts:
    FILE_TMPL = "%s/dump%05d.dmp"

    def __init__(self, dumps_path, test_count):
        self.base = dumps_path
        self.test_count = test_count
        self.inst_count = None
        self.mcache = Cache(ElmoBinInsts.FILE_TMPL, dumps_path, 4000)
    def get_inst_count(self):
        if self.inst_count is None:
            info = os.stat(ElmoBinInsts.FILE_TMPL %(self.base,1))
            self.inst_count = int(info.st_size / (3 * 4))
        return self.inst_count
    def get_insts(self, idx_start, idx_end):
        mat = np.empty((self.test_count, idx_end- idx_start,  3), dtype=np.uint32)

        temp = np.empty((idx_end - idx_start)* 3, dtype = np.uint32)
        for idx in range(1, self.test_count +1):
            with open(ElmoBinInsts.FILE_TMPL % (self.base, idx), "rb") as f:
                mm = mmap.mmap(f.fileno(), 0, prot=mmap.PROT_READ)
            #mm = self.mcache.get_mmap(idx)
                INST_LEN = 4 * 3 # sizeof(3 * uint32_t)
                mm.seek(INST_LEN*idx_start)
                dbytes = mm.read(INST_LEN*(idx_end - idx_start))
                temp = struct.unpack(str((idx_end - idx_start)*3)+'I', dbytes)
                mat[idx-1] = np.reshape(temp,(idx_end - idx_start,3))
                mm.close()
            #self.mcache.put_mmap(mm)
        return mat

# use Elmo traces instead of running full range evaluations for all instructions
class ElmoBinTraces:
    FILE_TMPL = "%s/trace%05d.trc"   
    def __init__(self, traces_path, test_count):
        self.test_count = test_count
        self.trace_count = None
        self.base = traces_path
    def get_trace_count(self):
        if self.trace_count is None:
            info = os.stat(ElmoBinTraces.FILE_TMPL %(self.base,1))
            self.trace_count = int(info.st_size / 8)
        return self.trace_count
    def get_traces(self, idx_start, idx_end):
        mat = np.empty((self.test_count, idx_end - idx_start), dtype=np.float64)
        for idx in range(1, self.test_count +1):
            with open(ElmoBinTraces.FILE_TMPL % (self.base, idx),"rb") as f:
                mm = mmap.mmap(f.fileno(), 0, prot=mmap.PROT_READ)
                mm.seek(8*idx_start)
                dbytes = mm.read(8*(idx_end - idx_start))
                mat[idx-1] = struct.unpack(str(idx_end - idx_start)+'d', dbytes)
                mm.close()
        return mat

e = ElmoBinTraces("/home/madura/wrk/ELMO-master/test/output/traces", 10000)

# mat = (tests, traces)


def calc_tvals(mat):
    tests = np.shape(mat)[0]
    traces = np.shape(mat)[1]

    tvals = np.empty((traces), dtype=np.float64)
    N = tests//2

    for trace in range(0, traces):
        A = mat[:,trace]
        F = A[0:N]
        R = A[N:]
        d = np.sqrt(N)*(np.mean(F) - np.mean(R))
        v =(np.sqrt(np.var(F) + np.var(R)))

        # make sure d!=0 cases where v==0 are highlighted
        if (v == 0.0):
            v = 0.000000000000000000000001
        
        tvals[trace] = d/v
    return tvals
#t = time.time()
traces = e.get_trace_count()
x = e.get_traces(0, traces)
#print(calc_snr(x))
np.savetxt("/home/madura/wrk/ELMO-master/test/testvectorprograms/ttesttestvectors.txt", calc_tvals(x))
#print(time.time() -t)


