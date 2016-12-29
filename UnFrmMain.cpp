//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnFrmMain.h"
//#include "AviGenerator.h"
#include "UnThAVICreator.h"
#include <jpeg.hpp>
#include <IniFiles.hpp>
#include <StrUtils.hpp>
//---------------------------------------------------------------------------

#pragma package(smart_init)
#pragma link "trayicon"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------

bool g_bAppClosed;
int g_nRecordTime, g_nFrameps, g_nFrameTotal; // һ��¼��ʱ�䣨��λ���룩��֡�ʣ���λ��֡/�룩����֡��
int rectLeft, rectTop, rectWidth, rectHeight; // ������Ļ����
AnsiString g_strJpegFileDir, g_strAviFileDir; // ��ʱJPEG�ļ�Ŀ¼��Avi�ļ�����Ŀ¼
AnsiString g_strStartTime; // ��ʼ¼��ʱ�䣬��ʽ��hhnnss

//---------------------------------------------------------------------------

TFrm_Main *Frm_Main;
//---------------------------------------------------------------------------
__fastcall TFrm_Main::TFrm_Main(TComponent* Owner)
    : TForm(Owner)
{
    g_bAppClosed = false;
    g_nFrameTotal = 0;
    g_strStartTime = "";

    GetInfoFromINI(); // ��ȡ�����Ϣ������

    timer_SaveJpegFile->Tag = 0;
    timer_SaveJpegFile->Interval = 1000 / g_nFrameps;
}
//---------------------------------------------------------------------------

/* ������������ʱ����ץ����ץ������ʱ����ΪAvi�ļ�
 * ����ʱ�䣺2007-09-24
 * ���ߣ�leexm
 */
void __fastcall TFrm_Main::timer_SaveJpegFileTimer(TObject *Sender)
{
    // �ﵽָ��֡��ʱֹͣץ��
    timer_SaveJpegFile->Tag++;
    if (timer_SaveJpegFile->Tag >= g_nFrameTotal)
        timer_SaveJpegFile->Enabled = false;

    // ץ��������ΪJPEG�ļ�
    AnsiString strJpegFilename = g_strJpegFileDir + g_strStartTime;
    strJpegFilename += "_" + IntToHex(timer_SaveJpegFile->Tag, 4) + ".jpg";
    SaveScreenToJpeg(strJpegFilename);

    // �ﵽָ��֡��ʱ��JPEG�ļ��б���ΪAvi�ļ�
    if (timer_SaveJpegFile->Tag >= g_nFrameTotal)
    {
        TDateTime dtStop = Now();
        AnsiString strAviFilename = g_strAviFileDir + "����XX" + "_" + g_strStartTime;
        strAviFilename += "_" + dtStop.FormatString("yyyymmddhhnnss") + ".avi";
        SaveAVIFile(timer_SaveJpegFile->Tag, g_strStartTime, g_strJpegFileDir, strAviFilename);
        timer_SaveJpegFile->Tag = 0;
        g_nFrameTotal = 0;
    }

    // ͼ�궯��
    trayicon_Avi->IconIndex = (trayicon_Avi->IconIndex + 1) % 4;;
    //imglst_Tray->GetIcon(trayicon_Avi->IconIndex, Application->Icon);
}
//---------------------------------------------------------------------------

void __fastcall TFrm_Main::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    // �رմ���ʱ�ж��Ƿ��˳��������ش���
    if (g_bAppClosed)
    {
        CanClose = True;
    }
    else
    {
        CanClose = False;
        this->Hide();
    }
}
//---------------------------------------------------------------------------

void __fastcall TFrm_Main::mn_ShowClick(TObject *Sender)
{
    this->Show();   
}
//---------------------------------------------------------------------------

void __fastcall TFrm_Main::mn_ExitClick(TObject *Sender)
{
    g_bAppClosed = true;
    this->Close();
}
//---------------------------------------------------------------------------

void __fastcall TFrm_Main::ScreenToAVI(TMessage& myMsg)
{
    if (myMsg.Msg == CM_SCREENTOAVI)
    {
        if (myMsg.WParam == 0) // ��ʼ¼��
        {
            if (g_nFrameTotal == 0) // ����¼��
            {
                TDateTime dtStart = Time();
                g_strStartTime = dtStart.FormatString("hhnnss");
            }
            g_nFrameTotal += g_nRecordTime * g_nFrameps;
            if (!timer_SaveJpegFile->Enabled)
                timer_SaveJpegFile->Enabled = true;
        }
        else if (myMsg.WParam == 1) // ֹͣ¼��
        {
            g_nFrameTotal = timer_SaveJpegFile->Tag;
        }
    }

    TForm::Dispatch(&myMsg);
}

/* ����������ץ��Ļָ�����򱣴�ΪJPEG�ļ�
 * ����ʱ�䣺2007-09-24
 * ���ߣ�leexm
 */
