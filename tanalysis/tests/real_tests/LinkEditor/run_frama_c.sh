frama-c.byte -cpp-command 'gcc -E -C -I. -I./include' -do-results -taint-analysis -config-file '../../../default.cfg' *.c
