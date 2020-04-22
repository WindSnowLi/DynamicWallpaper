
// DynamicWallpaperDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "DynamicWallpaper.h"
#include "DynamicWallpaperDlg.h"
#include "afxdialogex.h"
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define WM_SHOWTASK (WM_USER +1)

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

HWND workerw;
//水波点击力度
int overallClickStrength = 800;
//水波滑动力度
int overallSlidingStrength = 240;
//水波点击时间间隔
int overallClickFrequency = 100;
//水波滑动时间间隔
int overallSlidingFrequency = 200;
//获得鼠标位置时间间隔
int getCursorTimerInterval = 100;
//循环播放线程状态
bool cycleStatus = false;
//自动播放下一个
bool autoNextPlaystatus = false;
//当前播放的行数
int lineNumber;
//壁纸大小
long wallpaperSize;
//鼠标位置信息线程状态		false为不执行
bool cursorThreadStatus = false;
//水波纹点击线程状态
bool clikThreadStatus = false;
//滑动水波线程状态
bool slidingThreadStatus = false;
//检查系统壁纸线程状态
bool checkSystemThreadStatus = false;
//视频子线程需要的数据源
struct videoData {
	VideoPlayer* vp;
	CRipple* cr;
	CImage* ci;
};
videoData* videodata = new videoData();

char WindowTitle[100];				//鼠标下的窗口标题
CPoint mousePosition;				//鼠标位置

//不同层深部分注解
//https://blog.csdn.net/mkdym/article/details/7018318

BOOL CALLBACK EnumwindowProcFindDesktopwindow(HWND hwnd, LPARAM lparam) {
	HWND p = ::FindWindowEx(hwnd, NULL, L"SHELLDLL_DefView", NULL);
	if (p == NULL)return 1;
	workerw = ::FindWindowEx(NULL, hwnd, L"Workerw", NULL);
	return 0;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDynamicWallpaperDlg 对话框



CDynamicWallpaperDlg::CDynamicWallpaperDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DynamicWallpaper_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDynamicWallpaperDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILEPath, filepath);
	DDX_Control(pDX, IDC_volume, m_slider);
	DDX_Control(pDX, IDC_transparent, transparent);
	DDX_Control(pDX, IDC_autoStartStatus, autoStartStatus);
	DDX_Control(pDX, IDC_SlidingFrequency, slidingFrequency);
	DDX_Control(pDX, IDC_ClickFrequency, clickFrequency);
	DDX_Control(pDX, IDC_SlidingStrength, slidingStrength);
	DDX_Control(pDX, IDC_ClickStrength, clickStrength);
}

BEGIN_MESSAGE_MAP(CDynamicWallpaperDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SelectFile, &CDynamicWallpaperDlg::OnBnClickedSelectfile)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_MESSAGE(WM_SHOWTASK, OnShowTask)
	ON_COMMAND(ID_EXIT_RMENU, &CDynamicWallpaperDlg::OnExitRmenu)
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_transparent, &CDynamicWallpaperDlg::OnCbnSelchangetransparent)
	ON_BN_CLICKED(IDC_autoStartStatus, &CDynamicWallpaperDlg::OnBnClickedautostartstatus)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_loopPlayer, &CDynamicWallpaperDlg::OnBnClickedloopplayer)
	ON_BN_CLICKED(IDC_Waves, &CDynamicWallpaperDlg::OnBnClickedWaves)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_StopVideo, &CDynamicWallpaperDlg::OnBnClickedStopvideo)
	ON_BN_CLICKED(IDC_MysqlService, &CDynamicWallpaperDlg::OnBnClickedMysqlservice)
	ON_BN_CLICKED(IDC_GitblitService, &CDynamicWallpaperDlg::OnBnClickedGitblitservice)
	ON_BN_CLICKED(IDC_CheckService, &CDynamicWallpaperDlg::OnBnClickedCheckservice)
END_MESSAGE_MAP()


// CDynamicWallpaperDlg 消息处理程序

