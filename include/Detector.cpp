#include "stdafx.h"
#include "Detector.h"
#include "Define.h"
/* Initial the YoloDetector with the nmber of stage
 * The type of detector in each stage
 * And the path to detection folder
 * After that, base on these options, we will initilize 
 * actual detector inside this class
*/

YoloDetector::YoloDetector(const state_choose& sYoloStages, const std::vector<yolo_list>& yYoloListTypes, const std::string& sDetectorFolderPath):
	m_sYoloStages(sYoloStages), m_yYoloListTypes(yYoloListTypes), m_sDetectorFolderPath(sDetectorFolderPath)
{
		//always initial the first detector		
		//InitialDetector(Detector* pDetector, yolo_list& yDetectorType, DetectorInformation& dDetectorInformation);
		InitialDetector(m_pDetectors[0], m_yYoloListTypes[0], m_dDetectorInformation[0]);
		if (m_sYoloStages == TWO_STATE)
		{
			//Initial second detector if needed
			InitialDetector(m_pDetectors[1], m_yYoloListTypes[1], m_dDetectorInformation[1],1);
		}
}
/* Initial actual detector
 *  should set iWhatStage = 1 if stage equal 2 to indicate this is the detector of stage 2( default is 0)
 *  yDetectorType = YOLOV2 or YOLOV3 or YOLOV3_MOD
 *  dDetectorInformation will store the information of this detection include
 * 	std::string sDetectorName;//Name of detector
 *	std::string sDetectorPath;//Path to detector folder
 *	std::string sNamePath;//path to data     (name.data)
 *	std::string sConfigPath;//path to config (name.cfg)
 *	std::string sWeightPath;//path to weight ( name.weights)
*/
void YoloDetector::InitialDetector(Detector*& pDetector, yolo_list& yDetectorType, DetectorInformation& dDetectorInformation, int iWhatStage)
{
	if (iWhatStage == 0)
	{
		dDetectorInformation.sDetectorPath = m_sDetectorFolderPath + "\\stage1\\";
	}
	else
	{
		dDetectorInformation.sDetectorPath = m_sDetectorFolderPath + "\\stage2\\";
	}
	switch (yDetectorType)
	{
		case YOLOV2:
		{
			dDetectorInformation.sDetectorName = "yolov2";
			break;
		}
		case YOLOV3:
		{
			dDetectorInformation.sDetectorName = "yolov3";
			break;
		}
		case YOLOV3_MOD:
		{
			dDetectorInformation.sDetectorName = "yolov3_mod";
			break;
		}
		default:
		{
			dDetectorInformation.sDetectorName = "yolov3";
			break;
		}
	}
	dDetectorInformation.sConfigPath = dDetectorInformation.sDetectorPath + dDetectorInformation.sDetectorName + ".cfg";
	dDetectorInformation.sNamePath = dDetectorInformation.sDetectorPath + dDetectorInformation.sDetectorName + ".names";
	dDetectorInformation.sWeightPath = dDetectorInformation.sDetectorPath + dDetectorInformation.sDetectorName + ".weights";
	ReadNameFromFile(dDetectorInformation.sNamePath, dDetectorInformation.sClassNames);
	//Detector(std::string cfg_filename, std::string weight_filename, int gpu_id = 0);

	/*Hieu Tang 2020-06-09
	 *  the PC has only 1 gpu, the second stage should uses same gpu id0 with the first detector
	*/
	if(m_iNumOfGpus>1)
		pDetector = new Detector(dDetectorInformation.sConfigPath, dDetectorInformation.sWeightPath, iWhatStage);
	else
	{
		int GpuId = 0;
		pDetector = new Detector(dDetectorInformation.sConfigPath, dDetectorInformation.sWeightPath, GpuId);
	}
		
}

/* Read all names from sNameFilePath and put all into sClassName
 * recommend use sClassName in DetectorInformation structure to store this information
*/
void YoloDetector::ReadNameFromFile(std::string & sNameFilePath, std::vector<std::string>& sClassName)
{
	std::ifstream ifsNameFile(sNameFilePath);
	if (ifsNameFile.is_open())
	{
		std::string stemp;
		while (std::getline(ifsNameFile, stemp))
		{
			sClassName.push_back(stemp);
		}
	}
}

/* Use as public interface to client 
 * Detect base on 1 stage or 2 stages
 * It will automatically choose DetectOnluStage1 or CasCadeDetect
*/
void YoloDetector::Detect(cv::Mat mImage)
{
	
	switch (m_sYoloStages)
	{
	case state_choose::ONE_STATE:
		{
		DetectOnlyStage1(mImage);
		break;
		}
	case state_choose::TWO_STATE:
	{
		CasCadeDetect(mImage);
		break;
	}
	default:
		break;
	}
}

