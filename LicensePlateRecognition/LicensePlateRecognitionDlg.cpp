
// LicensePlateRecognitionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LicensePlateRecognition.h"
#include "LicensePlateRecognitionDlg.h"
#include "afxdialogex.h"
#include "PreprocessImage.h"
#include "Define.h"

namespace fs = std::filesystem;
//using namespace cv;
std::vector<cv::Scalar> YoloDetector::m_sRandomColorLookupTable = { cv::Scalar(0,0,0) };
int YoloDetector::m_iNumOfGpus = 0;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//use for choosing whether one state or 2 state (ONE_STATE, TWO_STATE)
std::map< state_choose, CString> CLicensePlateRecognitionDlg::m_MapForChooseState = { {ONE_STATE,"One State"},{TWO_STATE,"Two State"} };

//use for choosing the type of detector in each state( YOLOV2, YOLOV3,YOLOV3_MOD, YOLOV4 )
std::map< yolo_list, CString> CLicensePlateRecognitionDlg::m_MapForChooseDetectorType = { {YOLOV2,"Yolo V2"},{YOLOV3,"Yolo V3"},{YOLOV3_MOD,"YoloV3 Mod"}};
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLicensePlateRecognitionDlg dialog



CLicensePlateRecognitionDlg::CLicensePlateRecognitionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LICENSEPLATERECOGNITION_DIALOG, pParent)
	, m_hRotateImageNumShow(_T("0"))
	, m_hLightingImageNumShow(_T("0"))
	, m_hHistogramEqual(FALSE)
	, m_hBlurImage(FALSE)
{
	//call this function to initial richedit2 library
//::AfxInitRichEdit();
	::AfxInitRichEdit2();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLicensePlateRecognitionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SLIDER_ROTATE_IMAGE, m_hRotateImageSlideBar);
	DDX_Control(pDX, IDC_SLIDER_LIGHTING_IMAGE, m_hLightingImageSlideBar);
	DDX_Text(pDX, IDC_EDIT_ROTATE_IMAGE, m_hRotateImageNumShow);
	DDX_Text(pDX, IDC_EDIT_LIGHTING_IMAGE, m_hLightingImageNumShow);
	DDX_Control(pDX, IDC_COMBO_CHOOSE_STATE, m_hChooseState);
	DDX_Control(pDX, IDC_COMBO_STATE_1, m_hState1DetectType);
	DDX_Control(pDX, IDC_COMBO_STATE_2, m_hState2DetectType);
	DDX_Check(pDX, IDC_CHECK_HISTOGRAM_EQUAL, m_hHistogramEqual);
	DDX_Check(pDX, IDC_CHECK_BLUR_IMAGE, m_hBlurImage);
	DDX_Control(pDX, IDC_LIST_IMAGE, m_hListImagePath);
	DDX_Control(pDX, IDC_PICTURE_CONTROL, m_hImageControl);
	DDX_Control(pDX, IDC_EDIT_IMAGE_PATH, m_hImageDir);
	//DDX_Control(pDX, IDC_EDIT_RESULT, m_hStatusResultWindow);
	DDX_Control(pDX, IDC_RICHEDIT_STATUS_WINDOW, m_hStatusWindow);
}

BEGIN_MESSAGE_MAP(CLicensePlateRecognitionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_BN_CLICKED(IDC_BUTTON_OPEN, &CLicensePlateRecognitionDlg::OnBnClickedButtonOpen)
	ON_CBN_SELCHANGE(IDC_COMBO_CHOOSE_STATE, &CLicensePlateRecognitionDlg::OnCbnSelchangeComboChooseState)
	ON_LBN_DBLCLK(IDC_LIST_IMAGE, &CLicensePlateRecognitionDlg::OnLbnDblclkListImage)
	//ON_BN_CLICKED(IDC_BUTTON_LOAD_IMAGE, &CLicensePlateRecognitionDlg::OnBnClickedButtonLoadImage)
	ON_COMMAND(IDC_BUTTON_LOAD_IMAGE, &CLicensePlateRecognitionDlg::OnButtonLoadImage)
	ON_COMMAND(IDC_BUTTON_OPEN, &CLicensePlateRecognitionDlg::OnButtonOpen)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_HISTOGRAM_EQUAL, &CLicensePlateRecognitionDlg::OnBnClickedCheckHistogramEqual)
	ON_BN_CLICKED(IDC_CHECK_BLUR_IMAGE, &CLicensePlateRecognitionDlg::OnBnClickedCheckBlurImage)
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, &CLicensePlateRecognitionDlg::OnBnClickedButtonProcess)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_VIDEO, &CLicensePlateRecognitionDlg::OnBnClickedButtonLoadVideo)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CLicensePlateRecognitionDlg::OnBnClickedButtonClear)
END_MESSAGE_MAP()


// CLicensePlateRecognitionDlg message handlers

