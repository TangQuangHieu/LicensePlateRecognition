#pragma once
#include "opencv2\opencv.hpp"

// Rotate image by angle in degree
void RotateImage(cv::Mat& mImage, cv::Mat& mPreprocessImage, int angle=0);

// Adjust lighting
void LightingAdjust(cv::Mat& mImage, cv::Mat& mPreprocessImage, float iAlpha=1.0);

// Histogram equalization
void HistogramEqualization(cv::Mat& mImage, cv::Mat& mPreprocessImage);

// Blur Image
void BlurImage(cv::Mat& mImage, cv::Mat& mPreprocessImage);