/* Detector only stage 1
 * Use Single detector and detector 0 and m_bResultBoxes to get the result boxes
*/
void YoloDetector::DetectOnlyStage1(cv::Mat mImage)
{
#ifdef _CALCULATE_COMPUTATION_TIME
	//SYSTEMTIME current, start;
	//::GetLocalTime(&start);
	LARGE_INTEGER begin, end, freq;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&begin);
#endif
	SingleDetect(m_pDetectors[0], mImage, m_bResultBoxes[0]);
	//MNS through all classes, 1 license plate just have 1 character and 1 province( if it has province)
	DoNMSAllClasses(m_bResultBoxes[0]);
	std::vector<int> iSortedIds = ShortDigitInLP();
	ShowResultInConsole(m_bResultBoxes[0], m_dDetectorInformation[0].sClassNames, iSortedIds);
	DrawImage(mImage, m_bResultBoxes[0], m_dDetectorInformation[0].sClassNames);
#ifdef _CALCULATE_COMPUTATION_TIME
	QueryPerformanceCounter(&end);
	m_dDetectionTime = 1000. *(double)(end.QuadPart - begin.QuadPart) / ((double)freq.QuadPart);
	printf("Detection time:%3.2f ms\n", m_dDetectionTime);
#endif
}

//detect only one single detector
/* Detect in single stage
 * Detector:	Input detector
 * mImage:		Input Image
 * bResulBoxes: Output result box
*/
void YoloDetector::SingleDetect(Detector*& Detector, cv::Mat mImage, std::vector<bbox_t>& bResultBoxes)
{
	bResultBoxes = Detector->detect(mImage);
}



//Detect 2 detectors
void YoloDetector::CasCadeDetect(cv::Mat mImage)
{
#ifdef _CALCULATE_COMPUTATION_TIME
	//SYSTEMTIME current, start;
	//::GetLocalTime(&start);
	LARGE_INTEGER begin, end, freq;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&begin);
#endif
	static int iDeltaX = 15;
	static int iDeltaY = 15;
	SingleDetect(m_pDetectors[0], mImage, m_bResultBoxes[0]);
	float _fConfidentScore = 0;
	//keep only one box represent for license plate go to next step
	for (auto _itBox = m_bResultBoxes[0].begin(); _itBox != m_bResultBoxes[0].end();)
	{
		if ((*_itBox).obj_id<10 && (*_itBox).prob>_fConfidentScore)
		{
			_fConfidentScore = (*_itBox).prob;
			++_itBox;
		}
		else
		{
			_itBox = m_bResultBoxes[0].erase(_itBox);
		}
	}
	ShowResultInConsole(m_bResultBoxes[0], m_dDetectorInformation[0].sClassNames);
	//Check whether the license plate position is in the image and crop it
	cv::Mat mCroppedImg;
	if (m_bResultBoxes[0].size() == 1)
	{
		//Enlarge the bounding box dimention
		m_bResultBoxes[0][0].x -= iDeltaX;
		m_bResultBoxes[0][0].y -= iDeltaY;
		m_bResultBoxes[0][0].w += 2 * iDeltaX;
		m_bResultBoxes[0][0].h += 2 * iDeltaY;
		if (m_bResultBoxes[0][0].x < 0)m_bResultBoxes[0][0].x == 0;
		if (m_bResultBoxes[0][0].y < 0)m_bResultBoxes[0][0].y == 0;
		if (m_bResultBoxes[0][0].x + m_bResultBoxes[0][0].w >= mImage.cols)m_bResultBoxes[0][0].w == mImage.cols-1 - m_bResultBoxes[0][0].x;
		if (m_bResultBoxes[0][0].y + m_bResultBoxes[0][0].h >= mImage.rows)m_bResultBoxes[0][0].h == mImage.rows-1 - m_bResultBoxes[0][0].y;
		cv::Rect rRoi(m_bResultBoxes[0][0].x, m_bResultBoxes[0][0].y, m_bResultBoxes[0][0].w, m_bResultBoxes[0][0].h);
		mCroppedImg = mImage(rRoi);
#ifdef _DEBUG
		imshow("ROI", mCroppedImg);
		cv::waitKey(0);
#endif //_DEBUG
		
		SingleDetect(m_pDetectors[1], mCroppedImg, m_bResultBoxes[1]);

		//MNS through all classes, 1 license plate just have 1 character and 1 province( if it has province)
		DoNMSAllClasses(m_bResultBoxes[1]);
		//Offset the result box to the correct position in the original image
		//by add the coordinate of the box to the offset of the license plate
		for (auto& _bBox : m_bResultBoxes[1])
		{
			_bBox.x += m_bResultBoxes[0][0].x;
			_bBox.y += m_bResultBoxes[0][0].y;
		}
		std::vector<int> iSortedIds = ShortDigitInLP();
		ShowResultInConsole(m_bResultBoxes[1], m_dDetectorInformation[1].sClassNames, iSortedIds);
		DrawImage(mImage, m_bResultBoxes[1], m_dDetectorInformation[1].sClassNames);
	}
