/********************************************************************
 创建日期: 2004/09/02
 文件名称: DBWindow.cpp
  
      作者: 刘磊(vietor)
      版本: 2.0
      邮箱: liuleilover@163.com

  创建目的:
      用VC MFC/DLL编程时通常Debug版需要将测试信息通过控制台输出,而
      编译成Release版时需要将这些输出调试信息的代码给注释掉，通
      常这些代码较多操作比较麻烦。
      这个程序的作用就是为了程序员在进行MFC/DLL软件开发时Debug版可以
      在一个单独的控制台窗口进行调试程序的输出，而做成Release版时
      不必手工将这些代码注释掉，由此程序自动完成。
      注意：在一个进程之内只存在一个控制台窗口，对于多个可能同时调试的DLL

请用颜色识别。
  版权声明:
      您可以随意拷贝和使用这个程序的副本，但请保证所有文件的完整和
      不被修改，如果您有修改意见，请与作者联系。
 
*********************************************************************/


//#include "stdafx.h"
#include "DBWindow.h"
ConsoleWindow g_console;

//#ifdef _DEBUG

#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>

#define CONSOLE_TILE _T("聊天信息查看")



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