BOOL CDynamicWallpaperDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	this->SetWindowTextW(_T("动态壁纸"));
	m_slider.SetRange(0, 100);//设置滑动范围为1到20
	m_slider.SetTicFreq(1);//每1个单位画一刻度
	m_slider.SetPos(30);//设置滑块初始位置为1 


	//2000/100=20       800/20=40
	clickStrength.SetRange(0, 100);//设置滑动范围为1到20
	clickStrength.SetTicFreq(1);//每1个单位画一刻度
	clickStrength.SetPos(50);//设置滑块初始位置为1 

	//2000/100=20		240/20=12
	slidingStrength.SetRange(1, 100);//设置滑动范围为1到20
	slidingStrength.SetTicFreq(1);//每1个单位画一刻度
	slidingStrength.SetPos(12);//设置滑块初始位置为1 

	slidingFrequency.SetRange(1, 100);//设置滑动范围为1到100
	slidingFrequency.SetTicFreq(1);//每1个单位画一刻度1
	slidingFrequency.SetPos(5);//设置滑块初始位置为5

	clickFrequency.SetRange(1, 100);//设置滑动范围为1到100
	clickFrequency.SetTicFreq(1);//每1个单位画一刻度1
	clickFrequency.SetPos(10);//设置滑块初始位置为10

	CString a;
	a.Format(_T("%d"), 30);
	GetDlgItem(IDC_showVolume)->SetWindowTextW(a);
	a.Format(_T("%d"), 12);
	GetDlgItem(IDC_showSlidingStrength)->SetWindowTextW(a);
	a.Format(_T("%d"), 40);
	GetDlgItem(IDC_showClickStrength)->SetWindowTextW(a);
	a.Format(_T("%d"), 5);
	GetDlgItem(IDC_showSlidingFrequency)->SetWindowTextW(a);
	a.Format(_T("%d"), 10);
	GetDlgItem(IDC_showClickFrequency)->SetWindowTextW(a);

	//生成右下角托盘图标
	SetTimer(2, 10, NULL);

	HWND hwnd_progman = ::FindWindow(L"Progman", NULL);
	if (hwnd_progman == NULL) {
		MessageBox(_T("初始化错误"));
	}
	DWORD_PTR result = 0;
	SendMessageTimeout(hwnd_progman, 0x052c, NULL, NULL, SMTO_NORMAL, 1000, &result);

	EnumWindows(EnumwindowProcFindDesktopwindow, NULL);
	videoPlayer->workerw = workerw;
	g_Ripple->workerw = workerw;

	int index = transparent.FindStringExact(0, _T("0.4"));
	//0表示从索引为0的选项开始查找.如果找到有叫three的选项就返回它的索引
	transparent.SetCurSel(index);

	setTransparent(255 * 0.4);


	//初始化子线程需要的数据
	videodata->vp = videoPlayer;
	videodata->cr = g_Ripple;
	videodata->ci = &DynamicBackground;

	if (IsAutoBoot()) {
		((CButton*)GetDlgItem(IDC_autoStartStatus))->SetCheck(1);
		AutoBootSet();
	}

	GetModuleFileName(0, szfilePath, MAX_PATH); //文件路径

	CString startingmethod = GetCommandLine();//启动方式

	CString szFilePathTemp = szfilePath;  //文件路径
	szFilePathTemp = _T("\"") + szFilePathTemp + _T("\"");

	startingmethod.Replace(_T(" "), _T(""));
	szFilePathTemp.Replace(_T(" "), _T(""));

	if (startingmethod.Compare(szFilePathTemp))
	{
		SetTimer(1, 10, NULL); //若开机自动播放成功，则自动隐藏窗口
	}

	PathRemoveFileSpec(szfilePath);//得到应用程序路径
	//获取最近一次播放的视频的地址
	CString tempCSPath = szfilePath;
	tempCSPath += _T("\\config\\ARecentVideo.xml");
	ifstream file(tempCSPath);
	boost::archive::xml_iarchive ia(file);
	string recentVideo;
	ia >> BOOST_SERIALIZATION_NVP(recentVideo);  //不需要指定范围/大小

	PathAppend(szfilePath, _T("AutoStartPath.LYJ"));//添加文件名构造出绝对路径
	char* tempSzfilePath = CString_char(szfilePath);
	//判断最近的一次播放的视频是否存在，若不存在，就寻找播放目录存在的视频，若还不存在，就不播放
	string fileBuff;
	if (judgeVedioFile((char*)recentVideo.c_str()))
	{
		fileBuff = (char*)recentVideo.c_str();
		
	}
	else if (judgeVedioFile(tempSzfilePath)) {
		fileBuff = tempSzfilePath;
	}
	if (fileBuff != "" || judgeVedioFile((char*)fileBuff.c_str())) {
		char* waitDelete = EncodeToUTF8((char*)fileBuff.c_str());
		videoPlayer->loadPlayer(waitDelete);
		GetDlgItem(IDC_FILEPath)->SetWindowTextW(char_CString((char*)fileBuff.c_str()));
		setLoop();
	}
	else
	{
		//若启动播放视频失败，则显示窗口
		this->ShowWindow(SW_SHOW);
		::SendMessage(AfxGetMainWnd()->m_hWnd, WM_SYSCOMMAND, SC_RESTORE, NULL);
	}
	delete tempSzfilePath;
	
	//检查服务,本宝宝也很无奈，技术所限，老感觉有问题
	CDynamicWallpaperDlg::OnBnClickedCheckservice();

	//循环播放设为选中	
	((CButton*)GetDlgItem(IDC_loopPlayer))->SetCheck(1);

	//获取系统壁纸大小
	wallpaperSize = wallpaperFileByte();
	//加载系统壁纸
	DynamicBackground.Load(char_CString((char*)pathConvert((char*)buffWallpaperFilePath.c_str()).c_str()));

	buffImg = &DynamicBackground;
	//转化系统壁纸格式并传给水波纹对象
	HBITMAP hbmp = (HBITMAP)buffImg->operator HBITMAP();
	g_Ripple->InitRipple(GetSafeHwnd(), hbmp, 20);

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}



bool CDynamicWallpaperDlg::judgeVedioFile(char* temp) {
	ifstream file(temp);
	if (file)
	{
		file.close();
		return true;
	}
	file.close();
	return false;
}
string CDynamicWallpaperDlg::judgeFile(char* temp) {

	ifstream file(temp);
	string fileBuff;
	int i = 0;
	if (file)
	{
		string line;
		while (getline(file, line)) // line中不包括每行的换行符  
		{
			i++;
			if (i <= lineNumber) {
				continue;
			}
			if (judgeVedioFile((char*)line.c_str())) {
				lineNumber++;
				return line;
				break;
			}
		}
		lineNumber = 0;

	}
	file.close();
	return " ";
}


void CDynamicWallpaperDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
	if (nID == SC_MINIMIZE) {
		toTray(); //最小化到托盘的函数
		this->ShowWindow(SW_HIDE);
	}
}
// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDynamicWallpaperDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDynamicWallpaperDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