BOOL CLicensePlateRecognitionDlg::OnInitDialog()
{

	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_hImageDir.SetWindowTextA(m_hIni.GetString("Globals", "ImagePath", ""));
	LoadImageListPath();

	//Initilize detector choose combo list
	for (auto element:m_MapForChooseState) m_hChooseState.AddString(_T(element.second));
	m_hChooseState.SetCurSel(1);

	//Initilize detector for state 1 style
	for (auto element : m_MapForChooseDetectorType) m_hState1DetectType.AddString(_T(element.second));
	m_hState1DetectType.SetCurSel(1);

	//Initilize detector for state 2 style
	for (auto element : m_MapForChooseDetectorType) m_hState2DetectType.AddString(_T(element.second));
	m_hState2DetectType.SetCurSel(1);

	OnInitialScrollBars();
	
	//Initial the detector folder for program
	m_sDetectorPath = m_hIni.GetString("Globals", "DetectorPath", "..\\..\\detectors");
	m_strObjectNames.clear();//clear object name

	//Initial load video false
	m_bIsLoadVideo = false;
	YoloDetector::m_iNumOfGpus = m_hIni.GetInt("Globals", "NumOfGpus", 1);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLicensePlateRecognitionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLicensePlateRecognitionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	//Display school's logo
	CString cstrLogoPath = MgGetModuleFileDirectory();
	cstrLogoPath.Append("\\" + m_hIni.GetString("Globals", "LogoPath", ""));
	m_hLoloImage = cv::imread((std::string)CT2CA(cstrLogoPath), cv::IMREAD_COLOR);
	OnDrawObject(m_hLoloImage);
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLicensePlateRecognitionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//try to open image directory if we click open
//void CLicensePlateRecognitionDlg::OnBnClickedButtonOpen()
//{
//	// TODO: Add your control notification handler code here
//
//	//Try to get the default image dir from m_hImageDir
//	CString _ImageDir;
//	m_hImageDir.GetWindowText(_ImageDir);
//	CFolderPickerDialog open_folder("", OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_ENABLESIZING, this,
//		sizeof(OPENFILENAME));
//
//	//Check whether the default image dir exist, if so, initial the opening search dialog by that 
//	if (MgIsDirectoryExists(_ImageDir))
//	{
//		open_folder.m_ofn.lpstrInitialDir = _ImageDir;
//	}
//	open_folder.DoModal();
//	//After select the folder, reload it into m_hImageDir and reload the m_hListImagePath
//	m_hImageDir.SetWindowTextA(open_folder.GetPathName());
//	LoadImageListPath();
//}


//When click ..., open the folder which its path is in m_hImageDir
void CLicensePlateRecognitionDlg::OnCbnSelchangeComboChooseState()
{
	// TODO: Add your control notification handler code here
	//m_hChooseState.GetLBText(m_hChooseState.GetCurSel(), m_strTextCtrl);
	UpdateData(FALSE);
}


//Load list of all path in folder onto m_hListImagePath
void CLicensePlateRecognitionDlg::LoadImageListPath()
{
	CString _ImageDir;
	m_hImageDir.GetWindowText(_ImageDir);
	if (MgIsDirectoryExists((LPCTSTR)_ImageDir))
	{

		//Reset all old content
		m_hListImagePath.ResetContent();
		
		//If file is exist, open and get all jpg images inside folder, not recursive =.=
		for (const auto& entry : fs::directory_iterator(std::string(_ImageDir)))
		{
			//std::cout << entry.path() << std::endl;
			auto &pathfile = entry.path();
			if (pathfile.extension() == ".jpg")
			{
				m_hListImagePath.AddString(CW2T((WCHAR*)pathfile.c_str()));
			}
		}
		OnReDrawHorizontalScroolBar();

	}
}

/* Function to initilize color table result for YOloDetector class
 * set the sRandomColorLookupTable in YoloDetector for each class
*/
void CLicensePlateRecognitionDlg::OnInitYoloResultColor()
{
	std::vector<std::string> strObjectNames;
	if (m_pDetector != NULL && m_strObjectNames.size()==0)
	{
		switch (m_pDetector->GetStageNum())
		{
		case state_choose::ONE_STATE:
		{
			m_strObjectNames = (m_pDetector->GetObjectNames(0));
			break;
		}
		case state_choose::TWO_STATE:
		{
			m_strObjectNames = (m_pDetector->GetObjectNames(1));
			break;
		}
		}
		for (int _ObjectIdex = 0; _ObjectIdex < m_strObjectNames.size(); ++_ObjectIdex)
		{
			cv::Scalar sColor;

			sColor.val[0] = m_rngRandomColorGenerator.uniform(0, 7) << 6;
			sColor.val[1] = m_rngRandomColorGenerator.uniform(0, 7) << 6;
			sColor.val[2] = m_rngRandomColorGenerator.uniform(0, 7) << 6;

			if (abs((int)sColor.val[0] - 128) < 30 &&
				abs((int)sColor.val[0] - 128) < 30 &&
				abs((int)sColor.val[0] - 128) < 30)
			{
				sColor.val[m_rngRandomColorGenerator.uniform(0, 2)] = 0;
			}
			YoloDetector::m_sRandomColorLookupTable.push_back(sColor);
		}
	}
	
}
//Use for drawing m_hImage on m_hImageControl
void CLicensePlateRecognitionDlg::OnDrawObject(cv::Mat& mDrawImage, bool bDisplayText)
{
	if (!mDrawImage.empty())
	{
		RECT r;
		m_hImageControl.GetClientRect(&r);
		cv::Size winSize(480, 480);
		cv::Mat cvImgTmp(winSize, CV_8UC3);
		if (mDrawImage.size() != winSize)
		{
			cv::resize(mDrawImage, cvImgTmp, winSize);
		}
		else
		{
			cvImgTmp = mDrawImage.clone();
		}
#ifdef _DEBUG
		{
			cv::imshow("Temp Mat", cvImgTmp);
			cv::waitKey(0);
			//rotate image
			//cv::flip(cvImgTmp, cvImgTmp,0);
			cv::imshow("Flipped Mat", cvImgTmp);
			cv::waitKey(0);
		}
#endif
		//create MFC image
		CImage* mfcImg = new CImage();
		mfcImg->Create(winSize.width, winSize.height, 24);
		//Create header for mfc
		BITMAPINFO bitInfo;
		bitInfo.bmiHeader.biBitCount = 24;
		bitInfo.bmiHeader.biWidth = winSize.width;
		bitInfo.bmiHeader.biHeight = -winSize.height;
		bitInfo.bmiHeader.biPlanes = 1;
		bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitInfo.bmiHeader.biCompression = BI_RGB;
		bitInfo.bmiHeader.biClrImportant = 0;
		bitInfo.bmiHeader.biClrUsed = 0;
		bitInfo.bmiHeader.biSizeImage = 3 * winSize.width*winSize.height;
		bitInfo.bmiHeader.biXPelsPerMeter = 0;
		bitInfo.bmiHeader.biYPelsPerMeter = 0;
		//Add header and OpenCV image's data to mfcImg
		StretchDIBits(mfcImg->GetDC(), 0, 0,
			winSize.width, winSize.height, 0, 0,
			winSize.width, winSize.height,
			cvImgTmp.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY
		);
		
		//Display license plate into image
		if(bDisplayText)
			DisplayKoreanLicensePlateOnImage(*mfcImg);

		//Display mfcImg in MFC window
		mfcImg->BitBlt(::GetDC(m_hImageControl.m_hWnd), 0, 0);
		//Release mfcImg
		if (mfcImg)
		{
			mfcImg->ReleaseDC();
			delete mfcImg; mfcImg = NULL;
		}
	}
}

void CLicensePlateRecognitionDlg::OnInitialScrollBars()
{
	// Initialize for rotate slide bar
	m_hRotateImageSlideBar.SetRange(-10, 10, TRUE);
	m_hRotateImageSlideBar.SetPos(0);
	m_hRotateImageNumShow.Format("%d",m_hRotateImageSlideBar.GetPos());
	SetDlgItemText(IDC_EDIT_ROTATE_IMAGE, m_hRotateImageNumShow);
	//Initilize for lighting slide bar
	m_hLightingImageSlideBar.SetRange(-100, 100, TRUE);
	m_hLightingImageSlideBar.SetPos(0);
	m_hLightingImageNumShow.Format("%d", m_hLightingImageSlideBar.GetPos());
	SetDlgItemText(IDC_SLIDER_LIGHTING_IMAGE, m_hRotateImageNumShow);
}

//Redraw horizonetal scrool bar for m_hListImagePath if the text is lenghthy
void CLicensePlateRecognitionDlg::OnReDrawHorizontalScroolBar()
{
	CString str;
	CSize sz;
	int dx = 0;
	CDC* pDC = m_hListImagePath.GetDC();
	for (int i = 0; i < m_hListImagePath.GetCount(); i++)
	{
		m_hListImagePath.GetText(i, str);
		sz = pDC->GetTextExtent(str);

		if (sz.cx > dx)
			dx = sz.cx;

	}
	m_hListImagePath.ReleaseDC(pDC);

	if (m_hListImagePath.GetHorizontalExtent() < dx)
	{
		m_hListImagePath.SetHorizontalExtent(dx);
		ASSERT(m_hListImagePath.GetHorizontalExtent() == dx);

	}
}

/* Update the status result window by the result text
 * After prediction
*/
void CLicensePlateRecognitionDlg::UpdateStatusResultWindow()
{
	// step1: update result box. Class name;
	//each class have percent and location
	auto& bAllBoxes = m_pDetector->GetResultBoxes();
	std::vector<bbox_t>* pbLincesenBox=NULL;
	std::vector<bbox_t>* pbLicenseType=NULL;
	auto _NumOfState = m_pDetector->GetStageNum();
	if (_NumOfState == state_choose::ONE_STATE)
	{
		pbLincesenBox = &bAllBoxes[0];
		pbLicenseType = &bAllBoxes[1];
	}
	else
	{
		pbLincesenBox = &bAllBoxes[1];
		pbLicenseType = &bAllBoxes[0];
	}
	//Display license plate type
	COLORREF colLicensePlateColor = RGB(0, 0, 0);
	if (pbLicenseType->size()>0)
	{
		colLicensePlateColor = RGB(YoloDetector::m_sRandomColorLookupTable[pbLicenseType[0][0].obj_id].val[0],
			YoloDetector::m_sRandomColorLookupTable[pbLicenseType[0][0].obj_id].val[1],
			YoloDetector::m_sRandomColorLookupTable[pbLicenseType[0][0].obj_id].val[2]);
		CString cstrLicenePlateType{ m_cstrKoreanObjectNames[pbLicenseType[0][0].obj_id].c_str() };
		cstrLicenePlateType.Append("\r\n");
		AppendToLogAndScroll(cstrLicenePlateType, colLicensePlateColor);
	}

	//Display license plate num
	CString cstrObjectName;
	
	if (pbLincesenBox->size()>0)
	{
		for (auto& _iID : m_pDetector->GetSortedIDs())
		{
			CString cstrTempName{ m_cstrKoreanObjectNames[pbLincesenBox[0][_iID].obj_id].c_str() };
			cstrObjectName.Append(cstrTempName);
	
		}
		cstrObjectName.Append("\r\n");
		AppendToLogAndScroll(cstrObjectName, colLicensePlateColor);
		CString cstrDetail;
		for (auto& _iID : m_pDetector->GetSortedIDs())
		{
			CString cstrDetailTemp { m_cstrKoreanObjectNames[pbLincesenBox[0][_iID].obj_id].c_str()};
			cstrDetail.Format("; x:%d; y:%d; w:%d; h:%d; Prob:%2.2f\r\n",
				pbLincesenBox[0][_iID].x, pbLincesenBox[0][_iID].y,
				pbLincesenBox[0][_iID].w, pbLincesenBox[0][_iID].h,
				pbLincesenBox[0][_iID].prob);
				COLORREF colCharacterColor = RGB(YoloDetector::m_sRandomColorLookupTable[pbLincesenBox[0][_iID].obj_id].val[0],
												YoloDetector::m_sRandomColorLookupTable[pbLincesenBox[0][_iID].obj_id].val[1],
												YoloDetector::m_sRandomColorLookupTable[pbLincesenBox[0][_iID].obj_id].val[2]);
				cstrDetailTemp.Append(cstrDetail);
				AppendToLogAndScroll(cstrDetailTemp, colCharacterColor);
		};
	}

	//Display processing time
	CString cstrProcessingTime;
	cstrProcessingTime.Format("Processing Time: %f ms\r\n ------------------------------------\r\n", m_pDetector->GetDetectionTime());
	AppendToLogAndScroll(cstrProcessingTime, colLicensePlateColor);
}

/* Function to add string and its color into console
*/
int CLicensePlateRecognitionDlg::AppendToLogAndScroll(CString str, COLORREF color,UINT iFontSize)
{
	long nVisible = 0;
	long nInsertionPoint = 0;
	CHARFORMAT cf;

	// Initialize character format structure
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR;
	cf.dwEffects = 0; // To disable CFE_AUTOCOLOR

	cf.crTextColor = color;
	// Set insertion point to end of text
	nInsertionPoint = m_hStatusWindow.GetWindowTextLength();
	m_hStatusWindow.SetSel(nInsertionPoint, -1);

	// Set the character format
	m_hStatusWindow.SetSelectionCharFormat(cf);

	// Replace selection. Because we have nothing
	// selected, this will simply insert
	// the string at the current caret position.
	m_hStatusWindow.ReplaceSel(str);

	// Get number of currently visible lines or maximum number of visible lines
	// (We must call GetNumVisibleLines() before the first call to LineScroll()!)
	nVisible = GetNumVisibleLines(&m_hStatusWindow);

	// Now this is the fix of CRichEditCtrl's abnormal behaviour when used
	// in an application not based on dialogs. Checking the focus prevents
	// us from scrolling when the CRichEditCtrl does so automatically,
	// even though ES_AUTOxSCROLL style is NOT set.
	if (&m_hStatusWindow != m_hStatusWindow.GetFocus())
	{
		m_hStatusWindow.LineScroll(INT_MAX);
		m_hStatusWindow.LineScroll(1 - nVisible);
	}

	return 0;
}

/* Helper function of AppendToLogAndScroll to determine the current line in the console
*/
int CLicensePlateRecognitionDlg::GetNumVisibleLines(CRichEditCtrl* pCtrl)
{
	CRect rect;
	long nFirstChar, nLastChar;
	long nFirstLine, nLastLine;

	// Get client rect of rich edit control
	pCtrl->GetClientRect(rect);

	// Get character index close to upper left corner
	nFirstChar = pCtrl->CharFromPos(CPoint(0, 0));

	// Get character index close to lower right corner
	nLastChar = pCtrl->CharFromPos(CPoint(rect.right, rect.bottom));
	if (nLastChar < 0)
	{
		nLastChar = pCtrl->GetTextLength();
	}

	// Convert to lines
	nFirstLine = pCtrl->LineFromChar(nFirstChar);
	nLastLine = pCtrl->LineFromChar(nLastChar);

	return (nLastLine - nFirstLine);
}

/* Put FPS on top right of image by put
 * the detection time to the image
*/
void CLicensePlateRecognitionDlg::PutFPSOnImage(cv::Mat & mImage)
{
	cv::Scalar sColor;
	cv::Scalar sInvColor;
	char chFPS[64];
	cv::Size szFpsTextSize;
	int FontFace = cv::FONT_HERSHEY_COMPLEX_SMALL;
	double FontScale = 2;
	int  baseline;
	auto dComputationTime = m_pDetector->GetDetectionTime();
	float fFPS = 1000 / dComputationTime;	
	//Initial color
	int iRandomNum = (int)(m_rngRandomColorGenerator.uniform(0, YoloDetector::m_sRandomColorLookupTable.size() - 1));
	sColor = YoloDetector::m_sRandomColorLookupTable[iRandomNum];
	sInvColor = cv::Scalar(255, 255, 255) - sColor;
	//calculate the text size
	sprintf(chFPS, "FPS: %5.3f", fFPS);
	szFpsTextSize = cv::getTextSize(chFPS, FontFace, FontScale, 2, &baseline);

	auto& bAllBoxes = m_pDetector->GetResultBoxes();
	std::vector<bbox_t>* pbLincesenBox = NULL;
	std::vector<bbox_t>* pbLicenseType = NULL;
	auto _NumOfState = m_pDetector->GetStageNum();
	std::vector<std::string> strObjectNameList;
	if (_NumOfState == state_choose::ONE_STATE)
	{
		pbLincesenBox = &bAllBoxes[0]; 
		strObjectNameList = m_pDetector->GetDetectionInformation(0).sClassNames;
		pbLicenseType = &bAllBoxes[1];
	}
	else
	{
		pbLincesenBox = &bAllBoxes[1]; 
		strObjectNameList = m_pDetector->GetDetectionInformation(1).sClassNames;
		pbLicenseType = &bAllBoxes[0];
	}

	//Display license plate num
	std::string strObjectNames;
	cv::Size szNameTextSize;
	if (pbLincesenBox->size() > 0)
	{
		for (auto& _iID : m_pDetector->GetSortedIDs())
		{
			
			strObjectNames.append(strObjectNameList[pbLincesenBox[0][_iID].obj_id]);
		}
	}
	szNameTextSize = cv::getTextSize(strObjectNames, FontFace, FontScale, 2, &baseline);
	//Make a retangle to carry the text inside it, the retangle will have the 
	//width and height same as the object name
	cv::Point pTopLeftOfBackground(mImage.cols - (szFpsTextSize.width> szNameTextSize.width? szFpsTextSize.width: szNameTextSize.width) - 1, 0);
	cv::Rect rTextBackground = cv::Rect(pTopLeftOfBackground, 
		cv::Point(mImage.cols - 1, szFpsTextSize.height+ szNameTextSize.height+ 6));
	cv::rectangle(mImage, rTextBackground, sColor, -1);
	//put FPS character in image
	cv::Point pBotLeftofFps(mImage.cols - szFpsTextSize.width - 2, szFpsTextSize.height + 2);//Bottom left
	cv::putText(mImage, chFPS, pBotLeftofFps, FontFace, FontScale, sInvColor, 2);
	//put Name in image
	cv::Point pBotLeftofName(mImage.cols - szNameTextSize.width - 2, 
		szFpsTextSize.height+szNameTextSize.height + 4);//Bottom left
	cv::putText(mImage, strObjectNames, pBotLeftofName, FontFace, FontScale, sInvColor, 2);
	
}

/* Use for running loop video in m_hThread
*/
void CLicensePlateRecognitionDlg::RunVideo()
{
	int iFrameCnt = 0;
	int iTotalFrames = m_hVideoCapture.get(cv::CAP_PROP_FRAME_COUNT);
	while (m_bIsLoadVideo)
	{
		if (!m_hVideoCapture.isOpened()) break;
		m_hVideoCapture >> m_hPreprocessImage; ++iFrameCnt;
		if (iFrameCnt == iTotalFrames)
		{
			m_hVideoCapture.set(cv::CAP_PROP_POS_FRAMES, 0);
			iFrameCnt = 0;
		}
		m_pDetector->Detect(m_hPreprocessImage);
		PutFPSOnImage(m_hPreprocessImage);
		m_hVideoWriter << m_hPreprocessImage;
		OnDrawObject(m_hPreprocessImage,true);
		//Update result to window console
		UpdateStatusResultWindow();
		Sleep(0);
	}
}

/* depend on the stage, we will update korean character look up table
*/
void CLicensePlateRecognitionDlg::InitKoreanObjectNames()
{
	if (m_pDetector != NULL && m_pDetector->GetStageNum() == state_choose::TWO_STATE)
	{
		std::vector<std::wstring> wstrKoreanObjectNames = {
			L"WHITE LONG",
			L"WHITE SHORT",
			L"YELLOW SHORT",
			L"YELLOW LONG",
			L"GREEN OLD",
			L"GREEN NEW", L"AMBASS", L"CONSTR", L"INTER", L"ARMY",
			L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"",
			L"바", L"사", L"아", L"자", L"하", L"허", L"호", L"배", L"", L"", L"가", L"나", L"다", L"라", L"마", L"거", L"너",
			L"더", L"러", L"머", L"버", L"서", L"어", L"저", L"고", L"노", L"도", L"로", L"모", L"보", L"소", L"오", L"조",
			L"구", L"누", L"두", L"루", L"무", L"부", L"수", L"우", L"주", L"육", L"해", L"공", L"국", L"합", L"", L"", L"",
			L"서울", L"부산", L"대구", L"인천", L"광주", L"대전", L"울산", L"세정", L"", L"", L"경기", L"강원", L"충북",
			L"충남", L"전북", L"전남", L"경북", L"경남", L"제주" };
		m_cstrKoreanObjectNames = wstrKoreanObjectNames;
	}
	else
	{
		std::vector<std::wstring> wstrKoreanObjectNames1Stage = {
			L"Plate",
			L"Normal",
			L"Old",
			L"2004",
			L"2005",
			L"Special", L"2006 Local", L"CONSTR", L"INTER", L"Ambass",
			L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9",
			L"바", L"사", L"아", L"자", L"하", L"허", L"호", L"배",  L"가", L"나", L"다", L"라", L"마", L"거", L"너",
			L"더", L"러", L"머", L"버", L"서", L"어", L"저", L"고", L"노", L"도", L"로", L"모", L"보", L"소", L"오", L"조",
			L"구", L"누", L"두", L"루", L"무", L"부", L"수", L"우", L"주", L"육", L"해", L"공", L"국", L"합",
			L"서울", L"부산", L"대구", L"인천", L"광주", L"대전", L"울산", L"세정", L"경기", L"강원", L"충북",
			L"충남", L"전북", L"전남", L"경북", L"경남", L"제주" };
		m_cstrKoreanObjectNames = wstrKoreanObjectNames1Stage;
	}



}

/* Hieu Tang 2020-06-10
 * Display Korean license plate on image
 * using MFC function
 * able to display korean character
*/
void CLicensePlateRecognitionDlg::DisplayKoreanLicensePlateOnImage(CImage& Image)
{
		CString cstrResult;
		auto& iSortedIds = m_pDetector->GetSortedIDs();
		auto& bResultBoxes = m_pDetector->GetResultBoxes();
		
		RECT rDrawArea;
		double dScaleRatioY = (Image.GetHeight() / (double)m_hImage.rows);
		double dScaleRatioX = (Image.GetWidth() / (double)m_hImage.cols);
		rDrawArea.top = 10; rDrawArea.left = Image.GetWidth() / 2-80; rDrawArea.right = Image.GetWidth() / 2 + 80; rDrawArea.bottom = 90;
		if (iSortedIds.size() == 0)
			return;
		else
		{
			//store address of vector of license plate boxes
			const std::vector<bbox_t>* pBoxArray;
			//store address of vector of object name
			if (m_pDetector->GetStageNum() == state_choose::ONE_STATE)
			{
				pBoxArray = &bResultBoxes[0];
				////set draw area lower the license plate
				rDrawArea.left =(LONG)(pBoxArray->at(iSortedIds[0]).x*dScaleRatioX);
				rDrawArea.top = (LONG)((pBoxArray->at(iSortedIds[0]).y + bResultBoxes[0][0].h+30)*dScaleRatioY);
				rDrawArea.right = (LONG)(rDrawArea.left + 200);
				rDrawArea.bottom = (LONG)(rDrawArea.top + 90);
			}
				
			else
			{
				pBoxArray = &bResultBoxes[1];
				////set draw area lower the license plate
				rDrawArea.left = (LONG)(pBoxArray->at(iSortedIds[0]).x*dScaleRatioX);
				rDrawArea.top = (LONG)((pBoxArray->at(iSortedIds[0]).y + bResultBoxes[1][0].h+30)*dScaleRatioY);
				rDrawArea.right = (LONG)(rDrawArea.left + 200);
				rDrawArea.bottom = (LONG)(rDrawArea.top + 90);
			}
				
			/* Drawing section*/
			//Get draw context object to draw text into image
			HDC hdc = Image.GetDC();
			
			//set draw area
		

			for (auto& iBoxId : iSortedIds)
			{
				CString cstrChar(m_cstrKoreanObjectNames[pBoxArray->at(iBoxId).obj_id].c_str());	
				cstrResult.Append(cstrChar);
				
			}
			COLORREF colCharacterColor = RGB(YoloDetector::m_sRandomColorLookupTable[pBoxArray->at(iSortedIds[0]).obj_id].val[0],
				YoloDetector::m_sRandomColorLookupTable[pBoxArray->at(iSortedIds[0]).obj_id].val[1],
				YoloDetector::m_sRandomColorLookupTable[pBoxArray->at(iSortedIds[0]).obj_id].val[2]);
			SetTextColor(hdc, colCharacterColor);
			/* Make font bigger
			*/
			// initialize font here (I prefer CreateFontIndirect to CreateFont,
			// as it's easier to deal with unused params using memset.)
			LOGFONT logFont;
			memset(&logFont, 0, sizeof(logFont));
			logFont.lfHeight = -24; // see PS
			logFont.lfWeight = FW_BOLD;
			strcpy(logFont.lfFaceName, "Arial");
			auto hfont = CreateFontIndirect(&logFont);
			SelectObject(hdc, hfont);
			DrawText(hdc, cstrResult, strlen(cstrResult), &rDrawArea, DT_INTERNAL);
			DeleteObject(hfont);
		}

}

////when double click any record in m_hListImagePath, we load image to m_hImageControl as well as show status to result window m_hStatusResultWindow
void CLicensePlateRecognitionDlg::OnLbnDblclkListImage()
{
	// TODO: Add your control notification handler code here
	//printf("hello world");
	//First get the seletected index from m_hListImagePath and check whether it is valid or not
	int SelectedIndex = m_hListImagePath.GetCurSel();
	if (SelectedIndex != LB_ERR)
	{
		OnInitialScrollBars();//reinitial scroll bar
		CString _ImagePath;
		m_hListImagePath.GetText(SelectedIndex, _ImagePath);
		m_hImage = cv::imread(std::string(_ImagePath),cv::IMREAD_COLOR);
		m_hPreprocessImage = m_hImage.clone();
#ifdef _DEBUG		
		{
			cv::imshow("Input Mat", m_hImage);
			cv::waitKey(0);
		}
#endif
		
		OnDrawObject(m_hImage);
	}
	
}

//when LOAD IMAGE button, we load image to m_hImageControl as well as show status to result window m_hStatusResultWindow
//void CLicensePlateRecognitionDlg::OnBnClickedButtonLoadImage()
//{
//	// TODO: Add your control notification handler code here
//		//First get the seletected index from m_hListImagePath and check whether it is valid or not
//	int SelectedIndex = m_hListImagePath.GetCurSel();
//	if (SelectedIndex != LB_ERR)
//	{
//		CString _ImagePath;
//		m_hListImagePath.GetText(SelectedIndex, _ImagePath);
//		m_hImage = cv::imread(std::string(_ImagePath), IMREAD_COLOR);
//		cv::imshow("Input Mat", m_hImage);
//		cv::waitKey(0);
//		OnDrawObject();
//	}
//}


void CLicensePlateRecognitionDlg::OnButtonLoadImage()
{
	// TODO: Add your command handler code here
	int SelectedIndex = m_hListImagePath.GetCurSel();
	if (SelectedIndex != LB_ERR)
	{
		CString _ImagePath;
		m_hListImagePath.GetText(SelectedIndex, _ImagePath);
		m_hImage = cv::imread(std::string(_ImagePath), cv::IMREAD_COLOR);
		m_hPreprocessImage = m_hImage.clone();
#ifdef _DEBUG
		{
			cv::imshow("Input Mat", m_hImage);
			cv::waitKey(0);
		}
#endif
		OnDrawObject(m_hImage);
	}
}


void CLicensePlateRecognitionDlg::OnButtonOpen()
{
	// TODO: Add your command handler code here
		//Try to get the default image dir from m_hImageDir
	CString _ImageDir;
	m_hImageDir.GetWindowText(_ImageDir);
	CFolderPickerDialog open_folder("", OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_ENABLESIZING, this,
		sizeof(OPENFILENAME));

	//Check whether the default image dir exist, if so, initial the opening search dialog by that 
	if (MgIsDirectoryExists(_ImageDir))
	{
		open_folder.m_ofn.lpstrInitialDir = _ImageDir;
	}
	open_folder.DoModal();
	//After select the folder, reload it into m_hImageDir and reload the m_hListImagePath
	m_hImageDir.SetWindowTextA(open_folder.GetPathName());
	LoadImageListPath();
}

//////////////////////////
// nSBCode: The operation performed on the slider  
// nPos: New position of the slider  
// pScrollBar: The scrollbar (slider ctrl in this case) that sent the notification 
void CLicensePlateRecognitionDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//printf("Change slider bar!");
	CSliderCtrl* pSlider = reinterpret_cast<CSliderCtrl*>(pScrollBar);

	// Check which slider sent the notification  
	if (pSlider == &m_hRotateImageSlideBar)
	{
		int iAngle = m_hRotateImageSlideBar.GetPos();
		m_hRotateImageNumShow.Format("%d", iAngle);
		SetDlgItemText(IDC_EDIT_ROTATE_IMAGE, m_hRotateImageNumShow);
		RotateImage(m_hImage, m_hPreprocessImage, iAngle);
		OnDrawObject(m_hPreprocessImage);
	}
	else if (pSlider == &m_hLightingImageSlideBar)
	{
		int iAlpha = m_hLightingImageSlideBar.GetPos();
		m_hLightingImageNumShow.Format("%d", iAlpha);
		SetDlgItemText(IDC_EDIT_LIGHTING_IMAGE, m_hLightingImageNumShow);
		LightingAdjust(m_hImage, m_hPreprocessImage, iAlpha);
		OnDrawObject(m_hPreprocessImage);
	}
	
}


