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
	//Detector(std::string cfg_filename, std::string weight_filename, int gpu_id = 0);
	pDetector = new Detector(dDetectorInformation.sConfigPath, dDetectorInformation.sWeightPath, iWhatStage);
}

//detect only one single detector
void YoloDetector::SingleDetect(cv::Mat & mImage)
{
	m_bResultBoxes[0] = m_pDetectors[0]->detect(mImage);
	
}

//Detect 2 detectors
void YoloDetector::CasCadeDetect(cv::Mat & mImage)
{
	SingleDetect(mImage);
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
	cv::Mat CroppedImg;
	if (m_bResultBoxes[0].size() == 1)
	{
		if (m_bResultBoxes[0][0].x < 0)m_bResultBoxes[0][0].x == 0;
		if (m_bResultBoxes[0][0].y < 0)m_bResultBoxes[0][0].y == 0;
		if (m_bResultBoxes[0][0].x >= mImage.cols)m_bResultBoxes[0][0].x == mImage.cols-1;
		if (m_bResultBoxes[0][0].y >= mImage.rows)m_bResultBoxes[0][0].y == mImage.rows-1;
		
	}
}


std::vector<std::vector<bbox_t>>& YoloDetector::GetResultBoxes() const
{
	return m_bResultBoxes;
}
