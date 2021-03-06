//! \file User.cxx
//! \brief For customizing rootbeer for a specific experiment.
//! \details Here is where users can tell rootbeer how to attach to data sources,
//! and how to read and unpack data buffers from those sources.  This is done by defining
//! a class that inherits from rb::BufferSource and implements all of its pure virtual methods.
//! For more information on those methods and what they should do, see the doxygen page on 
//! rb::BufferSource.  Additionally, users must implement the static rb::BufferSource::New()
//! method; this function shold return a \c new instance of the user's freshly created class
//! \code
//! rb::BufferSource* rb::BufferSource::New() {
//!    return new rb::MyBufferSourceDerivedClass();
//! }
//! \endcode
//! This is called in a few places (all in Buffer.cxx) where an instance of the <i>a priori</i>
//! unknown user-defined class is needed.
//!
//! Pre-defined classes are included with stock rootbeer for handling MIDAS (at TRIUMF) or
//! NSCL (spectrodaq) data. If this is what you're using for data acquisition, then all you should
//! need to do is to enable the appropriate branch of the code (by uncommenting the correct lines
//! in Makefile.user) and to implement the UnpackBuffer() member function in this file. Also, note
//! that in both the NSCL and MIDAS cases, the compiler checks if the extra software needed to connect
//! to an online data source is installed on your system. If it is, then the appropriate macros are 
//! #defined and the appropriate branches of the code are compiled. Otherwise, rootbeer will be
//! compiled with attaching to online data disabled.
#include "User.hxx"
#include "utils/Error.hxx"


rb::BufferSource* rb::BufferSource::New() {
  return new rb::Midas();
}

Bool_t rb::Midas::ConnectOnline(const char* host, const char* experiment, char** unused, int unused2) {
#ifdef MIDAS_ONLINE
  TMidasOnline* onlineMidas = TMidasOnline::instance();
  Int_t err = onlineMidas->connect(host, experiment, "rootbeer");
  if (err) return kFALSE; // Message from TOnlineMidas::connect

  onlineMidas->setTransitionHandlers(RunStart, RunStop, RunPause, RunResume);
  onlineMidas->registerTransitions();
  fRequestId = onlineMidas->eventRequest("SYSTEM", -1, -1, (1<<1));
  return kTRUE;
#else
  return kFALSE;
#endif
}

Bool_t rb::Midas::ReadBufferOnline() {
#ifdef MIDAS_ONLINE
  Bool_t ret = kFALSE;
  TMidasOnline* onlineMidas = TMidasOnline::instance();
  char pEvent[100*1024];
  int size = 0;
  do { // loop until we get an error or event, or quit polling, or unattach
    size = onlineMidas->receiveEvent(fRequestId, pEvent, sizeof(pEvent), kTRUE);
  } while (size == 0 && rb::Thread::IsRunning(rb::attach::ONLINE_THREAD_NAME) && onlineMidas->poll(1000));

  if(size == 0) // Unattached or stopped polling
		 ret = kFALSE;
  else if(size > 0) { // Got data, copy into midas event  
    //! \todo: byte ordering??
    memcpy(fBuffer.GetEventHeader(), pEvent, sizeof(EventHeader_t));
    fBuffer.SetData(size, pEvent+sizeof(EventHeader_t));
    ret = kTRUE;
  }    
  else { // Error reading event
    err::Error("rb::Midas::ReadBufferOnline") << "onlineMidas->receiveEvent return val: " << size << ".";
    ret = kFALSE;
  }
  return ret;
#else
  return kFALSE;
#endif
}

Bool_t rb::Midas::UnpackBuffer() {
#ifdef MIDAS_BUFFERS
  // (DRAGON test setup)
  Short_t eventId = fBuffer.GetEventId();
  switch(eventId) {
  case DRAGON_EVENT: // event
		 {	 
			 // Figure out timestamp matching
			 // ....

			 rb::Event* gamma_event = rb::Event::Instance<GammaEvent>();
			 gamma_event->Process(&fBuffer, 0);

			 rb::Event* hi_event = rb::Event::Instance<HeavyIonEvent>();
			 hi_event->Process(&fBuffer, 0);

			 rb::Event* coinc_event = rb::Event::Instance<CoincidenceEvent>();
			 CoincEventPair_t coinc =
					std::make_pair(static_cast<GammaEvent*>(gamma_event), static_cast<HeavyIonEvent*>(hi_event));
			 coinc_event->Process(reinterpret_cast<void*>(&coinc), 0);
		 
			 break;
		 }
  case DRAGON_SCALER: // scaler
    break;
  default:
    //    Warning("UnpackBuffer", "Unrecognized Event Id: %d", eventId);
    break;
  }
  return kTRUE;
#else
  return kFALSE;
#endif
}

CoincidenceEvent::CoincidenceEvent(): fDragon("coinc", this, false, "") { }

Bool_t CoincidenceEvent::DoProcess(void* addr, Int_t nchar) {
	CoincEventPair_t* coinc = Cast(addr);
	dragon::gamma::Gamma* gamma = coinc->first->fGamma.Get();
	dragon::hion::HeavyIon* hi  = coinc->second->fHeavyIon.Get();
	fDragon->read_event(*gamma, *hi);
	return true;
}

GammaEvent::GammaEvent(): fGamma("gamma", this, true, "") { }

Bool_t GammaEvent::DoProcess(void* addr, Int_t nchar) {
  TMidasEvent* fEvent = Cast(addr);
  if(fEvent) {
		fGamma->unpack(*fEvent);
		fGamma->read_data();
    return true;
  }
  else return false;
}

HeavyIonEvent::HeavyIonEvent(): fHeavyIon("hi", this, true, "") { }

Bool_t HeavyIonEvent::DoProcess(void* addr, Int_t nchar) {
  TMidasEvent* fEvent = Cast(addr);
  if(fEvent) {
		fHeavyIon->unpack(*fEvent);
		fHeavyIon->read_data();
    return true;
  }
  else return false;
}

void rb::Rint::RegisterEvents() {
  // Register events here //
  RegisterEvent<CoincidenceEvent>(COINCIDENCE_EVENT, "CoincidenceEvent");
	RegisterEvent<GammaEvent>(GAMMA_EVENT, "GammaEvent");
	RegisterEvent<HeavyIonEvent>(HI_EVENT, "HeavyIonEvent");
}
