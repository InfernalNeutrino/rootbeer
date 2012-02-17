//! \file Hist.hxx
//! \brief Defines the histogram class used in <tt>ROOTBEER</tt>.
#ifndef __HIST__
#define __HIST__
#include <string>
#include <sstream>
#include <vector>
#include <bitset>
#include <list>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TFile.h>
#include <TTree.h>
#include <TTreeFormula.h>
#include <TROOT.h>
#include <TError.h>
#include <TObjArray.h>
#include <TMutex.h>
#include "utils/LockingPointer.hxx"


namespace rb
{
  /// Rootbeer Histogram class.

  //! This class extends the stock ROOT TH*D histograms to allow additional functionality.
  //! The main thing that is added is the ability to associate each histogram instance directly
  //! with data values (using TTreeFormula) and then automatically Fill itself with those values.
  //! The class is also (supposed to be) thread-safe, since the aforementioned filling typically
  //! happens in a background thread while the user can still interact with the histogram in CINT.
  //! Thread safety is achieved by marking any shared class members as \c volatile and then accessing
  //! them through the RAII mutex locking LockingPointer class. Mainly for this reason, rb::Hist wraps
  //! a stock ROOT histogram rather than inheriting from it, so that we can basically treat the entire
  //! wrapped histogram as a critical object shared between threads. Wrapping also allows the cration of
  //! just one rb::Hist class rather than one for each dimension, since we can simply polymorphically cast
  //! the wrapped TH1* to the right type in the constructor.
  class Hist : public TNamed
  {
  public:
    /// Typedef for a std::list of rb::Hist pointers.
    typedef std::list<rb::Hist*> List_t;

  protected:
    /// Constructor error code: true = success, false = failure.
    Bool_t kConstructorSuccess;

    /// Number of dimensions (axes).
    UInt_t kDimensions;

    /// The original title.
    //! Used for changing the title in Regate().
    std::string kInitialTitle;

    /// Default title, set from the parameters and gate condition.
    std::string kDefaultTitle;

    /// Clone of the internal histogram.
    //! This is basically the only thing that CINT users can access, via the GetHist() function.
    //! The reason for doing it this way is thread safety. By only allowing CINT users access to
    //! a copy of fHistogram (created within a mutex lock), we ensure that there will never be any
    //! conflicts between the main therad and others that can modify the internal histogram.
    TH1* fHistogramClone;

    /// Directory owning this rb::Hist instance.
    TDirectory* fDirectory;


    /// Struct wrapping all of the non-static critical elements in rb::Hist

    //! Any rb::Hist instance member that can be shared between threads is contained
    //! inside this struct, and then the whole thing is declared volatile and should
    //! be accessed via a LockingPointer (or, in cases where safe/appropriate, a LockFreePointer).
    //! The struct also provides access to \c static critical members via getter functions.
    //! \note We also create an instance, fCritical when defining the struct.
    volatile struct CriticalElements
    {
      /// Internal histogram.
      //! This is what is filled with the evaluation of fParams, and what is used for Drawing, etc.
      //! It is cast to the appropriate derived class (TH1D*, TH2D*, or TH3D*) in the constructor, depending
      //! on the number of dimensions.
      TH1* fHistogram;

      /// Formula to evaluate the histogram gate condition.
      TTreeFormula* fGate;

      /// Vector of formulas allowing evaluation of the histogram parameters.
      //! One entry per axis (0 = X, 1 = Y, 2 = Z).
      std::vector<TTreeFormula*> fParams;

      /// Returns a pointer to fgTree.
      //! \note We can safely return a LockFreePointer here since we're already inside a \c volatile
      //! member, which means that fgMutex should already be locked.
      TTree* GetTree() {
	return LockFreePointer<TTree>(fgTree).Get();
      }

      /// Returns a pointer to fgList.
      //! \note We can safely return a LockFreePointer here since we're already inside a \c volatile
      //! member, which means that fgMutex should already be locked.
      List_t* GetList() {
	return LockFreePointer<List_t>(&fgList).Get();
      }
    } fCritical;

    /// List of all existing rb::Hist objects.
    //! \note Declared \c volatile because it is a resource shared between threads.
    //! Access via LockingPointer.
    static volatile List_t fgList;

    /// TTree for calculating parameter and gate values.
    //! \note Declared \c volatile because it is a resource shared between threads.
    //! Access via LockingPointer.
    static volatile TTree fgTree;

    /// Global mutex
    //! Protects all rb::Hist objects.
    static TMutex fgMutex;