#ifdef _CALCULATE_COMPUTATION_TIME
	QueryPerformanceCounter(&end);
	m_dDetectionTime = 1000. *(double)(end.QuadPart - begin.QuadPart) / ((double)freq.QuadPart);
	printf("Detection time:%3.2f ms\n", m_dDetectionTime);
#endif
}

/* Use to do non maximum suppression for all classes in license plate
 * The ideal is to eliminate all order redundance character base on
 * the rule the in 1 license plate just have only one character and
 * one province( if it has)
 * bResultBoxes is input and also output of this function
*/
void YoloDetector::DoNMSAllClasses(std::vector<bbox_t>& bResultBoxes)
{
	//step1: NMS on character; character class id from 20->69
	float fMaxCharScore = 0.0f;
	for (auto itBox = bResultBoxes.begin(); itBox != bResultBoxes.end();)
	{
		if ((*itBox).obj_id >= 20 && (*itBox).obj_id <= 69)
		{
			//the class in range of character
			if ((*itBox).prob > fMaxCharScore)
			{
				//if the probability is greater than threshold, keep it, otherwise, erase it from result vector
				fMaxCharScore = (*itBox).prob;
				++itBox;
			}
			else
			{
				itBox = bResultBoxes.erase(itBox);
			}
		}
		else ++itBox;
	}
	//step2: NMS on province; province class id from 70
	float fMaxProvinceScore = 0;
	for (auto itBox = bResultBoxes.begin(); itBox != bResultBoxes.end();)
	{
		if ((*itBox).obj_id >= 70)
		{
			//the class in range of character
			if ((*itBox).prob > fMaxProvinceScore)
			{
				//if the probability is greater than threshold, keep it, otherwise, erase it from result vector
				fMaxProvinceScore = (*itBox).prob;
				++itBox;
			}
			else
			{
				itBox = bResultBoxes.erase(itBox);
			}
		}
		else ++itBox;
	}
	//step3: Do NMS by IOU for the rest of classes
	DoNMSByIouAllClasses(bResultBoxes);
}

/* Show the result into a stream
 * oStream is input stream ( can be file or out stream ...)
 * bResultBoxes is the vector of result
*/
void YoloDetector::ShowResultInConsole(std::vector<bbox_t>& bResultBoxes, std::vector<std::string>& sClassNames, std::vector<int> iSortedIds)
{
	if(iSortedIds.size()>0)
		for (auto& iSortedIndex : iSortedIds)
		{
			auto& bBox = bResultBoxes[iSortedIndex];
			std::cout <<std::setw(10)<<"Name: "<< sClassNames[bBox.obj_id]<<"; x:" << bBox.x << "; y:" << bBox.y << "; w:" << bBox.w << "; h:" << bBox.h << std::endl;
		}
	else 
		for (auto& _bBox : bResultBoxes)
		{
			std::cout << std::setw(10) << "Name: " << sClassNames[_bBox.obj_id] << "; x:" << _bBox.x << "; y:" << _bBox.y << "; w:" << _bBox.w << "; h:" << _bBox.h << std::endl;
		}
}

std::array<std::vector<bbox_t>,2> YoloDetector::GetResultBoxes() const 
{
	return m_bResultBoxes;
}


