#!/bin/bash

# what is eval ?
# ===============
# it's a shell built-in to evaluate a string as a shell command
# it builds a command line out ot its args, and run it as shell command
# exit value is the exit value of the shell command. If no args, 0 is returned.

# eval example 1:
# ===============
mkdir ~/somedir
touch ~/somedir/a.txt ~/somedir/b.txt
a="ls ~/somedir | more"
# try $a. without eval, string is not interperted as a shell command
$a
# try with eval
eval $a
rm -rf ~/somedir

# eval actually makes an input line to be parsed twice:
# =====================================================
# From Shell command POSIX, reacll how the shell interpret a line:
# <1>reads the line, <2>break into tokens, <3>parse into simple & compound commands, <4>make expansions, 
# <5>performs redirections, <6>performs functions, buit-ins, scripts,.., <7>exec the command and waits for it to complete and collect its status

### first parsing
### =============
# so on the first round the shell reads the line and removes first level of quoting
# then on step 6, it identify "eval" as a built-in which causes the shell to start a new second round with the processed line

### second parsing
### ==============
# here comes the second parsing phase which is actually executed.

# eval example 2:
# ===============
a=b b=c
eval echo \$$a
# to understand what is done in the first round, use printf:
printf '<%s> ' eval echo \$$a

# eval example 3: white spaces, etc...
# ====================================
items="\"ab cd\" \"ef gh\""
echo $items

# debug with -x

set -x
set -- $items
set +x

for i in "$@"
do
    echo $i
done

# now try with "eval set --" to parse it correctly
set -x
eval set -- $items
set +x

for i in "$@"
do
    echo $i
done


