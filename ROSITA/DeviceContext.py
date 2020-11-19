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

import InstBuilder
import CycleCounter


class DeviceContext:
    def get_cyclecounter(self) -> CycleCounter.CycleCounter:
        raise NotImplemented
    def get_instbuilder(self) -> InstBuilder.InstBuilder:
        raise NotImplemented


class ARMDeviceContext:
    ib = InstBuilder.ARMInstBuilder()
    cc = CycleCounter.ARMCycleCounter()

    def get_cyclecounter(self) -> CycleCounter.ARMCycleCounter:
        return self.cc
    def get_instbuilder(self) -> InstBuilder.ARMInstBuilder:
        return self.ib