/* Draw function, draw result into image
 * Random color to make it colorful
 * mImage is the input image
 * bBoxes is the bBoxes vector
 * sClassNames is the class name list
*/
void YoloDetector::DrawImage(cv::Mat & mImage, std::vector<bbox_t>& bBoxes,
	std::vector<std::string>& sClassNames)
{
	//color and inverse color to draw name and background
	cv::Scalar sColor;
	cv::Scalar sInvColor;
	std::string strObjectName;
	cv::Size textSize;
	int FontFace = cv::FONT_HERSHEY_COMPLEX_SMALL;
	double FontScale = 2;
	int  baseline;
	for (auto& _bBox : bBoxes)
	{
		//Initial color
		sColor = m_sRandomColorLookupTable[_bBox.obj_id];
		sInvColor = cv::Scalar(255, 255, 255) - sColor;

		//calculate the text size
		strObjectName = sClassNames[_bBox.obj_id];
		textSize = cv::getTextSize(strObjectName, FontFace, FontScale, 2, &baseline);

		//Make a retangle to carry the text inside it, the retangle will have the 
		//width and height same as the object name
		cv::Point pTopLeftOfBbox(_bBox.x, _bBox.y- textSize.height);
		cv::Rect rTextBackground = cv::Rect(pTopLeftOfBbox, cv::Point(pTopLeftOfBbox.x + textSize.width + 4, pTopLeftOfBbox.y + textSize.height - 4));
		cv::rectangle(mImage, rTextBackground, sColor, -1);

		//put character in image
		cv::Point pTopLeftofText(_bBox.x, _bBox.y);//Bottom left
		cv::putText(mImage, strObjectName, pTopLeftofText, FontFace, FontScale, sInvColor, 2);

		//Draw bbox around object
		cv::Rect rBbox(_bBox.x, _bBox.y, _bBox.w, _bBox.h);
		cv::rectangle(mImage, rBbox, sColor, 2);
	}
}

/* Short 2 lines detection in license plate by angle
 * by order of location in license plate
 * In license plate type, the yellow short  ( classid=2),
 * Green old(classid=4) and green new(classid=5) have 2 lines
*/
std::vector<int> YoloDetector::ShortDigitInLP()
{
	std::vector<int> iIndexOfSortedResultBoxes;
	iIndexOfSortedResultBoxes.clear();
	switch (m_sYoloStages)
	{
	case state_choose::ONE_STATE:
	{
		//Find license style
		for (auto _itbBox = m_bResultBoxes[0].begin(); _itbBox < m_bResultBoxes[0].end(); ++_itbBox)
			if ((*_itbBox).obj_id < 10)
			{
				m_bResultBoxes[1].push_back((*_itbBox));
				m_bResultBoxes[0].erase(_itbBox);
				break;
			}
		iIndexOfSortedResultBoxes = SortDigit(m_bResultBoxes[0]);
		break;
	}
	case state_choose::TWO_STATE:
	{
		//Find license style
		iIndexOfSortedResultBoxes = SortDigit(m_bResultBoxes[1]);
		break;
	}
	default: 
		break;
	}
	return iIndexOfSortedResultBoxes;
}

/* Get Detection info by its stage
*/
DetectorInformation YoloDetector::GetDetectionInformation(int iStage) const
{
	// TODO: insert return statement here
	if(iStage<2 && iStage>=0)
		return m_dDetectorInformation[iStage];
	else return DetectorInformation();
}

/* Get num of stage in Yolo Detector
 * return ONE_STAGE or TWO_STAGE
*/
state_choose YoloDetector::GetStageNum() const
{
	// TODO: insert return statement here
	return m_sYoloStages;
}

std::vector<std::string> YoloDetector::GetObjectNames(int iStageNum) const
{
	if (iStageNum >= 0 && iStageNum < 2)
		return m_dDetectorInformation[iStageNum].sClassNames;
	else
		return std::vector<std::string>();
}

/* Get detection time
*/
double YoloDetector::GetDetectionTime() const
{
	return m_dDetectionTime;
}

std::vector<int> YoloDetector::GetSortedIDs() const
{
	return m_iSortedIds;
}

/* Hieu Tang 2020-06-10
 * Helper function for DoNMSAllClasses
 * This function use IOU to do nms for all classes
*/
void YoloDetector::DoNMSByIouAllClasses(std::vector<bbox_t>& ResultBox,double dNmsThreshold)
{
	bool bRestartLoop = false;
	if (ResultBox.size() > 1)
	{
		for (auto itCurrentBox = ResultBox.begin(); itCurrentBox < ResultBox.end() - 1;)
		{
			bRestartLoop = false;
			for (auto itNextBox = itCurrentBox + 1; itNextBox < ResultBox.end();)
			{
				if (CalcIOU(*itCurrentBox, *itNextBox) > dNmsThreshold)
				{
					printf("IOU:%f\n", CalcIOU(*itCurrentBox, *itNextBox));
					if (itCurrentBox->prob > itNextBox->prob)
					{
						itNextBox = ResultBox.erase(itNextBox);
					}
					else
					{
						itCurrentBox = ResultBox.erase(itCurrentBox);
						bRestartLoop = true;
						break;
					}
				}
				else ++itNextBox;
			}
			if (!bRestartLoop) ++itCurrentBox;
		}
	}
	
		
}