char* CDynamicWallpaperDlg::EncodeToUTF8(const char* mbcsStr)
{
	wchar_t* wideStr;
	char* utf8Str;
	int charLen;

	charLen = MultiByteToWideChar(936, 0, mbcsStr, -1, NULL, 0);////////936 ----- gb2312
	wideStr = (wchar_t*)malloc(sizeof(wchar_t) * charLen);
	MultiByteToWideChar(936, 0, mbcsStr, -1, wideStr, charLen);

	charLen = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);

	utf8Str = (char*)malloc(charLen);

	WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, utf8Str, charLen, NULL, NULL);

	free(wideStr);
	return utf8Str;

}

string CDynamicWallpaperDlg::pathConvert(char* ch) {

	string s;
	string temp = ch;
	for (int i = 0; i < temp.length(); i++) {
		if (temp[i] == '\\') {
			s = s + "\\\\";
		}
		else {
			s = s + temp[i];
		}
	}
	return s;
}

CString CDynamicWallpaperDlg::char_CString(char* ch)
{
	CString temp;

	int charLen = strlen(ch);
	//计算多字节字符的大小，按字符计算。
	int len = MultiByteToWideChar(CP_ACP, 0, ch, charLen, NULL, 0);
	//为宽字节字符数组申请空间，数组大小为按字节计算的多字节字符大小
	TCHAR* buf = new TCHAR[(unsigned __int64)len + 1];
	//多字节编码转换成宽字节编码
	MultiByteToWideChar(CP_ACP, 0, ch, charLen, buf, len);
	buf[len] = '\0'; //添加字符串结尾，注意不是len+1
	//将TCHAR数组转换为CString
	temp.Append(buf);
	//删除缓冲区
	delete[]buf;

	return temp;
}


void CDynamicWallpaperDlg::OnBnClickedSelectfile()
{
	char* videopath;
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY,
		_T("视频 (*.asf,*.avi,*.divx,*.dv,*.flv,"
			"*.gxf,*.m1v,*.m2v,*.m2ts,*.m4v,*.mkv,*.mov,"
			"*.mp2,*.mp4,*.mpeg,*.mpeg1,*.mpeg2,*.mpeg4,"
			"*.mpg,*.mts,*.mxf,*.ogg,*.ogm,*.ps,*.ts,*.vob,"
			"*.wmv,*.a52,*.aac,*.ac3,*.dts,*.flac,*.m4a,"
			"*.m4p,*.mka,*.mod,*.mp1,*.mp2,*.mp3,*.ogg)|"
			"(*.asf;*.avi;*.divx;*.dv;*.flv;*.gxf;*.m1v;"
			"*.m2v;*.m2ts;*.m4v;*.mkv;*.mov;*.mp2;*.mp4;"
			"*.mpeg;*.mpeg1;*.mpeg2;*.mpeg4;*.mpg;*.mts;"
			"*.mxf;*.ogg;*.ogm;*.ps;*.ts;*.vob;*.wmv;*.a52;"
			"*.aac;*.ac3;*.dts;*.flac;*.m4a;*.m4p;*.mka;"
			"*.mod;*.mp1;*.mp2;*.mp3;*.ogg)|"
			"所有 (*.*)|*.*||"), this);
	USES_CONVERSION;
	if (dlg.DoModal() == IDOK)
	{
		cancelLoop();
		filePath = dlg.GetPathName();
		videopath = T2A(filePath);
		char* waitDelete = EncodeToUTF8(pathConvert(videopath).c_str());

		videoPlayer->loadPlayer(waitDelete);
		GetDlgItem(IDC_FILEPath)->SetWindowTextW(filePath);

		TCHAR startfilePath[MAX_PATH + 1];
		GetModuleFileName(0, startfilePath, MAX_PATH);
		PathRemoveFileSpec(startfilePath);//得到应用程序路径
		//将最近设置的视频储存起来
		CString tempCSPath = startfilePath;
		tempCSPath += _T("\\config\\ARecentVideo.xml");
		std::ofstream file(tempCSPath);
		boost::archive::xml_oarchive oa(file);

		string toBoost= videopath;
		oa& BOOST_SERIALIZATION_NVP(toBoost);

		PathAppend(startfilePath, _T("AutoStartPath.LYJ"));//添加文件名构造出绝对路径

		ofstream outfile;
		outfile.open(startfilePath, ios::out | ios::app);
		string write = videopath;
		write = write + "\n";;
		outfile << write;
		outfile.close();
		if (((CButton*)GetDlgItem(IDC_loopPlayer))->GetCheck() == 1) {
			setLoop();
		}
		free(waitDelete);
	}
}




