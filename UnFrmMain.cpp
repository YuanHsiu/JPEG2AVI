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
int g_nRecordTime, g_nFrameps, g_nFrameTotal; // 一次录像时间（单位：秒），帧率（单位：帧/秒），总帧数
int rectLeft, rectTop, rectWidth, rectHeight; // 捕获屏幕区域
AnsiString g_strJpegFileDir, g_strAviFileDir; // 临时JPEG文件目录，Avi文件保存目录
AnsiString g_strStartTime; // 开始录像时间，格式：hhnnss

//---------------------------------------------------------------------------

TFrm_Main *Frm_Main;
//---------------------------------------------------------------------------
__fastcall TFrm_Main::TFrm_Main(TComponent* Owner)
    : TForm(Owner)
{
    g_bAppClosed = false;
    g_nFrameTotal = 0;
    g_strStartTime = "";

    GetInfoFromINI(); // 读取相关信息并设置

    timer_SaveJpegFile->Tag = 0;
    timer_SaveJpegFile->Interval = 1000 / g_nFrameps;
}
//---------------------------------------------------------------------------

/* 功能描述：定时连续抓屏，抓屏结束时保存为Avi文件
 * 创建时间：2007-09-24
 * 作者：leexm
 */
void __fastcall TFrm_Main::timer_SaveJpegFileTimer(TObject *Sender)
{
    // 达到指定帧数时停止抓屏
    timer_SaveJpegFile->Tag++;
    if (timer_SaveJpegFile->Tag >= g_nFrameTotal)
        timer_SaveJpegFile->Enabled = false;

    // 抓屏，保存为JPEG文件
    AnsiString strJpegFilename = g_strJpegFileDir + g_strStartTime;
    strJpegFilename += "_" + IntToHex(timer_SaveJpegFile->Tag, 4) + ".jpg";
    SaveScreenToJpeg(strJpegFilename);

    // 达到指定帧数时将JPEG文件列表保存为Avi文件
    if (timer_SaveJpegFile->Tag >= g_nFrameTotal)
    {
        TDateTime dtStop = Now();
        AnsiString strAviFilename = g_strAviFileDir + "方向XX" + "_" + g_strStartTime;
        strAviFilename += "_" + dtStop.FormatString("yyyymmddhhnnss") + ".avi";
        SaveAVIFile(timer_SaveJpegFile->Tag, g_strStartTime, g_strJpegFileDir, strAviFilename);
        timer_SaveJpegFile->Tag = 0;
        g_nFrameTotal = 0;
    }

    // 图标动画
    trayicon_Avi->IconIndex = (trayicon_Avi->IconIndex + 1) % 4;;
    //imglst_Tray->GetIcon(trayicon_Avi->IconIndex, Application->Icon);
}
//---------------------------------------------------------------------------

void __fastcall TFrm_Main::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    // 关闭窗体时判断是否退出还是隐藏窗体
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
        if (myMsg.WParam == 0) // 开始录像
        {
            if (g_nFrameTotal == 0) // 启动录像
            {
                TDateTime dtStart = Time();
                g_strStartTime = dtStart.FormatString("hhnnss");
            }
            g_nFrameTotal += g_nRecordTime * g_nFrameps;
            if (!timer_SaveJpegFile->Enabled)
                timer_SaveJpegFile->Enabled = true;
        }
        else if (myMsg.WParam == 1) // 停止录像
        {
            g_nFrameTotal = timer_SaveJpegFile->Tag;
        }
    }

    TForm::Dispatch(&myMsg);
}

/* 功能描述：抓屏幕指定区域保存为JPEG文件
 * 创建时间：2007-09-24
 * 作者：leexm
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
    //dcDest = GetWindowDC(0); // 获得窗口的设备环境，包括非客户区域
    dcDest = GetDC(0); // 获得窗口的客户区域的设备环境

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

    // 将bmp转换为JPEG
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

/* 功能描述：将JPEG文件转换为AVI
 * 创建时间：2007-09-24
 * 作者：leexm
 */
void TFrm_Main::SaveAVIFile(int nLastCount, AnsiString strStartTime, AnsiString strDirJpeg, AnsiString strAviFilename)
{
    Th_AVICreator *aviCreator;
    aviCreator = new Th_AVICreator(g_nFrameps, rectWidth, rectHeight, nLastCount,
        strStartTime, strDirJpeg, strAviFilename);
    /*
    //TODO: Add your source code here
    AviGenerator generator; // Avi生成对象
    AnsiString strJpegFilename = "";

    // 将JPEG文件添加到帧列表
    for (int i = 1; i <= nLastCount; ++i) {
        strJpegFilename = strDirJpeg + strStartTime + "_" + IntToHex(i, 4) + ".jpg";
        if (FileExists(strJpegFilename))
            generator.add_frame(strJpegFilename.c_str());
    }

    // 生成AVI文件
    generator.set_avi_file(strAviFilename.c_str()); // Avi文件名
    generator.set_fps(g_nFrameps); // 帧率（15帧/秒）
    generator.set_avi_size(rectWidth, rectHeight); // 每帧图像大小
    generator.generate_avi();
    */
    /*
    // 删除JPEG临时文件
    for (int i = 1; i <= nLastCount; ++i) {
        strJpegFilename = strDirJpeg + IntToStr(i) + ".jpg";
        DeleteFile(strJpegFilename);
    }
    */
}

/* 功能描述: 关闭windows消息
   创建时间: 2007-09-26
   作者：leexm
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
    // 开始录像
    SendMessage(this->Handle, CM_SCREENTOAVI, 0, 0);
}
//---------------------------------------------------------------------------

void __fastcall TFrm_Main::spbtn_RecordEndClick(TObject *Sender)
{
    // 停止录像
    SendMessage(this->Handle, CM_SCREENTOAVI, 1, 0);
}
//---------------------------------------------------------------------------

/* 功能描述: 从ini文件读取相关信息
   创建时间: 2007-10-11
   作者：leexm
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

    g_nRecordTime = pIniFile->ReadInteger("RecordFormat", "recordtime", 5); // 缺省一次录像5秒
    g_nFrameps = pIniFile->ReadInteger("RecordFormat", "frameps", 15); // 缺省每秒15帧

    delete pIniFile;

    if (!DirectoryExists(g_strJpegFileDir))
    {
        //CreateDir(g_strJpegFileDir);
        ForceDirectories(g_strJpegFileDir); // ForceDirectories适用于创建多级目录
    }
    if (!DirectoryExists(g_strAviFileDir))
    {
        //CreateDir(g_strAviFileDir);
        ForceDirectories(g_strAviFileDir); 
    }
}
