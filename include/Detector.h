#pragma once
#include "yolo_v2_class.hpp"

//Enum for choosing state
enum state_choose { ONE_STATE, TWO_STATE };

//Enum for chosing yolo list
enum yolo_list { YOLOV2, YOLOV3, YOLOV3_MOD};
typedef struct _DetectorInformation
{
	std::string sDetectorName;//Name of detector
	std::string sDetectorPath;//Path to detector folder
	std::string sNamePath;//path to data
	std::string sConfigPath;//path to config
	std::string sWeightPath;//path to weight
} DetectorInformation;
//Class use to wrap the actual yolo class, useful for 2 detectors stages
class YoloDetector 
{
public:

	//Construct with number of detector and type of them
	YoloDetector(const state_choose& sYoloStages, const std::vector<yolo_list>& yYoloListTypes, const std::string& sDetectorFolderPath);

	//initial actual detector, the inputs are yDetectorType and 
	void InitialDetector(Detector* pDetector, yolo_list& yDetectorType, DetectorInformation& dDetectorInformation, int iWhatStage=0);
	//Use for detect 1 stage
	void SingleDetect(cv::Mat& mImage);
	
	//Use for detect 2 stages
	void CasCadeDetect(cv::Mat& mImage);

	//get box result
	std::vector<std::vector<bbox_t>>& GetResultBoxes() const;

	//Draw into image
	void DrawImage(cv::Mat& mImage);

	void SetDetectorFolder(const std::string& sDetectorFolderPath);
	//variable definition
private:
	state_choose m_sYoloStages;// yolo stages ( 1 stage or 2 stages)
	std::vector<yolo_list> m_yYoloListTypes;//yolo list for each stage
	std::string m_sDetectorFolderPath;//store path to detectors folder
	DetectorInformation	m_dDetectorInformation[2];//each detector information
	Detector* m_pDetectors[2];//2 detector
	std::vector<bbox_t> m_bResultBoxes[2];
	
};