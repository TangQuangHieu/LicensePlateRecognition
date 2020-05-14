
// LicensePlateRecognitionDlg.h : header file
//
#pragma once
#include "Ini.h"
#include "PreprocessImage.h"

//Enum for choosing state
enum state_choose{ONE_STATE, TWO_STATE};

//Enum for chosing yolo list
enum yolo_list { YOLOV2, YOLOV3, YOLOV3_MOD, YOLOV4 };


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
	afx_msg void OnBnClickedButtonOpen();
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

	//Use for drawing m_hImage on m_hImageControl
	void OnDrawObject();
private:

	//function used for initilize properly list box, if the path of text in list box is too lenghthy, we make the horizontal scrool bar to that text
	void OnReDrawHorizontalScroolBar();

public:
	// Hold the folder containing images
	CEdit m_hImageDir;
	afx_msg void OnLbnDblclkListImage();
	// Show status and result information
	CEdit m_hStatusResultWindow;
};