    /// Function to initialize a histogram.
    //! Basically this is just a helper function that is called by the various
    //! 1d, 2d, 3d flavors of rb::Hist::New().  It handles the following:
    //!    - Creation of the rb::Hist object itself via the protected constructor.
    //!    - Initialization of the internal fHistogram TH1* pointer to the appropriate type
    //!      (e.g. TH1D for 1d, etc)
    //!    - Adding of the rb::Hist object to both ROOT's gDirectory->fList and the static
    //!      rb::Hist::fgList list of all rb::Hist objects.
    static Bool_t Initialize(const char* name, const char* title,
			     const char* param, const char* gate, UInt_t ndim,
			     Int_t nbinsx, Double_t xlow, Double_t xhigh,
			     Int_t nbinsy, Double_t ylow, Double_t yhigh,
			     Int_t nbinsz, Double_t zlow, Double_t zhigh);

    /// Internal function to fill the histogram.
    //! Called from the public Fill() and FillAll(), does not do any mutex locking,
    //! instead relies on being passed already locked components.
    virtual Int_t DoFill(TH1* hst, TTreeFormula* gate, std::vector<TTreeFormula*>& params);

    /// Constructor
    //! Set the internal \c TTree and \c fGate fields.
    //! \note This is made \c private so that CINT users can't create explicit instances of rb::Hist
    //! objects. Instead they use rb::Hist::New and then access via the automatic pointer CINT provides. 
    Hist(const char* name, const char* title, const char* param, const char* gate, UInt_t npar);

  public:
    /// Fill the histogram from its internal parameter value(s).
    //! This function first evaluates fGate, and if true, evaluates each index of fParams, then calls
    //! fHistogram->Fill() using the results.
    virtual Int_t Fill();

    /// Returns a copy of fHistogram.
    //! This is so that users can get access to the histogram data in CINT, thus allowing higher-order analysis
    //! such as fitting, but without having to worry about thread-related issues. Since the histogram returned is
    //! a /emph copy of fHistogram, other threads can happily continue to alter fHistogram behind the scenes while
    //! users do analysis on the returned copy.
    TH1* GetHist();

    /// Draw function.
    //! \Note that this draws fHistogram directly, not a copy (to that it can still continue to be auto-updated
    //! by the rb::Canvas functions).
    void Draw(Option_t* option = "") {
      LockingPointer<CriticalElements>(fCritical, fgMutex)->fHistogram->Draw(option);
    }

    /// Clear function.
    //! Zeros-out all axes of fHistogram.
    virtual void Clear() {
      LockingPointer<CriticalElements> critical(fCritical, fgMutex);
      TH1D* hist = static_cast<TH1D*>(critical->fHistogram);
      for(Int_t p = 0; p < hist->fN; ++p) hist->fArray[p] = 0.;
    }

    /// Default constructor.
    //! Does nothing, just here to make rootcint happy.
    Hist() { } ;

    /// Destructor
    //! Free resorces allocted to TTreeFormulae, remove this from fgList.
    virtual ~Hist();

    /// Function to change the histogram gate.
    //! Updates \c fGate to reflect the new gate formula. Returns 0 if successful,
    //!  -1 if \c newgate isn't valid. In case of invalid \c newgate, the histogram
    //!  gate condition remains unchanged.
    virtual Int_t Regate(const char* newgate);

    /// Return the number of dimensions.
    UInt_t GetNdimensions() {
      return kDimensions;
    }

    /// Return the gate argument
    std::string GetGate() {
      return LockingPointer<CriticalElements>(fCritical, fgMutex)->fGate->GetExpFormula().Data();
    }

    /// Return the parameter name associated with the specified axis.
    std::string GetParam(UInt_t axis);

    /// Set line color.
    void SetLineColor(Color_t lcolor) {
      LockingPointer<CriticalElements>(fCritical, fgMutex)->fHistogram->SetLineColor(lcolor);
    }

    /// Set marker color.
    void SetMarkerColor(Color_t mcolor) {
      LockingPointer<CriticalElements>(fCritical, fgMutex)->fHistogram->SetMarkerColor(mcolor);
    }

    /// Return a reference to the global histogram mutex.
    static TMutex& GetMutex() {
      return fgMutex;
    }

    /// Function to fill all histograms
    static void FillAll();

    /// Returns a copy of fgTree.
    //! \note Since this uses TTree::Clone(), it dynamically allocetes memory that
    //! should be deleted by the user.
    static TTree* GetTreeClone() {
      return LockingPointer<TTree>(fgTree, fgMutex)->CloneTree(0);
    }

    /// Function to add branches to fgTree.
    static TBranch* AddBranch(const char* name, const char* classname, void** obj,
			      Int_t bufsize = 32000, Int_t splitlevel = 99) {
      return LockingPointer<TTree>(fgTree, fgMutex)->Branch(name, classname, obj, bufsize, splitlevel);
    }

