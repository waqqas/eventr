FROM ubuntu:bionic

LABEL Name=eventr Version=0.0.1 Maintainer=waqqas.jabbar@gmail.com

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update
RUN apt-get install tzdata -y
RUN apt-get install --no-install-recommends apt-utils -y
RUN apt-get install vim nano -y
RUN apt-get install build-essential cmake cmake-curses-gui -y
RUN apt-get install git-all -y
RUN apt-get install doxygen python3-sphinx graphviz -y
RUN apt-get install libgtest-dev -y
RUN apt-get install libssl-dev -y
RUN apt-get install libboost-filesystem-dev libboost-program-options-dev libboost-test-dev libboost-date-time-dev -y
RUN apt-get install zlib1g-dev -y
RUN apt-get install netcat -y
RUN apt-get install net-tools -y
RUN apt-get install gdb gdbserver -y

WORKDIR /code
