
/*
 * Copyright (C) 2010 Yves Quemener
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */


/*
 * This program is just a test utility. It plays a video file or a camera
 * output through opencv's HighGUI. Its purpose is to allow
 * a user to check if a given file can be read by opencv
 * and if opencv has correctly been compiled on the installed
 * machine.
 */


#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <map>
#include <iostream>
#include <string>


using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
  VideoCapture capture;

	int argi=1;
	bool	param_open_cam		 = false;
	char *	param_input =0;


	if((argc<2)||(argc>3))
	{
		std::cout << "Usage : cvplayer [-cam] <input>"<<std::endl;
		return 1;
	}

	while(argi<argc)
	{
		if(!strcmp(argv[argi],"-cam"))
			param_open_cam = true;
		else
			param_input = argv[argi];
		argi++;
	}

	if(param_open_cam)
	{
    capture.open(atoi(param_input));
    capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	}
  else
	{
    capture.open( param_input );
  }

  if(!capture.isOpened())
	{
		std::cerr << "Could not open video file: "<< param_input << std::endl;
		return 1;
	}
  Mat testImg;
	cvNamedWindow( "result", 1 );


  while(true)
	{
    capture >> testImg;

		if(cvWaitKey(5) >= 10)
			return 0;

    imshow("result", testImg);
	}
}
