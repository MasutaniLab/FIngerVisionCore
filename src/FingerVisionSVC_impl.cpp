// -*-C++-*-
/*!
 * @file  FingerVisionSVC_impl.cpp
 * @brief Service implementation code of FingerVision.idl
 *
 */

#include "FingerVisionSVC_impl.h"

#include "FingerVisionCore.h"


/*
 * Example implementational code for IDL interface FingerVision::CoreService
 */
FingerVision_CoreServiceSVC_impl::FingerVision_CoreServiceSVC_impl()
{
  // Please add extra constructor code here.
}


FingerVision_CoreServiceSVC_impl::~FingerVision_CoreServiceSVC_impl()
{
  // Please add extra destructor code here.
}


/*
 * Methods corresponding to IDL attributes and operations
 */
void FingerVision_CoreServiceSVC_impl::pause()
{
  m_pRTC->Pause();
}

void FingerVision_CoreServiceSVC_impl::resume()
{
  m_pRTC->Resume();
}

void FingerVision_CoreServiceSVC_impl::set_frame_skip(::CORBA::Long num)
{
  m_pRTC->SetFrameSkip(num);
}

void FingerVision_CoreServiceSVC_impl::stop_detect_obj()
{
  m_pRTC->StopDetectObj();
}

void FingerVision_CoreServiceSVC_impl::start_detect_obj()
{
  m_pRTC->StartDetectObj();
}

void FingerVision_CoreServiceSVC_impl::clear_obj()
{
  m_pRTC->ClearObj();
}



// End of example implementational code



