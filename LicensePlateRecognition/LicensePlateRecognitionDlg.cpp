
// LicensePlateRecognitionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LicensePlateRecognition.h"
#include "LicensePlateRecognitionDlg.h"
#include "afxdialogex.h"
#include "PreprocessImage.h"

namespace fs = std::filesystem;
using namespace cv;

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
	DDX_Control(pDX, IDC_EDIT_RESULT, m_hStatusResultWindow);
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

//Use for drawing m_hImage on m_hImageControl
void CLicensePlateRecognitionDlg::OnDrawObject(cv::Mat& mDrawImage)
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
	m_hLightingImageSlideBar.SetRange(0, 100, TRUE);
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
		m_hImage = cv::imread(std::string(_ImagePath),IMREAD_COLOR);
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
		m_hImage = cv::imread(std::string(_ImagePath), IMREAD_COLOR);
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
