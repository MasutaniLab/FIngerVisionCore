// -*- C++ -*-
/*!
 * @file  FingerVisionCore.cpp
 * @brief Core module for FingerVision
 * @date $Date$
 *
 * $Id$
 */

#include <chrono>
#include <thread>
#include "FingerVisionCore.h"

using namespace std;

// Module specification
// <rtc-template block="module_spec">
static const char* fingervisioncore_spec[] =
  {
    "implementation_id", "FingerVisionCore",
    "type_name",         "FingerVisionCore",
    "description",       "Core module for FingerVision",
    "version",           "1.0.0",
    "vendor",            "MasutaniLab",
    "category",          "Sensor",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.cam_config", "config/cam1.yaml",
    "conf.default.blobtrack_config", "config/cam1.yaml",
    "conf.default.objdettrack_config", "config/cam1.yaml",
    "conf.default.blob_calib_prefix", "blob_",
    "conf.default.vout_base", "vout-",
    "conf.default.frame_skip", "0",
    "conf.default.target_fps", "0",
    "conf.default.pkg_dir", ".",

    // Widget
    "conf.__widget__.cam_config", "text",
    "conf.__widget__.blobtrack_config", "text",
    "conf.__widget__.objdettrack_config", "text",
    "conf.__widget__.blob_calib_prefix", "text",
    "conf.__widget__.vout_base", "text",
    "conf.__widget__.frame_skip", "text",
    "conf.__widget__.target_fps", "text",
    "conf.__widget__.pkg_dir", "text",
    // Constraints

    "conf.__type__.cam_config", "string",
    "conf.__type__.blobtrack_config", "string",
    "conf.__type__.objdettrack_config", "string",
    "conf.__type__.blob_calib_prefix", "string",
    "conf.__type__.vout_base", "string",
    "conf.__type__.frame_skip", "int",
    "conf.__type__.target_fps", "double",
    "conf.__type__.pkg_dir", "string",

    ""
  };
// </rtc-template>

void OnMouse(int event, int x, int y, int flags, void *data);

//ros::Rateの代わり
class Rate
{
private:
  std::chrono::high_resolution_clock::time_point start_;
  std::chrono::nanoseconds expected_cycle_time_, actual_cycle_time_;

public:
  Rate(double frequency)
    : start_(std::chrono::high_resolution_clock::now())
    , expected_cycle_time_((long long)(1e9 / frequency))
    , actual_cycle_time_(0)
  {}

  bool sleep()
  {
    std::chrono::high_resolution_clock::time_point
      expected_end = start_ + expected_cycle_time_;

    std::chrono::high_resolution_clock::time_point
      actual_end = std::chrono::high_resolution_clock::now();

    // detect backward jumps in time
    if (actual_end < start_)
    {
      expected_end = actual_end + expected_cycle_time_;
    }

    //calculate the time we'll sleep for
    std::chrono::nanoseconds sleep_time = expected_end - actual_end;

    //set the actual amount of time the loop took in case the user wants to know
    actual_cycle_time_ = actual_end - start_;

    //make sure to reset our start time
    start_ = expected_end;

    //if we've taken too much time we won't sleep
    if (sleep_time <= std::chrono::nanoseconds(0))
    {
      // if we've jumped forward in time, or the loop has taken more than a full extra
      // cycle, reset our cycle
      if (actual_end > expected_end + expected_cycle_time_)
      {
        start_ = actual_end;
      }
      return true;
    }

    std::this_thread::sleep_for(sleep_time);
    return true;
  }

  void reset() {
    start_ = std::chrono::high_resolution_clock::now();
  }

  std::chrono::nanoseconds cycleTime() {
    return actual_cycle_time_;
  }

  std::chrono::nanoseconds expectedCycleTime() {
    return expected_cycle_time_;
  }

};


/*!
 * @brief constructor
 * @param manager Maneger Object
 */
