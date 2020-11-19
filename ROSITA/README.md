READ ME
=======

Introduction
------------
Rosita is a code rewriting engine that mitigates power analysis based side-channels. Rosita
depends on a modified version of ELMO leakage emulator to detect the leakage sites. 
ELMO & Rosita only support ARM thumb instruction set currently. Rosita is run in
successive calls to two tools `rosita.py` and `emulatetraces.py`. `rosita.py` is used for analysing
and applying code rewrites while `evaluate.py` is used for evaluate the binary that results
from modified source files.

Running Rosita
--------------

1. Set following environment variables
   
	`ELMO_DIR` - Path to patched ELMO simulator directory 

	`TOOLCHAIN_DIR` - ARM Toolchain bin directory

2. Run Rosita by following commands (make sure the project directory has a build.json file)
	* Build project & run (for runs after initial build & fix use `-b --from` switches)
  
		`./emulatetraces -b /path/to/project/build.json`  
	
	* Run Rosita to see leakage points
  
		`./rosita -a /path/to/project/build.json`

	* Run Rosita and fix leakage points

		`./rosita -m /path/to/project/build.json`

	* Build project (from fixed assembler files) & run

		`./emulatetraces -b --from-asm /path/to/project/build.json`

Additional Functionalities
--------------------------

* Get cycle accurate t-value output

	`./rosita --emit-tvals /path/to/project/build.json` 

