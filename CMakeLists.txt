cmake_minimum_required(VERSION 2.8)


PROJECT( HeadCounter )

FIND_PACKAGE( OpenCV REQUIRED )

ADD_EXECUTABLE( cvplayer cvPlayer/cvPlayer.cpp )
ADD_EXECUTABLE( headcounter headCounter.cpp )

TARGET_LINK_LIBRARIES( cvplayer ${OpenCV_LIBS} )
TARGET_LINK_LIBRARIES( 	headcounter ${OpenCV_LIBS} )
