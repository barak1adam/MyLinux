#!/bin/bash

script=`basename $0`

############################################################################
# Get script parameters
#
ARGUMENT_LIST=(
"master"
"branch"
)

# read arguments
opts=$(getopt \
    --longoptions "help,$(printf "%s:," "${ARGUMENT_LIST[@]}")" \
    --name "$(basename "$0")" \
    --options "" \
    -- "$@"
)

eval set --$opts

while [[ $# -gt 0 ]]; do
    case $1 in
        --master)
            master=$2
            shift 2
            ;;

        --branch)
            branch=$2
            shift 2
            ;;

        --help)
            echo "Usage: $script --master <current-master-branch> --branch <current-personal-branch>"
            exit 0
            ;;

        *)
            break
            ;;
    esac
done

############################################################################
# Check script parameters
#
if [ -z ${master} ] || [ -z ${branch} ] ; then
    echo "Error: Wrong branch parameters! Leaving..."
    echo "Usage: $script --master <current-master-branch> --branch <current-personal-branch>"
    exit 1
fi

while true ; do
    echo -n "Are you sure all changes commited? (y/N) "
    read yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) echo "Doing nothing. Bye bye ..."; exit;;
        * )     echo "Doing nothing. Bye bye ..."; exit;;
    esac
done

while true ; do
    echo -n "Rebasing [${branch}] on [${master}]. Are you sure (y/N)? "
    read yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) echo "Doing nothing. Bye bye ..."; exit;;
        * )     echo "Doing nothing. Bye bye ..."; exit;;
    esac
done

############################################################################
# Update local master branch from git server master branch
#
echo "========================================================================="
echo "Performing local [${master}] branch update..."

echo "--------"
cmd_run="git checkout ${master}"
echo "[$cmd_run]"
$cmd_run
echo "--------"

retval=$?
if [ $retval -ne 0 ]; then
    echo "Error: [$cmd_run] failed."
    exit 1
fi

echo "--------"
cmd_run="git pull --rebase"
echo "[$cmd_run]"
$cmd_run
echo "--------"

retval=$?
if [ $retval -ne 0 ]; then
    echo "Error: [$cmd_run] failed."
    exit 1
fi

echo "Performing local [${master}] branch update... Done."

############################################################################
# Create local backup branch for feature branch
#
# Time format example
# - Short time format: 20180325_130918
# - Full  time format: Sun Mar 25 13:09:18 IDT 2018
time_postfix=`date +"%Y%m%d_%H%M%S"`
backup_branch="${branch}_backup_${time_postfix}"

echo "========================================================================="
echo "Creating backup local branch [${backup_branch}]..."
cmd_run="git checkout -b ${backup_branch} ${branch}"
echo "--------"
echo "[$cmd_run]"
$cmd_run
echo "--------"

retval=$?
if [ $retval -ne 0 ]; then
    echo "Error: [$cmd_run] failed."
    exit 1
fi

echo "Creating backup local branch ${backup_branch}... Done."

############################################################################
# Print the list of commits that will be rebased
#
echo "========================================================================="
echo "Showing relative commits from both branches before rebase..."

echo "--------"
cmd_run="git show-branch ${master} ${branch}"
echo "[$cmd_run]"
$cmd_run
echo "--------"

retval=$?
if [ $retval -ne 0 ]; then
    echo "Error: [$cmd_run] failed."
    exit 1
fi

echo "Showing relative commits from both branches before rebase... Done."

############################################################################
# Rebase local feature branch on updated local master branch
#
echo "========================================================================="
echo "Performing local rebase of [${branch}] branch on [${master}]..."

echo "--------"
cmd_run="git checkout ${branch}"
echo "[$cmd_run]"
$cmd_run
echo "--------"

retval=$?
if [ $retval -ne 0 ]; then
    echo "Error: [$cmd_run] failed."
    exit 1
fi

echo "--------"
cmd_run="git rebase ${master}"
echo "[$cmd_run]"
$cmd_run
echo "--------"

retval=$?
if [ $retval -ne 0 ]; then
    echo "Error: [$cmd_run] failed."
    exit 1
fi

echo "Performing local rebase of [${branch}] branch on [${master}]... Done."

############################################################################
# Print the list of commits that will be rebased
#
echo "========================================================================="
echo "Showing relative commits from both branches after rebase..."

echo "--------"
cmd_run="git show-branch ${master} ${branch}"
echo "[$cmd_run]"
$cmd_run
echo "--------"

retval=$?
if [ $retval -ne 0 ]; then
    echo "Error: [$cmd_run] failed."
    exit 1
fi

echo "Showing relative commits from both branches after rebase... Done."

############################################################################
# Add some notes to help resolve source code conflicts
#
echo "" # add empty line for more visibility
echo "========================================================================="
echo "Follow the below steps:"
echo "- git status"
echo "- git diff"
echo "- Resolve source code conflicts"
echo "- git status"
echo "- Add resolved file to staged changes [git add --update]"
echo "- git status"
echo "- Continue git rebase [git rebase --continue]"
echo "NOTE: To CANCEL rebase do [git rebase --abort]"
echo "NOTE: To delete local backup branch [git branch -D ${backup_branch}]"
echo "========================================================================="
