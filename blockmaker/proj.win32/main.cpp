#include "main.h"
#include "../Classes/AppDelegate.h"
#include "CCEGLView.h"


std::string toNarrowString(int code,const wchar_t* pStr,int len)
{    
	int nChars = ::WideCharToMultiByte( code , 0 , 
		pStr , len , NULL , 0 , NULL , NULL ) ; 
	if ( len == -1 )
		-- nChars ; 
	if ( nChars == 0 )
		return "" ;

	// convert the wide string to a narrow string
	// nb: slightly naughty to write directly into the string like this
	std::string buf ;
	buf.resize(nChars);
	::WideCharToMultiByte(code,0,pStr,len,const_cast<char*>(buf.c_str()),nChars,NULL,NULL); 

	return buf ; 
}

std::string toNarrowString(int code,const std::wstring& strW)
{
	const wchar_t* pStr = strW.c_str();
	int len = strW.length();
	return toNarrowString(code,pStr,len);
}

std::vector<std::string> SplitCmd(const std::string &cmd){
	std::vector<std::string> ret;
	char *str = new char[cmd.size()+1];
	strcpy(str,cmd.c_str());
	char *tmp = strtok(str," ");
	while(tmp){
		ret.push_back(tmp);
		tmp = strtok(NULL," ");
	};
	return ret;
}


USING_NS_CC;

extern std::string input;
extern std::string output;
extern int xCount;
extern int yCount;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	std::vector<std::string> args = SplitCmd(toNarrowString(CP_ACP,lpCmdLine));

	if(args.size() != 4) return 0;

	input = args[0];
	output = args[1];
	xCount = atol(args[2].c_str());
	yCount = atol(args[3].c_str());


    // create the application instance
    AppDelegate app;
    CCEGLView* eglView = CCEGLView::sharedOpenGLView();
    eglView->setViewName("HelloCpp");
    eglView->setFrameSize(480,320);//2048, 1536);
    // The resolution of ipad3 is very large. In general, PC's resolution is smaller than it.
    // So we need to invoke 'setFrameZoomFactor'(only valid on desktop(win32, mac, linux)) to make the window smaller.
    //eglView->setFrameZoomFactor(1.0f);
    CCApplication::sharedApplication()->run();
	return 0;
}