void CDynamicWallpaperDlg::OnTimer(UINT_PTR nIDEvent)
{
	float temp;
	switch (nIDEvent) {
	case 1:
		this->ShowWindow(SW_HIDE); //隐藏窗口
		KillTimer(1);
		break;
	case 2:
		toTray();
		KillTimer(2);
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}




void CDynamicWallpaperDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSlidCtrl = (CSliderCtrl*)pScrollBar;
	int pos = pSlidCtrl->GetPos();
	CString b;
	b.Format(_T("%d"), pos);
	switch (pSlidCtrl->GetDlgCtrlID()) {
	case IDC_volume:
		videoPlayer->setVolume(pos);
		GetDlgItem(IDC_showVolume)->SetWindowTextW(b);
		break;
	case IDC_SlidingStrength:
		overallSlidingStrength = pos * 20;
		GetDlgItem(IDC_showSliding)->SetWindowTextW(b);
		break;
	case IDC_ClickStrength:
		overallClickStrength = pos * 20;
		GetDlgItem(IDC_showClickStrength)->SetWindowTextW(b);
		break;
	case IDC_SlidingFrequency:
		overallSlidingFrequency = 1000 / pos;
		getCursorTimerInterval = overallSlidingFrequency > overallClickFrequency ? overallClickFrequency : overallSlidingFrequency;
		GetDlgItem(IDC_showSlidingFrequency)->SetWindowTextW(b);
		break;
	case IDC_ClickFrequency:
		overallClickFrequency = 1000 / pos;
		getCursorTimerInterval = overallSlidingFrequency > overallClickFrequency ? overallClickFrequency : overallSlidingFrequency;
		GetDlgItem(IDC_showClickFrequency)->SetWindowTextW(b);
		break;
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CDynamicWallpaperDlg::toTray()
{
	// TODO: 在此处添加实现代码.
	NOTIFYICONDATA nid;
	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd = this->m_hWnd;
	nid.uID = IDR_MAINFRAME;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_SHOWTASK;//自定义的消息名称
	nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

	wcscpy_s(nid.szTip, L"动态桌面"); //信息提示条

	nid.cbSize = sizeof(NOTIFYICONDATA);
	Shell_NotifyIcon(NIM_ADD, &nid); //在托盘区添加图标
	//this->ShowWindow(SW_HIDE); //隐藏主窗口
}
LRESULT CDynamicWallpaperDlg::OnShowTask(WPARAM wParam, LPARAM lParam)
{
	switch (lParam) {
	case WM_RBUTTONUP:
	{
		LPPOINT lpoint = new tagPOINT;
		::GetCursorPos(lpoint);//得到鼠标位置
		CMenu menu;
		menu.LoadMenuW(IDR_MENU1);
		CMenu* pPopup = menu.GetSubMenu(0);
		SetForegroundWindow();
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, lpoint->x, lpoint->y, this);
		delete lpoint;
	} break;
	case WM_LBUTTONDOWN: //左键的处理
	{
		if (IsWindowVisible()) {
			this->ShowWindow(SW_HIDE);
		}
		else {
			this->ShowWindow(SW_SHOW);
			::SendMessage(AfxGetMainWnd()->m_hWnd, WM_SYSCOMMAND, SC_RESTORE, NULL);
		}
		//DeleteTray();
	} break;
	default: break;
	}
	return 0;
}
void CDynamicWallpaperDlg::DeleteTray()
{
	NOTIFYICONDATA nid;
	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd = this->m_hWnd;
	nid.uID = IDR_MAINFRAME;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_SHOWTASK; //自定义的消息名称
	nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	wcscpy_s(nid.szTip, L"动态桌面"); //信息提示条为“计划任务提醒”
	nid.cbSize = sizeof(NOTIFYICONDATA);
	Shell_NotifyIcon(NIM_DELETE, &nid); //在托盘区删除图标
}

void CDynamicWallpaperDlg::OnExitRmenu()
{
	// TODO: 在此添加命令处理程序代码
	DeleteTray();
	::SendMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE, 0, NULL);
}


void CDynamicWallpaperDlg::OnClose()
{
	HDC hDC = ::GetWindowDC(workerw);
	::SetStretchBltMode(hDC, COLORONCOLOR);

	CRect rect;
	::GetWindowRect(workerw, &rect);
	DynamicBackground.Draw(hDC, rect);
	restoresWallpaper();
	CDialogEx::OnClose();
}


void CDynamicWallpaperDlg::OnCbnSelchangetransparent()
{
	CString csValue;
	int in = transparent.GetCurSel();
	transparent.GetLBText(in, csValue);
	float n = _ttof(csValue);
	setTransparent(n);
	setTransparent(n);
}

void CDynamicWallpaperDlg::setTransparent(float transparent) {

	SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) ^ 0x80000); HINSTANCE hInst = LoadLibrary(L"User32.DLL");
	if (hInst)
	{
		typedef BOOL(WINAPI* MYFUNC)(HWND, COLORREF, BYTE, DWORD);
		MYFUNC fun = NULL;
		//取得SetLayeredWindowAttributes函数指针 
		fun = (MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		if (fun)fun(this->GetSafeHwnd(), 0, (1 - transparent + 0.05) * 255, 2);
		FreeLibrary(hInst);
	}

}

//判断程序是否开机自动启动
BOOL CDynamicWallpaperDlg::IsAutoBoot()
{
	HKEY key;
	LPBYTE path_Get = new BYTE[254];
	DWORD type = REG_SZ;
	DWORD dwBytes = 254;
	bool temp = false;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_READ, &key) == ERROR_SUCCESS)
	{
		// 查询注册表 值
		if (RegQueryValueEx(key, _T("MFCVideo"), 0, &type, path_Get, &dwBytes) == ERROR_SUCCESS) {
			temp = true;
		}
		else {
			temp = false;
		}
	}
	RegCloseKey(key);
	delete[]path_Get;
	return temp;
}



char* CDynamicWallpaperDlg::CString_char(CString str)
{

	//注意：以下n和len的值大小不同，n是按字符计算的，len是按字节计算的
	int n = str.GetLength();

	//获取宽字节字符的大小，大小是按字节计算的
	int len = WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), NULL, 0, NULL, NULL);

	//为多字节字符数组申请空间，数组大小为按字节计算的宽字节字节大小
	char* pFileName = new char[(unsigned __int64)len + 1];   //以字节为单位

	//宽字节编码转换成多字节编码          
	WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), pFileName, len, NULL, NULL);
	pFileName[len] = '\0';   //多字节字符以'/0'结束

	return pFileName;
}

