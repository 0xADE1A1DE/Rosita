README
===
## Introduction
Rosita is a code rewrite framework aims at eliminating side-channel leakage from masked implementations. 
It uses a leakage emulator (based on [ELMO](https://github.com/sca-research/ELMO)) to identify
leaking instructions, and applies code modifications to eliminate the leakage.

For more information, see the [NDSS 2021 paper](https://eprint.iacr.org/2019/1445.pdf)
and the [datasets](https://adelaide.figshare.com/projects/Rosita/91580) it describes. 
Our talk on Rosita at Real World Crpyto 2021 is available [here](https://www.youtube.com/watch?v=kO-3Uh7tq60).

Please use these [scripts and tools](https://adelaide.figshare.com/articles/software/SCRIPTS/13270925)
to draw the corresponding figures in our paper.

## Setting up environment

Run the following commands in a `bash` shell. Please make sure that you run all
commands regarding Rosita after you have sourced the content of `envvars`.

~~~
./setup.sh
source envvars                                  # setup required environment variables 
~~~

## Running Rosita

Rosita's workflow is as follows,

1. Build the cipher implementation to be tested using customizations needed and run
ELMO to emulate power traces. The `--from-asm` option is needed for all runs after
the first run. This is due to the modifications from Rosita is applied to assembler files
and if built from the source files, these files get overwritten.

        ./ROSITA/emulatetraces --reps <TRACE-COUNT> --fixed-inputs <NO-OF-FIXED-INPUTS> -b [--from-asm] <BUILD-JSON-FILE>
        # Examples:
        # Note for Xoodoo: You may need to run patchror.sh in Xoodoo test directory to replace ror instructions 
        # according to .syntax unified/divided directives which Rosita doesn't support currently
        ./ROSITA/emulatetraces --reps 10000 --fixed-inputs 1 -b [--from-asm] ./TESTS/xoodoo/build_2share.json 
        ./ROSITA/emulatetraces --reps 10000 --fixed-inputs 1 -b [--from-asm] ./TESTS/chacha20/build_2share.json
        ./ROSITA/emulatetraces --reps 10000 --fixed-inputs 1 -b [--from-asm] ./TESTS/aes/build.json

3. Apply code rewrites using Rosita's pattern library
       
        ./ROSITA/rosita -m <BUILD-JSON-FILE>
        # Example: 
        ./ROSITA/rosita -m ./TESTS/xoodoo/build_2share.json

4. Repeat from 1. until the leakage measure is zero  

## Additional Options

+ View instruction level leakage and root causes (after running ELMO using `-b` option for emulatetraces)

        ./ROSITA/rosita -a ./TESTS/aes/build.json

+ Dump t-test values as a text file

        ./ROSITA/rosita --emit-tvals ./TESTS/aes/build.json


## Acknowledgments
This project was supported by:
* European Union Horizon 2020 research and innovation program under CERBERO project (grant agreement number 732105)
* The Australian Research Council project numbers DE200101577, DP200102364, and DP210102670
* Data61, CSIRO
* The Research Center for Cyber Security at Tel- Aviv University established by the State of Israel, the Prime
Minister’s Office and Tel-Aviv University
* A gift from
Intel.
