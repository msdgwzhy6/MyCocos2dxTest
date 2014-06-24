/********************************************************************
 创建日期: 2004/09/02
 文件名称: DBWindow.h
  
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

#ifndef _DBWINDOW_
#define _DBWINDOW_

#include <windows.h>

//控制台输出时的文本颜色
#define WDS_T_RED    FOREGROUND_RED
#define WDS_T_GREEN  FOREGROUND_GREEN
#define WDS_T_BLUE  FOREGROUND_BLUE
//控制台输出时的文本背景颜色
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

//设置控制台输出窗口标题
BOOL DBWindowTile(ConsoleWindow *console,LPCTSTR tile);
//格式化文本输出
BOOL DBWindowWrite(ConsoleWindow *console,LPCTSTR fmt,...);
//带颜色格式化文本输出
//BOOL DBWindowWrite(ConsoleWindow *console,WORD Attrs,LPCTSTR fmt,...);

//#else

//#define DBWindowTile
//#define DBWindowWrite

//#endif

extern ConsoleWindow g_console;

#endif