TCHAR* CDynamicWallpaperDlg::char2TCAHR(char* str)
{
	int size = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	TCHAR* retStr = new TCHAR[size * sizeof(TCHAR)];
	MultiByteToWideChar(CP_ACP, 0, str, -1, retStr, size);
	return retStr;
}

BOOL CDynamicWallpaperDlg::AutoBootSet()
{
	HKEY hKey;

	//找到系统的启动项   
	CString lpRun = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");

	//打开启动项Key   
	long lRet = RegOpenKeyEx(HKEY_CURRENT_USER, lpRun, 0, KEY_ALL_ACCESS, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		TCHAR pFileName[MAX_PATH] = { 0 };

		//得到程序自身的全路径   
		DWORD dwRet = GetModuleFileName(NULL, pFileName, MAX_PATH);
		TRACE(pFileName);
		CString te = pFileName;
		char* temp1 = CString_char(te);
		string temp = temp1;
		string temp2 = "\"" + temp + "\"   /auto";
		char* temp3 = (char*)temp2.c_str();

		CString ss = char_CString((char*)temp2.c_str());

		CString ssTag = _T(" ") + ss;
		BYTE* bp = (BYTE*)ssTag.GetBuffer(ssTag.GetLength());
		BYTE expected[16];
		CopyMemory(expected, bp, sizeof(expected));


		lRet = RegSetValueEx(hKey, _T("MFCVideo"), 0, REG_SZ, bp, (lstrlen(ss) + 1) * sizeof(TCHAR));
		RegCloseKey(hKey);
		/*

		电脑注册表Run路径有时无法写入标准的路径，所以在这用的在Run“标记”的方式，实际自启信息在RunOnce，
		每次启动检查“标记”，如果存在，再次写入RunOnce

		*/

		CRegKey key1;
		key1.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"));
		key1.SetStringValue(_T("MFCVideo"), ss);
		key1.Close();
		delete temp1;
		if (lRet != ERROR_SUCCESS)
		{
			return false;
		}
		return true;
	}
	return false;
}


//取消程序开机启动
BOOL CDynamicWallpaperDlg::AutoBootCancel()
{
	HKEY hKey;
	CString lpRun = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	long lRet = RegOpenKeyEx(HKEY_CURRENT_USER, lpRun, 0, KEY_ALL_ACCESS, &hKey);
	RegDeleteValue(hKey, _T("MFCVideo"));
	RegCloseKey(hKey);
	CRegKey key1;
	key1.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"));
	key1.SetStringValue(_T("MFCVideo"), L" ");
	key1.Close();
	return true;
}


void CDynamicWallpaperDlg::OnBnClickedautostartstatus()
{
	switch (this->IsDlgButtonChecked(IDC_autoStartStatus))
	{
	case BST_CHECKED:
		AutoBootSet();

		break;
	case BST_UNCHECKED:
		AutoBootCancel();

		break;
	}
}

void CAboutDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
}


void CDynamicWallpaperDlg::PostNcDestroy()
{
	cancelPutStonesThread();
	delete g_Ripple;
	restoresWallpaper();

	CDialogEx::PostNcDestroy();
}

void CDynamicWallpaperDlg::restoresWallpaper() {
	HDC hDC = ::GetWindowDC(workerw);
	::SetStretchBltMode(hDC, COLORONCOLOR);
	CRect rect;
	::GetWindowRect(workerw, &rect);
	DynamicBackground.Draw(hDC, rect);
}

int CDynamicWallpaperDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	HANDLE   hMutex = ::CreateMutex(NULL, TRUE, L"MFCVideo");
	if (hMutex != NULL)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			MessageBox(L"已经有一个程序运行.", L"提示");
			PostMessage(WM_QUIT, 0, 0);
			return   FALSE;
		}
	}
	return 0;
}


void CDynamicWallpaperDlg::setLoop() {
	cycleStatus = true;
	cycleHandle = CreateThread(NULL, 0, loopPlayback, NULL, 0, NULL);
}

void CDynamicWallpaperDlg::cancelLoop() {
	cycleStatus = false;
}
void CDynamicWallpaperDlg::OnBnClickedloopplayer()
{
	switch (this->IsDlgButtonChecked(IDC_loopPlayer))
	{
	case BST_CHECKED:
		cancelAutoNextPlayThread();
		setLoop();
		break;
	case BST_UNCHECKED:
		//TerminateThread(cycleHandle, 0);
		cancelLoop();
		setAutoNextPlayThread();
		break;
	}
}

void CDynamicWallpaperDlg::setAutoNextPlayThread() {
	autoNextPlaystatus = true;
	autoNextPlayHandle = CreateThread(NULL, 0, autoNextPlay, NULL, 0, NULL);
}
void CDynamicWallpaperDlg::cancelAutoNextPlayThread() {
	autoNextPlaystatus = false;
}


void CDynamicWallpaperDlg::OnBnClickedWaves()
{
	struct DWDlg {
		CDynamicWallpaperDlg* dw;
		CRipple* cp;
		CRipple** CP;
	};
	DWDlg* dwdlg = new DWDlg();
	switch (this->IsDlgButtonChecked(IDC_Waves))
	{
	case BST_CHECKED:
		dwdlg->dw = this;
		dwdlg->cp = g_Ripple;
		dwdlg->CP = &g_Ripple;
		setPutStonesThread();
		CreateThread(NULL, 0, CheckSystemWallpaper, dwdlg, 0, 0);
		break;
	case BST_UNCHECKED:
		delete dwdlg;
		cancelPutStonesThread();
		restoresWallpaper();
		break;
	}

}

