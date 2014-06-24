/********************************************************************
 ��������: 2004/09/02
 �ļ�����: DBWindow.h
  
      ����: ����(vietor)
      �汾: 2.0
      ����: liuleilover@163.com

  ����Ŀ��:
      ��VC MFC/DLL���ʱͨ��Debug����Ҫ��������Ϣͨ������̨���,��
      �����Release��ʱ��Ҫ����Щ���������Ϣ�Ĵ����ע�͵���ͨ
      ����Щ����϶�����Ƚ��鷳��
      �����������þ���Ϊ�˳���Ա�ڽ���MFC/DLL�������ʱDebug�����
      ��һ�������Ŀ���̨���ڽ��е��Գ���������������Release��ʱ
      �����ֹ�����Щ����ע�͵����ɴ˳����Զ���ɡ�
      ע�⣺��һ������֮��ֻ����һ������̨���ڣ����ڶ������ͬʱ���Ե�DLL
      ������ɫʶ��
  ��Ȩ����:
      ���������⿽����ʹ���������ĸ��������뱣֤�����ļ���������
      �����޸ģ���������޸����������������ϵ��
 
*********************************************************************/

#ifndef _DBWINDOW_
#define _DBWINDOW_

#include <windows.h>

//����̨���ʱ���ı���ɫ
#define WDS_T_RED    FOREGROUND_RED
#define WDS_T_GREEN  FOREGROUND_GREEN
#define WDS_T_BLUE  FOREGROUND_BLUE
//����̨���ʱ���ı�������ɫ
#define WDS_BG_RED  BACKGROUND_RED
#define WDS_BG_GREEN BACKGROUND_GREEN
#define WDS_BG_BLUE  BACKGROUND_BLUE

class ConsoleWindow  
{
public:
  ConsoleWindow();
  virtual ~ConsoleWindow();

  BOOL SetTile(LPCTSTR lpTile);
  BOOL WriteString(LPCTSTR lpString);
  BOOL WriteString(WORD Attrs,LPCTSTR lpString);
private:
  HANDLE m_hConsole;
  BOOL   m_bCreate;
  BOOL   m_bAttrs;
  WORD   m_OldColorAttrs; 
};
//#ifdef _DEBUG

//���ÿ���̨������ڱ���
BOOL DBWindowTile(ConsoleWindow *console,LPCTSTR tile);
//��ʽ���ı����
BOOL DBWindowWrite(ConsoleWindow *console,LPCTSTR fmt,...);
//����ɫ��ʽ���ı����
//BOOL DBWindowWrite(ConsoleWindow *console,WORD Attrs,LPCTSTR fmt,...);

//#else

//#define DBWindowTile
//#define DBWindowWrite

//#endif

extern ConsoleWindow g_console;

#endif


