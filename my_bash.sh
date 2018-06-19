#!/bin/bash

#use #!/bin/bash -x for debug

#bash Quoting
#quoting is used to disable the meaning of special chars or keywords
var=hello
echo $var
#disable $ by single quoting ''
echo '$var'
#using dobule quoting "", all enclosed chars are literally preserved except $, ``, \
echo "$var"
echo "\$var"

#backticks (``) are used for command subtitution. `command` = $(command)
#all enclosed chars are literally preserved except $, ``, \
echo "`hostname` $var"
echo `date`
#example for using $ in ``: shell is trying to execute "hello"...
echo `$var`

#quoting is used for srings that have spaces between the words
#"" '' can be used
#'' don't support any kind of variable substitution or escaping of special chars
a="hello \"there\" world"
echo $a
a='hello "there" world'
echo $a
#a='hello \'there\' world'    # causes an error



#arrays

#getopt


ARGUMENT_LIST=(
"master"
"branch"
)
echo ""${ARGUMENT_LIST[@]}""

echo "help,$(printf "%s:," "${ARGUMENT_LIST[@]}")"

echo --name "$(basename "$0")"

exit 0

#ARGUMENT_LIST=("master" "branch")
#
#A="help,$(printf "%s:," "${ARGUMENT_LIST[@]}")"
#
#echo $A
ARRAY=(one two three)
array=(gree red blue)
for i in "${array[@]}"; do   # The quotes are necessary here
    echo "$i"
done

list='"item 1" "item 2" "item 3"'
for i in $list; do
    echo $i
done
for i in "$list"; do
    echo $i
done
for i in ${array[@]}; do
    echo $i
done
exit 0

# read arguments
#opts=$(getopt \
#    --longoptions "help,$(printf "%s:," "${ARGUMENT_LIST[@]}")" \
#    --name "$(basename "$0")" \
#    --options "" \
#    -- "$@"
#)

#what is eval ?
#================
#it's a bash internal command to evaluate a string as code
#builds a command line out ot its args, and run it as shell command
#same as open a bash shell and run the command
#exit value is the exit value of the shell command. If no args, 0 is returned.

#eval example:
#=============
a="ls ~/Desktop | more"
#try $a. without eval string is not interperted as a shell command
$a
#try with eval
eval $a

#what is set?
#Without arguments, set will print all shell variables
#The set command takes any arguments after the options (here "--" signals the end of the options)
#and assigns them to the positional parameters ($0..$n)
#with not options nor arguments (set --), it will unset / clear all positional arguments

echo $1
echo $2

#try set -- to unset all args and print again
#set --

echo $1
echo $2
 

#now try all together: eval + set --
#run: ./my_bash.sh "ab cd" "ef gh"
items=
for i in "$@"
do
    items="$items \"$i\""
done

echo $items

for i in $items
do
    echo $i
done

for i in "$@"
do
    echo $i
done

#now try with "eval set --"
eval set -- $items

for i in "$@"
do
    echo $i
done


#what is getopt ?
#################


exit 0

#print line numbers while running (bash -x my_sdcript.sh)
#PS1,PS2,PS3,PS4 are available prompts in linux
#for further info: https://www.linuxnix.com/linuxunix-ps4-prompt-explained-with-examples/
PS4='$LINENO: '

#print my script and dir names
SCRIPT_DIR=`dirname $0`
SCRIPT_NAME=`basename $0`

echo "SCRIPT_DIR holds the full path of my script, except the script file name iteslf: $SCRIPT_DIR"
echo "SCRIPT_NAME holds just the script file name iteslf: $SCRIPT_NAME"

#special aruments
echo "File Name: $0"
echo "First Parameter : $1"
echo "Second Parameter : $2"
echo "Quoted Values: $@"
#$*, $@
echo "Quoted Values: $*"
echo "Total Number of Parameters : $#"
#let's understand the diff between $* and $@
#try to run: ./my_bahsh.sh "arg 1" "arg 2"
for word in $*; do echo "$word"; done
for word in $@; do echo "$word"; done
for word in "$*"; do echo "$word"; done
for word in "$@"; do echo "$word"; done
#$?
echo "For last command ret value use $?: $?"
cp ./no_such_file /tmp
RET=$?
if [ RET = 0 ]; then
  echo "cp command exit OK"
else
  echo "cp command exit NOT OK"
fi

#echo -e




#use a list

#list of arguments:w


