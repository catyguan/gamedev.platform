#!/bin/bash

BOOTPATH=$[boot.bootpath]
BOOTPID=${BOOTPATH}boot.pid
BOOTLOG=${BOOTPATH}boot.log

export LANG=zh_CN.UTF-8
export TZ="Asia/Shanghai"

JAVAHOME=$[boot.javahome]
JAVAOPT='-Xmx$[boot.maxmem]m'
JAVACP="${BOOTPATH}classes:${BOOTPATH}libs/*"

MAINCLASS='bma.common.langutil.ai.boot.AIServerBoot'
MAINARGS=""
WORKHOME=$[boot.bootpath]

startBoot()
{

$JAVAHOME/bin/java $JAVAOPT -cp $JAVACP  -Duser.dir="$WORKHOME" $MAINCLASS $MAINARGS >> $BOOTLOG 2>&1 &

pid="$!";
echo "$pid" > $BOOTPID
echo "start ... ">>${BOOTLOG}
echo "child pid is $pid">>${BOOTLOG}
echo "status is $?">>${BOOTLOG}
}

if [ -f $BOOTPID ]
then
    pid="`cat $BOOTPID`"
    if test `ps -p $pid | wc -l` -gt 1
    then
        echo "progress $pid exists!"
        exit       
    fi
fi

startBoot

while [ 1 ]
do
wait $pid
exitstatus="$?"
echo "**************************" >>${BOOTLOG}
echo "child pid=$pid is gone, $exitstatus" >>${BOOTLOG}
echo `date` >> ${BOOTLOG}
echo "**************************" >>${BOOTLOG}

sleep 5

startBoot

done