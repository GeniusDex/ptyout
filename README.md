# PTYout
PTYout is a utility to let a program produce output as if it is running inside of an interactive terminal.

The original use case for this is to run programs without significant buffering of their output, to be able
to parse the standard output and react on that with little delay.

Virtual terminal sequences are stripped from the program output.

# Supported operating systems
Currently every operating system that supports `forkpty()` is supported. It has only been tested on Linux.

Experiments with ConPTY were done for Windows support. However, there are situations in which ConPTY fails to
identify as an interactive terminal properly, defeating the purpose of this tool.