void CLicensePlateRecognitionDlg::OnBnClickedCheckHistogramEqual()
{
	// TODO: Add your control notification handler code here
	
		CButton * pCheckHistEqual= (CButton*)GetDlgItem(IDC_CHECK_HISTOGRAM_EQUAL);
	if (pCheckHistEqual->GetCheck()==BST_CHECKED)
	{
		HistogramEqualization(m_hImage, m_hPreprocessImage);
		
	}
	else m_hImage.copyTo(m_hPreprocessImage);
	OnDrawObject(m_hPreprocessImage);
}


void CLicensePlateRecognitionDlg::OnBnClickedCheckBlurImage()
{
	// TODO: Add your control notification handler code here
	CButton * pCheckBlur = (CButton*)GetDlgItem(IDC_CHECK_BLUR_IMAGE);
	
	if (pCheckBlur->GetCheck()== BST_CHECKED)
	{
		BlurImage(m_hImage, m_hPreprocessImage);
	}
	else m_hImage.copyTo(m_hPreprocessImage);
	OnDrawObject(m_hPreprocessImage);
}


/* Click Process button
 * Do detect the image in m_hPreprocessImage
 * take the current configure from choose state ( 1 stage or 2 stages
 * configure network from state 1 and state 2
*/
void CLicensePlateRecognitionDlg::OnBnClickedButtonProcess()
{
	// TODO: Add your control notification handler code here
	int iSelIndex = m_hChooseState.GetCurSel();
	CString cstrChooseState;
	if (iSelIndex != LB_ERR)
	{
		m_hChooseState.GetLBText(iSelIndex, cstrChooseState);
		for (auto& _mChooseStatePair : m_MapForChooseState)
			if (cstrChooseState.Compare(_mChooseStatePair.second) == 0)
			{
				//Choose type of each stage
				std::vector<yolo_list> yYoloListTypes(2);
				CString cstrChooseTypes[2];
				m_hState1DetectType.GetLBText(m_hState1DetectType.GetCurSel(), cstrChooseTypes[0]);
				m_hState2DetectType.GetLBText(m_hState2DetectType.GetCurSel(), cstrChooseTypes[1]);
				for (auto &_StageTypePair : m_MapForChooseDetectorType)
				{
					if (cstrChooseTypes[0].Compare(_StageTypePair.second) == 0) yYoloListTypes[0] = _StageTypePair.first;
					if(cstrChooseTypes[1].Compare(_StageTypePair.second) == 0) yYoloListTypes[1] = _StageTypePair.first;
				}

				//YoloDetector(const state_choose& sYoloStages, const std::vector<yolo_list>& yYoloListTypes, const std::string& sDetectorFolderPath)
				if (m_pDetector == NULL)
				{
					AppendToLogAndScroll("Loading Network...\r\n", (0, 0, 0));
					m_pDetector = new YoloDetector(_mChooseStatePair.first, yYoloListTypes, std::string(m_sDetectorPath));
					InitKoreanObjectNames();
					OnInitYoloResultColor();
				}
				if (!m_bIsLoadVideo)
				{
					m_pDetector->Detect(m_hPreprocessImage);
					OnDrawObject(m_hPreprocessImage,true);
					//Update result to window console
					UpdateStatusResultWindow();
				}
				else
				{
					std::thread(&CLicensePlateRecognitionDlg::RunVideo,this).detach();
				}

			}
		
	}
}