void CDynamicWallpaperDlg::setPutStonesThread() {
	cursorThreadStatus = true;		//鼠标位置信息线程状态		false为不执行
	clikThreadStatus = true;			//水波纹点击线程状态
	slidingThreadStatus = true;		//滑动水波线程状态
	videodata->cr->startTimer();
	CreateThread(NULL, 0, GetCursorDowncharWindowTitle, NULL, 0, NULL);
	CreateThread(NULL, 0, CursorMovePutStones, NULL, 0, 0);
	CreateThread(NULL, 0, ClickPutStones, NULL, 0, 0);
}

void CDynamicWallpaperDlg::cancelPutStonesThread() {
	//TerminateThread(putStonesHandle, 0);
	cursorThreadStatus = false;		//鼠标信息状态		false为不执行
	clikThreadStatus = false;			//水波纹点击线程状态
	slidingThreadStatus = false;		//滑动水波线程状态
	g_Ripple->cancelTimer();
}

//获取所有的文件名
void CDynamicWallpaperDlg::GetAllFiles(string path, vector<string>& wallpaperFilesName)
{

	intptr_t   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;//用来存储文件信息的结构体  
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)  //第一次查找
	{
		do
		{
			if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)  //进入文件夹查找
			{
				wallpaperFilesName.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile); //结束查找
	}

}

long CDynamicWallpaperDlg::wallpaperFileByte() {

	//vector<string> wallpaperFilesName;
	////读取所有的文件，包括子文件的文件
	//GetAllFiles(wallpaperFilePath, wallpaperFilesName);
	TCHAR userName[MAX_PATH + 1];
	DWORD dwSize = 256;
	GetUserName(userName, &dwSize);
	CString buffUserName = userName;
	buffUserName = _T("C:\\Users\\") + buffUserName + _T("\\AppData\\Roaming\\Microsoft\\Windows\\Themes\\CachedFiles\\CachedImage_1920_1080_POS4.jpg");
	buffUserName.Replace(_T(" "), _T(""));
	char* temp = CString_char(buffUserName);
	string wallpaperFilePath = temp;
	delete temp;
	FILE* pFile;
	long tempWallpaperSize = 0;
	pFile = fopen(wallpaperFilePath.c_str(), "rb");
	if (pFile != 0)
	{
		fseek(pFile, 0, SEEK_END);   //将文件指针移动文件结尾
		tempWallpaperSize = ftell(pFile); //求出当前文件指针距离文件开始的字节数
		fclose(pFile);
	}
	buffWallpaperFilePath = wallpaperFilePath;
	return tempWallpaperSize;
}

DWORD CDynamicWallpaperDlg::autoNextPlay(LPVOID lpParameter)
{
	TCHAR szfilePath[MAX_PATH + 1];
	GetModuleFileName(0, szfilePath, MAX_PATH); //文件路径
	PathRemoveFileSpec(szfilePath);//得到应用程序路径
	PathAppend(szfilePath, _T("AutoStartPath.LYJ"));//添加文件名构造出绝对路径
	char* tempSzfilePath = CString_char(szfilePath);
	while (1) {
		Sleep(100);
		if (videodata->vp->get_position() >= 0.9) {

			string loopPath = judgeFile(tempSzfilePath);
			ifstream LYJFilePath(loopPath);
			if (LYJFilePath) {
				char* waitDelete = EncodeToUTF8((char*)loopPath.c_str());
				videodata->vp->loadPlayer(waitDelete);
				free(waitDelete);
				videodata->vp->set_position(0.05);
			}
		}
		if (!autoNextPlaystatus) {
			break;
		}
	}
	delete tempSzfilePath;
	return 0;
}

DWORD CDynamicWallpaperDlg::GetCursorDowncharWindowTitle(LPVOID lpParameter)
{
	while (1) {
		Sleep(getCursorTimerInterval);
		GetCursorPos(&mousePosition); // 获取鼠标当前位置
		::GetWindowTextA(*WindowFromPoint(mousePosition), WindowTitle, sizeof(WindowTitle)); // 获取窗口标题
		if (!cursorThreadStatus) {
			break;
		}
	}
	return 0;
}

DWORD CDynamicWallpaperDlg::CursorMovePutStones(LPVOID lpParameter)
{
	CPoint tempMousePosition;
	while (1) {
		Sleep(overallSlidingFrequency);
		if ((strcmp(WindowTitle, "FolderView") == 0) && tempMousePosition != mousePosition) {
			tempMousePosition = mousePosition;
			videodata->cr->DropStone(mousePosition.x, mousePosition.y, 2, overallSlidingStrength);
		}
		if (!slidingThreadStatus) {
			break;
		}
	}
	return 0;
}



DWORD CDynamicWallpaperDlg::ClickPutStones(LPVOID lpParameter)
{


	/*
	CPoint mousePosition;
	//获取不到信息
	//https://blog.csdn.net/yaotuzhi/article/details/80068556
	//只限当前线程的窗口
	*/
	/*
	MSG msg;
	while (GetMessage(&msg, *GetDesktopWindow(), 0, 0))
	{
		if (msg.message == VK_LBUTTON) {
			GetCursorPos(&mousePosition);
			date->cr->DropStone(mousePosition.x, mousePosition.y, 5, 1000);
		}
	}
	*/


	//将GetKeyState函数更改为GetAsyncKeyState，
	//具体差异见
	//https://blog.csdn.net/flowshell/article/details/5422395
	//https://blog.csdn.net/qq_40501731/article/details/86990934

	//不同层深部分注解
	//https://blog.csdn.net/mkdym/article/details/7018318

	while (1) {
		Sleep(overallClickFrequency);
		if ((strcmp(WindowTitle, "FolderView") == 0) && (GetKeyState(1) & 0x8000 || GetAsyncKeyState(2) & 0x8000)) {
			videodata->cr->DropStone(mousePosition.x, mousePosition.y, 2, overallClickStrength);
		}
		if (!clikThreadStatus) {
			break;
		}
	}

	return 0;
}


