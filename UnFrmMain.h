//---------------------------------------------------------------------------

#ifndef UnFrmMainH
#define UnFrmMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include "trayicon.h"
#include <ImgList.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------

const CM_SCREENTOAVI = WM_USER + 0x0924;

//---------------------------------------------------------------------------
class TFrm_Main : public TForm
{
__published:	// IDE-managed Components
    TTrayIcon *trayicon_Avi;
    TImageList *imglst_Tray;
    TTimer *timer_SaveJpegFile;
    TPopupMenu *popmn_Tray;
    TMenuItem *mn_Show;
    TMenuItem *mn_Exit;
    TSpeedButton *spbtn_RecordStart;
    TSpeedButton *spbtn_RecordEnd;
    void __fastcall timer_SaveJpegFileTimer(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall mn_ShowClick(TObject *Sender);
    void __fastcall mn_ExitClick(TObject *Sender);
    void __fastcall spbtn_RecordStartClick(TObject *Sender);
    void __fastcall spbtn_RecordEndClick(TObject *Sender);
private:
    void SaveScreenToJpeg(const AnsiString strFilename);
    void SaveAVIFile(int nLastCount, AnsiString strStartTime, AnsiString strDirJpeg, AnsiString strAviFilename);
    void GetInfoFromINI();
public:		// User declarations
    __fastcall TFrm_Main(TComponent* Owner);

    void virtual __fastcall ScreenToAVI(TMessage& myMsg);
    void virtual __fastcall QueryEndSession(TMessage& myMsg);
protected:
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(CM_SCREENTOAVI, TMessage, ScreenToAVI)
        MESSAGE_HANDLER(WM_QUERYENDSESSION, TMessage, QueryEndSession) //WM_ENDSESSION
        // Add any number of additional message handlers here.
    END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TFrm_Main *Frm_Main;
//---------------------------------------------------------------------------
#endif