/* Load video to the h_Image
*/
void CLicensePlateRecognitionDlg::OnBnClickedButtonLoadVideo()
{
	// TODO: Add your control notification handler code here
	m_bIsLoadVideo = !m_bIsLoadVideo;
	if (m_bIsLoadVideo)
	{
		
		GetDlgItem(IDC_BUTTON_LOAD_VIDEO)->SetWindowText("STOP VIDEO");
		m_hVideoPath = MgGetModuleFileDirectory();
		m_hVideoPath = m_hIni.GetString("Globals", "VideoPath", "");
		m_hVideoCapture.open((std::string)CT2CA(m_hVideoPath));
		m_hVideoCapture >> m_hImage; m_hPreprocessImage = m_hImage.clone();
		OnDrawObject(m_hPreprocessImage);
		SYSTEMTIME stTimeNow; ::GetLocalTime(&stTimeNow);
		CString _cstrResultVideoPath = m_hIni.GetString("Globals", "ResultVideoPath", "");
		CString _cstrResultVideoName; _cstrResultVideoName.Format("\\%04d_%02d_%02d__%02d_%02d_%02d.avi",
			stTimeNow.wYear, stTimeNow.wMonth, stTimeNow.wDay,
			stTimeNow.wHour, stTimeNow.wMinute, stTimeNow.wSecond);
		_cstrResultVideoPath.Append(_cstrResultVideoName);
		m_hVideoWriter.open((std::string)CT2CA(_cstrResultVideoPath),
			CV_FOURCC('F', 'M', 'P', '4'), 10, m_hPreprocessImage.size());
		if (!m_hVideoWriter.isOpened())printf("Couldnt open video writer!\n");
	}
	else
	{
		m_hVideoWriter.release();
		m_hVideoCapture.release();
		GetDlgItem(IDC_BUTTON_LOAD_VIDEO)->SetWindowText("LOAD VIDEO");
	}
}

/* Clear detector
*/
void CLicensePlateRecognitionDlg::OnBnClickedButtonClear()
{
	// TODO: Add your control notification handler code here
	delete m_pDetector;
	m_pDetector = NULL;
	m_hStatusWindow.SetSel(0, -1);
	m_hStatusWindow.Clear();
}
