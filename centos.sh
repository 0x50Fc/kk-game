#!/bin/sh

HOME=`pwd`

docker run --rm -v $HOME:/home:rw  -w /home --entrypoint ./build.sh registry.cn-beijing.aliyuncs.com/kk/kk-game:latest

rm -rf $HOME/../bin/kk-app
cp $HOME/bin/kk-app $HOME/../bin/kk-app
