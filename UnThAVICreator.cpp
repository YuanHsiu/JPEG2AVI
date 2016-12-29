//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnThAVICreator.h"
#include "AviGenerator.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall Th_AVICreator::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall Th_AVICreator::Th_AVICreator(int nFrameps, int nFrameWidth, int nFrameHeight, int nLastCount,
    AnsiString strStartTime, AnsiString strDirJpeg, AnsiString strAviFilename)
    : TThread(false)
{
    m_nFrameps = nFrameps;
    m_nFrameWidth = nFrameWidth;
    m_nFrameHeight = nFrameHeight;
    m_nLastCount = nLastCount;
    m_strStartTime = strStartTime;
    m_strDirJpeg = strDirJpeg;
    m_strAviFilename = strAviFilename;

    FreeOnTerminate = True;
}
//---------------------------------------------------------------------------
void __fastcall Th_AVICreator::Execute()
{
    //---- Place thread code here ----
    SaveAviFileFromJpg();
}
//---------------------------------------------------------------------------

/* ������������JPEG�ļ�����ΪAVI����ɾ����ʱJPEG�ļ�
 * ����ʱ�䣺2007-10-11
 * ���ߣ�leexm
 */
void __fastcall Th_AVICreator::SaveAviFileFromJpg()
{
    //TODO: Add your source code here
    AviGenerator generator; // Avi���ɶ���
    AnsiString strJpegFilename = "";

    // ��JPEG�ļ���ӵ�֡�б�
    for (int i = 1; i <= m_nLastCount; ++i) {
        strJpegFilename = m_strDirJpeg + m_strStartTime + "_" + IntToHex(i, 4) + ".jpg";
        if (FileExists(strJpegFilename))
            generator.add_frame(strJpegFilename.c_str());

        if (Terminated) return;
    }

    // ����AVI�ļ�
    generator.set_avi_file(m_strAviFilename.c_str()); // Avi�ļ���
    generator.set_fps(m_nFrameps); // ֡�ʣ�15֡/�룩
    generator.set_avi_size(m_nFrameWidth, m_nFrameHeight); // ÿ֡ͼ���С
    generator.generate_avi();

    // ɾ��JPEG��ʱ�ļ�
    for (int i = 1; i <= m_nLastCount; ++i) {
        strJpegFilename = m_strDirJpeg + m_strStartTime + "_" + IntToHex(i, 4) + ".jpg";
        DeleteFile(strJpegFilename);
    }
}
