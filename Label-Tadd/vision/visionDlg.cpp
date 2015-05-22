
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
vector<KeyPoint> keypoints_object;//, keypoints_ref, keypoints_scene, keypoints_roi, keypoints_extroi;
int minHessian = 400;
SurfFeatureDetector detector(minHessian);
SurfDescriptorExtractor extractor;
//Mat img_object, img_scene, descriptors_object, descriptors_ref, descriptors_scene,descriptors_roi,descriptors_extroi;
Mat descriptors_object;
  Mat element1=getStructuringElement(MORPH_RECT,Size(19,19));
  Mat element2=getStructuringElement(MORPH_RECT,Size(29,29));
  vector<RotatedRect> minRect11; 
  Scalar color = Scalar(0,255,255 );
  Point2f rect_points1[4]; 
  double max_dist = 0; 
  double min_dist = 100;
    //std::vector<DMatch> good_matches;
  //std::vector<Point2f> obj;
  //std::vector<Point2f> scene;

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
 
    CString path2= dlg2.GetPathName(); // the selected filename
	  IplImage *image2;                      // This is image pointer
 
      image2= cvLoadImage(path2);             // load the image
	  
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
 
    CString path1= dlg1.GetPathName(); // the selected filename
	  IplImage *image1;                      // This is image pointer
 
      image1= cvLoadImage(path1);             // load the image
	  
      
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
 
    CString path3= dlg3.GetPathName(); // the selected filename
	  IplImage *imtest;                      // This is image pointer
 
      imtest= cvLoadImage(path3);             // load the image
	  
      
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
	//system("imagereg ref1.jpg ref2.jpg test.jpg");
 Mat img_object= imread("ref1.jpg", CV_LOAD_IMAGE_GRAYSCALE );
  Mat img_ref= imread("ref2.jpg", CV_LOAD_IMAGE_GRAYSCALE);
  Mat img_scene= imread("initialtestimage.jpg", CV_LOAD_IMAGE_GRAYSCALE);	
 
 // if( !img_object.data || !img_ref.data||!img_scene.data )
  //{ std::cout<< " --(!) Error reading images " << std::endl; }

  
  //-- Step 1: Detect the keypoints using SURF Detector

 
  //vector<KeyPoint> keypoints_object, keypoints_ref, keypoints_scene, keypoints_roi, keypoints_extroi;
  //SurfFeatureDetector detector(minHessian);
  //SurfDescriptorExtractor extractor;
  detector.detect(img_object, keypoints_object );

  vector<KeyPoint> keypoints_ref;
  detector.detect(img_ref, keypoints_ref );

  //-- Step 2: Calculate descriptors (feature vectors)
  
  extractor.compute( img_object, keypoints_object, descriptors_object );
  Mat descriptors_ref; 
  extractor.compute( img_ref, keypoints_ref, descriptors_ref );
  //extractor.compute( img_scene, keypoints_scene, descriptors_scene );

 //-- Step 3: Matching obj and ref descriptor vectors using FLANN matcher
  //double max_dist1 = 0; double min_dist1 = 100;
  
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
 // Mat H = estimateRigidTransform(obj,scene,false);
  
  Mat H1 = findHomography(ref, obj1, CV_RANSAC);
  Mat img_dst1;//=Mat::zeros(img_object.rows, img_object.cols,img_object.type());
   
  //warpPerspective( img_ref, img_dst1, H1, img_object.size(),INTER_LINEAR, BORDER_CONSTANT,Scalar(int(img_object.at<uchar>(300,1))) );
  warpPerspective( img_ref, img_dst1, H1, img_object.size(),INTER_LINEAR );

   //namedWindow("imagereg",WINDOW_AUTOSIZE );
  //imshow( "imagereg", img_dst );

  //waitKey(0);
  Mat diff_img1, blur_img1, bw_img1,mor_img1;
  absdiff(img_object,img_dst1,diff_img1);
  blur(diff_img1,blur_img1,Size(40,40));
  Scalar meanthresh1=mean(blur_img1);
  float imgthresh1=3*meanthresh1.val[0];
  threshold(blur_img1,bw_img1,imgthresh1,255,THRESH_BINARY);
  //namedWindow( "bw", CV_WINDOW_NORMAL );
  //imshow( "bw", bw_img );
 // waitKey(0);

  //Mat element2=getStructuringElement(MORPH_ELLIPSE,Size(5,5),Point(2,2));
  morphologyEx(bw_img1,mor_img1, MORPH_OPEN, element1);
  morphologyEx(mor_img1,mor_img1, MORPH_CLOSE, element2);
  //morphologyEx(mor_img,mor_img, MORPH_CLOSE, element);
  //morphologyEx(mor_img,mor_img, MORPH_OPEN, element);
  vector<vector<Point> > contours1;
  vector<Vec4i> hierarchy1;
  findContours(mor_img1,contours1,hierarchy1,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    /// Find the rotated rectangles and ellipses for each contour
  vector<RotatedRect> minRect1( contours1.size() ); 
  minRect1[0] = minAreaRect( Mat(contours1[0]) );
     
  // Draw 
  // t=clock()-t;
  // cout<<"it took "<<((float)t)/CLOCKS_PER_SEC<<" seconds.\n";

  /*Mat drawing1;
  cvtColor(img_dst1,drawing1,CV_GRAY2BGR);*/
  

  // rotated rectangle
  
  minRect1[0].points( rect_points1);
  //for( int j = 0; j < 4; j++ )
  //{line( drawing1, rect_points1[j], rect_points1[(j+1)%4], color, 3, 8 );}
  
  //cout<<"Centre point: "<<minRect1[0].center<<endl;
  //cout<<"Orientation: "<<minRect1[0].angle<<endl;
  minRect11=minRect1;

  vector<KeyPoint> keypoints_scene;
  detector.detect(img_scene, keypoints_scene );

  Mat descriptors_scene;
  extractor.compute( img_scene, keypoints_scene, descriptors_scene );

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

  for( int i = 0; i < descriptors_object.rows; i++ )
  { if( matches[i].distance < 3*min_dist )
    { good_matches.push_back( matches[i]); }
  }

  //std::vector<Point2f> obj;
  std::vector<Point2f> obj;
  std::vector<Point2f> scene;

  for( size_t i = 0; i < good_matches.size(); i++ )
  {
    //-- Get the keypoints from the good matches
    obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
    scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
  }
 // Mat H = estimateRigidTransform(obj,scene,false);
  
  Mat H = findHomography(scene, obj, CV_RANSAC);
  

 // cout << "H = "<< endl << " "  << H << endl << endl;
   
   Mat img_dst;//=Mat::zeros(img_object.rows, img_object.cols,img_object.type());
   //warpPerspective( img_scene, img_dst, H, img_object.size(),INTER_LINEAR, BORDER_CONSTANT,Scalar(int(img_object.at<uchar>(300,1)-10)));
   warpPerspective( img_scene, img_dst, H, img_object.size(),INTER_LINEAR);
  //namedWindow("imagereg",WINDOW_AUTOSIZE );
  //imshow( "imagereg", img_dst );

  Mat diff_img, blur_img, bw_img,mor_img;
  absdiff(img_object,img_dst,diff_img);
  blur(diff_img,blur_img,Size(40,40));
  Scalar meanthresh=mean(blur_img);
  float imgthresh=3*meanthresh.val[0];
  
  threshold(blur_img,bw_img,imgthresh,255,THRESH_BINARY);
 
  morphologyEx(bw_img,mor_img, MORPH_OPEN, element1);
  morphologyEx(mor_img,mor_img, MORPH_CLOSE, element2);

  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  findContours(mor_img,contours,hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

   // Find the rotated rectangles and ellipses for each contour
  vector<RotatedRect> minRect( contours.size() );
  minRect[0] = minAreaRect( Mat(contours[0]) );

  float poe=abs(minRect[0].angle-minRect1[0].angle);

  float pe=sqrt((minRect[0].center.x-minRect1[0].center.x)*(minRect[0].center.x-minRect1[0].center.x)+(minRect[0].center.y-minRect1[0].center.y)*(minRect[0].center.y-minRect1[0].center.y));
  
  //cout<<"Centre point: "<<minRect[0].center<<endl;
  //cout<<"Orientation: "<<minRect[0].angle<<endl;
  //cout<<"Position error: "<<pe<<endl;
  //cout<<"Orientation error: "<<min(90-poe,poe)<<endl;

  // Draw 
  // t=clock()-t;
  // cout<<"it took "<<((float)t)/CLOCKS_PER_SEC<<" seconds.\n";
  Mat drawing;
  cvtColor(img_dst,drawing,CV_GRAY2BGR);
  Scalar color1 = Scalar(255,0,0);

  Point2f rect_points[4]; minRect[0].points( rect_points );
  //Point2f rect_points1[4]; minRect1[0].points( rect_points1 );
  for( int j = 0; j < 4; j++ )
  { line( drawing, rect_points[j], rect_points[(j+1)%4], color, 3, 8 );
  line( drawing, rect_points1[j], rect_points1[(j+1)%4], color1, 3, 8 );
  }

  namedWindow( "Test Result", CV_WINDOW_AUTOSIZE );
  imshow( "Test Result", drawing );
  waitKey(0);

	cvDestroyAllWindows();

	CString refCenterx;
    refCenterx.Format(_T("%f"), minRect1[0].center.x);
	GetDlgItem(IDC_EDIT2)->SetWindowText(refCenterx);

	CString refCentery;
    refCentery.Format(_T("%f"), minRect1[0].center.y);
	GetDlgItem(IDC_EDIT8)->SetWindowText(refCentery);

	CString refAngle;
    refAngle.Format(_T("%f"), minRect1[0].angle);
    //AfxMessageBox(msg);
	GetDlgItem(IDC_EDIT3)->SetWindowText(refAngle) ;

	CString testCenterx;
    testCenterx.Format(_T("%f"), minRect[0].center.x);
    //AfxMessageBox(msg);
	GetDlgItem(IDC_EDIT4)->SetWindowText(testCenterx);

	CString testCentery;
    testCentery.Format(_T("%f"), minRect[0].center.y);
    //AfxMessageBox(msg);
	GetDlgItem(IDC_EDIT9)->SetWindowText(testCentery);
	


	CString testAngle;
    testAngle.Format(_T("%f"), minRect[0].angle);
    //AfxMessageBox(msg);
	GetDlgItem(IDC_EDIT5)->SetWindowText(testAngle);

	CString testpe;
    testpe.Format(_T("%f"), pe);
    //AfxMessageBox(msg);
	GetDlgItem(IDC_EDIT6)->SetWindowText(testpe);

	CString testoe;
    testoe.Format(_T("%f"), min(90-poe,poe));
    //AfxMessageBox(msg);
	GetDlgItem(IDC_EDIT7)->SetWindowText(testoe);


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
 
    CString path4= dlg4.GetPathName(); // the selected filename
	  IplImage *image4;                      // This is image pointer
 
      image4= cvLoadImage(path4);             // load the image
	  
      
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
 
    CString path5= dlg5.GetPathName(); // the selected filename
	  IplImage *image5;                      // This is image pointer
 
      image5= cvLoadImage(path5);             // load the image
	  
      
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
	 
  }


  void CvisionDlg::OnBnClickedOk9()
  {
	 
	   //Mat img_object= imread("ref1.jpg", CV_LOAD_IMAGE_GRAYSCALE );
  //Mat img_ref= imread("ref2.jpg", CV_LOAD_IMAGE_GRAYSCALE);
  Mat img_scene= imread("testimage.jpg", CV_LOAD_IMAGE_GRAYSCALE);	
  
 // SurfFeatureDetector detector(minHessian);
  //SurfDescriptorExtractor extractor;
 
  vector<KeyPoint> keypoints_scene ;
 detector.detect(img_scene, keypoints_scene );

 Mat descriptors_scene;
  extractor.compute( img_scene, keypoints_scene, descriptors_scene );

  
  FlannBasedMatcher matcher;
  std::vector< DMatch > matches;
  matcher.match( descriptors_object, descriptors_scene, matches );

  std::vector< DMatch > good_matches;

  for( int i = 0; i < descriptors_object.rows; i++ )
  { double dist = matches[i].distance;
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
  }

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
 // Mat H = estimateRigidTransform(obj,scene,false);
  
  Mat H = findHomography(scene, obj, CV_RANSAC);
  

 // cout << "H = "<< endl << " "  << H << endl << endl;
   
   Mat img_dst;//=Mat::zeros(img_object.rows, img_object.cols,img_object.type());
   //warpPerspective( img_scene, img_dst, H, img_object.size(),INTER_LINEAR, BORDER_CONSTANT,Scalar(int(img_object.at<uchar>(300,1)-10)));
   Mat img_object= imread("ref1.jpg", CV_LOAD_IMAGE_GRAYSCALE );
   warpPerspective( img_scene, img_dst, H, img_object.size(),INTER_LINEAR);
  //namedWindow("imagereg",WINDOW_AUTOSIZE );
  //imshow( "imagereg", img_object );
  //waitKey(0);
      

  Mat diff_img, blur_img, bw_img,mor_img;

  absdiff(img_object,img_dst,diff_img);

  blur(diff_img,blur_img,Size(40,40));
  Scalar meanthresh=mean(blur_img);

  float imgthresh=3*meanthresh.val[0];
  

  threshold(blur_img,bw_img,imgthresh,255,THRESH_BINARY);



  morphologyEx(bw_img,mor_img, MORPH_OPEN, element1);
  morphologyEx(mor_img,mor_img, MORPH_CLOSE, element2);

  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  findContours(mor_img,contours,hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

   // Find the rotated rectangles and ellipses for each contour
  vector<RotatedRect> minRect( contours.size() );
  minRect[0] = minAreaRect( Mat(contours[0]) );

  float poe=abs(minRect[0].angle-minRect11[0].angle);

  float pe=sqrt((minRect[0].center.x-minRect11[0].center.x)*(minRect[0].center.x-minRect11[0].center.x)+(minRect[0].center.y-minRect11[0].center.y)*(minRect[0].center.y-minRect11[0].center.y));
  
  CString testCenterx;
    testCenterx.Format(_T("%f"), minRect[0].center.x);
    //AfxMessageBox(msg);
	GetDlgItem(IDC_EDIT4)->SetWindowText(testCenterx);

	CString testCentery;
    testCentery.Format(_T("%f"), minRect[0].center.y);
    //AfxMessageBox(msg);
	GetDlgItem(IDC_EDIT9)->SetWindowText(testCentery);
	


	CString testAngle;
    testAngle.Format(_T("%f"), minRect[0].angle);
    //AfxMessageBox(msg);
	GetDlgItem(IDC_EDIT5)->SetWindowText(testAngle);

	CString testpe;
    testpe.Format(_T("%f"), pe);
    //AfxMessageBox(msg);
	GetDlgItem(IDC_EDIT6)->SetWindowText(testpe);

	CString testoe;
    testoe.Format(_T("%f"), min(90-poe,poe));
    //AfxMessageBox(msg);
	GetDlgItem(IDC_EDIT7)->SetWindowText(testoe);

 /* cout<<"Centre point: "<<minRect[0].center<<endl;
  cout<<"Orientation: "<<minRect[0].angle<<endl;
  cout<<"Position error: "<<pe<<endl;
  //cout<<"Possible orientation error 1 : "<<poe<<endl;
  //cout<<"Possible orientation error 2: "<<90-poe<<endl;
  cout<<"Orientation error: "<<min(90-poe,poe)<<endl;
  */
  // Draw 
  // t=clock()-t;
  // cout<<"it took "<<((float)t)/CLOCKS_PER_SEC<<" seconds.\n";
  Mat drawing;
  cvtColor(img_dst,drawing,CV_GRAY2BGR);
  Scalar color1 = Scalar(255,0,0);

  Point2f rect_points[4]; minRect[0].points( rect_points );
  //Point2f rect_points1[4]; minRect1[0].points( rect_points1 );
  for( int j = 0; j < 4; j++ )
  { line( drawing, rect_points[j], rect_points[(j+1)%4], color, 3, 8 );
  line( drawing, rect_points1[j], rect_points1[(j+1)%4], color1, 3, 8 );
  }

 /*float min_x=min(rect_points[0].x,rect_points[1].x);
   min_x=min(min_x,rect_points[2].x);
   min_x=min(min_x,rect_points[3].x);

  float min_y=min(rect_points[0].y,rect_points[1].y);
   min_y=min(min_y,rect_points[2].y);
  min_y=min(min_y,rect_points[3].y);

  float max_x=max(rect_points[0].x,rect_points[1].x);
   max_x=max(max_x,rect_points[2].x);
 max_x=max(max_x,rect_points[3].x);

  float max_y=max(rect_points[0].y,rect_points[1].y);
   max_y=max(max_y,rect_points[2].y);
   max_y=max(max_y,rect_points[3].y);*/
  /// Show in a window
  namedWindow( "Test result", CV_WINDOW_AUTOSIZE );
  imshow( "Test result", drawing );
  waitKey(0);
  cvDestroyAllWindows();
  //if ((pe>30)||(min(90-poe,poe)>15))
  //{cout<<"Reject the tray!"<<pe<<endl;}
  //else
  //{
	if (pe>80||min(90-poe,poe)>30)  
		{CString msg;
    msg.Format(_T("Should reject this tray!"));
	AfxMessageBox(msg);}
	else
	{
		CString msg1;
    msg1.Format(_T("Accept this tray for OCR."));
	AfxMessageBox(msg1);
 // vector<vector<Point>> contours_poly( contours.size() );
  vector<Rect> boundRect(contours.size());

  boundRect[0] = boundingRect( Mat(contours[0]) );
  boundRect[0].height=boundRect[0].height+20;
  boundRect[0].width=boundRect[0].width+20;
  boundRect[0].x=boundRect[0].x-10;
  boundRect[0].y=boundRect[0].y-10;

  Mat roi=img_dst(boundRect[0]);
  imwrite("roi.jpg",roi);
 // namedWindow( "ROI Image", CV_WINDOW_AUTOSIZE );
  //imshow( "ROI Image", roi );
 // waitKey(0);

  Mat refroi=imread("refOCR.jpg",0);

  //Mat mask_image(roi.size(),CV_8U, Scalar(0));
  Mat extroi=Mat::ones(roi.rows, roi.cols,CV_8U)*255;
  refroi.copyTo(extroi.colRange(0,refroi.cols).rowRange(0,refroi.rows));
 // extroi.copyTo(refroi,mask_image);
  
  //namedWindow( "Extended ROI Image", CV_WINDOW_AUTOSIZE );
 // imshow( "Extended ROI Image", extroi);
 // waitKey(0);

  /*SurfFeatureDetector detector_roi(minHessian);
  SurfDescriptorExtractor extractor_roi;

  detector_roi.detect(roi, keypoints_roi);
  detector_roi.detect(extroi, keypoints_extroi);
  extractor_roi.compute(extroi, keypoints_extroi, descriptors_extroi);*/

  //SurfFeatureDetector detector(minHessian);
  //SurfDescriptorExtractor extractor;

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

  //printf("-- Max dist : %f \n", max_dist );
  //printf("-- Min dist : %f \n", min_dist );

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
 // Mat H = estimateRigidTransform(obj,scene,false);
  
  Mat H2 = findHomography(proi,pextroi, CV_RANSAC);
  

 // cout << "H = "<< endl << " "  << H << endl << endl;
   
   Mat img_roi;//=Mat::zeros(img_object.rows, img_object.cols,img_object.type());
   //warpPerspective( roi, img_roi, H2, extroi.size(),INTER_LINEAR, BORDER_CONSTANT,Scalar(int(roi.at<uchar>(300,1)-10)));
   warpPerspective( roi, img_roi, H2, extroi.size(),INTER_LINEAR);
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
 

  vector<vector<Point> > contoursroi;
  vector<Vec4i> hierarchyroi;
  findContours(mor_roi,contoursroi,hierarchyroi,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

  vector<Rect> boundRectroi(contoursroi.size());
  //approxPolyDP( Mat(contours[0]), contours_poly[0], 3, true );
  boundRectroi[0] = boundingRect( Mat(contoursroi[0]) );
  boundRectroi[0].height=boundRectroi[0].height;
  boundRectroi[0].width=boundRectroi[0].width+8;
  boundRectroi[0].x=boundRectroi[0].x-5;
  boundRectroi[0].y=boundRectroi[0].y;

  Mat ocr=img_roi(boundRectroi[0]);
  imwrite("ocr.jpg",ocr);
  //namedWindow( "OCR Image", CV_WINDOW_AUTOSIZE );
  //imshow( "OCR Image", ocr);
  //waitKey(0);
  //FreeConsole(); 
 
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
  //system("new.exe ocr.jpg 255 2 128");
  ShellExecute(NULL, NULL, "new.exe ocr.jpg 255 2 128", NULL, NULL,SW_HIDE);
    string str;
	ifstream infile("ocrresult.txt");
	while(getline(infile, str)) // To get you all the lines.getline(infile,str)
        {
			infile>>str;
			GetDlgItem(IDC_EDIT1)->SetWindowText(str.c_str());
        }
	infile.close();	
   }
  }


 