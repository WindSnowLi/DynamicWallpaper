
// DynamicWallpaperDlg.h: 头文件
//

#include <windows.h>
#include <WinUser.h>
#include <iostream>
#define ssize_t SSIZE_T
#include <vlc/vlc.h>
#include <thread>
#include <fstream>
#include <string>
#include "VideoPlayer.h"
#include "CRipple.h"
#include <io.h>
#include <vector>


using namespace std;
#pragma comment(lib, "libvlc.lib")
#pragma comment(lib, "libvlccore.lib")
#pragma once


// CDynamicWallpaperDlg 对话框
class CDynamicWallpaperDlg : public CDialogEx
{
// 构造
public:
	CDynamicWallpaperDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DynamicWallpaper_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	static bool judgeVedioFile(char* temp);
	static string judgeFile(char* temp);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	static char* EncodeToUTF8(const char* mbcsStr);
	DECLARE_MESSAGE_MAP()
public:
	CEdit filepath;
	CString filePath;
	CSliderCtrl m_slider;
	CPoint pt;
	CComboBox transparent;
	CButton autoStartStatus;

private:
	CImage DynamicBackground;
	CBitmap* buffBitmap;
	CImage* buffImg;
	//水波背景图句柄
	HBITMAP hBmpRipple;				
	//点击x，y坐标
	int cx, cy;						
	//系统当前壁纸路径
	string buffWallpaperFilePath;		
	//程序所在路径
	TCHAR szfilePath[MAX_PATH + 1];  
	 //循环播放线程句柄
	HANDLE cycleHandle;               
	//水波线程句柄
	HANDLE putStonesHandle;				
	//自动播放下一个句柄
	HANDLE autoNextPlayHandle;			

	CRipple* g_Ripple = new CRipple();
	VideoPlayer* videoPlayer = new VideoPlayer();

public:
	//循环播放线程
	static DWORD WINAPI  loopPlayback(          
		LPVOID lpParameter   
	);
	//自动下一个线程
	static DWORD WINAPI autoNextPlay(			
		LPVOID lpParameter   
	);


	//获得鼠标位置和鼠标下的窗口标题
	static DWORD WINAPI GetCursorDowncharWindowTitle(			
		LPVOID lpParameter   
	);
	//鼠标移动水波纹线程
	static DWORD WINAPI CursorMovePutStones(			
		LPVOID lpParameter   
	);
	//点击水波纹线程
	static DWORD WINAPI ClickPutStones(				
		LPVOID lpParameter  
	);
	//检查系统壁纸是否变化
	static DWORD WINAPI CheckSystemWallpaper(				
		LPVOID lpParameter   
	);

	static char* CString_char(CString str);
	static string pathConvert(char* ch);
	static CString char_CString(char* ch);

	

	void setPutStonesThread();
	void cancelPutStonesThread();
	void GetAllFiles(string path, vector<string>& wallpaperFilesName);
	//获得系统壁纸大小
	long wallpaperFileByte();
	//最小化到托盘
	void toTray();
	//删除托盘图标
	void DeleteTray();
	void setTransparent(float transparent);
	TCHAR* char2TCAHR(char* str);
	BOOL IsAutoBoot();
	BOOL AutoBootSet();
	BOOL AutoBootCancel();

	void setLoop();
	void cancelLoop();
	void restoresWallpaper();
	void setAutoNextPlayThread();
	void cancelAutoNextPlayThread();

	virtual void PostNcDestroy();
	afx_msg void OnBnClickedautostartstatus();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedloopplayer();
	afx_msg LRESULT OnShowTask(WPARAM wParam, LPARAM lParam);
	afx_msg void OnExitRmenu();
	afx_msg void OnClose();
	afx_msg void OnCbnSelchangetransparent();
	afx_msg void OnBnClickedSelectfile();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedWaves();

	CSliderCtrl slidingFrequency;
	CSliderCtrl clickFrequency;
	CSliderCtrl slidingStrength;
	CSliderCtrl clickStrength;
	afx_msg void OnBnClickedStopvideo();
};