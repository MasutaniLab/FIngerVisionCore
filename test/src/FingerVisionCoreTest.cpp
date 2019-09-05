// -*- C++ -*-
/*!
 * @file  FingerVisionCoreTest.cpp
 * @brief Core module for FingerVision
 * @date $Date$
 *
 * $Id$
 */

#include "FingerVisionCoreTest.h"

// Module specification
// <rtc-template block="module_spec">
static const char* fingervisioncore_spec[] =
  {
    "implementation_id", "FingerVisionCoreTest",
    "type_name",         "FingerVisionCoreTest",
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

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
FingerVisionCoreTest::FingerVisionCoreTest(RTC::Manager* manager)
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
FingerVisionCoreTest::~FingerVisionCoreTest()
{
}



RTC::ReturnCode_t FingerVisionCoreTest::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("blob_moves0", m_blob_moves0In);
  addInPort("prox_vision0", m_prox_vision0In);
  addInPort("blob_moves1", m_blob_moves1In);
  addInPort("prox_vision1", m_prox_vision1In);
  
  // Set OutPort buffer
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  m_servicePort.registerConsumer("coreService", "FingerVision::CoreService", m_coreService);
  
  // Set CORBA Service Ports
  addPort(m_servicePort);
  
  // </rtc-template>

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
RTC::ReturnCode_t FingerVisionCoreTest::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t FingerVisionCoreTest::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t FingerVisionCoreTest::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t FingerVisionCoreTest::onActivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t FingerVisionCoreTest::onDeactivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t FingerVisionCoreTest::onExecute(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t FingerVisionCoreTest::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t FingerVisionCoreTest::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t FingerVisionCoreTest::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t FingerVisionCoreTest::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t FingerVisionCoreTest::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void FingerVisionCoreTestInit(RTC::Manager* manager)
  {
    coil::Properties profile(fingervisioncore_spec);
    manager->registerFactory(profile,
                             RTC::Create<FingerVisionCoreTest>,
                             RTC::Delete<FingerVisionCoreTest>);
  }
  
};


