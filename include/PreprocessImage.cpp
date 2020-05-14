#include "stdafx.h"
#include "PreprocessImage.h"

void ConvertMat2CImage(cv::Mat & SrcImage, CImage & DestImage)
{
	BITMAPINFO bitInfo;
	
	//Set bit information for both src and dst
	bitInfo.bmiHeader.biBitCount = 24;
	bitInfo.bmiHeader.biClrImportant = 0;
	bitInfo.bmiHeader.biClrUsed = 0;
	bitInfo.bmiHeader.biCompression = BI_RGB;
	bitInfo.bmiHeader.biHeight = -SrcImage.rows;
	bitInfo.bmiHeader.biPlanes = 1;
	bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo.bmiHeader.biSizeImage = 0;//!
	bitInfo.bmiHeader.biWidth = SrcImage.cols;
	bitInfo.bmiHeader.biXPelsPerMeter = 0;
	bitInfo.bmiHeader.biYPelsPerMeter = 0;

		StretchDIBits(DestImage.GetDC(), 0, 0,
			SrcImage.cols, SrcImage.rows, 0, 0,
			SrcImage.cols, SrcImage.rows,
			SrcImage.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY);
}
