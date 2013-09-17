#!/bin/bash

ARGV="$@"

SHELLPATH=$[boot.servicepath]
SHELL=${SHELLPATH}boot.sh;
SHELLPID=${SHELLPATH}shell.pid
BOOTPID=${SHELLPATH}boot.pid

startBoot()
{
    echo 'start...';
    if [ -f $SHELLPID ]
    then
        pid="`cat $SHELLPID`"
        if test `ps -p $pid | wc -l` -gt 1
        then
            echo 'progress exists!';
            exit;
        fi
    fi
    $SHELL &
    shpid="$!"
    echo "$shpid" > $SHELLPID;
}

stopBoot()
{
    echo 'stop boot ...';
    if [ -f $BOOTPID ]
    then
        bpid="`cat $BOOTPID`";
        if test `ps -p $bpid | wc -l` -gt 1
        then
            kill $bpid;
            echo "kill $BOOTPID - $bpid";
            return
        fi
    fi    
    echo "boot not exists";    
}

stopShell()
{
    echo 'stop shell ...';
    if [ -f $SHELLPID ]
    then
        pid="`cat $SHELLPID`";
        if test `ps -p $pid | wc -l` -gt 1
        then
            kill $pid;
            echo "kill $SHELLPID - $pid";
            return
        fi
    fi
    echo "shell not exists";    
}

case $ARGV in
start)
    startBoot
    ;;
stop)
    stopShell
    stopBoot
    ;;
*)
    echo './service.sh start|stop'
esac