    /// Function to access entries of \c Hist::fgList
    static rb::Hist* Find(const char* name) {
      LockingPointer<List_t> hlist(fgList, fgMutex);
      List_t::iterator it;
      for(it = hlist->begin(); it != hlist->end(); ++it) {
	if(!strcmp((*it)->GetName(), name)) return *it;
      }
      fprintf(stderr, "Error in <rb::Hist::Find>: %s was not found\n", name);
      return 0;
    }

    /// Deletes all entries of \c Hist::fgList
    static void DeleteAll();

    /// Returns the total number of entries in \c fgList
    static UInt_t GetNumber() {
      return LockingPointer<List_t>(fgList, fgMutex)->size();
    }

    /// Set an alias in \c fgTree
    static Bool_t SetAlias(const char* aliasName, const char* aliasFormula) {
      LockingPointer<TTree>(fgTree, fgMutex)->SetAlias(aliasName, aliasFormula);
    }

    /// One-dimensional creation function
    static void New(const char* name, const char* title,
		    Int_t nbinsx, Double_t xlow, Double_t xhigh,
		    const char* param, const char* gate = "");

    /// Two-dimensional creation function
    static void New(const char* name, const char* title,
		    Int_t nbinsx, Double_t xlow, Double_t xhigh,
		    Int_t nbinsy, Double_t ylow, Double_t yhigh,
		    const char* param, const char* gate = "");

    /// Three-dimensional creation function
    static void New(const char* name, const char* title,
		    Int_t nbinsx, Double_t xlow, Double_t xhigh,
		    Int_t nbinsy, Double_t ylow, Double_t yhigh,
		    Int_t nbinsz, Double_t zlow, Double_t zhigh,
		    const char* param, const char* gate = "");

    ClassDef(Hist, 0);
  };
  

  /// Summary histogram class.

  //! A "summary" histogram is a type of 2d histogram in which each bin along one of the axes ("parameter axis") corresponds
  //! to a specific parameter. The other axis ("summary axis") corresponds to the parameter values, and the z axis corresponds
  //! to counts. In other wirds, the 1d projection of each bin along the first axis would be identical to
  //! a 1d histogram for the corresponding parameter.  Orientation can either be hirozontal or vertical. Horizontal
  //! means that the parameter axis is the y axis (so the parameter values extend horizontally), vertical means it's
  //! the x axis (parameter values extend vertically).
  class SummaryHist : public Hist
  {
  protected:
    //! Histogram orientation.
    //! 0 = vertical, 1 = horizontal
    Int_t kOrient;

    //! Number of parameters summarized.
    Int_t nPar;

    //! Constructor
    SummaryHist(const char* name, const char* title, const char* params, const char* gate, const char* orient);

    //! Internal filling function.
    //! Same idea as for rb::Hist, but implemented as needed for a summary histogram.
    virtual Int_t DoFill(TH1* hst, TTreeFormula* gate, std::vector<TTreeFormula*>& params);

  public:
    //! Public creation function.
    //! \param nbins Number of bins along the summary axis
    //! \param low Low bin along the summary axis
    //! \param high High bin along the summary axis
    //! \param paramList List of all the parameters to be displayed separated by semicolons.
    //!        Arrays can also be specified as (e.g. from index 0 to index 15 inclusive) \c someArray[0-15] 
    //! \param orientation "v" = vertical "h" = horizontal, not case sensitive.
    static void New(const char* name, const char* title,
		    Int_t nbins, Double_t low, Double_t high,
		    const char* paramList,  const char* gate = "",
		    const char* orientation = "v");

    //! Return the number of parameters.
    Int_t GetNPar() {
      return nPar;
    }

    //! CINT ClassDef macro.
    ClassDef(SummaryHist, 0);
  };

  /// Gamma histogram class.

  //! A "gamma" histogram is a type of histogram displaying multiple parameters at once. It appears as a normal
  //! 1d or 2d histogram, but the number of counts per bin increments whenever <i>any</i> of the specified
  //! parameters has a value in that bin.
  class GammaHist : public Hist
  {
  protected:
    //! Number of parameters.
    Int_t nPar;

    //! Constructor
    GammaHist(const char* name, const char* title, const char* params, const char* gate, Int_t ndimensions);

    //! Initialize function
    //! Helper function called by New().
    static Bool_t GInitialize(const char* name, const char* title,
			      const char* param, const char* gate, UInt_t ndim,
			      Int_t nbinsx, Double_t xlow, Double_t xhigh,
			      Int_t nbinsy = 0, Double_t ylow = 0, Double_t yhigh = 0,
			      Int_t nbinsz = 0, Double_t zlow = 0, Double_t zhigh = 0);

