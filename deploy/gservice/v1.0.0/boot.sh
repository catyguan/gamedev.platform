#!/bin/bash

BOOTPATH=/home/www/test_env/java/gservice/
BOOTPID=${BOOTPATH}boot.pid
BOOTLOG=${BOOTPATH}boot.log

export LANG=zh_CN.UTF-8
export TZ="Asia/Shanghai"

JAVAHOME=/usr/local/jdk1.6.0_31/
JAVAOPT='-Xmx256m'
JAVACP="${BOOTPATH}classes:${BOOTPATH}libs/*"

MAINCLASS='bma.common.langutil.spring.ServerBoot'
MAINARGS=""
WORKHOME=/home/www/test_env/java/gservice/

startBoot()
{

$JAVAHOME/bin/java $JAVAOPT -Djava.library.path="${WORKHOME}jni" -cp $JAVACP  -Duser.dir="$WORKHOME" $MAINCLASS $MAINARGS >> $BOOTLOG 2>&1 &

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