# if TEST-COMMANDS; then CONSEQUENT-COMMANDS; fi

# The TEST-COMMAND list is executed, and if its return status is zero !!!, the CONSEQUENT-COMMANDS list is executed.
# The return status is the exit status of the last command executed, or zero if no condition tested true.

# if not written in one command line, ';' after the TEST-COMMAND

# if syntax and examples
T1="foo"
T2="bar"
if [ "$T1" = "$T2" ]; then
    echo expression evaluated as true
else
    echo expression evaluated as false
fi

#if written in one command line, ';' also after each CONSEQUENT-COMMAND
# remember that the the return value of the TEST-COMMAND is evaluated. 
# if it is 0 it means success (0 is not false...), then we go into CONSEQUENT-COMMANDS...
# so if a command is using as the test expression, it will be true if its return value is 0 !
if [ grep $USER /etc/passwd ]; then echo expression evaluated as true; fi
if [ `grep "blabla" /etc/passwd` ]; then echo true; else echo false; fi
if [ "$T1" = "$T2" ]; then echo barak; echo adam; echo adam; fi

# Expressions used with if

# [ -a FILE ]	True if FILE exists.
# [ -b FILE ]	True if FILE exists and is a block-special file.
# [ -c FILE ]	True if FILE exists and is a character-special file.
# [ -d FILE ]	True if FILE exists and is a directory.
# [ -e FILE ]	True if FILE exists.
# [ -f FILE ]	True if FILE exists and is a regular file.
# [ -g FILE ]	True if FILE exists and its SGID bit is set.
# [ -h FILE ]	True if FILE exists and is a symbolic link.
# [ -k FILE ]	True if FILE exists and its sticky bit is set.
# [ -p FILE ]	True if FILE exists and is a named pipe (FIFO).
# [ -r FILE ]	True if FILE exists and is readable.
# [ -s FILE ]	True if FILE exists and has a size greater than zero.
# [ -t FD ]	True if file descriptor FD is open and refers to a terminal.
# [ -u FILE ]	True if FILE exists and its SUID (set user ID) bit is set.
# [ -w FILE ]	True if FILE exists and is writable.
# [ -x FILE ]	True if FILE exists and is executable.
# [ -O FILE ]	True if FILE exists and is owned by the effective user ID.
# [ -G FILE ]	True if FILE exists and is owned by the effective group ID.
# [ -L FILE ]	True if FILE exists and is a symbolic link.
# [ -N FILE ]	True if FILE exists and has been modified since it was last read.
# [ -S FILE ]	True if FILE exists and is a socket.
# [ FILE1 -nt FILE2 ]	True if FILE1 has been changed more recently than FILE2, or if FILE1 exists and FILE2 does not.
# [ FILE1 -ot FILE2 ]	True if FILE1 is older than FILE2, or is FILE2 exists and FILE1 does not.
# [ FILE1 -ef FILE2 ]	True if FILE1 and FILE2 refer to the same device and inode numbers.
# [ -o OPTIONNAME ]	True if shell option "OPTIONNAME" is enabled.
# [ -z STRING ]	True of the length if "STRING" is zero.
# [ -n STRING ] or [ STRING ]	True if the length of "STRING" is non-zero.
# [ STRING1 == STRING2 ] 	True if the strings are equal. "=" may be used instead of "==" for strict POSIX compliance.
# [ STRING1 != STRING2 ] 	True if the strings are not equal.
# [ STRING1 < STRING2 ] 	True if "STRING1" sorts before "STRING2" lexicographically in the current locale.
# [ STRING1 > STRING2 ] 	True if "STRING1" sorts after "STRING2" lexicographically in the current locale.
# [ ARG1 OP ARG2 ]	"OP" is one of -eq, -ne, -lt, -le, -gt or -ge. These arithmetic binary operators return true if "ARG1" is equal to, not equal to, less than, less than or equal to, greater than, or greater than or equal to "ARG2", respectively. "ARG1" and "ARG2" are integers.

# Combining expressions

# [ ! EXPR ]	True if EXPR is false.
# [ ( EXPR ) ]	Returns the value of EXPR. This may be used to override the normal precedence of operators.
# [ EXPR1 -a EXPR2 ]	True if both EXPR1 and EXPR2 are true.
# [ EXPR1 -o EXPR2 ]	True if either EXPR1 or EXPR2 is true.

if [ -f /var/log/messages ]
  then
    echo "/var/log/messages exists."
fi