DWORD CDynamicWallpaperDlg::CheckSystemWallpaper(LPVOID lpParameter)
{
	struct DWDlg {
		CDynamicWallpaperDlg* dw;
		CRipple* cp;
		CRipple** CP;
	};
	DWDlg* dwdlg = (DWDlg*)lpParameter;
	while (1) {
		Sleep(300);
		if (wallpaperSize != dwdlg->dw->wallpaperFileByte()) {
			dwdlg->dw->cancelPutStonesThread();
			wallpaperSize = dwdlg->dw->wallpaperFileByte();

			dwdlg->dw->DynamicBackground.Destroy();
			dwdlg->dw->DynamicBackground.Load(char_CString((char*)dwdlg->dw->buffWallpaperFilePath.c_str()));

			dwdlg->dw->buffImg = &dwdlg->dw->DynamicBackground;
			dwdlg->dw->hBmpRipple = (HBITMAP)dwdlg->dw->buffImg->operator HBITMAP();
			dwdlg->cp->InitRipple(dwdlg->dw->GetSafeHwnd(), dwdlg->dw->hBmpRipple, 20);
			dwdlg->dw->setPutStonesThread();
		}
	}
	return 0;
}


DWORD CDynamicWallpaperDlg::loopPlayback(LPVOID lpParameter)
{
	float temp;
	Sleep(2700);
	while (1) {
		Sleep(300);
		temp = videodata->vp->get_position();
		if (temp >= 0.9) {
			videodata->vp->set_position(0.1);
		}
		if (!cycleStatus) {
			break;
		}
	}
	return 0;
}


void CDynamicWallpaperDlg::OnBnClickedStopvideo()
{
	cancelLoop();
	videoPlayer->releasePlayer();
	restoresWallpaper();
}


void CDynamicWallpaperDlg::OnBnClickedMysqlservice()
{
	struct ServiceParameters {
		LPCTSTR lpszAppPath;   // 执行程序的文件名
		LPCTSTR lpParameters;  // 参数
		LPCTSTR lpszDirectory; // 执行环境目录
		DWORD dwMilliseconds;    //等待时长
	};
	ServiceParameters* sp = new ServiceParameters();
	int returnValue = 0;
	TCHAR szfilePath[MAX_PATH + 1];
	GetModuleFileName(0, szfilePath, MAX_PATH); //文件路径
	PathRemoveFileSpec(szfilePath);//得到应用程序路径

	sp->dwMilliseconds = 0;
	sp->lpszDirectory = szfilePath;
	sp->lpszAppPath = _T("Service.exe");
	switch (this->IsDlgButtonChecked(IDC_MysqlService))
	{
	case BST_CHECKED:
		sp->lpParameters = _T("设 Mysql 1");
		CreateThread(NULL, 0, SetServiceStatus, sp, 0, 0);
		break;
	case BST_UNCHECKED:
		sp->lpParameters = _T("设 Mysql 0");
		CreateThread(NULL, 0, SetServiceStatus, sp, 0, 0);
		break;
	}
}


void CDynamicWallpaperDlg::OnBnClickedGitblitservice()
{
	struct ServiceParameters {
		LPCTSTR lpszAppPath;   // 执行程序的文件名
		LPCTSTR lpParameters;  // 参数
		LPCTSTR lpszDirectory; // 执行环境目录
		DWORD dwMilliseconds;    //等待时长
	};
	ServiceParameters* sp = new ServiceParameters();
	TCHAR szfilePath[MAX_PATH + 1];
	GetModuleFileName(0, szfilePath, MAX_PATH); //文件路径
	PathRemoveFileSpec(szfilePath);//得到应用程序路径

	sp->dwMilliseconds = 0;
	sp->lpszDirectory = szfilePath;
	sp->lpszAppPath = _T("Service.exe");
	switch (this->IsDlgButtonChecked(IDC_MysqlService))
	{
	case BST_CHECKED:
		sp->lpParameters = _T("设 gitblit 1");
		CreateThread(NULL, 0, SetServiceStatus, sp, 0, 0);
		break;
	case BST_UNCHECKED:
		sp->lpParameters = _T("设 gitblit 0");
		CreateThread(NULL, 0, SetServiceStatus, sp, 0, 0);
		break;
	}

}