void TFrm_Main::SaveScreenToJpeg(const AnsiString strFilename)
{
    //TODO: Add your source code here
    Graphics::TBitmap *bmpSrc;
    HDC dcDest;
    TCanvas *canvTmp;
    TRect rectSrc;

    bmpSrc = new Graphics::TBitmap();
    canvTmp = new TCanvas();
    //dcDest = GetWindowDC(0); // ��ô��ڵ��豸�����������ǿͻ�����
    dcDest = GetDC(0); // ��ô��ڵĿͻ�������豸����

    try
    {
        canvTmp->Handle = dcDest;
        rectSrc = Rect(rectLeft, rectTop, rectLeft + rectWidth, rectTop + rectHeight);
        bmpSrc->Width = rectWidth;
        bmpSrc->Height = rectHeight;
        //BitBlt(bmpSrc->Canvas->Handle, 0, 0, bmpSrc->Width, bmpSrc->Height, dcDest, 0, 0, SRCCOPY);
        bmpSrc->Canvas->CopyRect(rectSrc, canvTmp, rectSrc);
    }
    catch(...)
    {
        ReleaseDC(0, dcDest);
        return;
    }

    ReleaseDC(0, dcDest);
    canvTmp->Handle = 0;
    delete canvTmp;

    // ��bmpת��ΪJPEG
    TJPEGImage *jpgDest = new TJPEGImage();
    try
    {
        jpgDest->Assign(bmpSrc);
        jpgDest->SaveToFile(strFilename);
    }
    __finally
    {
        delete jpgDest;
    }

    delete bmpSrc;
}

/* ������������JPEG�ļ�ת��ΪAVI
 * ����ʱ�䣺2007-09-24
 * ���ߣ�leexm
 */
void TFrm_Main::SaveAVIFile(int nLastCount, AnsiString strStartTime, AnsiString strDirJpeg, AnsiString strAviFilename)
{
    Th_AVICreator *aviCreator;
    aviCreator = new Th_AVICreator(g_nFrameps, rectWidth, rectHeight, nLastCount,
        strStartTime, strDirJpeg, strAviFilename);
    /*
    //TODO: Add your source code here
    AviGenerator generator; // Avi���ɶ���
    AnsiString strJpegFilename = "";

    // ��JPEG�ļ���ӵ�֡�б�
    for (int i = 1; i <= nLastCount; ++i) {
        strJpegFilename = strDirJpeg + strStartTime + "_" + IntToHex(i, 4) + ".jpg";
        if (FileExists(strJpegFilename))
            generator.add_frame(strJpegFilename.c_str());
    }

    // ����AVI�ļ�
    generator.set_avi_file(strAviFilename.c_str()); // Avi�ļ���
    generator.set_fps(g_nFrameps); // ֡�ʣ�15֡/�룩
    generator.set_avi_size(rectWidth, rectHeight); // ÿ֡ͼ���С
    generator.generate_avi();
    */
    /*
    // ɾ��JPEG��ʱ�ļ�
    for (int i = 1; i <= nLastCount; ++i) {
        strJpegFilename = strDirJpeg + IntToStr(i) + ".jpg";
        DeleteFile(strJpegFilename);
    }
    */
}

/* ��������: �ر�windows��Ϣ
   ����ʱ��: 2007-09-26
   ���ߣ�leexm
 */
void __fastcall TFrm_Main::QueryEndSession(TMessage& myMsg)
{
    if(myMsg.Msg == WM_QUERYENDSESSION)
    {
        g_bAppClosed = true;
        this->Close();
    }

    TForm::Dispatch(&myMsg);
}

void __fastcall TFrm_Main::spbtn_RecordStartClick(TObject *Sender)
{
    // ��ʼ¼��
    SendMessage(this->Handle, CM_SCREENTOAVI, 0, 0);
}
//---------------------------------------------------------------------------

void __fastcall TFrm_Main::spbtn_RecordEndClick(TObject *Sender)
{
    // ֹͣ¼��
    SendMessage(this->Handle, CM_SCREENTOAVI, 1, 0);
}
//---------------------------------------------------------------------------

/* ��������: ��ini�ļ���ȡ�����Ϣ
   ����ʱ��: 2007-10-11
   ���ߣ�leexm
 */
void TFrm_Main::GetInfoFromINI()
{
    //TODO: Add your source code here
    AnsiString strIniFilename = Application->ExeName;
    strIniFilename = AnsiReplaceStr(strIniFilename, ExtractFileExt(strIniFilename), ".ini");
    TIniFile *pIniFile = new TIniFile(strIniFilename);

    rectLeft = pIniFile->ReadInteger("FrameRect", "rectLeft", 0);
    rectTop = pIniFile->ReadInteger("FrameRect", "rectTop", 0);
    rectWidth = pIniFile->ReadInteger("FrameRect", "rectWidth", 384);
    rectHeight = pIniFile->ReadInteger("FrameRect", "rectHeight", 288);
    g_strJpegFileDir = pIniFile->ReadString("SaveDir", "tmpDir", "d:\\KB5100_AVI\\JpegTemp\\");
    g_strAviFileDir = pIniFile->ReadString("SaveDir", "aviDir", "d:\\KB5100_AVI\\AviFiles\\");

    g_nRecordTime = pIniFile->ReadInteger("RecordFormat", "recordtime", 5); // ȱʡһ��¼��5��
    g_nFrameps = pIniFile->ReadInteger("RecordFormat", "frameps", 15); // ȱʡÿ��15֡

    delete pIniFile;

    if (!DirectoryExists(g_strJpegFileDir))
    {
        //CreateDir(g_strJpegFileDir);
        ForceDirectories(g_strJpegFileDir); // ForceDirectories�����ڴ����༶Ŀ¼
    }
    if (!DirectoryExists(g_strAviFileDir))
    {
        //CreateDir(g_strAviFileDir);
        ForceDirectories(g_strAviFileDir); 
    }
}
