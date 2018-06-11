FROM centos:centos6

MAINTAINER hailongz "hailongz@qq.com"

RUN echo "Asia/shanghai" >> /etc/timezone

RUN rpm -ivh http://ftp.riken.jp/Linux/fedora/epel/6/i386/epel-release-6-8.noarch.rpm

RUN curl -Lks https://people.centos.org/tru/devtools-2/devtools-2.repo > /etc/yum.repos.d/devtools-2.repo

RUN yum update -y

RUN yum install -y devtoolset-2-gcc devtoolset-2-binutils devtoolset-2-gcc-c++ devtoolset-2-libtool

RUN ln -s /opt/rh/devtoolset-2/root/usr/bin/gcc /usr/bin/gcc
RUN ln -s /opt/rh/devtoolset-2/root/usr/bin/c++ /usr/bin/c++
RUN ln -s /opt/rh/devtoolset-2/root/usr/bin/g++ /usr/bin/g++

RUN yum install -y openssl-static glibc-static libstdc++-static zlib-static libevent-static

RUN yum install -y make 
RUN yum install -y cmake 
RUN yum install -y automake
RUN yum install -y golang

COPY ./lib/libuv /root/libuv
WORKDIR /root/libuv
RUN ./autogen.sh
RUN ./configure --enable-shared=no --enable-static=yes
RUN make
RUN make install
WORKDIR /root
RUN rm -rf libuv

COPY ./lib/libwebsockets /root/libwebsockets
WORKDIR /root/libwebsockets
RUN rm -rf CMakeFiles
RUN rm -rf CMakeCache.txt
RUN cmake CMakeLists.txt
RUN make clean
RUN make
RUN make install
WORKDIR /root
RUN rm -rf libwebsockets

COPY ./lib/Chipmunk2D /root/Chipmunk2D
WORKDIR /root/Chipmunk2D
RUN rm -rf CMakeFiles
RUN rm -rf CMakeCache.txt
RUN cmake CMakeLists.txt
RUN make clean
RUN make
RUN make install
WORKDIR /root
RUN rm -rf Chipmunk2D

RUN mkdir /go

RUN mkdir /go/src

RUN cd /go

ENV GOPATH /go

RUN g++ --version

RUN go env

RUN yum clean all

RUN rm -rf /var/cache/yum/*

