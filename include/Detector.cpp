#include "stdafx.h"
#include "Detector.h"

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
void YoloDetector::InitialDetector(Detector* pDetector, yolo_list& yDetectorType, DetectorInformation& dDetectorInformation, int iWhatStage)
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
	pDetector = new Detector(dDetectorInformation.sConfigPath, dDetectorInformation.sWeightPath, iWhatStage);
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
void YoloDetector::Detect(cv::Mat & mImage)
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
void YoloDetector::DetectOnlyStage1(cv::Mat& mImage)
{
	SingleDetect(m_pDetectors[0], mImage, m_bResultBoxes[0]);
	//MNS through all classes, 1 license plate just have 1 character and 1 province( if it has province)
	DoNMSAllClasses(m_bResultBoxes[0]);
	ShowResultInConsole(m_bResultBoxes[0], m_dDetectorInformation[0].sClassNames);

}

//detect only one single detector
/* Detect in single stage
 * Detector:	Input detector
 * mImage:		Input Image
 * bResulBoxes: Output result box
*/
void YoloDetector::SingleDetect(Detector* Detector, cv::Mat & mImage, std::vector<bbox_t>& bResultBoxes)
{
	bResultBoxes = Detector->detect(mImage);
}



//Detect 2 detectors
void YoloDetector::CasCadeDetect(cv::Mat & mImage)
{
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
		ShowResultInConsole(m_bResultBoxes[1], m_dDetectorInformation[1].sClassNames);
	}
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
	}
}

/* Show the result into a stream
 * oStream is input stream ( can be file or out stream ...)
 * bResultBoxes is the vector of result
*/
void YoloDetector::ShowResultInConsole(std::vector<bbox_t>& bResultBoxes, std::vector<std::string>& sClassNames)
{
	for (auto& bBox : bResultBoxes)
	{
		std::cout <<std::setw(10)<<"Name: "<< sClassNames[bBox.obj_id]<<"; x:" << bBox.x << "; y:" << bBox.y << "; w:" << bBox.w << "; h:" << bBox.h << std::endl;
	}
}

std::array<std::vector<bbox_t>,2> YoloDetector::GetResultBoxes() const 
{
	return m_bResultBoxes;
}
