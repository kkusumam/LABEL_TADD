
	// visionDlg.cpp : implementation file
	//

	#include "stdafx.h"
	#include "vision.h"
	#include "visionDlg.h"
	#include "afxdialogex.h"
	#include <string>
	#include <windows.h> 
	#define _WIN32_WINNT 0x0500 

	//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	//#include <iostream>
 

	#ifdef _DEBUG
	#define new DEBUG_NEW
	#endif

	using namespace cv;
	using namespace std;
	// CAboutDlg dialog used for App About
	extern "C"
	{
	int PASCAL WinMain(HINSTANCE inst,HINSTANCE dumb,LPSTR param,int show);
	};

	//global arguments
	vector<KeyPoint> keypoints_object;

	// The variable minHessian controls the number of the detected SURF keypoints but it is not the exact number of the keypoints
	// In actual cases, it is better to have a value 300-500
	// The larger the minHessian, the smaller the number of detected keypoints

	int minHessian = 400;
	SurfFeatureDetector detector(minHessian);
	SurfDescriptorExtractor extractor;

	Mat descriptors_object;

	// Set the element for image erosion and dilation
	Mat element1=getStructuringElement(MORPH_RECT,Size(19,19));
	Mat element2=getStructuringElement(MORPH_RECT,Size(29,29));

	// Set other global arguments
	vector<RotatedRect> minRect11; 
	Scalar color = Scalar(0,255,255 );
	Point2f rect_points1[4]; 
	double max_dist = 0; 
	double min_dist = 100;

	class CAboutDlg : public CDialogEx
	{
	public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
	protected:
	DECLARE_MESSAGE_MAP()
	};

	CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
	{
	}

	void CAboutDlg::DoDataExchange(CDataExchange* pDX)
	{
	CDialogEx::DoDataExchange(pDX);
	}

	BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	END_MESSAGE_MAP()


	// CvisionDlg dialog

	CvisionDlg::CvisionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CvisionDlg::IDD, pParent)
	{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	}

	void CvisionDlg::DoDataExchange(CDataExchange* pDX)
	{
	CDialogEx::DoDataExchange(pDX);
	}

	BEGIN_MESSAGE_MAP(CvisionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK3, &CvisionDlg::OnBnClickedOk3)
	ON_BN_CLICKED(IDOK4, &CvisionDlg::OnBnClickedOk4)
	ON_BN_CLICKED(IDOK5, &CvisionDlg::OnBnClickedOk5)
	ON_BN_CLICKED(IDOK6, &CvisionDlg::OnBnClickedOk6)
	ON_EN_CHANGE(IDC_EDIT2, &CvisionDlg::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, &CvisionDlg::OnEnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT4, &CvisionDlg::OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT5, &CvisionDlg::OnEnChangeEdit5)
	ON_EN_CHANGE(IDC_EDIT6, &CvisionDlg::OnEnChangeEdit6)
	ON_EN_CHANGE(IDC_EDIT7, &CvisionDlg::OnEnChangeEdit7)
	ON_EN_CHANGE(IDC_EDIT8, &CvisionDlg::OnEnChangeEdit8)
	ON_EN_CHANGE(IDC_EDIT9, &CvisionDlg::OnEnChangeEdit9)
	ON_BN_CLICKED(IDOK7, &CvisionDlg::OnBnClickedOk7)
	ON_EN_CHANGE(IDC_EDIT1, &CvisionDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDOK9, &CvisionDlg::OnBnClickedOk9)
	ON_BN_CLICKED(IDOK8, &CvisionDlg::OnBnClickedOk8)
	END_MESSAGE_MAP()


	// CvisionDlg message handlers

	BOOL CvisionDlg::OnInitDialog()
	{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	return TRUE;  // return TRUE  unless you set the focus to a control
	}

	void CvisionDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

	void CvisionDlg::OnPaint()
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

	HCURSOR CvisionDlg::OnQueryDragIcon()
	{
	return static_cast<HCURSOR>(m_hIcon);
	}


	void CvisionDlg::OnBnClickedOk3()
	{
	// TODO: Add your control notification handler code here
	
	CFileDialog dlg2(TRUE);
	//OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
	//_T("image files (*.bmp; *.jpg) |*.bmp;*.jpg
		//                             |All Files (*.*)|*.*||"),NULL);
 
	dlg2.m_ofn.lpstrTitle= _T("Load Reference Image 2");
 
	if (dlg2.DoModal() == IDOK) {
 
		// the selected filename
	CString path2= dlg2.GetPathName();

	// This is image pointer
		IplImage *image2;                      
 
		// load the image
		image2= cvLoadImage(path2);            
	  
		// Display the loaded image and wait for pressing the Esc key to close the display window
		while(cvWaitKey(33) != 27)
	{
		cvNamedWindow("Reference Image 2",CV_WINDOW_NORMAL);
		cvShowImage("Reference Image 2",image2);
		cvSaveImage("ref2.jpg", image2);
	}

	cvDestroyAllWindows();
	cvReleaseImage(&image2);
	}
  
	}


	void CvisionDlg::OnBnClickedOk4()
	{
	// TODO: Add your control notification handler code here
	CFileDialog dlg1(TRUE);

	//OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
	//_T("image files (*.bmp; *.jpg) |*.bmp;*.jpg
		//                             |All Files (*.*)|*.*||"),NULL);
 
	dlg1.m_ofn.lpstrTitle= _T("Load Reference Image 1");
 
	if (dlg1.DoModal() == IDOK) {
 
		// the selected filename
	CString path1= dlg1.GetPathName(); 

	// This is image pointer
		IplImage *image1;                      
 
		// load the image
		image1= cvLoadImage(path1);             
	  
		// Display the loaded image and wait for pressing the Esc key to close the display window
		while(cvWaitKey(33) != 27)
	{
		cvNamedWindow("Reference Image 1",CV_WINDOW_NORMAL);
		cvShowImage("Reference Image 1",image1);
		cvSaveImage("ref1.jpg", image1);
	}

		cvDestroyAllWindows();
	cvReleaseImage(&image1);

	}
	}


	void CvisionDlg::OnBnClickedOk5()
	{
	CFileDialog dlg3(TRUE);

	//OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
	//_T("image files (*.bmp; *.jpg) |*.bmp;*.jpg
		//                             |All Files (*.*)|*.*||"),NULL);
 
	dlg3.m_ofn.lpstrTitle= _T("Load the initial test image");
 
	if (dlg3.DoModal() == IDOK) {
 
		// the selected filename
	CString path3= dlg3.GetPathName();

	// This is image pointer
		IplImage *imtest;                      
 
		// load the image
		imtest= cvLoadImage(path3);             
	  
		// Display the loaded image and wait for pressing the Esc key to close the display window
		while(cvWaitKey(33) != 27)
	{
		cvNamedWindow("Initial test image",CV_WINDOW_NORMAL);
		cvShowImage("Initial test image",imtest);
		cvSaveImage("initialtestimage.jpg", imtest);
	}  
		cvDestroyAllWindows();
	cvReleaseImage(&imtest);
	}
	}

	void CvisionDlg::OnBnClickedOk6()
	{
	
	Mat img_object= imread("ref1.jpg", CV_LOAD_IMAGE_GRAYSCALE );
	Mat img_ref= imread("ref2.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img_scene= imread("initialtestimage.jpg", CV_LOAD_IMAGE_GRAYSCALE);	
 
	//-- The following 2 lines can check if the input files are images.
	// if( !img_object.data || !img_ref.data||!img_scene.data )
	//{ std::cout<< " --(!) Error reading images " << std::endl; }


	/////////// Processing for the two reference images ////////////

	//-- Detect the keypoints using SURF Detector

	detector.detect(img_object, keypoints_object );

	vector<KeyPoint> keypoints_ref;
	detector.detect(img_ref, keypoints_ref );

	//-- Calculate descriptors (feature vectors)
  
	extractor.compute( img_object, keypoints_object, descriptors_object );
	Mat descriptors_ref; 
	extractor.compute( img_ref, keypoints_ref, descriptors_ref );

	//-- Matching obj and ref descriptor vectors using FLANN matcher
  
	FlannBasedMatcher matcher1;
	std::vector< DMatch> matches1;  
	matcher1.match(descriptors_object, descriptors_ref,matches1);

	//-- Quick calculation of max and min distances between keypoints
	for( int i = 0; i < descriptors_object.rows; i++ )
	{ double dist = matches1[i].distance;
	if( dist < min_dist ) min_dist = dist;
	if( dist > max_dist ) max_dist = dist;
	}
	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector< DMatch > good_matches1;

	for( int i = 0; i < descriptors_object.rows; i++ )
	{ if( matches1[i].distance < 3*min_dist )
	{ good_matches1.push_back( matches1[i]); }
	}
	//-- Localize the object from img_1 in img_2
	std::vector<Point2f> obj1;
	std::vector<Point2f> ref;

	for( size_t i = 0; i < good_matches1.size(); i++ )
	{
	//-- Get the keypoints from the good matches
	obj1.push_back( keypoints_object[ good_matches1[i].queryIdx ].pt );
	ref.push_back( keypoints_ref[ good_matches1[i].trainIdx ].pt );
	}
  
	//-- Find the tranform matrix through the RANSAC algorithm
	Mat H1 = findHomography(ref, obj1, CV_RANSAC);
	Mat img_dst1;
   
	//-- Use the following line if the background is not quite black (i.e., gray).
	warpPerspective( img_ref, img_dst1, H1, img_object.size(),INTER_LINEAR, BORDER_CONSTANT,Scalar(int(img_object.at<uchar>(300,1))));

	//-- Use the following line if the background is almost black.
	//warpPerspective( img_ref, img_dst1, H1, img_object.size(),INTER_LINEAR );

	//-- The following 3 lines display the registration result of the reference images.
	//namedWindow("imagereg",WINDOW_AUTOSIZE );
	//imshow( "imagereg", img_dst );
	//waitKey(0);

	//-- Capture difference image
	Mat diff_img1, blur_img1, bw_img1,mor_img1;
	absdiff(img_object,img_dst1,diff_img1);

	//-- Blur the difference image and transfer it to a binary image by thresholding
	blur(diff_img1,blur_img1,Size(40,40));
	Scalar meanthresh1=mean(blur_img1);
	float imgthresh1=3*meanthresh1.val[0];
	threshold(blur_img1,bw_img1,imgthresh1,255,THRESH_BINARY);

	//-- The following 3 lines display the binary image
	//namedWindow( "bw", CV_WINDOW_NORMAL );
	//imshow( "bw", bw_img );
	// waitKey(0);

	//-- Image morphology (erosion and dilation) for connecting disjointed close white regions and removing small white regions
	morphologyEx(bw_img1,mor_img1, MORPH_OPEN, element1);
	morphologyEx(mor_img1,mor_img1, MORPH_CLOSE, element2);
  
	//-- Find the contours of the white regoin corresponding to the label.
	vector<vector<Point> > contours1;
	vector<Vec4i> hierarchy1;
	findContours(mor_img1,contours1,hierarchy1,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Find the rotated rectangles and ellipses for each contour
	vector<RotatedRect> minRect1( contours1.size() ); 
	minRect1[0] = minAreaRect( Mat(contours1[0]) );
	minRect1[0].points( rect_points1);
	minRect11=minRect1;

	/////////// Processing for the reference image with the label at the correct position and the initial test image ////////////

	//-- Detect SURF keypoints in the initial test image
	vector<KeyPoint> keypoints_scene;
	detector.detect(img_scene, keypoints_scene );

	//-- Compute SURF descriptors for the keypoints
	Mat descriptors_scene;
	extractor.compute( img_scene, keypoints_scene, descriptors_scene );

	//-- Find matches between the reference image and the initial test image
	double max_dist = 0; double min_dist = 100;
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match( descriptors_object, descriptors_scene, matches );

	//-- Quick calculation of max and min distances between keypoints
	for( int i = 0; i < descriptors_object.rows; i++ )
	{ double dist = matches[i].distance;
	if( dist < min_dist ) min_dist = dist;
	if( dist > max_dist ) max_dist = dist;
	}

	std::vector< DMatch > good_matches;

	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	for( int i = 0; i < descriptors_object.rows; i++ )
	{ if( matches[i].distance < 3*min_dist )
	{ good_matches.push_back( matches[i]); }
	}

	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for( size_t i = 0; i < good_matches.size(); i++ )
	{
	//-- Get the keypoints from the good matches
	obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
	scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
	}

	//-- -- Find the tranform matrix through the RANSAC algorithm
	Mat H = findHomography(scene, obj, CV_RANSAC);
  
	Mat img_dst;

	//-- Use the following line if the background is not quite black (i.e., gray).
	warpPerspective( img_scene, img_dst, H, img_object.size(),INTER_LINEAR, BORDER_CONSTANT,Scalar(int(img_object.at<uchar>(300,1))));

	//-- Use the following line if the background is almost black.
	//warpPerspective( img_scene, img_dst, H, img_object.size(),INTER_LINEAR);
  
	//-- Visualise the registration result
	//namedWindow("imagereg",WINDOW_AUTOSIZE );
	//imshow( "imagereg", img_dst );
  
	//-- Capture the difference image through thresholding, blurring, erosion and dilation
	Mat diff_img, blur_img, bw_img,mor_img;
	absdiff(img_object,img_dst,diff_img);
	blur(diff_img,blur_img,Size(40,40));
	Scalar meanthresh=mean(blur_img);
	float imgthresh=3*meanthresh.val[0];
  
	threshold(blur_img,bw_img,imgthresh,255,THRESH_BINARY);
 
	morphologyEx(bw_img,mor_img, MORPH_OPEN, element1);
	morphologyEx(mor_img,mor_img, MORPH_CLOSE, element2);

	//- Find the contours
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(mor_img,contours,hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	// Find the rotated rectangles and ellipses for each contour
	vector<RotatedRect> minRect( contours.size() );
	minRect[0] = minAreaRect( Mat(contours[0]) );

	// Compute the orientation error
	float poe=abs(minRect[0].angle-minRect1[0].angle);

	// Compute the positional errorr (the distance between the centres of the labels)
	float pe=sqrt((minRect[0].center.x-minRect1[0].center.x)*(minRect[0].center.x-minRect1[0].center.x)+(minRect[0].center.y-minRect1[0].center.y)*(minRect[0].center.y-minRect1[0].center.y));
  
	// Draw the bounding boxes to denote label positions
	Mat drawing;
	cvtColor(img_dst,drawing,CV_GRAY2BGR);
	Scalar color1 = Scalar(255,0,0);

	Point2f rect_points[4]; minRect[0].points( rect_points );
  
	for( int j = 0; j < 4; j++ )
	{ line( drawing, rect_points[j], rect_points[(j+1)%4], color, 3, 8 );
	line( drawing, rect_points1[j], rect_points1[(j+1)%4], color1, 3, 8 );
	}

	// Show the result
	namedWindow( "Test Result", CV_WINDOW_AUTOSIZE);
	imshow( "Test Result", drawing );
	waitKey(0);

	destroyAllWindows();

	// Display the statistics of positional information of the label
	CString refCenterx;
	refCenterx.Format(_T("%f"), minRect1[0].center.x);
	GetDlgItem(IDC_EDIT2)->SetWindowText(refCenterx);

	CString refCentery;
	refCentery.Format(_T("%f"), minRect1[0].center.y);
	GetDlgItem(IDC_EDIT8)->SetWindowText(refCentery);

	CString refAngle;
	refAngle.Format(_T("%f"), minRect1[0].angle);
	GetDlgItem(IDC_EDIT3)->SetWindowText(refAngle) ;

	CString testCenterx;
	testCenterx.Format(_T("%f"), minRect[0].center.x);
	GetDlgItem(IDC_EDIT4)->SetWindowText(testCenterx);

	CString testCentery;
	testCentery.Format(_T("%f"), minRect[0].center.y);
	GetDlgItem(IDC_EDIT9)->SetWindowText(testCentery);
	
	CString testAngle;
	testAngle.Format(_T("%f"), minRect[0].angle);
	GetDlgItem(IDC_EDIT5)->SetWindowText(testAngle);

	CString testpe;
	testpe.Format(_T("%f"), pe);
	GetDlgItem(IDC_EDIT6)->SetWindowText(testpe);

	CString testoe;
	testoe.Format(_T("%f"), min(90-poe,poe));
	GetDlgItem(IDC_EDIT7)->SetWindowText(testoe);

	// Create a message box as a reminder
	CString msg;
	msg.Format(_T("If the initialisation result is not good, redo the training and initial test"));
	AfxMessageBox(msg);
	}
  
	void CvisionDlg::OnEnChangeEdit2()
	{
		// TODO:  If this is a RICHEDIT control, the control will not
		// send this notification unless you override the CDialogEx::OnInitDialog()
		// function and call CRichEditCtrl().SetEventMask()
		// with the ENM_CHANGE flag ORed into the mask.
		// TODO:  Add your control notification handler code here
	}


	void CvisionDlg::OnEnChangeEdit3()
	{
		// TODO:  If this is a RICHEDIT control, the control will not
		// send this notification unless you override the CDialogEx::OnInitDialog()
		// function and call CRichEditCtrl().SetEventMask()
		// with the ENM_CHANGE flag ORed into the mask.

		// TODO:  Add your control notification handler code here
	}


	void CvisionDlg::OnEnChangeEdit4()
	{
		// TODO:  If this is a RICHEDIT control, the control will not
		// send this notification unless you override the CDialogEx::OnInitDialog()
		// function and call CRichEditCtrl().SetEventMask()
		// with the ENM_CHANGE flag ORed into the mask.

		// TODO:  Add your control notification handler code here
	}


	void CvisionDlg::OnEnChangeEdit5()
	{
		// TODO:  If this is a RICHEDIT control, the control will not
		// send this notification unless you override the CDialogEx::OnInitDialog()
		// function and call CRichEditCtrl().SetEventMask()
		// with the ENM_CHANGE flag ORed into the mask.

		// TODO:  Add your control notification handler code here
	}


	void CvisionDlg::OnEnChangeEdit6()
	{
		// TODO:  If this is a RICHEDIT control, the control will not
		// send this notification unless you override the CDialogEx::OnInitDialog()
		// function and call CRichEditCtrl().SetEventMask()
		// with the ENM_CHANGE flag ORed into the mask.

		// TODO:  Add your control notification handler code here
	}


	void CvisionDlg::OnEnChangeEdit7()
	{
		// TODO:  If this is a RICHEDIT control, the control will not
		// send this notification unless you override the CDialogEx::OnInitDialog()
		// function and call CRichEditCtrl().SetEventMask()
		// with the ENM_CHANGE flag ORed into the mask.

		// TODO:  Add your control notification handler code here
	}


	void CvisionDlg::OnEnChangeEdit8()
	{
		// TODO:  If this is a RICHEDIT control, the control will not
		// send this notification unless you override the CDialogEx::OnInitDialog()
		// function and call CRichEditCtrl().SetEventMask()
		// with the ENM_CHANGE flag ORed into the mask.

		// TODO:  Add your control notification handler code here
	}


	void CvisionDlg::OnEnChangeEdit9()
	{
		// TODO:  If this is a RICHEDIT control, the control will not
		// send this notification unless you override the CDialogEx::OnInitDialog()
		// function and call CRichEditCtrl().SetEventMask()
		// with the ENM_CHANGE flag ORed into the mask.

		// TODO:  Add your control notification handler code here
	}


	void CvisionDlg::OnBnClickedOk7()
	{
		CFileDialog dlg4(TRUE);
	//OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
	//_T("image files (*.bmp; *.jpg) |*.bmp;*.jpg
		//                             |All Files (*.*)|*.*||"),NULL);
 
	dlg4.m_ofn.lpstrTitle= _T("Load a new test image");
 
	if (dlg4.DoModal() == IDOK) {
 
		// the selected filename
	CString path4= dlg4.GetPathName();

		// This is image pointer
		IplImage *image4;                     
     
		// load the image
		image4= cvLoadImage(path4);             
	  
		// Display the loaded image and wait for pressing the Esc key to close the display window
		while(cvWaitKey(33) != 27)
	{
		cvNamedWindow("Test image",CV_WINDOW_NORMAL);
		cvShowImage("Test image",image4);
		cvSaveImage("testimage.jpg", image4);
	}
    
		cvDestroyAllWindows();
	cvReleaseImage(&image4);

	}
	}

	void CvisionDlg::OnBnClickedOk8()
	{
		CFileDialog dlg5(TRUE);
	//OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
	//_T("image files (*.bmp; *.jpg) |*.bmp;*.jpg
		//                             |All Files (*.*)|*.*||"),NULL);
 
	dlg5.m_ofn.lpstrTitle= _T("Load a reference OCR image");
 
	if (dlg5.DoModal() == IDOK) {
 
	// the selected filename
	CString path5= dlg5.GetPathName(); 

	// This is image pointer
		IplImage *image5; 

	// load the image
		image5= cvLoadImage(path5);            
	  
	// Display the image (reference OCR image) and wait for pressing the Esc key to close the display window
		while(cvWaitKey(33) != 27)
	{
		cvNamedWindow("Reference OCR image",CV_WINDOW_NORMAL);
		cvShowImage("Reference OCR image",image5);
		cvSaveImage("refOCR.jpg", image5);
	}
    
		cvDestroyAllWindows();
	cvReleaseImage(&image5);
	}
	}

	void CvisionDlg::OnEnChangeEdit1()
	{
		// TODO:  If this is a RICHEDIT control, the control will not
		// send this notification unless you override the CDialogEx::OnInitDialog()
		// function and call CRichEditCtrl().SetEventMask()
		// with the ENM_CHANGE flag ORed into the mask.

		// TODO:  Add your control notification handler code here
	}


	void CvisionDlg::OnBnClickedOk9()
	{
	 
	//////// Processing for the test image and the reference image with the label at the correct position /////////
	Mat img_scene= imread("testimage.jpg", CV_LOAD_IMAGE_GRAYSCALE);	
  
	//-- Detect SURF keypoints
	vector<KeyPoint> keypoints_scene ;
	detector.detect(img_scene, keypoints_scene );

	//-- Compute descriptors for the keypoints
	Mat descriptors_scene;
	extractor.compute( img_scene, keypoints_scene, descriptors_scene );

	//-- Find the matches
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match( descriptors_object, descriptors_scene, matches );

	std::vector< DMatch > good_matches;

	for( int i = 0; i < descriptors_object.rows; i++ )
	{ double dist = matches[i].distance;
	if( dist < min_dist ) min_dist = dist;
	if( dist > max_dist ) max_dist = dist;
	}
  
	//-- Find the good mathces
	for( int i = 0; i < descriptors_object.rows; i++ )
	{ if( matches[i].distance < 3*min_dist )
	{ good_matches.push_back( matches[i]); }
	}

	//-- Reset the thresholding distances for processing the next test images
	max_dist = 0; 
	min_dist = 100;

	std::vector<Point2f> obj;
	std::vector<Point2f> scene;
  
	for( size_t i = 0; i < good_matches.size(); i++ )
	{
	//-- Get the keypoints from the good matches
	  
	obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
	scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
	}
  
	// Compute the transform matrix via RANSAC
	Mat H = findHomography(scene, obj, CV_RANSAC);
   
	Mat img_dst;
	Mat img_object= imread("ref1.jpg", CV_LOAD_IMAGE_GRAYSCALE );

	//-- Use the following line if the background is not quite black (i.e., gray).
	warpPerspective( img_scene, img_dst, H, img_object.size(),INTER_LINEAR, BORDER_CONSTANT,Scalar(int(img_object.at<uchar>(300,1))));

	

	//-- Use the following line if the background is almost black.
	warpPerspective( img_scene, img_dst, H, img_object.size(),INTER_LINEAR);

	//-- Display the registration result
	//namedWindow("imagereg",WINDOW_AUTOSIZE );
	//imshow( "imagereg", img_object );
	//waitKey(0);
      
	//Difference image, blurring, erosion and dilation
	Mat diff_img, blur_img, bw_img,mor_img;

	absdiff(img_object,img_dst,diff_img);

	blur(diff_img,blur_img,Size(40,40));
	Scalar meanthresh=mean(blur_img);

	float imgthresh=3*meanthresh.val[0];
  
	threshold(blur_img,bw_img,imgthresh,255,THRESH_BINARY);

	morphologyEx(bw_img,mor_img, MORPH_OPEN, element1);
	morphologyEx(mor_img,mor_img, MORPH_CLOSE, element2);

	//-- Find contours for potential label
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(mor_img,contours,hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	// Find the rotated rectangles for each contour
	vector<RotatedRect> minRect( contours.size() );
	minRect[0] = minAreaRect( Mat(contours[0]) );

	// Orientation and position of the pack
	float poe=abs(minRect[0].angle-minRect11[0].angle);

	float pe=sqrt((minRect[0].center.x-minRect11[0].center.x)*(minRect[0].center.x-minRect11[0].center.x)+(minRect[0].center.y-minRect11[0].center.y)*(minRect[0].center.y-minRect11[0].center.y));
  
	// Display the statistics
	CString testCenterx;
	testCenterx.Format(_T("%f"), minRect[0].center.x);
	GetDlgItem(IDC_EDIT4)->SetWindowText(testCenterx);

	CString testCentery;
	testCentery.Format(_T("%f"), minRect[0].center.y);
	GetDlgItem(IDC_EDIT9)->SetWindowText(testCentery);

	CString testAngle;
	testAngle.Format(_T("%f"), minRect[0].angle);
	GetDlgItem(IDC_EDIT5)->SetWindowText(testAngle);

	CString testpe;
	testpe.Format(_T("%f"), pe);
	GetDlgItem(IDC_EDIT6)->SetWindowText(testpe);

	CString testoe;
	testoe.Format(_T("%f"), min(90-poe,poe));
	GetDlgItem(IDC_EDIT7)->SetWindowText(testoe);

	// Draw the result of label detection
	Mat drawing;
	cvtColor(img_dst,drawing,CV_GRAY2BGR);
	Scalar color1 = Scalar(255,0,0);

	Point2f rect_points[4]; minRect[0].points( rect_points );
	for( int j = 0; j < 4; j++ )
	{ line( drawing, rect_points[j], rect_points[(j+1)%4], color, 3, 8 );
	line( drawing, rect_points1[j], rect_points1[(j+1)%4], color1, 3, 8 );
	}
 
	/// Show the result in a window
	namedWindow( "Test result", CV_WINDOW_AUTOSIZE );
	imshow( "Test result", drawing );
	waitKey(0);
	destroyAllWindows();

	// If the position error is larger than 80 or the orientation error is larger than 30 degree, create a message box showing
	if (pe>80||min(90-poe,poe)>30)  
		{CString msg;
	msg.Format(_T("Should reject this tray!"));
	AfxMessageBox(msg);}

	else // or another message box to accept the tray for OCR
	{
		CString msg1;
	msg1.Format(_T("Accept this tray for OCR."));
	AfxMessageBox(msg1);

		// Find the bounding box slightly larger than the label region
	vector<Rect> boundRect(contours.size());

	boundRect[0] = boundingRect( Mat(contours[0]) );
	boundRect[0].height=boundRect[0].height+20;
	boundRect[0].width=boundRect[0].width+20;
	boundRect[0].x=boundRect[0].x-10;
	boundRect[0].y=boundRect[0].y-10;

	// Cut out the bounding box and save it on the disk
	Mat roi=img_dst(boundRect[0]);
	imwrite("roi.jpg",roi);

	// Display the content within the bounding box as a new image
	// namedWindow( "ROI Image", CV_WINDOW_AUTOSIZE );
	//imshow( "ROI Image", roi );
	// waitKey(0);

	Mat refroi=imread("refOCR.jpg",0);
 
	// Extend the reference OCR image
	Mat extroi=Mat::ones(roi.rows, roi.cols,CV_8U)*255;
	refroi.copyTo(extroi.colRange(0,refroi.cols).rowRange(0,refroi.rows));
 
  
	//namedWindow( "Extended ROI Image", CV_WINDOW_AUTOSIZE );
	// imshow( "Extended ROI Image", extroi);
	// waitKey(0);

	////// Processing for the OCR image /////
	vector<KeyPoint> keypoints_roi, keypoints_extroi;
	Mat descriptors_roi,descriptors_extroi;

	detector.detect(roi, keypoints_roi);
	detector.detect(extroi, keypoints_extroi);
	extractor.compute(roi, keypoints_roi, descriptors_roi);
	extractor.compute(extroi, keypoints_extroi, descriptors_extroi);

	double max_dist2 = 0; double min_dist2 = 100;
	FlannBasedMatcher matcher2;
	vector< DMatch > matches2;
	matcher2.match( descriptors_roi, descriptors_extroi, matches2 );

	//-- Quick calculation of max and min distances between keypoints
	for( int i = 0; i < descriptors_roi.rows; i++ )
	{ double dist = matches2[i].distance;
	if( dist < min_dist2 ) min_dist2 = dist;
	if( dist > max_dist2 ) max_dist2 = dist;
	}

	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector< DMatch > good_matches2;

	for( int i = 0; i < descriptors_roi.rows; i++ )
	{ if( matches2[i].distance < 8*min_dist2 )
	{ good_matches2.push_back( matches2[i]); }
	}

	//std::vector<Point2f> obj;
	std::vector<Point2f> proi;
	std::vector<Point2f> pextroi;

	for( size_t i = 0; i < good_matches2.size(); i++ )
	{
	//-- Get the keypoints from the good matches
	proi.push_back( keypoints_roi[ good_matches2[i].queryIdx ].pt );
	pextroi.push_back( keypoints_extroi[ good_matches2[i].trainIdx ].pt );
	}
  
	Mat H2 = findHomography(proi,pextroi, CV_RANSAC);
  
	Mat img_roi;

	warpPerspective( roi, img_roi, H2, extroi.size(),INTER_LINEAR, BORDER_CONSTANT,Scalar(int(roi.at<uchar>(300,1))));
	//warpPerspective( roi, img_roi, H2, extroi.size(),INTER_LINEAR);

	//namedWindow( "Registered ROI Image", CV_WINDOW_AUTOSIZE );
	//imshow( "Registered ROI Image", img_roi);
	//waitKey(0);

	Mat diff_roi, blur_roi, bw_roi,mor_roi;
	absdiff(img_roi,extroi,diff_roi);

	//namedWindow( "Difference Image", CV_WINDOW_AUTOSIZE );
	//imshow( "Difference Image", diff_roi);
	// waitKey(0);

	Rect r(0, 0,refroi.cols, refroi.rows);
	diff_roi = diff_roi(r);

	//namedWindow( "Difference Image", CV_WINDOW_AUTOSIZE );
	//imshow( "Difference Image", diff_roi);
	//waitKey(0);

	blur(diff_roi,blur_roi,Size(15,15));
	Scalar meanthreshroi=mean(blur_roi);
	float roithresh=2.5*meanthreshroi.val[0];
  
	threshold(blur_roi,bw_roi,roithresh,255,THRESH_BINARY);

	//Mat element1=getStructuringElement(MORPH_RECT,Size(19,19));
	//Mat element3=getStructuringElement(MORPH_RECT,Size(15,15));

	morphologyEx(bw_roi,mor_roi, MORPH_OPEN, element1);
	morphologyEx(mor_roi,mor_roi, MORPH_CLOSE, element2);
 
	// Find the bounding box of the target text line
	vector<vector<Point> > contoursroi;
	vector<Vec4i> hierarchyroi;
	findContours(mor_roi,contoursroi,hierarchyroi,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<Rect> boundRectroi(contoursroi.size());
	boundRectroi[0] = boundingRect( Mat(contoursroi[0]) );
	boundRectroi[0].height=boundRectroi[0].height;
	boundRectroi[0].width=boundRectroi[0].width+8;
	boundRectroi[0].x=boundRectroi[0].x-5;
	boundRectroi[0].y=boundRectroi[0].y;

	Mat ocr=img_roi(boundRectroi[0]);
	imwrite("ocr.jpg",ocr);

	// Display the image input to the OCR reader of tesseract
	//namedWindow( "OCR Image", CV_WINDOW_AUTOSIZE );
	//imshow( "OCR Image", ocr);
	//waitKey(0);
	//FreeConsole(); 
 
	//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

	/////// OCR using tesseract //////
	system("new.exe ocr.jpg 255 2 128");
	//ShellExecute(NULL, NULL, "new.exe ocr.jpg 255 2 128", NULL, NULL,SW_HIDE);
	string str;

	// Open the file where the OCR result is saved in Unicode format
	ifstream infile("ocrresult.txt");

	// Get the lines
	while(getline(infile, str)) 
		{
			infile>>str;

			// Show the OCR result in the TADD interface in UTF8 format
			// It could be different from what you see in ocrresult.txt due to the transformation from Unicode to UTF8
			GetDlgItem(IDC_EDIT1)->SetWindowText(str.c_str());
		}
	infile.close();	
	}
	}


 