#ifndef HAAR_MODIFIED_H
#define HAAR_MODIFIED_H

/*#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/core/internal.hpp"*/

#include "ml.h"
//#include "cxcore.hpp"
//#include "cxmisc.h"
#include "cv.h"
#include "cvtypes.h"
#include "highgui.h"
//#include "internal.hpp"

#include <stdio.h>

namespace cv
{
	typedef std::vector<Rect> ConcurrentRectVector;
};

/*	CV_IMPL CvSeq*
	changed_cvHaarDetectObjects( const CvArr* _img,
						 CvHaarClassifierCascade* cascade,
						 CvMemStorage* storage, double scaleFactor,
						 int minNeighbors, int flags, CvSize minSize, CvSize maxSize );*/

	CVAPI(CvSeq*) changed_cvHaarDetectObjects( const CvArr* image,
						 CvHaarClassifierCascade* cascade,
						 CvMemStorage* storage, double scale_factor CV_DEFAULT(1.1),
						 int min_neighbors CV_DEFAULT(3), int flags CV_DEFAULT(0),
						 CvSize min_size CV_DEFAULT(cvSize(0,0)),
						 CvSize max_size CV_DEFAULT(cvSize(0,0)));


#endif // HAAR_MODIFIED_H
