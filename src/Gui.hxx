#ifndef GUI_HXX
#define GUI_HXX


#ifndef ROOT_TGDockableFrame
#include "TGDockableFrame.h"
#endif
#ifndef ROOT_TGMenu
#include "TGMenu.h"
#endif
#ifndef ROOT_TGMdiDecorFrame
#include "TGMdiDecorFrame.h"
#endif
#ifndef ROOT_TG3DLine
#include "TG3DLine.h"
#endif
#ifndef ROOT_TGMdiFrame
#include "TGMdiFrame.h"
#endif
#ifndef ROOT_TGMdiMainFrame
#include "TGMdiMainFrame.h"
#endif
#ifndef ROOT_TGMdiMenu
#include "TGMdiMenu.h"
#endif
#ifndef ROOT_TGColorDialog
#include "TGColorDialog.h"
#endif
#ifndef ROOT_TGColorSelect
#include "TGColorSelect.h"
#endif
#ifndef ROOT_TGListBox
#include "TGListBox.h"
#endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
#endif
#ifndef ROOT_TGScrollBar
#include "TGScrollBar.h"
#endif
#ifndef ROOT_TGComboBox
#include "TGComboBox.h"
#endif
#ifndef ROOT_TGuiBldHintsEditor
#include "TGuiBldHintsEditor.h"
#endif
#ifndef ROOT_TGuiBldNameFrame
#include "TGuiBldNameFrame.h"
#endif
#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif
#ifndef ROOT_TGFileDialog
#include "TGFileDialog.h"
#endif
#ifndef ROOT_TGShutter
#include "TGShutter.h"
#endif
#ifndef ROOT_TGButtonGroup
#include "TGButtonGroup.h"
#endif
#ifndef ROOT_TGCanvas
#include "TGCanvas.h"
#endif
#ifndef ROOT_TGFSContainer
#include "TGFSContainer.h"
#endif
#ifndef ROOT_TGFontDialog
#include "TGFontDialog.h"
#endif
#ifndef ROOT_TGuiBldEditor
#include "TGuiBldEditor.h"
#endif
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
#ifndef ROOT_TGFSComboBox
#include "TGFSComboBox.h"
#endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGMsgBox
#include "TGMsgBox.h"
#endif
#ifndef ROOT_TRootGuiBuilder
#include "TRootGuiBuilder.h"
#endif
#ifndef ROOT_TGTab
#include "TGTab.h"
#endif
#ifndef ROOT_TGListView
#include "TGListView.h"
#endif
#ifndef ROOT_TGSplitter
#include "TGSplitter.h"
#endif
#ifndef ROOT_TGStatusBar
#include "TGStatusBar.h"
#endif
#ifndef ROOT_TGListTree
#include "TGListTree.h"
#endif
#ifndef ROOT_TGuiBldGeometryFrame
#include "TGuiBldGeometryFrame.h"
#endif
#ifndef ROOT_TGToolTip
#include "TGToolTip.h"
#endif
#ifndef ROOT_TGToolBar
#include "TGToolBar.h"
#endif
#ifndef ROOT_TRootEmbeddedCanvas
#include "TRootEmbeddedCanvas.h"
#endif
#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif
#include "Riostream.h"

namespace rb { class Signals; }
class TGRbeerFrame : public TGMainFrame
{
	 TGCompositeFrame *fMainFrame1596;
	 TGCompositeFrame *fMainFrame6310;
	 TGGroupFrame *fGroupCanvas; // "Canvas"
	 TGTextButton *fZeroAll; // "Zero All"
	 TGTextButton *fDivideCurrent; // "Divide Current"
	 TGTextEntry *fEntryName;
	 TGTextButton *fCreateNew; // "Create New"
	 TGLabel *fLabelName; // "Name:"
	 TGTextButton *fSelectCanvas; // "Cd"
	 TGNumberEntry *fUpdateRate;
	 TGLabel *fLabelRate; // "Rate [sec.]"
	 TGTextButton *fStartRefresh; // "Start Refresh"
	 TGTextButton *fRefreshCurrent; // "Refresh Current"
	 TGTextButton *fRefreshAll; // "Refresh All"
	 TGTextButton *fZeroCurrent; // "Zero Current"
	 TGTextButton *fClearCurrent; // "Clear Canvas"
	 TGGroupFrame *fGroupConfig; // "Configuration"
	 TGTextButton *fConfigLoad; // "Load"
	 TGTextButton *fConfigSave; // "Save"
	 TGGroupFrame *fGroupData; // "Data"
	 TGTextButton *fAttachOnline; // "Attach Online"
	 TGTextButton *fAttachFile; // "Attach File"
	 TGTextButton *fAttachList; // "Attach List"
	 TGTextButton *fUnattach; // "Unattach"
	 TGTextEntry *fEntryHost;
	 TGLabel *fLabelHost; // "Host:"
	 TGTextEntry *fEntryPort;
	 TGLabel *fLabelPort; // "Expt:"
	 TGCheckButton *fIsContinuous; // "Continuous"
	 TGCheckButton *fSaveData; // "Save Data"
	 TGCheckButton *fSaveHist; // "Save Histograms"
	 TGLabel *fLabelSource; // "[none]"
	 TGLabel *fLabelDataSource; // "Data source:"
	 TGLabel *fNbuffersLabelDivider; // " | "
	 TGLabel *fNbuffersLabel; // "Buffers Analyzed:"
	 TGLabel *fNbuffers; // "0"

public:
	 TGRbeerFrame(const TGWindow* p = 0, UInt_t w = 1, UInt_t h = 1, UInt_t options = kVerticalFrame) :
		 TGMainFrame(p, w, h, options) { }
	 ~TGRbeerFrame();
	 void GuiLayout();
	 void MakeConnections();

	 friend class rb::Signals;
};



#endif
