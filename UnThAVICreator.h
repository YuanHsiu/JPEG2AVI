//---------------------------------------------------------------------------

#ifndef UnThAVICreatorH
#define UnThAVICreatorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class Th_AVICreator : public TThread
{            
private:
    int m_nFrameps;
    int m_nFrameWidth;
    int m_nFrameHeight;
    int m_nLastCount;
    AnsiString m_strStartTime;
    AnsiString m_strDirJpeg;
    AnsiString m_strAviFilename;

    void __fastcall SaveAviFileFromJpg();
protected:
    void __fastcall Execute();
public:
    __fastcall Th_AVICreator(int nFrameps, int nFrameWidth, int nFrameHeight, int nLastCount,
        AnsiString strStartTime, AnsiString strDirJpeg, AnsiString strAviFilename);
};
//---------------------------------------------------------------------------
#endif
