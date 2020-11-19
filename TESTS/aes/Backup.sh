#! /bin/bash

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

ELMO_PATH=$HOME/wrk/ELMO-master/
NEW_PATH=$HOME/wrk/backup/`date +%Y%m%d-%H%M%S`
ANALYZE_PATH=$HOME/wrk/elmo-trace-analyze/analyze 
mkdir -p $NEW_PATH/src/
mkdir -p $NEW_PATH/ttest/
mkdir -p $NEW_PATH/asmoutput/
cp $ELMO_PATH/test/output/asmoutput/asmtrace00001.txt $NEW_PATH/asmoutput
cp $ELMO_PATH/test/testvectorprograms/ttesttestvectors.txt $NEW_PATH/ttest
cp ./final $NEW_PATH/src
cp ./final_old $NEW_PATH/src

cp ./*.c $NEW_PATH/src
cp ./*.s $NEW_PATH/src

cd $NEW_PATH
$ANALYZE_PATH

