**NOTE: This project is a very old one that is unlikely to give interesting results for nowadays applications. It uses a now more or less deprecated version of OpenCV so even running it will take some work.**

Head Counter
============

The aim of this software is to be able to count the number of people present in a crowd, especially in protests, and provide an estimate as accurate as possible. This software is currently at a very early stage.

Compilation
-----------

You need OpenCV to compile this software. It should work well with the latest opencv from debian and ubuntu as well. It uses CMake to build. The process is as follow :

cd HeadCounter
mkdir build
cd build
cmake ..
make

this should compile both cvplayer and headcounter.

cvplayer is a simple test utility to check that your opencv configuration manage to read a specific file

headcounter is the main program. headcounter -h gives you instruction on how to use it.


