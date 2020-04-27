
#ifndef DYNAMICWALLPAPERDLG_H_
#define DYNAMICWALLPAPERDLG_H_
#define ssize_t SSIZE_T

#include <WinUser.h>
#include <iostream>
#include <thread>
#include <string>
#include <io.h>
#include <vector>
using namespace std;
#pragma comment(lib, "CRipple.lib")
#pragma comment(lib, "SERVICEDLL.lib")
#pragma comment(lib, "Version.lib ")
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
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	
	DECLARE_MESSAGE_MAP()
private:
	CEdit filepath;
	CString filePath;
	CSliderCtrl m_slider;
	CPoint pt;
	CComboBox transparent;
	CButton autoStartStatus;
				
	//点击x，y坐标
	int cx, cy;								
	//程序所在路径
	TCHAR szfilePath[MAX_PATH + 1];  	
	void GetAllFiles(string path, vector<string>& wallpaperFilesName);
	//最小化到托盘
	void toTray();
	//删除托盘图标
	void DeleteTray();
	//设置透明度
	void setTransparent(float transparent);
	TCHAR* char2TCAHR(char* str);
	//检查是否自启
	BOOL IsAutoBoot();
	//设置自启
	BOOL AutoBootSet();
	//取消自启
	BOOL AutoBootCancel();
	//设置循环
	void setLoop();
	//取消循环
	void cancelLoop();
	//恢复壁纸
	void restoresWallpaper();
	//设置自动播放下一个
	void setAutoNextPlayThread();
	//取消自动播放下一个
	void cancelAutoNextPlayThread();
	//获取版本信息
	CString GetVersion();
	//写入版本信息
	void WrittenVersionInformation();
	//检查更新
	bool CheckUpdate(string tempversion);
	//清理无效播放列表
	static void CleanPlaylist();
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
	afx_msg void OnBnClickedMysqlservice();
	afx_msg void OnBnClickedGitblitservice();
	afx_msg void OnBnClickedCheckservice();
	afx_msg void OnBnClickedCleanplaylist();
public:
	//获得系统壁纸大小
	long wallpaperFileByte();
	//取消水波纹初始点
	void cancelPutStonesThread();
	CImage DynamicBackground;
	CBitmap* buffBitmap;
	CImage* buffImg;
	//水波背景图句柄
	HBITMAP hBmpRipple;
	//设置水波纹初始点
	void setPutStonesThread();
	//系统当前壁纸路径
	string buffWallpaperFilePath;
};

#endif // ! DYNAMICWALLPAPERDLG_H_