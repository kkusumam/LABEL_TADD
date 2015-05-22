#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/video/tracking.hpp"
//#include <time.h>

using namespace std;
using namespace cv;

int main( int argc, char** argv )
{
     if( argc != 4 )
  { return -1; }
 
  Mat img_object= imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE );
  Mat img_ref= imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);
  Mat img_scene= imread(argv[3], CV_LOAD_IMAGE_GRAYSCALE);	
 
  if( !img_object.data || !img_ref.data||!img_scene.data )
  { std::cout<< " --(!) Error reading images " << std::endl; return -1; }

  
  //-- Step 1: Detect the keypoints using SURF Detector
  int minHessian = 400;
  SurfFeatureDetector detector(minHessian);
 
  vector<KeyPoint> keypoints_object, keypoints_ref, keypoints_scene, keypoints_roi, keypoints_extroi;
  detector.detect(img_object, keypoints_object );
  detector.detect(img_ref, keypoints_ref );

  //-- Step 2: Calculate descriptors (feature vectors)
  
  SurfDescriptorExtractor extractor;
 
  Mat descriptors_object, descriptors_ref, descriptors_scene,descriptors_roi,descriptors_extroi;
  
  extractor.compute( img_object, keypoints_object, descriptors_object );
  extractor.compute( img_ref, keypoints_ref, descriptors_ref );
  //extractor.compute( img_scene, keypoints_scene, descriptors_scene );

 //-- Step 3: Matching obj and ref descriptor vectors using FLANN matcher
  double max_dist1 = 0; double min_dist1 = 100;
  
  FlannBasedMatcher matcher1;
  std::vector< DMatch> matches1;  
  matcher1.match(descriptors_object, descriptors_ref,matches1);

  //-- Quick calculation of max and min distances between keypoints
   for( int i = 0; i < descriptors_object.rows; i++ )
  { double dist = matches1[i].distance;
    if( dist < min_dist1 ) min_dist1 = dist;
    if( dist > max_dist1 ) max_dist1 = dist;
   }
 //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
  std::vector< DMatch > good_matches1;

  for( int i = 0; i < descriptors_object.rows; i++ )
  { if( matches1[i].distance < 3*min_dist1 )
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
   
  warpPerspective( img_ref, img_dst1, H1, img_object.size(),INTER_LINEAR, BORDER_CONSTANT,Scalar(int(img_object.at<uchar>(300,1))) );
  //warpPerspective( img_ref, img_dst1, H1, img_object.size(),INTER_LINEAR );

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

  Mat element1=getStructuringElement(MORPH_RECT,Size(19,19));
  Mat element2=getStructuringElement(MORPH_RECT,Size(29,29));

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
  Scalar color = Scalar(0,255,255 );

  // rotated rectangle
  Point2f rect_points1[4]; 
  minRect1[0].points( rect_points1);
  //for( int j = 0; j < 4; j++ )
  //{line( drawing1, rect_points1[j], rect_points1[(j+1)%4], color, 3, 8 );}
  
  cout<<"Centre point: "<<minRect1[0].center<<endl;
  cout<<"Orientation: "<<minRect1[0].angle<<endl;
  /// Show in a window
  //namedWindow( "Reference Image", CV_WINDOW_AUTOSIZE );
  //imshow( "Reference Image", drawing1 );
  //waitKey(0);

  //-----------------------------------------------------------------------------------------------------------
  //-- Step 4: Matching obj and scene descriptor vectors using FLANN matcher
  int isContinue=0;
  do{
  detector.detect(img_scene, keypoints_scene );
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

  //printf("-- Max dist : %f \n", max_dist );
  //printf("-- Min dist : %f \n", min_dist );

  //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
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
   warpPerspective( img_scene, img_dst, H, img_object.size(),INTER_LINEAR, BORDER_CONSTANT,Scalar(int(img_object.at<uchar>(300,1)-10)));
   //warpPerspective( img_scene, img_dst, H, img_object.size(),INTER_LINEAR);
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
  
  cout<<"Centre point: "<<minRect[0].center<<endl;
  cout<<"Orientation: "<<minRect[0].angle<<endl;
  cout<<"Position error: "<<pe<<endl;
  //cout<<"Possible orientation error 1 : "<<poe<<endl;
  //cout<<"Possible orientation error 2: "<<90-poe<<endl;
  cout<<"Orientation error: "<<min(90-poe,poe)<<endl;

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
  namedWindow( "Test Image", CV_WINDOW_AUTOSIZE );
  imshow( "Test Image", drawing );
  waitKey(0);

  //if ((pe>30)||(min(90-poe,poe)>15))
  //{cout<<"Reject the tray!"<<pe<<endl;}
  //else
  //{
	if (pe>80||min(90-poe,poe)>30)  
  cout<<"Should reject the tray..."<<endl;

 // vector<vector<Point>> contours_poly( contours.size() );
  vector<Rect> boundRect(contours.size());
  //approxPolyDP( Mat(contours[0]), contours_poly[0], 3, true );
  //boundRect[0] = boundingRect( Mat(contours[0]) );
  //boundRect[0].height=max_y-min_y;//boundRect[0].height+60;
  //boundRect[0].width=max_x-min_x;//boundRect[0].width+60;
  //boundRect[0].x=min_x;//boundRect[0].x-120;
  //boundRect[0].y=min_y;//boundRect[0].y+40;

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

  Mat refroi=imread("refroi.jpg",0);

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

    SurfFeatureDetector detector(minHessian);
  SurfDescriptorExtractor extractor;

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
  system("new.exe ocr.jpg 255 2 128");
  //}
  //-- Show detected matches
 // namedWindow("Good Matches & Object detection",WINDOW_NORMAL );
 // imshow( "Good Matches & Object detection", img_matches );

 // waitKey(0);
  //float centre_diff,leftmost_diff;
  cout<<"Continue?"<<endl;
  cout<<"1: Yes; Ctrl+C: Quit"<<endl;
  cin>>isContinue;
  if (isContinue==1)
	  cout<<"Have you put a food tray on the conveyor?"<<endl;
  cout<<"1: Yes; Ctrl+C: Quit"<<endl;
  //char Filename[20];
  cin>>isContinue;
  img_scene= imread("imgsmall0000.jpg", CV_LOAD_IMAGE_GRAYSCALE);   
  //namedWindow( "New Test Image", CV_WINDOW_AUTOSIZE );
  //imshow( "New Test Image", img_scene );
  //waitKey(0);
  } while (isContinue==1);

  return 0;
}

/**
 * @function readme
 */

//void readme()
//{ std::cout << " Usage: ./SURF_Homography <img1> <img2>" << std::endl; }