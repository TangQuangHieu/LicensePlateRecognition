#pragma once
#include "yolo_v2_class.hpp"
#include <string>
//Enum for choosing state
enum state_choose { ONE_STATE, TWO_STATE };

//Enum for chosing yolo list
enum yolo_list { YOLOV2, YOLOV3, YOLOV3_MOD};
typedef struct _DetectorInformation
{
	std::string					sDetectorName;		//Name of detector
	std::string					sDetectorPath;		//Path to detector folder
	std::string					sNamePath;			//path to data
	std::string					sConfigPath;		//path to config
	std::string					sWeightPath;		//path to weight
	std::vector<std::string>	sClassNames;		//name of all classes of detector
} DetectorInformation;
//Class use to wrap the actual yolo class, useful for 2 detectors stages
class YoloDetector 
{
public:
	//initilize outside class, put in LicensePlateRecognitionDlg 
	static std::vector<cv::Scalar> m_sRandomColorLookupTable;	//use to store color of each class to draw in image
	static int m_iNumOfGpus;			//The number of gpu
	//Construct with number of detector and type of them
	YoloDetector(const state_choose& sYoloStages, const std::vector<yolo_list>& yYoloListTypes, const std::string& sDetectorFolderPath);

	//initial actual detector, the inputs are yDetectorType and 
	void InitialDetector(Detector*& pDetector, yolo_list& yDetectorType, DetectorInformation& dDetectorInformation, int iWhatStage=0);
	//Read Name From name file
	void ReadNameFromFile(std::string& sNameFilePath, std::vector<std::string>& sClassName);

	//Use to summary all kind of detect base on the 1 stage or 2 stage
	void Detect(cv::Mat mImage);

	//Use for detect 1 stage
	void SingleDetect(Detector*& Detector, cv::Mat  mImage, std::vector<bbox_t>& bResultBoxes);
	void DetectOnlyStage1(cv::Mat mImage);
	
	//Use for detect 2 stages
	void CasCadeDetect(cv::Mat mImage);
	void DoNMSAllClasses(std::vector<bbox_t > & bResultBoxes);
	

	//Show the result in the console
	void ShowResultInConsole(std::vector<bbox_t > & bResultBoxes, std::vector<std::string>& sClassNames, std::vector<int> iSortedIds = {});
	//get box result
	std::array<std::vector<bbox_t>, 2> GetResultBoxes() const;

	//Draw into image
	void DrawImage(cv::Mat& mImage, std::vector<bbox_t>& bBoxes, 
		std::vector<std::string>& sClassNames);

	void SetDetectorFolder(const std::string& sDetectorFolderPath);
	//variable definition
	std::vector<int> ShortDigitInLP();
	
	//Interface to get the variables
	DetectorInformation GetDetectionInformation(int iStage)const;
	//Get num of state in Yolo detector
	state_choose GetStageNum() const;
	//Get object name
	std::vector<std::string> GetObjectNames(int iStageNum) const;
	//get detection time
	double GetDetectionTime() const;
	//Get sorted id
	std::vector<int> GetSortedIDs() const;
	void DoNMSByIouAllClasses(std::vector<bbox_t>& ResultBox, double dNmsThreshold=0.6);
	double CalcIntersect(bbox_t& box1, bbox_t& box2);
	double CalcUnion(bbox_t& box1, bbox_t& box2);
	double CalcIOU(bbox_t& box1, bbox_t& box2);
private:
	std::vector<int> SortDigit(std::vector<bbox_t>& bBoxes);
private:
	state_choose m_sYoloStages;// yolo stages ( 1 stage or 2 stages)
	std::vector<yolo_list> m_yYoloListTypes;//yolo list for each stage
	std::string m_sDetectorFolderPath;//store path to detectors folder
	DetectorInformation	m_dDetectorInformation[2];//each detector information
	std::array<Detector*,2> m_pDetectors;//2 detector
	std::array<std::vector<bbox_t>,2> m_bResultBoxes;
	double m_dDetectionTime;//calculate detection time
	std::vector<int> m_iSortedIds;//Sorted ID order of m_bResultBoxes 
	
};