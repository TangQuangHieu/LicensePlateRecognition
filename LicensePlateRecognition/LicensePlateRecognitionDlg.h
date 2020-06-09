
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
//L"바", L"사", L"아", L"자", L"하", L"허", L"호", L"배", L"", L"", L"가", L"나", L"다", L"라", L"마", L"거", L"너",
//L"더", L"러", L"머", L"버", L"서", L"어", L"저", L"고", L"노", L"도", L"로", L"모", L"보", L"소", L"오", L"조",
//L"구", L"누", L"두", L"루", L"무", L"부", L"수", L"우", L"주", L"육", L"해", L"공", L"국", L"합", L"", L"", L"",
//L"서울", L"부산", L"대구", L"인천", L"광주", L"대전", L"울산", L"세정", L"", L"", L"경기", L"강원", L"충북",
//L"충남", L"전북", L"전남", L"경북", L"경남", L"제주"};
//
//const std::vector<std::wstring> wstrKoreanObjectNames1Stage = {
//L"Plate",
//L"Normal",
//L"Old",
//L"2004",
//L"2005",
//L"Special", L"2006 Local", L"CONSTR", L"INTER", L"Ambass",
//L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", 
//L"바", L"사", L"아", L"자", L"하", L"허", L"호", L"배",  L"가", L"나", L"다", L"라", L"마", L"거", L"너",
//L"더", L"러", L"머", L"버", L"서", L"어", L"저", L"고", L"노", L"도", L"로", L"모", L"보", L"소", L"오", L"조",
//L"구", L"누", L"두", L"루", L"무", L"부", L"수", L"우", L"주", L"육", L"해", L"공", L"국", L"합", 
//L"서울", L"부산", L"대구", L"인천", L"광주", L"대전", L"울산", L"세정", L"경기", L"강원", L"충북",
//L"충남", L"전북", L"전남", L"경북", L"경남", L"제주" };

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