    //! Internal filling function.
    //! Same idea as for rb::Hist, but implemented as needed for a gamma histogram.
    virtual Int_t DoFill(TH1* hst, TTreeFormula* gate, std::vector<TTreeFormula*>& params);

  public:
    //! Public creation function for 1d.
    //! Looks like the normal 1d creation function, but the multiple parameters are
    //! delimited with a semicolon.
    static void New(const char* name, const char* title,
		    Int_t nbinsx, Double_t xlow, Double_t xhigh,
		    const char* params,  const char* gate = "");

    //! Public creation function for 2d.
    //! Looks like the normal 2d creation function, but the multiple parameters are
    //! delimited with a semicolon.  The standard prescription applies for dividing x and y axes: Y:X.
    static void New(const char* name, const char* title,
		    Int_t nbinsx, Double_t xlow, Double_t xhigh,
		    Int_t nbinsy, Double_t ylow, Double_t yhigh,
		    const char* params,  const char* gate = "");

    //! CINT ClassDef macro.
    ClassDef(GammaHist, 0);
  };

  /// Bitmask histogram class.

  //! A bitmask histogram displays the true bits in a parameter.  For each event,
  //! the bin corresponding to a given bit in a word increments if that bit is set to 1.
  //! This is a template class, with the template argument being the number of bits to
  //! display.
  //! \note Currently, CINT dictionaries are only generated for the following numbers of 
  //! bits: 1, 2, 4, 8, 16, 32, and 64. If you need others, you'll need to add
  //! \code
  //! #pragma link C++ class rb::BitHist<N>+; // N is the number of bits you want
  //! \endcode
  //! to HistLinkdef.h

  template <Int_t NBITS>
  class BitHist : public Hist
  {
  protected:
    //! Constructor
    BitHist(const char* name, const char* title, const char* param, const char* gate);

    //! New() helper function
    static Bool_t BitInitialize(const char* name, const char* title, const char* param, const char* gate);

    //! Internal filling function
    virtual Int_t DoFill(TH1* hst, TTreeFormula* gate, std::vector<TTreeFormula*>& params); 

  public:
    //! Creation function
    static void New(const char* name, const char* title, const char* param, const char* gate);

    ClassDef(BitHist, 0);
  };
}


#ifndef __MAKECINT__ // Template implementations

template <Int_t NBITS>
rb::BitHist<NBITS>::BitHist(const char* name, const char* title, const char* param, const char* gate) :
  Hist(name, title, param, gate, 1) { }

template <Int_t NBITS>
inline void rb::BitHist<NBITS>::New(const char* name, const char* title, const char* param, const char* gate) {
  BitInitialize(name, title, param, gate);
}

template <Int_t NBITS>
Int_t rb::BitHist<NBITS>::DoFill(TH1* hst, TTreeFormula* gate, std::vector<TTreeFormula*>& params) {
  if(!gate->EvalInstance()) return 0;

  Int_t ret = 0;
  std::bitset<NBITS> bits (params[0]->EvalInstance());
  for(Int_t i=0; i< NBITS; ++i) {
    if (bits[i]) { 
      static_cast<TH1D*>(hst)->Fill(i);
      ++ret;
    }
  }
  return ret;
}
  
template <Int_t NBITS>
Bool_t rb::BitHist<NBITS>::BitInitialize(const char* name, const char* title, const char* param, const char* gate) {
  // Create rb::Hist instance
  rb::BitHist<NBITS>* _this = new rb::BitHist<NBITS>(name, title, param, gate);
  if(!_this->kConstructorSuccess) return kFALSE;

  // Set internal histogram
  //! \note The histogram isn't accessable to any other threads until we add it to
  //! fgList, so it's safe to access the critical elements via a non-locking pointer.
  LockFreePointer<CriticalElements> unlocked_critical(_this->fCritical);

  Bool_t successfulHistCreation = kTRUE;
  TH1::AddDirectory(kFALSE);
  unlocked_critical->fHistogram =
    new TH1D(_this->fName, _this->fTitle, NBITS, 0, NBITS);

  std::string xtitle = unlocked_critical->fParams.at(0)->GetExpFormula().Data();
  xtitle += " [bits]";
  unlocked_critical->fHistogram->GetXaxis()->SetTitle(xtitle.c_str());
  TH1::AddDirectory(kTRUE);

  // Add to collections
  LockingPointer<List_t> hlist(fgList, fgMutex);
  if(successfulHistCreation) {
    hlist->push_back(_this);

    if(gDirectory) {
      _this->fDirectory = gDirectory;
      _this->fDirectory->Append(_this, kTRUE);
    }
  }
  return successfulHistCreation;
}
#endif // $ifndef __MAKECINT__


#endif
  