DWORD  CDynamicWallpaperDlg::SetServiceStatus(LPVOID lpParameter)
{
	struct ServiceParameters {
		LPCTSTR lpszAppPath;   // 执行程序的文件名
		LPCTSTR lpParameters;  // 参数
		LPCTSTR lpszDirectory; // 执行环境目录
		DWORD dwMilliseconds;	//等待时长
	};
	ServiceParameters* sp = (ServiceParameters*)lpParameter;
	LPCTSTR lpszAppPath = sp->lpszAppPath;
	LPCTSTR lpParameters = sp->lpParameters;
	LPCTSTR lpszDirectory = sp->lpszDirectory;
	DWORD dwMilliseconds = sp->dwMilliseconds;
	delete sp;
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = lpszAppPath;
	ShExecInfo.lpParameters = lpParameters;
	ShExecInfo.lpDirectory = lpszDirectory;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);

	// 指定时间没结束
	/*
	if (WaitForSingleObject(ShExecInfo.hProcess, dwMilliseconds) == WAIT_TIMEOUT)
	{    // 强行杀死进程
		TerminateProcess(ShExecInfo.hProcess, 0);
		return 0;    //强行终止
	}
	
	Sleep(dwMilliseconds);
	DWORD dwExitCode = 0;
	BOOL bOK = GetExitCodeProcess(ShExecInfo.hProcess, &dwExitCode);
	ASSERT(bOK);
	*/
	return 0;

}


DWORD  CDynamicWallpaperDlg::WinExecAndWait32(LPVOID lpParameter)
{

	struct ServiceParameters {
		char* function;
		char* serviceName;
		char* setStatus;
		int* returnValue;
	};
	ServiceParameters* sp = (ServiceParameters*)lpParameter;
	ServiceParameters* tempSp=new ServiceParameters();
	memcpy(tempSp, sp, sizeof(ServiceParameters));
	int returnValue = 0;
	returnValue = queryServiceEntrance(tempSp);
	if (sp->returnValue != NULL) {
		*sp->returnValue = returnValue;
	}
	delete sp;
	delete tempSp;

	/*
	struct ServiceParameters {
		LPCTSTR lpszAppPath;   // 执行程序的文件名
		LPCTSTR lpParameters;  // 参数
		LPCTSTR lpszDirectory; // 执行环境目录
		DWORD dwMilliseconds;	//等待时长
		int* returnValue;
	};
	ServiceParameters* sp = (ServiceParameters*)lpParameter;
	LPCTSTR lpszAppPath = sp->lpszAppPath;
	LPCTSTR lpParameters = sp->lpParameters;
	LPCTSTR lpszDirectory = sp->lpszDirectory;
	DWORD dwMilliseconds = sp->dwMilliseconds;
	int* returnValue = sp->returnValue;
	delete sp;
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = lpszAppPath;
	ShExecInfo.lpParameters = lpParameters;
	ShExecInfo.lpDirectory = lpszDirectory;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);

	// 指定时间没结束
	
	if (WaitForSingleObject(ShExecInfo.hProcess, dwMilliseconds) == WAIT_TIMEOUT)
	{    // 强行杀死进程
		TerminateProcess(ShExecInfo.hProcess, 0);
		return 0;    //强行终止
	}
	
	Sleep(dwMilliseconds);
	DWORD dwExitCode = 0;
	BOOL bOK = GetExitCodeProcess(ShExecInfo.hProcess, &dwExitCode);
	ASSERT(bOK);
	*returnValue = dwExitCode;
	*/

	return 0;
	
}


DWORD  CDynamicWallpaperDlg::SetServiceCheckBoxStatus(LPVOID lpParameter)
{
	struct ServiceCheckBoxParameters {
		CWnd* mysqlCheckBox;
		CWnd* gitblitCheckBox;
		int* mysqlReturnValue;
		int* gitblitReturnValue;
	};

	ServiceCheckBoxParameters* scbp = (ServiceCheckBoxParameters*)lpParameter;
	if (*(scbp->mysqlReturnValue) == 2 || *(scbp->mysqlReturnValue) == 3) {
		((CButton*)scbp->mysqlCheckBox)->SetCheck(1);
	}
	if (*(scbp->gitblitReturnValue) == 2 || *(scbp->gitblitReturnValue) == 3) {
		((CButton*)scbp->gitblitCheckBox)->SetCheck(1);
	}
	delete scbp->mysqlReturnValue;
	delete scbp->gitblitReturnValue;
	delete scbp;
	return 0;
}


void CDynamicWallpaperDlg::OnBnClickedCheckservice()
{
	struct ServiceParameters {
		char* function;
		char* serviceName;
		char* setStatus;
		int* returnValue;
	};
	ServiceParameters* mysqlsp = new ServiceParameters();
	int* mysqlReturnValue = new int();
	mysqlsp->function = "查";
	mysqlsp->serviceName = "Mysql";
	mysqlsp->setStatus = NULL;
	mysqlsp->returnValue = mysqlReturnValue;
	CreateThread(NULL, 0, WinExecAndWait32, mysqlsp, 0, 0);

	ServiceParameters* gitblitsp = new ServiceParameters();
	int* gitblitReturnValue = new int();
	gitblitsp->function = "查";
	gitblitsp->serviceName = "gitblit";
	gitblitsp->setStatus = NULL;
	gitblitsp->returnValue = gitblitReturnValue;
	CreateThread(NULL, 0, WinExecAndWait32, gitblitsp, 0, 0);

	struct ServiceCheckBoxParameters {
		CWnd* mysqlCheckBox;
		CWnd* gitblitCheckBox;
		int* mysqlReturnValue;
		int* gitblitReturnValue;
	};

	ServiceCheckBoxParameters* scbp = new ServiceCheckBoxParameters();
	scbp->gitblitCheckBox = GetDlgItem(IDC_GitblitService);
	scbp->mysqlCheckBox = GetDlgItem(IDC_MysqlService);
	scbp->mysqlReturnValue = mysqlReturnValue;
	scbp->gitblitReturnValue = gitblitReturnValue;
	CreateThread(NULL, 0, SetServiceCheckBoxStatus, scbp, 0, 0);
}
