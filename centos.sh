#!/bin/sh

HOME=`pwd`

DIR=$HOME/..

docker run --rm -v $DIR:/home:rw  -w /home/kk-game --entrypoint ./build.sh registry.cn-beijing.aliyuncs.com/kk/kk-game:latest

rm -rf $HOME/../bin/kk-app
cp $HOME/bin/kk-app $HOME/../bin/kk-app
