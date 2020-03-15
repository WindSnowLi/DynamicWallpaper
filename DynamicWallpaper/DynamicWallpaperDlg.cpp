
// DynamicWallpaperDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "DynamicWallpaper.h"
#include "DynamicWallpaperDlg.h"
#include "afxdialogex.h"

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
int overallClickStrength = 800;				//水波点击力度
int overallSlidingStrength = 240;				//水波滑动力度	
bool putStonesStatus = false;            //水波线程状态		false为不执行
bool cycleStatus = false;				//循环播放线程状态
bool autoNextPlaystatus = false;		//自动播放下一个句柄
static int lineNumber;					//当前播放的行数
long wallpaperSize;						//壁纸大小
bool cursorThreadStatus = false;		//鼠标信息线程状态
	//子线程需要的所有数据源
struct Date {
	VedioPlayer* vp;
	CRipple* cr;
	CImage* ci;
};
Date* date = new Date();


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
	//  DDX_Slider(pDX, IDC_SlidingStrength, slidingStrength);
	//  DDX_Slider(pDX, IDC_ClickStrength, clickStrength);
	DDX_Control(pDX, IDC_SlidingStrength, SlidingStrength);
	DDX_Control(pDX, IDC_ClickStrength, ClickStrength);
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
	ClickStrength.SetRange(0, 100);//设置滑动范围为1到20
	ClickStrength.SetTicFreq(1);//每1个单位画一刻度
	ClickStrength.SetPos(50);//设置滑块初始位置为1 
	
	//2000/100=20		240/20=12
	SlidingStrength.SetRange(0, 100);//设置滑动范围为1到20
	SlidingStrength.SetTicFreq(1);//每1个单位画一刻度
	SlidingStrength.SetPos(12);//设置滑块初始位置为1 

	CString a;
	a.Format(_T("%d"), 30);
	GetDlgItem(IDC_showVolume)->SetWindowTextW(a);
	a.Format(_T("%d"), 12);
	GetDlgItem(IDC_showSliding)->SetWindowTextW(a);
	a.Format(_T("%d"), 40);
	GetDlgItem(IDC_showClick)->SetWindowTextW(a);


	HWND hwnd_progman = ::FindWindow(L"Progman", NULL);
	if (hwnd_progman == NULL) {
		MessageBox(_T("初始化错误"));
	}
	DWORD_PTR result = 0;
	SendMessageTimeout(hwnd_progman, 0x052c, NULL, NULL, SMTO_NORMAL, 1000, &result);

	EnumWindows(EnumwindowProcFindDesktopwindow, NULL);
	vedioPlayer->workerw = workerw;
	g_Ripple->workerw = workerw;

	int index = transparent.FindStringExact(0, _T("0.4"));
	//0表示从索引为0的选项开始查找.如果找到有叫three的选项就返回它的索引
	transparent.SetCurSel(index);

	setTransparent(255*0.4);


	//初始化子线程需要的数据
	date->vp = vedioPlayer;
	date->cr = g_Ripple;
	date->ci = &DynamicBackground;
	/*
	if (mOldBackgroud.IsNull()) {
		HDC hDC = ::GetWindowDC(workerw);
		int nBitPerPixel = GetDeviceCaps(hDC, BITSPIXEL);
		int nWidth = GetDeviceCaps(hDC, HORZRES);
		int nHeight = GetDeviceCaps(hDC, VERTRES);

		mOldBackgroud.Create(nWidth, nHeight, nBitPerPixel);
		BitBlt(mOldBackgroud.GetDC(), 0, 0, nWidth, nHeight, hDC, 0, 0, SRCCOPY);
		mOldBackgroud.ReleaseDC();
	}
	*/
	/*
	CImage* img;

	CBitmap* bm;

	img=&mOldBackgroud;

	HBITMAP hbmp = (HBITMAP)img->operator HBITMAP();


	//CBitmap   bitmap;                            //定义位图
	//bitmap.LoadBitmap(IDB_RIPPLE1);           //这个IDB_BITMAP1要自己添加
	//hBmpRipple = bitmap;
	hBmpRipple = hbmp;
	g_Ripple.InitRipple(GetSafeHwnd(), hBmpRipple, 30);

	*/

	if (IsAutoBoot()) {
		((CButton*)GetDlgItem(IDC_autoStartStatus))->SetCheck(1);
		AutoBootSet();
	}

	GetModuleFileName(0, szfilePath, MAX_PATH); //文件路径

	CString startingmethod=GetCommandLine();//启动方式

	CString szFilePathTemp = szfilePath;  //文件路径
	szFilePathTemp = _T("\"")+ szFilePathTemp + _T("\"");   

	startingmethod.Replace(_T(" "), _T(""));
	szFilePathTemp.Replace(_T(" "), _T(""));

	if (startingmethod.Compare(szFilePathTemp))
	{
		PathRemoveFileSpec(szfilePath);//得到应用程序路径
		PathAppend(szfilePath, _T("AutoStartPath.LYJ"));//添加文件名构造出绝对路径

		if (judgeVedioFile(CString_char(szfilePath)))
		{
			string fileBuff;
			fileBuff = judgeFile(CString_char(szfilePath));
			if (judgeVedioFile((char*)fileBuff.c_str())) {
				vedioPlayer->loadPlayer(EncodeToUTF8((char*)fileBuff.c_str()));
				GetDlgItem(IDC_FILEPath)->SetWindowTextW(char_CString((char*)fileBuff.c_str()));
				SetTimer(2, 10, NULL);
				setLoop();
			}
			
		}
	}

	((CButton*)GetDlgItem(IDC_loopPlayer))->SetCheck(1);
	
	wallpaperSize = wallpaperFileByte();
	DynamicBackground.Load(char_CString((char*)pathConvert((char*)buffWallpaperFilePath.c_str()).c_str()));
	//MessageBox(char_CString((char*)pathConvert((char*)buffWallpaperFilePath.c_str()).c_str()));
	
	PathRemoveFileSpec(szfilePath);//得到应用程序路径
	PathAppend(szfilePath, _T("Spare.jpg"));//添加文件名构造出绝对路径
	if (DynamicBackground.IsNull()) {
		DynamicBackground.Load(szfilePath);
	}

	buffImg = &DynamicBackground;

	HBITMAP hbmp = (HBITMAP)buffImg->operator HBITMAP();
	g_Ripple->InitRipple(GetSafeHwnd(), hbmp, 30);

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
 string CDynamicWallpaperDlg::judgeFile(char*temp) {
	
	ifstream file(temp);
	string fileBuff;
	int i = 0;
	if (file)
	{
		string line;
		while (getline(file, line)) // line中不包括每行的换行符  
		{
			i++;
			if (i<=lineNumber) {
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
	if (nID == SC_MINIMIZE)
		toTray(); //最小化到托盘的函数
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
		else
		{
			s = s + temp[i];
		}
	}
	return s;
}

CString CDynamicWallpaperDlg::char_CString(char* ch)
{
	// TODO: 在此处添加实现代码.
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
		cycleStatus = false;
		filePath = dlg.GetPathName();
		videopath = T2A(filePath);
		vedioPlayer->loadPlayer(EncodeToUTF8(pathConvert(videopath).c_str()));
		GetDlgItem(IDC_FILEPath)->SetWindowTextW(filePath);
		TCHAR startfilePath[MAX_PATH + 1];

		GetModuleFileName(0, startfilePath, MAX_PATH);
		PathRemoveFileSpec(startfilePath);//得到应用程序路径
		PathAppend(startfilePath,_T("AutoStartPath.LYJ"));//添加文件名构造出绝对路径

		ofstream outfile;
		outfile.open(startfilePath, ios::out | ios::app);
		string write = videopath;
		write = write+ "\n";;
		outfile << write;
		outfile.close();
		if (((CButton*)GetDlgItem(IDC_loopPlayer))->GetCheck() == 1) {
			cycleStatus = true;
			setLoop();
		}
	}
}




void CDynamicWallpaperDlg::OnTimer(UINT_PTR nIDEvent)
{
	float temp;
	switch (nIDEvent) {
	case 1:
		//temp = vedioPlayer->get_position();
		//if (temp >= 0.9) {
		//	vedioPlayer->set_position(0.1);
		//}
		break;
	case 2:
		this->ShowWindow(SC_MINIMIZE);
		toTray();
		KillTimer(2);
		break;
	case 3:
		//if (vedioPlayer->get_position()>=0.9) {
		//	TCHAR szfilePath[MAX_PATH + 1];
		//	GetModuleFileName(0, szfilePath, MAX_PATH); //文件路径
		//	PathRemoveFileSpec(szfilePath);//得到应用程序路径
		//	PathAppend(szfilePath, _T("AutoStartPath.LYJ"));//添加文件名构造出绝对路径
		//	string loopPath = judgeFile(CString_char(szfilePath));
		//	ifstream test(loopPath);
		//	if (test) {
		//		vedioPlayer->loadPlayer((char*)loopPath.c_str());
		//		vedioPlayer->set_position(0.05);
		//	}
		//}
		break;
	case 4:
		/*GetCursorPos(&pt);
		cx = pt.x;
		cy = pt.y;
		g_Ripple->DropStone(cx, cy, 2, 256);*/
		break;
	case 5:
		if (wallpaperSize != wallpaperFileByte()) {
			wallpaperSize = wallpaperFileByte();
			//KillTimer(4);
			cancelPutStonesThread();
			//g_Ripple->cancelTimer();

			//HDC hDC = ::GetWindowDC(workerw);
			//int nBitPerPixel = GetDeviceCaps(hDC, BITSPIXEL);
			//int nWidth = GetDeviceCaps(hDC, HORZRES);
			//int nHeight = GetDeviceCaps(hDC, VERTRES);
			//DynamicBackground.Destroy();
			//DynamicBackground.Create(nWidth, nHeight, nBitPerPixel);
			//BitBlt(DynamicBackground.GetDC(), 0, 0, nWidth, nHeight, hDC, 0, 0, SRCCOPY);
			//DynamicBackground.ReleaseDC();
			DynamicBackground.Destroy();
			DynamicBackground.Load(char_CString((char*)buffWallpaperFilePath.c_str()));
			if (DynamicBackground.IsNull()) {
				PathRemoveFileSpec(szfilePath);//得到应用程序路径
				PathAppend(szfilePath, _T("Spare.jpg"));//添加文件名构造出绝对路径
				DynamicBackground.Load(szfilePath);
			}
			
			buffImg = &DynamicBackground;
			hBmpRipple = (HBITMAP)buffImg->operator HBITMAP();

			delete g_Ripple;
			g_Ripple = NULL;
			g_Ripple = new CRipple();
			g_Ripple->workerw = workerw;

			g_Ripple->InitRipple(GetSafeHwnd(), hBmpRipple, 30);
			g_Ripple->startTimer();
			//SetTimer(4, 100, NULL);
			setPutStonesThread();
		}
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
		vedioPlayer->setVolume(pos);
		GetDlgItem(IDC_showVolume)->SetWindowTextW(b);
		break;
	case IDC_SlidingStrength:
		overallSlidingStrength = pos*20;
		GetDlgItem(IDC_showSliding)->SetWindowTextW(b);
		break;
	case IDC_ClickStrength:
		overallClickStrength = pos*20;
		GetDlgItem(IDC_showClick)->SetWindowTextW(b);
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
	this->ShowWindow(SW_HIDE); //隐藏主窗口
}
LRESULT CDynamicWallpaperDlg::OnShowTask(WPARAM wParam, LPARAM lParam)
{
	switch (lParam){
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
		this->ShowWindow(SW_SHOW);//简单的显示主窗口完事儿
		DeleteTray();
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	/*
	if (!mOldBackgroud.IsNull()) {
		HDC hDC = ::GetWindowDC(workerw);
		::SetStretchBltMode(hDC, COLORONCOLOR);

		CRect rect;
		::GetWindowRect(workerw, &rect);
		mOldBackgroud.Draw(hDC, rect);
	}
	*/
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
		if (fun)fun(this->GetSafeHwnd(), 0, (1-transparent+0.05)*255, 2);
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
		if (RegQueryValueEx(key, _T("MFCVedio"), 0, &type, path_Get, &dwBytes) == ERROR_SUCCESS){
			temp = true;
		}
		else{
			temp = false;
		}
	}
	RegCloseKey(key);
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

		CString ssTag = _T(" ")+ss;
		BYTE* bp = (BYTE*)ssTag.GetBuffer(ssTag.GetLength());
		BYTE expected[16];
		CopyMemory(expected, bp, sizeof(expected));

	
		lRet = RegSetValueEx(hKey, _T("MFCVedio"), 0, REG_SZ, bp, (lstrlen(ss) + 1) * sizeof(TCHAR)); 
		RegCloseKey(hKey);
		/*
		
		因为本人电脑注册表Run路径有时无法写入标准的路径，所以在这用的在Run“标记”的方式，实际自启信息在RunOnce，
		每次启动检查“标记”，如果存在，再次写入RunOnce
		
		*/

		CRegKey key1;
		key1.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"));
		key1.SetStringValue(_T("MFCVedio"), ss);
		key1.Close();
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
	RegDeleteValue(hKey, _T("MFCVedio"));
	RegCloseKey(hKey);
	CRegKey key1;
	key1.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"));
	key1.SetStringValue(_T("MFCVedio"), L" ");
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
	//if (!mOldBackgroud.IsNull()) {
	//	HDC hDC = ::GetWindowDC(workerw);
	//	::SetStretchBltMode(hDC, COLORONCOLOR);
	//	CRect rect;
	//	::GetWindowRect(workerw, &rect);
	//	mOldBackgroud.Draw(hDC, rect);
	//}
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
	HANDLE   hMutex = ::CreateMutex(NULL, TRUE, L"MFCVedio");
	if (hMutex != NULL)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			MessageBox(L"已经有一个程序运行.",L"提示");
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
	// TODO: 在此添加控件通知处理程序代码
	switch (this->IsDlgButtonChecked(IDC_loopPlayer))
	{
	case BST_CHECKED:
		//KillTimer(3);
		cancelAutoNextPlayThread();
		setLoop();
		break;
	case BST_UNCHECKED:
		//TerminateThread(cycleHandle, 0);
		cancelLoop();
		//SetTimer(3, 100, NULL);
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

//void CDynamicWallpaperDlg::OnBnClickedButton2()
//{
//	vedioPlayer->set_stop();
//	// TODO: 在此添加控件通知处理程序代码
//}


//void CDynamicWallpaperDlg::OnBnClickedButton1()
//{
//	// TODO: 在此添加控件通知处理程序代码
//}


void CDynamicWallpaperDlg::OnBnClickedWaves()
{
	switch (this->IsDlgButtonChecked(IDC_Waves))
	{
	case BST_CHECKED:
		SetTimer(5, 3000, NULL);
		//SetTimer(4, 100, NULL);
		//putstonesdate->cr = g_Ripple;
		setPutStonesThread();
		//g_Ripple->startTimer();
		break;
	case BST_UNCHECKED:
		KillTimer(5);
		//KillTimer(4);
		cancelPutStonesThread();
		//g_Ripple->cancelTimer();
		restoresWallpaper();
		break;
	}
	
	// TODO: 在此添加控件通知处理程序代码
}

void CDynamicWallpaperDlg::setPutStonesThread() {
	putStonesStatus = true;
	putStonesHandle = CreateThread(NULL, 0, putStones, date, 0, NULL);
}

void CDynamicWallpaperDlg::cancelPutStonesThread() {
	//TerminateThread(putStonesHandle, 0);
	putStonesStatus = false;
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
	CString buffUserName=userName;
	buffUserName = _T("C:\\Users\\")+ buffUserName + _T("\\AppData\\Roaming\\Microsoft\\Windows\\Themes\\CachedFiles\\CachedImage_1920_1080_POS4.jpg");
	buffUserName.Replace(_T(" "), _T(""));
	char* temp = CString_char(buffUserName);
	string wallpaperFilePath = temp;
	delete temp;
	FILE* pFile;
	long tempWallpaperSize=0;
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



//void CDynamicWallpaperDlg::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	g_Ripple->DropStone(point.x, point.y, 2, 2000);
//	CDialogEx::OnLButtonDown(nFlags, point);
//}
DWORD CDynamicWallpaperDlg::autoNextPlay(LPVOID lpParameter)
{
	TCHAR szfilePath[MAX_PATH + 1];
	GetModuleFileName(0, szfilePath, MAX_PATH); //文件路径
	PathRemoveFileSpec(szfilePath);//得到应用程序路径
	PathAppend(szfilePath, _T("AutoStartPath.LYJ"));//添加文件名构造出绝对路径
	while (1) {
		Sleep(100);
		if (date->vp->get_position() >= 0.9) {
			string loopPath = judgeFile(CString_char(szfilePath));
			ifstream test(loopPath);
			if (test) {
				date->vp->loadPlayer((char*)loopPath.c_str());
				date->vp->set_position(0.05);
			}
		}
		if (!autoNextPlaystatus) {
			break;
		}
	}
	return 0;
}

DWORD CDynamicWallpaperDlg::putStones(LPVOID lpParameter)
{
	CPoint mousePosition;
	CPoint tempMousePosition;

	date->cr->startTimer();

	CreateThread(NULL, 0, CursorMessage,NULL,0,0);
	cursorThreadStatus = true;

	while (1) {
		Sleep(300);
		GetCursorPos(&tempMousePosition);
		if (tempMousePosition != mousePosition) {
			mousePosition = tempMousePosition;
			date->cr->DropStone(mousePosition.x, mousePosition.y, 2, overallSlidingStrength);
		}
		if (!putStonesStatus) {
			cursorThreadStatus = false;
			break;
		}
	}
	date->cr->cancelTimer();
	//delete tempPutstonesdate;
	return 0;
}

DWORD CDynamicWallpaperDlg::CursorMessage(LPVOID lpParameter) 
{
	CPoint mousePosition;
	
	/*
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
		Sleep(200);
		if (GetAsyncKeyState(1) & 0x8000 || GetAsyncKeyState(2) & 0x8000) {
			GetCursorPos(&mousePosition);
			date->cr->DropStone(mousePosition.x, mousePosition.y, 2, overallClickStrength);
		}
		if (!cursorThreadStatus) {
			break;
		}
	}
	
	return 0;
}

DWORD CDynamicWallpaperDlg::loopPlayback(LPVOID lpParameter)
{
	float temp;
	Sleep(2700);
	/*
	Sleep(2700);
	int vedioLength = date->vp->get_length();
	int sleeptime = vedioLength / 11;
	if (sleeptime==0) {
		sleeptime = 300;
	}
	*/
	while (1) {
		Sleep(300);
		temp = date->vp->get_position();
		if (temp >= 0.9) {
			date->vp->set_position(0.1);
		}
		if (!cycleStatus) {
			break;
		}
	}
	//delete tempdate;
	return 0;
}


//void CDynamicWallpaperDlg::OnNMCustomdrawvolume(NMHDR* pNMHDR, LRESULT* pResult)
//{
//	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
//	// TODO: 在此添加控件通知处理程序代码
//	*pResult = 0;
//}
