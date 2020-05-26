#include "stdafx.h"
#include "PreprocessImage.h"

//Rotate image by angle in degree, the mImage is input image, mPreprocessImage
//is output image
void RotateImage(cv::Mat & mImage, cv::Mat& mPreprocessImage, int angle)
{
	if (!mImage.empty())
	{
		cv::Point2f pCenterPoint(mImage.cols / 2, mImage.rows / 2);
		//Rotate image
		cv::Mat mRotateImage = cv::getRotationMatrix2D(pCenterPoint, angle, 1.0);
		cv::warpAffine(mImage, mPreprocessImage, mRotateImage, mImage.size());
#ifdef _DEBUG
		imshow("Rotate Image", mPreprocessImage);
		cv::waitKey(0);
#endif
	}

}

//Adjust light by alpha; pixel = (1+alpha)*pixel
//use cast to make sure image in saturation mode
//the mImage is input image, mPreprocessImage is output image
void LightingAdjust(cv::Mat & mImage, cv::Mat& mPreprocessImage, float iAlpha)
{
	if (!mImage.empty())
	{
		mImage.convertTo(mPreprocessImage, -1, iAlpha / 100.0 + 1);
#ifdef _DEBUG
		imshow("Adjust Lighting Image", mPreprocessImage);
		cv::waitKey(0);
#endif
	}
}

//make histogram equalization 
//just use for gray image
void HistogramEqualization(cv::Mat & mImage, cv::Mat& mPreprocessImage)
{
	printf("Enter histogram function\n");
	if (!mImage.empty() && mImage.channels() == 1)
	{
		cv::equalizeHist(mImage,mPreprocessImage);
#ifdef _DEBUG
		imshow("Histogram Equalized Image", mPreprocessImage);
		cv::waitKey(0);
#endif
	}
}

//Make blur image, in this one use 
void BlurImage(cv::Mat & mImage, cv::Mat& mPreprocessImage)
{
	if (!mImage.empty())
	{
		cv::blur(mImage, mPreprocessImage, cv::Size(9, 9));
#ifdef _DEBUG
		imshow("Blured Image", mPreprocessImage);
		cv::waitKey(0);
#endif
	}
}

//