/* Hieu Tang 2020-06-10
 * Calculate intersect between 2 box
*/
double YoloDetector::CalcIntersect(bbox_t & box1, bbox_t & box2)
{
	double h, w;
	if (box1.x < box2.x)
	{
		if (box1.x + box1.w <= box2.x) return 0;
		else if (box1.x + box1.w <= box2.x + box2.w) w = box1.x + box1.w - box2.x;
		else if (box1.x + box1.w > box2.x + box2.w) w = box2.w;
		else w = 0;
	}
	else
	{
		if (box2.x + box2.w <= box1.x) return 0;
		else if (box2.x + box2.w <= box1.x + box1.w) w = box2.x + box2.w - box1.x;
		else if (box2.x + box2.w > box1.x + box1.w) w = box1.w;
		else w = 0;
	}
	if (box1.y < box2.y)
	{
		if (box1.y + box1.h <= box2.y) return 0;
		else if (box1.y + box1.h <= box2.y + box2.h) h = box1.y + box1.h - box2.y;
		else if (box1.y + box1.h > box2.y + box2.h) h = box2.h;
		else h = 0;
	}
	else
	{
		if (box2.y + box2.h <= box1.y) return 0;
		else if (box2.y + box2.h <= box1.y + box1.h) h = box2.y + box2.h - box1.y;
		else if (box2.y + box2.h > box1.y + box1.h) h = box1.h;
		else h = 0;
	}
	return w*h;
}

double YoloDetector::CalcUnion(bbox_t & box1, bbox_t & box2)
{
	return box1.w*box1.h + box2.w*box2.h - CalcIntersect(box1, box2);
}

double YoloDetector::CalcIOU(bbox_t & box1, bbox_t & box2)
{
	return CalcIntersect(box1,box2)/CalcUnion(box1,box2);
}

/* Helper function for ShortDigitInLP()
 * Sort digit in box vector by its position
*/
std::vector<int> YoloDetector::SortDigit(std::vector<bbox_t>& bBoxes)
{
	//Short by x first, we use insertion sort
	//std::vector<int> iIndexSortedByX;
	std::vector<int> iXSortedValues;
	m_iSortedIds.clear();
	iXSortedValues.clear();
	for (int iIndex = 0; iIndex < bBoxes.size(); ++iIndex)
	{
		auto itUpperBound = std::upper_bound(iXSortedValues.begin(), iXSortedValues.end(), bBoxes[iIndex].x);
		int iDistanceFromBegin2InsertPoint = std::distance(iXSortedValues.begin(), itUpperBound);
		iXSortedValues.insert(itUpperBound, bBoxes[iIndex].x);
		m_iSortedIds.insert(m_iSortedIds.begin() + iDistanceFromBegin2InsertPoint, iIndex);
	}

	//sorted by y
	std::vector<int> iIndexSortedByYInSecondLine;
	iIndexSortedByYInSecondLine.clear();
	bool bIs2LinesUpper = false;//lower line
	for (auto _itIndexSortedByX = m_iSortedIds.begin(); _itIndexSortedByX < m_iSortedIds.end() -1;)
	{
		int _iPreIndexInbBoxes =  *_itIndexSortedByX;
		int _iPostIndexInbBoxes = *(_itIndexSortedByX + 1);
		float atan = (bBoxes[_iPostIndexInbBoxes].y - bBoxes[_iPreIndexInbBoxes].y) /
			(float)(bBoxes[_iPostIndexInbBoxes].x - bBoxes[_iPreIndexInbBoxes].x);
		//45 degree has tan = 1 since sin(45)=cos(45)
		if (abs(atan) > 1.0)
		{
			if (atan>0) bIs2LinesUpper = true;
			iIndexSortedByYInSecondLine.push_back(_iPostIndexInbBoxes);
			m_iSortedIds.erase(_itIndexSortedByX + 1);
		}
		else ++_itIndexSortedByX;
	}
	//merge 2 lines.
	if (iIndexSortedByYInSecondLine.size() > 0)
		if (bIs2LinesUpper) m_iSortedIds.insert(m_iSortedIds.begin(),
			iIndexSortedByYInSecondLine.begin(), iIndexSortedByYInSecondLine.end());
		else m_iSortedIds.insert(m_iSortedIds.end(),
			iIndexSortedByYInSecondLine.begin(), iIndexSortedByYInSecondLine.end());
	return m_iSortedIds;
}