FingerVisionCore::FingerVisionCore(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_blob_moves0Out("blob_moves0", m_blob_moves0),
    m_prox_vision0Out("prox_vision0", m_prox_vision0),
    m_blob_moves1Out("blob_moves1", m_blob_moves1),
    m_prox_vision1Out("prox_vision1", m_prox_vision1),
    m_servicePort("service")

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
FingerVisionCore::~FingerVisionCore()
{
}



RTC::ReturnCode_t FingerVisionCore::onInitialize()
{
  RTC_INFO(("onInitialize()"));
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  
  // Set OutPort buffer
  addOutPort("blob_moves0", m_blob_moves0Out);
  addOutPort("prox_vision0", m_prox_vision0Out);
  addOutPort("blob_moves1", m_blob_moves1Out);
  addOutPort("prox_vision1", m_prox_vision1Out);
  
  // Set service provider to Ports
  m_servicePort.registerProvider("coreService", "FingerVision::CoreService", m_coreService);
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  addPort(m_servicePort);
  
  // </rtc-template>

  //サービスポートクラスからRTCのクラスにアクセスできるようにする．
  m_coreService.setRTC(this);

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("cam_config", m_cam_config, "config/cam1.yaml");
  bindParameter("blobtrack_config", m_blobtrack_config, "config/cam1.yaml");
  bindParameter("objdettrack_config", m_objdettrack_config, "config/cam1.yaml");
  bindParameter("blob_calib_prefix", m_blob_calib_prefix, "blob_");
  bindParameter("vout_base", m_vout_base, "vout-");
  bindParameter("frame_skip", m_frame_skip, "0");
  bindParameter("target_fps", m_target_fps, "0");
  bindParameter("pkg_dir", m_pkg_dir, ".");
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t FingerVisionCore::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t FingerVisionCore::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t FingerVisionCore::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t FingerVisionCore::onActivated(RTC::UniqueId ec_id)
{
  RTC_INFO(("onActivated()"));
  m_rtcError = false;  //OpenRTM-aist-1.2.0のバグ回避

  //オリジナルのグローバル変数
  Running = true;
  Shutdown = false;
  DoCalibrate = false;
  CurrentWin = nullptr;

  FrameSkip = m_frame_skip;
  TargetFPS = m_target_fps;

  VecBlobMoves.resize(2);
  VecBlobMoves[0] = &m_blob_moves0;
  VecBlobMoves[1] = &m_blob_moves1;
  
  VecBlobMovesOut.resize(2);
  VecBlobMovesOut[0] = &m_blob_moves0Out;
  VecBlobMovesOut[1] = &m_blob_moves1Out;
  
  VecProxVision.resize(2);
  VecProxVision[0] = &m_prox_vision0;
  VecProxVision[1] = &m_prox_vision1;
  
  VecProxVisionOut.resize(2);
  VecProxVisionOut[0] = &m_prox_vision0Out;
  VecProxVisionOut[1] = &m_prox_vision1Out;

  BlobCalibPrefix = "blob_";
  DimLevels[0] = 0.3;
  DimLevels[1] = 0.7;
  DimLevels[2] = 1.0;
  DimIdx = 0;

  //オリジナルのmain()はここから．
  std::cerr << "pkg_dir: " << m_pkg_dir << std::endl;
  std::cerr << "cam_config: " << m_cam_config << std::endl;
  std::cerr << "blobtrack_config: " << m_blobtrack_config << std::endl;
  std::cerr << "objdettrack_config: " << m_objdettrack_config << std::endl;
  std::cerr << "blob_calib_prefix: " << m_blob_calib_prefix << std::endl;

  std::vector<TBlobTracker2Params> blobtrack_info;
  std::vector<TObjDetTrackBSPParams> objdettrack_info;
  ReadFromYAML(CamInfo, CheckYAMLExistence(m_pkg_dir + "/" + m_cam_config));
  ReadFromYAML(blobtrack_info, CheckYAMLExistence(m_pkg_dir + "/" + m_blobtrack_config));
  ReadFromYAML(objdettrack_info, CheckYAMLExistence(m_pkg_dir + "/" + m_objdettrack_config));
  BlobCalibPrefix = m_pkg_dir + "/" + m_blob_calib_prefix;

  if (CamInfo.size() <= 0) {
    RTC_ERROR(("カメラが設定できない"));
    m_rtcError = true;
    return RTC::RTC_ERROR;
  }

  if (CamInfo.size() > 2) {
    RTC_INFO(("カメラ%d個には対応できないので2個に変更", CamInfo.size()));
    CamInfo.resize(2);
  }
  cap.resize(CamInfo.size());
  SingleCamRectifier.resize(CamInfo.size());
  CamRectifier.resize(CamInfo.size());
  Frame.resize(CamInfo.size());
  CapTime.resize(CamInfo.size());
  for (int i_cam(0), i_cam_end(CamInfo.size()); i_cam<i_cam_end; ++i_cam)
  {
    TCameraInfo &info(CamInfo[i_cam]);
    if (!CapOpen(info, cap[i_cam])) {
      m_rtcError = true;
      return RTC::RTC_ERROR;
    }
    MutCamCapture.push_back(boost::shared_ptr<boost::mutex>(new boost::mutex));
    MutFrameCopy.push_back(boost::shared_ptr<boost::mutex>(new boost::mutex));

    if (info.Rectification)
    {
      // Setup rectification
      // NOTE: The rectification of StereoInfo overwrites this rectification.
      cv::Size size_in(info.Width, info.Height), size_out(info.Width, info.Height);
      SingleCamRectifier[i_cam].Setup(info.K, info.D, info.R, size_in, info.Alpha, size_out);
      CamRectifier[i_cam] = boost::bind(&TCameraRectifier::Rectify, SingleCamRectifier[i_cam], _1, /*border=*/cv::Scalar(0, 0, 0));
    } else
    {
      CamRectifier[i_cam] = &FingerVisionCore::DummyRectify;
    }
  }
  std::cerr << "Opened camera(s)" << std::endl;

  BlobTracker.resize(CamInfo.size());
  TwinPointersBT.resize(CamInfo.size());
  for (int j(0), j_end(CamInfo.size()); j<j_end; ++j)
  {
    BlobTracker[j].Name = CamInfo[j].Name + "-blob";
    BlobTracker[j].Params() = blobtrack_info[j];
    BlobTracker[j].Init();
    if (FileExists(BlobCalibPrefix + CamInfo[j].Name + ".yaml"))
      BlobTracker[j].LoadCalib(BlobCalibPrefix + CamInfo[j].Name + ".yaml");
    WindowInfo[BlobTracker[j].Name] = TWindowInfo(j, "BlobTracker", j);
    cv::namedWindow(BlobTracker[j].Name, 1);
    TwinPointersBT[j].first = this;
    TwinPointersBT[j].second = &BlobTracker[j].Name;
    cv::setMouseCallback(BlobTracker[j].Name, OnMouse, &TwinPointersBT[j]);
    IMShowStuff[BlobTracker[j].Name].Mutex = boost::shared_ptr<boost::mutex>(new boost::mutex);
    VideoOut[BlobTracker[j].Name].SetfilePrefix(m_vout_base + BlobTracker[j].Name);
    ShowTrackbars[BlobTracker[j].Name].Enabled = false;
    ShowTrackbars[BlobTracker[j].Name].Kind = "BlobTracker";
  }

  ObjDetTracker.resize(CamInfo.size());
  TwinPointersODT.resize(CamInfo.size());
  for (int j(0), j_end(CamInfo.size()); j<j_end; ++j)
  {
    ObjDetTracker[j].Name = CamInfo[j].Name + "-pxv";
    ObjDetTracker[j].Params() = objdettrack_info[j];
    ObjDetTracker[j].Init();
    WindowInfo[ObjDetTracker[j].Name] = TWindowInfo(j, "ObjDetTracker", j);
    cv::namedWindow(ObjDetTracker[j].Name, 1);
    TwinPointersODT[j].first = this;
    TwinPointersODT[j].second = &ObjDetTracker[j].Name;
    cv::setMouseCallback(ObjDetTracker[j].Name, OnMouse, &TwinPointersODT[j]);
    IMShowStuff[ObjDetTracker[j].Name].Mutex = boost::shared_ptr<boost::mutex>(new boost::mutex);
    VideoOut[ObjDetTracker[j].Name].SetfilePrefix(m_vout_base + ObjDetTracker[j].Name);
    ShowTrackbars[ObjDetTracker[j].Name].Kind = "ObjDetTracker";
  }

  show_fps = 0;

  // Dummy capture.
  for (int i_cam(0), i_cam_end(CamInfo.size()); i_cam<i_cam_end; ++i_cam)
  {
    cap[i_cam] >> Frame[i_cam];
    CapTime[i_cam] = GetCurrentTimeL();
  }

  // Calibrate ObjDetTracker
  if (ObjDetTracker.size()>0)
  {
    std::vector<std::vector<cv::Mat> > frames(CamInfo.size());
    for (int i_cam(0), i_cam_end(CamInfo.size()); i_cam<i_cam_end; ++i_cam)
      frames[i_cam] = CaptureSeq(cap[i_cam], i_cam, ObjDetTracker[i_cam].Params().NCalibBGFrames);
    for (int j(0), j_end(ObjDetTracker.size()); j<j_end; ++j)
      ObjDetTracker[j].CalibBG(frames[j]);
  }

  for (int j(0), j_end(CamInfo.size()); j < j_end; ++j) {
    th_blobtrack.push_back(
      boost::shared_ptr<boost::thread>(
        new boost::thread(boost::bind(&FingerVisionCore::ExecBlobTrack, this, j))));
  }
  for (int j(0), j_end(CamInfo.size()); j < j_end; ++j) {
    th_objdettrack.push_back(
      boost::shared_ptr<boost::thread>(
        new boost::thread(boost::bind(&FingerVisionCore::ExecObjDetTrack, this, j))));
  }

  f = 0;

  return RTC::RTC_OK;
}


RTC::ReturnCode_t FingerVisionCore::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_INFO(("onDeactivated()"));
  Shutdown = true;
  for (int j(0), j_end(th_blobtrack.size()); j < j_end; ++j) {
    th_blobtrack[j]->join();
  }
  for (int j(0), j_end(th_objdettrack.size()); j < j_end; ++j) {
    th_objdettrack[j]->join();
  }
  cv::destroyAllWindows();
  return RTC::RTC_OK;
}


RTC::ReturnCode_t FingerVisionCore::onExecute(RTC::UniqueId ec_id)
{
  if (m_rtcError) return RTC::RTC_ERROR; //OpenRTM-aist-1.2.0のバグ回避

  if (Running)
  {
    // Capture from cameras:
    for (int i_cam(0), i_cam_end(CamInfo.size()); i_cam<i_cam_end; ++i_cam)
    {
      cv::Mat frame = Capture(cap[i_cam], i_cam, /*rectify=*/false);
      if (FrameSkip <= 0 || f % (FrameSkip + 1) == 0)
      {
        boost::mutex::scoped_lock lock(*MutFrameCopy[i_cam]);
        Frame[i_cam] = frame;
        CapTime[i_cam] = GetCurrentTimeL();
      }
    }

    // Show windows
    for (std::map<std::string, TIMShowStuff>::iterator itr(IMShowStuff.begin()), itr_end(IMShowStuff.end()); itr != itr_end; ++itr)
    {
      boost::mutex::scoped_lock lock(*itr->second.Mutex);
      if (itr->second.Frame.total()>0)
        cv::imshow(itr->first, itr->second.Frame);
    }

    // Handle blob tracker calibration request
    if (DoCalibrate && BlobTracker.size()>0 && *CurrentWin != "" && WindowInfo[*CurrentWin].Kind == "BlobTracker")
    {
      Running = false;
      int i_cam(WindowInfo[*CurrentWin].CamIdx), idx(WindowInfo[*CurrentWin].Index);
      std::vector<cv::Mat> frames = CaptureSeq(cap[i_cam], i_cam, BlobTracker[idx].Params().NCalibPoints);
      BlobTracker[idx].Calibrate(frames);
      Running = true;
    }
    if (DoCalibrate && ObjDetTracker.size()>0 && *CurrentWin != "" && WindowInfo[*CurrentWin].Kind == "ObjDetTracker")
    {
      Running = false;
      int i_cam(WindowInfo[*CurrentWin].CamIdx), idx(WindowInfo[*CurrentWin].Index);
      std::vector<cv::Mat> frames = CaptureSeq(cap[i_cam], i_cam, ObjDetTracker[idx].Params().NCalibBGFrames);
      ObjDetTracker[idx].CalibBG(frames);
      Running = true;
    }
    DoCalibrate = false;

    // usleep(10*1000);
    if (show_fps == 0)
    {
      std::cerr << "FPS: " << VideoOut.begin()->second.FPS() << std::endl;
      show_fps = VideoOut.begin()->second.FPS() * 4;
    }
    --show_fps;

  }  // Running
  else
  {
    coil::usleep(200 * 1000);
  }

  if (!HandleKeyEvent()) {
    deactivate(ec_id);
  }

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t FingerVisionCore::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t FingerVisionCore::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t FingerVisionCore::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t FingerVisionCore::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t FingerVisionCore::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

//void DummyRectify(cv::Mat&) {}  // Do nothing function

void OnMouse(int event, int x, int y, int flags, void *data) {
  FingerVisionCore::TwinPointers *ptp
    = static_cast<FingerVisionCore::TwinPointers*>(data);
  FingerVisionCore *pfvc
    = static_cast<FingerVisionCore*>(ptp->first);
  if (pfvc) {
    pfvc->OnMouseInternal(event, x, y, flags, ptp->second);
  }
}

void FingerVisionCore::OnMouseInternal(int event, int x, int y, int flags, void *data)
{
  if (event != 0)
  {
    CurrentWin = reinterpret_cast<std::string*>(data);
    std::cerr << "CurrentWin: " << *CurrentWin << std::endl;
  }

  // if(flags!=0)  return;
  if (event == cv::EVENT_RBUTTONDOWN)
  {
    Running = !Running;
    std::cerr << (Running ? "Resume" : "Pause (Hit space/R-click to resume)") << std::endl;
  }
  if (event == cv::EVENT_LBUTTONDOWN && (flags & cv::EVENT_FLAG_SHIFTKEY))
  {
    if (WindowInfo[*CurrentWin].Kind == "ObjDetTracker")
    {
      int i_cam(WindowInfo[*CurrentWin].CamIdx), idx(WindowInfo[*CurrentWin].Index);
      cv::Mat frame;
      {
        boost::mutex::scoped_lock lock(*MutFrameCopy[i_cam]);
        Frame[i_cam].copyTo(frame);
      }
      ObjDetTracker[idx].AddToModel(frame, cv::Point(x, y));
    }
  }
}

bool FingerVisionCore::HandleKeyEvent()
{
  // keyboard interface:
  char c(cv::waitKey(1));
  //cout << "c: '" << c << "' (" << hex << int(c) << ")" << endl;
  if (c == '\x1b' || c == 'q') return false;
  else if (c == 'W')
  {
    for (std::map<std::string, TEasyVideoOut>::iterator itr(VideoOut.begin()), itr_end(VideoOut.end()); itr != itr_end; ++itr)
      itr->second.Switch();
  } else if (c == 'm')
  {
    DimIdx++;
    if (DimIdx >= int(sizeof(DimLevels) / sizeof(DimLevels[0])))  DimIdx = 0;
  } else if (c == ' ')
  {
    Running = !Running;
    std::cerr << (Running ? "Resume" : "Pause (Hit space/R-click to resume)") << std::endl;
  } else if (c == 'c')
  {
    DoCalibrate = true;
  } else if (c == 'r')
  {
    if (CurrentWin != NULL && WindowInfo[*CurrentWin].Kind == "ObjDetTracker")
    {
      int idx(WindowInfo[*CurrentWin].Index);
      ObjDetTracker[idx].ClearObject();
    }
  } else if (c == 'd')
  {
    if (CurrentWin != NULL && WindowInfo[*CurrentWin].Kind == "ObjDetTracker")
    {
      int idx(WindowInfo[*CurrentWin].Index);
      if (ObjDetTracker[idx].ModeDetect())  ObjDetTracker[idx].StopDetect();
      else                                 ObjDetTracker[idx].StartDetect();
      std::cerr << "Object detection mode is: " << (ObjDetTracker[idx].ModeDetect() ? "on" : "off") << std::endl;
    }
  } else if (c == 's')
  {
    if (CurrentWin != NULL && WindowInfo[*CurrentWin].Kind == "BlobTracker")
    {
      int i_cam(WindowInfo[*CurrentWin].CamIdx), idx(WindowInfo[*CurrentWin].Index);
      BlobTracker[idx].SaveCalib(BlobCalibPrefix + CamInfo[i_cam].Name + ".yaml");
      std::cerr << "Saved calibration data of " << BlobTracker[idx].Name << " to " << BlobCalibPrefix + CamInfo[i_cam].Name + ".yaml" << std::endl;
    }
  } else if (c == 'l')
  {
    if (CurrentWin != NULL && WindowInfo[*CurrentWin].Kind == "BlobTracker")
    {
      int i_cam(WindowInfo[*CurrentWin].CamIdx), idx(WindowInfo[*CurrentWin].Index);
      BlobTracker[idx].LoadCalib(BlobCalibPrefix + CamInfo[i_cam].Name + ".yaml");
      std::cerr << "Loaded calibration data of " << BlobTracker[idx].Name << " from " << BlobCalibPrefix + CamInfo[i_cam].Name + ".yaml" << std::endl;
    }
  } else if (c == 'C' && CurrentWin != NULL)
  {
    ShowTrackbars[*CurrentWin].Enabled = !ShowTrackbars[*CurrentWin].Enabled;
    if (ShowTrackbars[*CurrentWin].Enabled)
    {
      if (ShowTrackbars[*CurrentWin].Kind == "BlobTracker")
      {
        int idx(WindowInfo[*CurrentWin].Index);
        cv::createTrackbar("thresh_v", *CurrentWin, &(BlobTracker[idx].Params().ThreshV), 255, NULL);
      } else if (ShowTrackbars[*CurrentWin].Kind == "ObjDetTracker")
      {
        std::cerr << "Not implemented yet. Show trackbars for ObjDetTracker" << std::endl;
      }
    } else
    {
      // Remove trackbars from window.
      if (ShowTrackbars[*CurrentWin].Kind == "BlobTracker")
      {
        cv::destroyWindow(*CurrentWin);
        cv::namedWindow(*CurrentWin, 1);
        int idx(WindowInfo[*CurrentWin].Index);
        cv::setMouseCallback(*CurrentWin, OnMouse, &TwinPointersBT[idx]);
      } else if (ShowTrackbars[*CurrentWin].Kind == "ObjDetTracker")
      {
        cv::destroyWindow(*CurrentWin);
        cv::namedWindow(*CurrentWin, 1);
        int idx(WindowInfo[*CurrentWin].Index);
        cv::setMouseCallback(*CurrentWin, OnMouse, &TwinPointersODT[idx]);
      }
    }
  }
  //cout << "End of HandleKeyEvent()" << endl;
  return true;
}

void FingerVisionCore::ExecBlobTrack(int i_cam)
{
  TCameraInfo &info(CamInfo[i_cam]);
  TBlobTracker2 &tracker(BlobTracker[i_cam]);
  cv::Mat frame;
  int64_t t_cap(0);
  Rate rate(TargetFPS>0.0 ? TargetFPS : 1);
  for (int seq(0); !Shutdown; ++seq) {
    if (Running) {
      if (CapTime[i_cam] == t_cap) {
        coil::usleep(10 * 1000);
        continue;
      }
      if (TargetFPS>0.0)  rate.sleep();

      {
        boost::mutex::scoped_lock lock(*MutFrameCopy[i_cam]);
        Frame[i_cam].copyTo(frame);
        t_cap = CapTime[i_cam];
      }
      CamRectifier[i_cam](frame);
      tracker.Step(frame);
      tracker.Draw(frame);

      VideoOut[tracker.Name].Step(frame);
      VideoOut[tracker.Name].VizRec(frame);

      {
        // cv::imshow(info.Name, frame);
        boost::mutex::scoped_lock lock(*IMShowStuff[tracker.Name].Mutex);
        frame.copyTo(IMShowStuff[tracker.Name].Frame);
      }

      // Publish as BlobMoves
      {
        const std::vector<TPointMove2> &data(tracker.Data());
        FingerVision::BlobMoves *p = VecBlobMoves[i_cam];
        p->seq = seq;
        coil::TimeValue tv(coil::gettimeofday());
        p->tm.sec = tv.sec();
        p->tm.nsec = tv.usec() * 1000;
        p->frame_id = info.Name.c_str();
        p->camera_index = i_cam;
        p->camera_name = info.Name.c_str();
        p->width = info.Width;
        p->height = info.Height;
        p->data.length(data.size());
        int i(0);
        for (std::vector<TPointMove2>::const_iterator itr(data.begin()), itr_end(data.end()); itr != itr_end; ++itr, ++i) {
          FingerVision::BlobMove &m(p->data[i]);
          m.Pox = itr->Po.x;
          m.Poy = itr->Po.y;
          m.So = itr->So;
          m.DPx = itr->DP.x;
          m.DPy = itr->DP.y;
          m.DS = itr->DS;
        }
        VecBlobMovesOut[i_cam]->write();
      }
      // usleep(10*1000);
    }  // Running
    else {
      coil::usleep(200 * 1000);
    }
  }
}

void FingerVisionCore::ExecObjDetTrack(int i_cam)
{
  TCameraInfo &info(CamInfo[i_cam]);
  TObjDetTrackBSP &tracker(ObjDetTracker[i_cam]);
  cv::Mat frame;
  int64_t t_cap(0);
  Rate rate(TargetFPS>0.0 ? TargetFPS : 1);
  for (int seq(0); !Shutdown; ++seq) {
    if (Running) {
      if (CapTime[i_cam] == t_cap) {
        coil::usleep(10 * 1000);
        continue;
      }
      if (TargetFPS>0.0)  rate.sleep();

      {
        boost::mutex::scoped_lock lock(*MutFrameCopy[i_cam]);
        Frame[i_cam].copyTo(frame);
        t_cap = CapTime[i_cam];
      }
      CamRectifier[i_cam](frame);
      tracker.Step(frame);
      frame *= DimLevels[DimIdx];
      tracker.Draw(frame);

      VideoOut[tracker.Name].Step(frame);
      VideoOut[tracker.Name].VizRec(frame);

      {
        // cv::imshow(info.Name, frame);
        boost::mutex::scoped_lock lock(*IMShowStuff[tracker.Name].Mutex);
        frame.copyTo(IMShowStuff[tracker.Name].Frame);
      }

      // Publish as PXVPub
      {
        const cv::Mat &objs(tracker.ObjS()), &mvs(tracker.MvS());
        const cv::Moments &om(tracker.ObjMoments());
        FingerVision::ProxVision *p = VecProxVision[i_cam];
        p->seq = seq;
        coil::TimeValue tv(coil::gettimeofday());
        p->tm.sec = tv.sec();
        p->tm.nsec = tv.usec() * 1000;
        p->frame_id = info.Name.c_str();
        p->camera_index = i_cam;
        p->camera_name = info.Name.c_str();
        p->width = info.Width;
        p->height = info.Height;

        p->ObjM_m.length(10);
        p->ObjM_m[0] = om.m00;
        p->ObjM_m[1] = om.m10;
        p->ObjM_m[2] = om.m01;
        p->ObjM_m[3] = om.m20;
        p->ObjM_m[4] = om.m11;
        p->ObjM_m[5] = om.m02;
        p->ObjM_m[6] = om.m30;
        p->ObjM_m[7] = om.m21;
        p->ObjM_m[8] = om.m12;
        p->ObjM_m[9] = om.m03;

        p->ObjM_mu.length(7);
        p->ObjM_mu[0] = om.mu20;
        p->ObjM_mu[1] = om.mu11;
        p->ObjM_mu[2] = om.mu02;
        p->ObjM_mu[3] = om.mu30;
        p->ObjM_mu[4] = om.mu21;
        p->ObjM_mu[5] = om.mu12;
        p->ObjM_mu[6] = om.mu03;

        p->ObjM_nu.length(7);
        p->ObjM_nu[0] = om.nu20;
        p->ObjM_nu[1] = om.nu11;
        p->ObjM_nu[2] = om.nu02;
        p->ObjM_nu[3] = om.nu30;
        p->ObjM_nu[4] = om.nu21;
        p->ObjM_nu[5] = om.nu12;
        p->ObjM_nu[6] = om.nu03;

        p->ObjS.length(objs.rows*objs.cols);
        p->ObjS.length(objs.rows*objs.cols);
        for (int r(0), rend(objs.rows), i(0); r < rend; ++r) for (int c(0), cend(objs.cols); c < cend; ++c, ++i) {
          p->ObjS[i] = objs.at<float>(r, c);
        }
        p->MvS.length(mvs.rows*mvs.cols);
        for (int r(0), rend(mvs.rows), i(0); r < rend; ++r) for (int c(0), cend(mvs.cols); c < cend; ++c, ++i) {
          p->MvS[i] = mvs.at<float>(r, c);
        }
        VecProxVisionOut[i_cam]->write();
      }
      // usleep(10*1000);
    }  // Running
    else {
      coil::usleep(200 * 1000);
    }
  }
}

cv::Mat FingerVisionCore::Capture(cv::VideoCapture &cap, int i_cam, bool rectify)
{
  cv::Mat frame;
  {
    boost::mutex::scoped_lock lock(*MutCamCapture[i_cam]);
    while (!cap.read(frame)) {
      if (CapWaitReopen(CamInfo[i_cam], cap)) continue;
      else  return cv::Mat();
    }
  }
  if (CamInfo[i_cam].CapWidth != CamInfo[i_cam].Width || CamInfo[i_cam].CapHeight != CamInfo[i_cam].Height)
    cv::resize(frame, frame, cv::Size(CamInfo[i_cam].Width, CamInfo[i_cam].Height));
  Rotate90N(frame, frame, CamInfo[i_cam].NRotate90);
  if (rectify)  CamRectifier[i_cam](frame);
  return frame;
}

// Capture a sequence of images.
std::vector<cv::Mat> FingerVisionCore::CaptureSeq(cv::VideoCapture &cap, int i_cam, int num)
{
  std::vector<cv::Mat> frames;
  for (int i(0); i<num; ++i)
    frames.push_back(Capture(cap, i_cam, /*rectify=*/true));
  return frames;
}

inline std::string FingerVisionCore::CheckYAMLExistence(const std::string &filename)
{
  std::cerr << "Loading from YAML: " << filename << std::endl;
  if (!FileExists(filename))
    std::cerr << "!!!File not found: " << filename << std::endl;
  return filename;
}


extern "C"
{
 
  void FingerVisionCoreInit(RTC::Manager* manager)
  {
    coil::Properties profile(fingervisioncore_spec);
    manager->registerFactory(profile,
                             RTC::Create<FingerVisionCore>,
                             RTC::Delete<FingerVisionCore>);
  }
  
};


