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
 * This is the main file of the headcounter
 */

#include "opencv2/video/tracking.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "stdio.h"
#include <vector>
#include <map>
#include <iostream>
#include <fstream>


/*CV_IMPL CvSeq*
cvHaarDetectObjects2( const CvArr* _img,
					 CvHaarClassifierCascade* cascade,
					 CvMemStorage* storage, double scale_factor,
					 int min_neighbors, int flags, CvSize min_size );*/


using namespace std;
using namespace cv;

int inline sqr(int x) { return x*x; }

// All the program is in the main function. Isn't that lovely ?
int main(int argc, char *argv[])
{

	char * param_cascade_filename		= 0;
	char * param_infile_name			= 0;
	char * param_outfile_name			= 0;
	char * param_replayfile_name		= 0;
	char param_dump_prefix[128]			= "img_";
	bool param_grab_video				= true;
	bool param_show_video				= true;
	bool param_show_faces				= true;
	bool param_show_lines				= true;
	bool param_show_roi					= true;
	bool param_show_trails				= true;
	bool param_show_count				= true;
	bool param_dump						= false;
	int	 param_y_line					= 100;
	int  param_x_line					= 80;
	int	 param_min_face_size			= 20;
	int	 param_max_face_size			= 40;
	int	 param_neighbors				= 2;
	int  param_trail_length_threshold	= 5;

	char usage_string[] = "Usage: headcounter [OPTIONS] input_file \n\
  Options:\n\
	  --cascade filename \n\
	  -o output_file \n\
	  --replay filename\n\
	  --minfacesize MINSIZE (default : 20 pixels)\n\
	  --maxfacesize MAXSIZE (default : 40 pixels)\n\
	  --neighbors NEIGHBORS (default : 2, higher is more discriminant)\n\
	  --traillength min_trail_length (default : 5)\n\
	  --novideo\n\
	  --nodisplay\n\
	  --noface\n\
	  --noline\n\
	  --noROI\n\
	  --notrails\n\
	  --nocount\n\
	  --dump \n\
	  --dumpprefix prefix\n\
	  --linex X\n\
	  --liney Y";

	ofstream out_text_file;

	// Parsing of the arguments
	int argi=0;
	while(argi<argc-1)
	{
		argi++;
		if((!strcmp(argv[argi], "--cascade"))&&(argi<argc))
		{
			argi++;
			param_cascade_filename = argv[argi];
		}
		else if((!strcmp(argv[argi], "-o"))&&(argi<argc))
		{
			argi++;
			param_outfile_name=argv[argi];
		}
		else if((!strcmp(argv[argi], "--replay"))&&(argi<argc))
		{
			argi++;
			param_replayfile_name=argv[argi];
		}
		else if((!strcmp(argv[argi], "--maxfacesize"))&&(argi<argc))
		{
			argi++;
			param_max_face_size = atoi(argv[argi]);
		}
		else if((!strcmp(argv[argi], "--minfacesize"))&&(argi<argc))
		{
			argi++;
			param_min_face_size = atoi(argv[argi]);
		}
		else if((!strcmp(argv[argi], "--neighbors"))&&(argi<argc))
		{
			argi++;
			param_neighbors = atoi(argv[argi]);
		}
		else if((!strcmp(argv[argi], "--traillength"))&&(argi<argc))
		{
			argi++;
			param_trail_length_threshold = atoi(argv[argi]);
		}

		else if(!strcmp(argv[argi], "--novideo"))
		{
			param_grab_video = false;
		}
		else if(!strcmp(argv[argi], "--nodisplay"))
		{
			param_show_video = false;
		}
		else if(!strcmp(argv[argi], "--noface"))
		{
			param_show_faces = false;
		}
		else if(!strcmp(argv[argi], "--noline"))
		{
			param_show_lines = false;
		}
		else if(!strcmp(argv[argi], "--noROI"))
		{
			param_show_roi = false;
		}
		else if(!strcmp(argv[argi], "--notrails"))
		{
			param_show_trails = false;
		}
		else if(!strcmp(argv[argi], "--nocount"))
		{
			param_show_trails = false;
		}
		else if(!strcmp(argv[argi], "--dump"))
		{
			param_dump = true;
		}
		else if((!strcmp(argv[argi], "--dumpprefix"))&&(argi<argc))
		{
			argi++;
			snprintf(param_dump_prefix, 127, "%s", argv[argi]);
		}
		else if((!strcmp(argv[argi], "--linex"))&&(argi<argc))
		{
			argi++;
			param_x_line = atoi(argv[argi]);
		}
		else if((!strcmp(argv[argi], "--liney"))&&(argi<argc))
		{
			argi++;
			param_y_line = atoi(argv[argi]);
		}
		else if(param_infile_name==0)
		{
			param_infile_name=argv[argi];
		}
		else if(!strcmp(argv[argi], "-h"))
		{
			std::cout << usage_string << std::endl;
			return 0;
		}
		else
		{
			std::cerr << "Invalid argument or wrong argument numbers." << std::endl;
			std::cout << usage_string << std::endl;
			return 1;
		}
	}
	if(param_infile_name==0)
	{
		std::cerr << "Missing input filename" << std::endl;
		std::cout << usage_string << std::endl;
		return 1;
	}

	if(param_cascade_filename==0)
	{
		param_cascade_filename = new char[1024];
    sprintf(param_cascade_filename,"data/haarcascade_frontalface_default.xml");
	}

	if(!param_show_video)
	{
		param_show_faces=false;
		param_show_lines=false;
		param_show_roi=false;
		param_show_trails=false;
		param_show_count=false;
	}

	if(!param_grab_video)
	{
		param_show_video=false;
		param_show_faces=false;
		param_show_lines=false;
		param_show_roi=false;
		param_show_trails=false;
		param_show_count=false;
	}
	// End of parameters parsing

	//This map allows to attribute a unique ID to each individual face
	map<int,int> faceIndices;
	int faceUID=0;

	//Tags face UIDs  that have been counted as crossing the lines
	map<int,bool> faceCounted;

	//Tags face indices that have been linked to one or two other
	// face indices
	map<int,int> faceLinks;

	// series of triplet values of faces coordinates:
	// center_x center_y frame#
	vector<int> pastFaces;

	// Gives the trail length of a given ID
	map<int,int> trailLength;

	// segments of trails stored as record of 5 integers:
	// x1 y1 x2 y2 uid
	vector<int> lines;

	// stores rectangles and frame numbers
	// #frame x y w h
	vector<int> replayContent;

	// The counter of the number of faces crossing the lines
	int crosscount=0;

	// Current frame number
	int numImg=0;

  //CvHaarClassifierCascade*  cascade;
  CascadeClassifier cascade;

	CvMemStorage* storage = cvCreateMemStorage(0);
  //cascade = (CvHaarClassifierCascade*)cvLoad( param_cascade_filename, 0,0,0 );
  cascade.load(param_cascade_filename);

  VideoCapture capture;

	if(param_grab_video)
	{
    capture.open( param_infile_name );

    if(!capture.isOpened())
		{
			std::cerr << "Could not open video device :" << param_infile_name << std::endl;
			return 1;
		}
	}
  Mat testImg;
  Mat gray;
  Rect ROI;

  ROI.width = -1;
  ROI.height = 720;
  ROI.x=0;
  ROI.y=00;

	if(param_show_video)
	{
		cvNamedWindow( "result", 1 );
	}

	if(param_outfile_name!=0)
	{
		out_text_file.open(param_outfile_name);
	}
	
	if(param_replayfile_name!=0)
	{
		FILE *fd = fopen(param_replayfile_name, "r");
		bool endreached = false;
		while(!endreached)
		{
			int num,x,y,w,h;
			int err;
			err = fscanf(fd, "%d %d %d %d %d\n", &num, &x, &y, &w, &h);
			replayContent.push_back(num);
			replayContent.push_back(x);
			replayContent.push_back(y);
			replayContent.push_back(w);
			replayContent.push_back(h);			
			if(err==EOF)
				endreached=true;
		}
	}

	bool quit=false;
	while(!quit)
	{
		if(param_grab_video)
		{
      capture >> testImg;

			char key = cvWaitKey(10);
			if( key == 27)
			{
				return 0;
			}
      if(ROI.width>0) testImg = testImg(ROI);
      cvtColor(testImg, gray, CV_BGR2GRAY);
		}


    //cvSetImageROI(gray, ROI);

		
		// rectangles found or replayes for the current frame
    vector<Rect>	current_faces;

		if(param_replayfile_name==0)
		{
      vector<Rect> tmp;

      cascade.detectMultiScale(gray, current_faces, 1.2
                               ,param_neighbors
                               ,CV_HAAR_SCALE_IMAGE
                               ,Size(param_min_face_size, param_min_face_size)
                               ,Size(param_max_face_size, param_max_face_size));
		}
		else
		{
			static unsigned int current_index_in_replay=0;

			while(replayContent[current_index_in_replay]<numImg)
			{
				current_index_in_replay+=5;
			}
			while(replayContent[current_index_in_replay]==numImg)
			{
        Rect r;
        r.x=replayContent[current_index_in_replay+1];
        r.y=replayContent[current_index_in_replay+2];
        r.width=replayContent[current_index_in_replay+3];
        r.height=replayContent[current_index_in_replay+4];
				current_faces.push_back(r);
				if(current_index_in_replay<replayContent.size()-5)
				{
					current_index_in_replay+=5;
				}
				else
				{
					quit=true;
					break;
				}
			}
		}
		
		static int farthest_past_value=0;
		static int farthest_past_index=0;

    if(numImg%10==0)
		{
			printf("Frame#%d(%.1f sec): doing %d computations (%d * %d). Cross count so far : %d\n",
			   numImg,
			   numImg*1/50.0f,
			   current_faces.size() * (pastFaces.size() - farthest_past_index),
			   current_faces.size(),
			   (pastFaces.size() - farthest_past_index),
			   crosscount);
		}

		for( unsigned int i = 0; i < current_faces.size(); i++ )
		{
      Rect r = current_faces[i];
      Point center;

      rectangle(testImg, r, Scalar(0,0,255), 2,0,0);
      center.x = cvRound((ROI.x + r.x + r.width*0.5));
      center.y = cvRound((ROI.y + r.y + r.height*0.5));

      pastFaces.push_back(cvRound((ROI.x + r.x + r.width*0.5)));
      pastFaces.push_back(cvRound((ROI.y + r.y + r.height*0.5)));
			pastFaces.push_back(numImg);


			if(param_outfile_name!=0)
			{
        out_text_file << numImg << " " << r.x << " " << r.y << " " << r.width << " " << r.height << std::endl;
			}

			// Find the closest face on the last frames and link them
			// through the use of the same face id
			int dist_min = 100000;
			const int FRAME_TO_FRAME_DIST_THRESHOLD = 1000;
			const float zDimension = 1.0f;
			int closest_x, closest_y, closest_index=0;
			for(unsigned int j=farthest_past_index;j<pastFaces.size();j+=3)
			{
				int dist;

				// The distance between two faces is an euclidian distance with
				// time being the 3rd dimension, corrected by the zDimension constant.

				dist = sqr(center.x - pastFaces[j]) + sqr(center.y - pastFaces[j+1]) + sqr(numImg - pastFaces[j+2])*zDimension;
				if((dist < dist_min)&&
				   (numImg>pastFaces[j+2])&&
				   (faceLinks.find(j/3)==faceLinks.end()))
				{
					dist_min=dist;
					closest_x = pastFaces[j];
					closest_y = pastFaces[j+1];
					closest_index=j;
				}
				while(farthest_past_value+20<pastFaces[j+2])
				{
					farthest_past_index+=3;
					farthest_past_value=pastFaces[farthest_past_index+2];
				}
			}

			if(dist_min < FRAME_TO_FRAME_DIST_THRESHOLD)
			{
				lines.push_back(closest_x);
				lines.push_back(closest_y);
				lines.push_back(center.x);
				lines.push_back(center.y);

				if(faceLinks.find(closest_index/3)==faceLinks.end())
				{	faceLinks[closest_index/3]=1; }
				else
				{	faceLinks[closest_index/3]++; }

				if(faceIndices.find(closest_index/3)==faceIndices.end())
				{
					faceIndices[closest_index/3]=faceUID;
					faceIndices[(pastFaces.size()-1)/3]=faceUID;
					lines.push_back(faceUID);
					faceCounted[faceUID]=false;
					trailLength[faceUID]=1;
					faceUID++;

				}
				else
				{
					faceIndices[(pastFaces.size()-1)/3]=faceIndices[closest_index/3];
					lines.push_back(faceIndices[closest_index/3]);
					trailLength[faceIndices[closest_index/3]]++;
					if((faceCounted[faceIndices[closest_index/3]])&&
						(trailLength[faceIndices[closest_index/3]]==param_trail_length_threshold))
					{
						crosscount++;
					}
				}

				// Check if the current link crosses one of the lines
				if(((closest_y<param_y_line)&&(center.y>=param_y_line))||
				   ((closest_y>param_y_line)&&(center.y<=param_y_line))||
				   ((closest_x<param_x_line)&&(center.x>=param_x_line))||
				  ((closest_x>param_x_line)&&(center.x<=param_x_line)))
				{
					if(!faceCounted[faceIndices[(pastFaces.size()-1)/3]])
					{
						faceCounted[faceIndices[(pastFaces.size()-1)/3]]=true;
						if(trailLength[faceIndices[(pastFaces.size()-1)/3]]>param_trail_length_threshold)
							crosscount++;

						//std::cout << "cross count = " << crosscount << std::endl;
					}
				}
			}
			if(param_show_faces)
			{
        Scalar color;
				if(faceIndices.find((pastFaces.size()-1)/3)!=faceIndices.end())
				{
					if(trailLength[faceIndices[(pastFaces.size()-1)/3]]<5)
						color = cvScalar(0,0,0);
					else
					{
						if(faceCounted[faceIndices[(pastFaces.size()-1)/3]])
              color = Scalar(0,0,255);
						else
              color = Scalar(0,255,0);
					}
				}
        Point p1 = cvPoint(ROI.x + r.x,
                   ROI.y + r.y);
        Point p2 = cvPoint(ROI.x + r.x + r.width,
                   ROI.y + r.y + r.height);

        rectangle(testImg,p1,p2, color, 2);
			}
		}

		//cvRectangleR(testImg, ROI, cvScalar(255,0,0),5);
		if(param_show_trails)
		{
      Scalar color;
			for(unsigned int i=0;i<lines.size();i+=5)
			{
				if(faceCounted[lines[i+4]])
          color = Scalar(0,0,255);
				else
          color = Scalar(0,255,0);

        line(testImg, Point(lines[i],lines[i+1]),
              Point(lines[i+2],lines[i+3]),
							color, 1);
			}
		}

		if(param_show_lines)
		{
      line(testImg, Point(0,param_y_line),
              Point(testImg.cols,param_y_line),
              Scalar(255,255,255), 1);

      line(testImg, Point(param_x_line,0),
              Point(param_x_line, testImg.rows),
              Scalar(255,255,255), 1);
		}

		if (param_show_count)
		{
			char bufcount[1000];
			sprintf(bufcount, "People found : %d", crosscount);
      putText(testImg, bufcount, Point(20,20),
              FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0,255,0), 2.0 );
		}


		if(param_show_video)
		{
      imshow("result", testImg);
		}

		if(param_dump)
		{
			char buf[256];
			sprintf(buf, "%s%08d.jpg", param_dump_prefix, numImg);
      imwrite(buf, testImg);
		}

		numImg++;
	}
	if(param_outfile_name)
	{
		out_text_file.close();
	}
}
