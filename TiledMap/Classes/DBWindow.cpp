/********************************************************************
 ��������: 2004/09/02
 �ļ�����: DBWindow.cpp
  
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


//#include "stdafx.h"
#include "DBWindow.h"
ConsoleWindow g_console;

//#ifdef _DEBUG

#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>

#define CONSOLE_TILE _T("������Ϣ�鿴")



ConsoleWindow::ConsoleWindow()
{
 m_hConsole=NULL;
 m_bCreate=FALSE;
 if(AllocConsole())
 {
  m_hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTitle(CONSOLE_TILE);
  SetConsoleMode(m_hConsole,ENABLE_PROCESSED_OUTPUT);
  m_bCreate=TRUE;
 }
 else{
  m_hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
  if(m_hConsole==INVALID_HANDLE_VALUE)
   m_hConsole=NULL;
 }
 if(m_hConsole)
 {
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 
  if(GetConsoleScreenBufferInfo(m_hConsole, &csbiInfo))
  {
   m_bAttrs=TRUE;
   m_OldColorAttrs = csbiInfo.wAttributes;      
  }
  else{
   m_bAttrs=FALSE;
   m_OldColorAttrs = 0;
  }
 }
}

ConsoleWindow::~ConsoleWindow()
{
 if(m_bCreate)
  FreeConsole();
}

BOOL ConsoleWindow::SetTile(LPCTSTR lpTile)
{
  return SetConsoleTitle(lpTile);
}

BOOL ConsoleWindow::WriteString(LPCTSTR lpString)
{
 BOOL ret=FALSE;
 if(m_hConsole)
 {
  ret=WriteConsole(m_hConsole,lpString,_tcslen(lpString),NULL,NULL);
 }
 return ret;
}

BOOL ConsoleWindow::WriteString(WORD Attrs,LPCTSTR lpString)
{
  BOOL ret=FALSE;
  if(m_hConsole)
  {
    if(m_bAttrs)SetConsoleTextAttribute(m_hConsole,Attrs);
    ret=WriteConsole(m_hConsole,lpString,_tcslen(lpString),NULL,NULL);
    if(m_bAttrs)SetConsoleTextAttribute(m_hConsole,m_OldColorAttrs);
  }
  return ret; 
}

//ConsoleWindow  ConWindow;

#define MAX_BUF_LEN 4096

BOOL DBWindowTile(ConsoleWindow *console,LPCTSTR tile)
{
  return console->SetTile(tile);
}

BOOL DBWindowWrite(ConsoleWindow *console,LPCTSTR fmt,...)
{
  TCHAR   message[MAX_BUF_LEN];
  va_list cur_arg;
  va_start(cur_arg,fmt);
  _vsntprintf(message,MAX_BUF_LEN,fmt,cur_arg);
  va_end(cur_arg);
  return console->WriteString(message);
}
/*
BOOL DBWindowWrite(ConsoleWindow *console,WORD Attrs,LPCTSTR fmt,...)
{
  TCHAR   message[MAX_BUF_LEN];
  va_list cur_arg;
  va_start(cur_arg,fmt);

  _vsntprintf(message,MAX_BUF_LEN,fmt,cur_arg);
  va_end(cur_arg);
  return console->WriteString(Attrs,message); 
}
*/
//#endif
