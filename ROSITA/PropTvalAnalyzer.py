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

class PropTvalAnalyzer:
    def __init__(self):
        self.indexes = [    15, 19,  20,       21,    22,    23,  24]
        self.markers = ['Op1I','L', 'S', 'Op1Op2', 'Op1', 'Op2', 'E']
    def is_leaky(self, ptvals):
        for i in range(0, len(self.indexes)):
            if abs(ptvals[self.indexes[i]]) > 4.5:
                return True
        return False
    def get_markers(self, ptvals):
        markers = ""
        for i in range(0, len(self.indexes)):
            if abs(ptvals[self.indexes[i]]) > 4.5:
                markers += self.markers[i]
                markers += ' '
        return markers
