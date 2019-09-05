// -*- C++ -*-
/*!
 * @file  FingerVisionCore.h
 * @brief Core module for FingerVision
 * @date  $Date$
 *
 * $Id$
 */

#ifndef FINGERVISIONCORE_H
#define FINGERVISIONCORE_H

#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>

// Service implementation headers
// <rtc-template block="service_impl_h">
#include "FingerVisionSVC_impl.h"

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">
#include "FingerVisionStub.h"

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="port_stub_h">
using namespace FingerVision;
// </rtc-template>

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>

#include "blob_tracker2.h"
#include "prox_vision.h"
#include "ay_vision/vision_util.h"
#include "ay_cpp/geom_util.h"
#include "ay_cpp/sys_util.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

using namespace RTC;
using namespace trick;

/*!
 * @class FingerVisionCore
 * @brief Core module for FingerVision
 *
 */
class FingerVisionCore
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  FingerVisionCore(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~FingerVisionCore();

  // <rtc-template block="public_attribute">
  
  // </rtc-template>

  // <rtc-template block="public_operation">
  
  // </rtc-template>

  /***
   *
   * The initialize action (on CREATED->ALIVE transition)
   * formaer rtc_init_entry() 
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onInitialize();

  /***
   *
   * The finalize action (on ALIVE->END transition)
   * formaer rtc_exiting_entry()
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onFinalize();

  /***
   *
   * The startup action when ExecutionContext startup
   * former rtc_starting_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onStartup(RTC::UniqueId ec_id);

  /***
   *
   * The shutdown action when ExecutionContext stop
   * former rtc_stopping_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onShutdown(RTC::UniqueId ec_id);

  /***
   *
   * The activated action (Active state entry action)
   * former rtc_active_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onActivated(RTC::UniqueId ec_id);

  /***
   *
   * The deactivated action (Active state exit action)
   * former rtc_active_exit()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onDeactivated(RTC::UniqueId ec_id);

  /***
   *
   * The execution action that is invoked periodically
   * former rtc_active_do()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

  /***
   *
   * The aborting action when main logic error occurred.
   * former rtc_aborting_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onAborting(RTC::UniqueId ec_id);

  /***
   *
   * The error action in ERROR state
   * former rtc_error_do()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onError(RTC::UniqueId ec_id);

  /***
   *
   * The reset action that is invoked resetting
   * This is same but different the former rtc_init_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onReset(RTC::UniqueId ec_id);
  
  /***
   *
   * The state update action that is invoked after onExecute() action
   * no corresponding operation exists in OpenRTm-aist-0.2.0
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onStateUpdate(RTC::UniqueId ec_id);

  /***
   *
   * The action that is invoked when execution context's rate is changed
   * no corresponding operation exists in OpenRTm-aist-0.2.0
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onRateChanged(RTC::UniqueId ec_id);


 protected:
  // <rtc-template block="protected_attribute">
  
  // </rtc-template>

  // <rtc-template block="protected_operation">
  
  // </rtc-template>

  // Configuration variable declaration
  // <rtc-template block="config_declare">
  /*!
   * 
   * - Name:  cam_config
   * - DefaultValue: config/cam1.yaml
   */
  std::string m_cam_config;
  /*!
   * 
   * - Name:  blobtrack_config
   * - DefaultValue: config/cam1.yaml
   */
  std::string m_blobtrack_config;
  /*!
   * 
   * - Name:  objdettrack_config
   * - DefaultValue: config/cam1.yaml
   */
  std::string m_objdettrack_config;
  /*!
   * 
   * - Name:  blob_calib_prefix
   * - DefaultValue: blob_
   */
  std::string m_blob_calib_prefix;
  /*!
   * 
   * - Name:  vout_base
   * - DefaultValue: vout-
   */
  std::string m_vout_base;
  /*!
   * 
   * - Name:  frame_skip
   * - DefaultValue: 0
   */
  int m_frame_skip;
  /*!
   * 
   * - Name:  target_fps
   * - DefaultValue: 0
   */
  double m_target_fps;
  /*!
   * 
   * - Name:  pkg_dir
   * - DefaultValue: .
   */
  std::string m_pkg_dir;

  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  FingerVision::BlobMoves m_blob_moves0;
  /*!
   */
  OutPort<FingerVision::BlobMoves> m_blob_moves0Out;
  FingerVision::ProxVision m_prox_vision0;
  /*!
   */
  OutPort<FingerVision::ProxVision> m_prox_vision0Out;
  FingerVision::BlobMoves m_blob_moves1;
  /*!
   */
  OutPort<FingerVision::BlobMoves> m_blob_moves1Out;
  FingerVision::ProxVision m_prox_vision1;
  /*!
   */
  OutPort<FingerVision::ProxVision> m_prox_vision1Out;
  
  // </rtc-template>

  // CORBA Port declaration
  // <rtc-template block="corbaport_declare">
  /*!
   */
  RTC::CorbaPort m_servicePort;
  
  // </rtc-template>

  // Service declaration
  // <rtc-template block="service_declare">
  /*!
   */
  FingerVision_CoreServiceSVC_impl m_coreService;
  
  // </rtc-template>

  // Consumer declaration
  // <rtc-template block="consumer_declare">
  
  // </rtc-template>


 private:
  // <rtc-template block="private_attribute">
  
  // </rtc-template>

  // <rtc-template block="private_operation">
  
  // </rtc-template>

   bool m_rtcError;  //OpenRTM-aist-1.2.0のバグ回避

   //オリジナルのグローバル変数
   bool Running;
   bool Shutdown;
   bool DoCalibrate;
   std::string *CurrentWin;

   int FrameSkip;
   double TargetFPS;

   std::string BlobCalibPrefix;
   std::vector<TCameraInfo> CamInfo;
   std::vector<TBlobTracker2> BlobTracker;  // Marker tracker
   std::vector<TObjDetTrackBSP> ObjDetTracker;  // Proximity vision
   std::vector<TCameraRectifier> SingleCamRectifier;
   std::vector<boost::function<void(cv::Mat&)> > CamRectifier;  // Functions to rectify camera images.
   //friend void ::DummyRectify(cv::Mat&);
   static void DummyRectify(cv::Mat&) {}  // Do nothing function

   std::map<std::string, TEasyVideoOut> VideoOut;
   struct TShowTrackbars
   {
     bool Enabled;
     std::string Kind;
   };
   std::map<std::string, TShowTrackbars> ShowTrackbars;

   std::vector<FingerVision::BlobMoves *> VecBlobMoves;
   std::vector<OutPort<FingerVision::BlobMoves> *> VecBlobMovesOut;
   std::vector<FingerVision::ProxVision *> VecProxVision;
   std::vector<OutPort<FingerVision::ProxVision> *> VecProxVisionOut;

   std::vector<cv::Mat> Frame;
   std::vector<int64_t> CapTime;
   std::vector<boost::shared_ptr<boost::mutex> > MutCamCapture;  // Mutex for capture
   std::vector<boost::shared_ptr<boost::mutex> > MutFrameCopy;  // Mutex for Frame
   struct TIMShowStuff
   {
     boost::shared_ptr<boost::mutex> Mutex;
     cv::Mat Frame;
   };
   std::map<std::string, TIMShowStuff> IMShowStuff;

   struct TWindowInfo
   {
     int CamIdx;  // Camera index.
     std::string Kind;  // Kind of computer vision.
     int Index;  // Index of computer vision object (BlobTracker, ObjDetTracker).
     TWindowInfo()
       : CamIdx(-1), Kind(), Index(-1)
     {
     }
     TWindowInfo(int cam_idx, const std::string &k, int idx)
       : CamIdx(cam_idx), Kind(k), Index(idx)
     {
     }
   };
   std::map<std::string, TWindowInfo> WindowInfo;

   // Dim-level of proximity vision.
   double DimLevels[3];
   int DimIdx;

   //オリジナルのmain()内変数
   std::vector<cv::VideoCapture> cap;
   int show_fps;
   std::vector<boost::shared_ptr<boost::thread> > th_blobtrack;
   std::vector<boost::shared_ptr<boost::thread> > th_objdettrack;
   int f;

   //クラス内でcv::setMouseCallback()を使うためのフレンド関数
   friend void OnMouse(int event, int x, int y, int flags, void *data);
   void OnMouseInternal(int event, int x, int y, int flags, void *data);
   bool HandleKeyEvent();


   //オリジナルのmain()以外の関数
   void ExecBlobTrack(int i_cam);
   void ExecObjDetTrack(int i_cam);
   cv::Mat Capture(cv::VideoCapture &cap, int i_cam, bool rectify);
   std::vector<cv::Mat> CaptureSeq(cv::VideoCapture &cap, int i_cam, int num);
   inline std::string CheckYAMLExistence(const std::string &filename);

 public:
   //OnMouse()のための構造体
   struct TwinPointers {
     void *first;
     void *second;
   };
 private:
   std::vector<TwinPointers> TwinPointersBT;
   std::vector<TwinPointers> TwinPointersODT;
 public:
   //サービスのための関数
   void Pause()
   {
     std::cerr << "Paused..." << std::endl;
     Running = false;
   }
   void Resume()
   {
     std::cerr << "Resumed..." << std::endl;
     Running = true;
   }
   void SetFrameSkip(long num)
   {
     std::cerr << "Setting frame skip as " << num << "..." << std::endl;
     FrameSkip = num;
   }
   void StopDetectObj()
   {
     std::cerr << "Stopping object detection..." << std::endl;
     for (int j(0), j_end(ObjDetTracker.size()); j<j_end; ++j)
       ObjDetTracker[j].StopDetect();
   }
   void StartDetectObj()
   {
     std::cerr << "Starting object detection..." << std::endl;
     for (int j(0), j_end(ObjDetTracker.size()); j<j_end; ++j)
       ObjDetTracker[j].StartDetect();
   }
   void ClearObj()
   {
     std::cerr << "Clearing object models..." << std::endl;
     for (int j(0), j_end(ObjDetTracker.size()); j<j_end; ++j)
       ObjDetTracker[j].ClearObject();
   }
};


extern "C"
{
  DLL_EXPORT void FingerVisionCoreInit(RTC::Manager* manager);
};

#endif // FINGERVISIONCORE_H
