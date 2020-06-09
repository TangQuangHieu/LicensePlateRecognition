
// LicensePlateRecognitionDlg.h : header file
//
#pragma once
#include "Ini.h"
#include "PreprocessImage.h"
#include "Detector.h"

//Enum for choosing state
//enum state_choose{ONE_STATE, TWO_STATE};

//Enum for chosing yolo list
//enum yolo_list { YOLOV2, YOLOV3, YOLOV3_MOD, YOLOV4 };
//
//const std::vector<std::wstring> wstrKoreanObjectNames = {
//L"WHITE LONG", 
//L"WHITE SHORT", 
//L"YELLOW SHORT", 
//L"YELLOW LONG", 
//L"GREEN OLD", 
//L"GREEN NEW", L"AMBASS", L"CONSTR", L"INTER", L"ARMY",
//L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"",
//L"��", L"��", L"��", L"��", L"��", L"��", L"ȣ", L"��", L"", L"", L"��", L"��", L"��", L"��", L"��", L"��", L"��",
//L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��",
//L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"", L"", L"",
//L"����", L"�λ�", L"�뱸", L"��õ", L"����", L"����", L"���", L"����", L"", L"", L"���", L"����", L"���",
//L"�泲", L"����", L"����", L"���", L"�泲", L"����"};
//
//const std::vector<std::wstring> wstrKoreanObjectNames1Stage = {
//L"Plate",
//L"Normal",
//L"Old",
//L"2004",
//L"2005",
//L"Special", L"2006 Local", L"CONSTR", L"INTER", L"Ambass",
//L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", 
//L"��", L"��", L"��", L"��", L"��", L"��", L"ȣ", L"��",  L"��", L"��", L"��", L"��", L"��", L"��", L"��",
//L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��",
//L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��", 
//L"����", L"�λ�", L"�뱸", L"��õ", L"����", L"����", L"���", L"����", L"���", L"����", L"���",
//L"�泲", L"����", L"����", L"���", L"�泲", L"����" };

// CLicensePlateRecognitionDlg dialog
class CLicensePlateRecognitionDlg : public CDialogEx
{
// Construction
public:
	CLicensePlateRecognitionDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LICENSEPLATERECOGNITION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	
	


	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnBnClickedButtonOpen();
	//use ini file for this program to read configuration file
	mg::CIni m_hIni;

	// use to rotate image by degree
	CSliderCtrl m_hRotateImageSlideBar;

	// use to increase or decrease lighting of image
	CSliderCtrl m_hLightingImageSlideBar;

	// Show the value of rotate image
	CString m_hRotateImageNumShow;

	// Show the lighting in image compare with old lighting in original one
	CString m_hLightingImageNumShow;
	
	// Choose whether using 1 state or 2 state detector
	CComboBox m_hChooseState;

	// choose state 1 type(Yolov2 or Yolov3 or Yolov3 mod) for both single state or concatenated state( 2 state)
	CComboBox m_hState1DetectType;

	// Choose the detector type for 2nd state (Only available for 2 state detectors)
	CComboBox m_hState2DetectType;

	// check it if we want to use histogram equalization
	BOOL m_hHistogramEqual;
	
	// Use if we want to blur image ( just for experiment)
	BOOL m_hBlurImage;

	//use for choosing whether one state or 2 state
	static std::map< state_choose, CString> m_MapForChooseState;

	//use for choosing the type of detector in each state( YOLOV2, YOLOV3, YOLOV3, YOLOV3_MOD)
	static std::map< yolo_list, CString> m_MapForChooseDetectorType;
	afx_msg void OnCbnSelchangeComboChooseState();
	
	
	//Object contains all image path list
	CListBox m_hListImagePath;
	
	void LoadImageListPath();
	CStatic m_hImageControl;
	cv::Mat  m_hImage;
	cv::Mat m_hPreprocessImage;//preprocessed image

	//Detector
	YoloDetector* m_pDetector = NULL;
	CString m_sDetectorPath;//Path to detector folder, one upper folder of stage1 and stage2 folder, initial in OninitDialog()
	std::vector<std::string> m_strObjectNames;//Contains object names
	cv::RNG m_rngRandomColorGenerator;	//Random generator, generate color of object's bbox for drawing
	//Initial color lookup table for YoloDetector class
	void OnInitYoloResultColor();

	//Use for drawing m_hImage on m_hImageControl
	void OnDrawObject(cv::Mat& mDrawImage);

	//initial or reinitial all scroll bar
	void OnInitialScrollBars();
private:

	//function used for initilize properly list box, if the path of text in list box is too lenghthy, we make the horizontal scrool bar to that text
	void OnReDrawHorizontalScroolBar();

	void UpdateStatusResultWindow();
	int AppendToLogAndScroll(CString str, COLORREF color, UINT iFontSize =0);
	int GetNumVisibleLines(CRichEditCtrl* pCtrl);
	void PutFPSOnImage(cv::Mat& mImage);
	void RunVideo();
	void InitKoreanObjectNames();
public:
	cv::Mat m_hLoloImage;//image logo
	// Hold the folder containing images
	CEdit m_hImageDir;
	afx_msg void OnLbnDblclkListImage();
	//// Show status and result information
	//CEdit m_hStatusResultWindow;
	//afx_msg void OnBnClickedButtonLoadImage();
	afx_msg void OnButtonLoadImage();
	afx_msg void OnButtonOpen();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg void OnBnClickedCheckHistogramEqual();
	afx_msg void OnBnClickedCheckBlurImage();
	afx_msg void OnBnClickedButtonProcess();
	CRichEditCtrl m_hStatusWindow;
	afx_msg void OnBnClickedButtonLoadVideo();
	cv::VideoCapture m_hVideoCapture;//object used for loading video
	cv::VideoWriter m_hVideoWriter;//
	bool m_bIsLoadVideo;//TRUE when video loaded, and FALSE when we stop loading video
	CString m_hVideoPath;//The path to video
	afx_msg void OnBnClickedButtonClear();
	std::vector<std::wstring> m_cstrKoreanObjectNames;
};
