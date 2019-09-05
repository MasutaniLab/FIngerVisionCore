// -*-C++-*-
/*!
 * @file  FingerVisionSVC_impl.h
 * @brief Service implementation header of FingerVision.idl
 *
 */

#include "BasicDataTypeSkel.h"

#include "FingerVisionSkel.h"

#ifndef FINGERVISIONSVC_IMPL_H
#define FINGERVISIONSVC_IMPL_H
 
class FingerVisionCore;

/*!
 * @class FingerVision_CoreServiceSVC_impl
 * Example class implementing IDL interface FingerVision::CoreService
 */
class FingerVision_CoreServiceSVC_impl
 : public virtual POA_FingerVision::CoreService,
   public virtual PortableServer::RefCountServantBase
{
 private:
   // Make sure all instances are built on the heap by making the
   // destructor non-public
   //virtual ~FingerVision_CoreServiceSVC_impl();

   FingerVisionCore* m_pRTC;

public:
  /*!
   * @brief standard constructor
   */
   FingerVision_CoreServiceSVC_impl();
  /*!
   * @brief destructor
   */
   virtual ~FingerVision_CoreServiceSVC_impl();

   // attributes and operations
   void pause();
   void resume();
   void set_frame_skip(::CORBA::Long num);
   void stop_detect_obj();
   void start_detect_obj();
   void clear_obj();

   void setRTC(FingerVisionCore* pRTC) {
     m_pRTC = pRTC;
   }
};



#endif // FINGERVISIONSVC_IMPL_H


