
// MFC_VedioDlg.h: 头文件
//

#include <windows.h>
#include <WinUser.h>
#include <iostream>
#define ssize_t SSIZE_T
#include <vlc/vlc.h>
#include <thread>
#include <fstream>
#include <string>
#include "VedioPlayer.h"
using namespace std;
#pragma comment(lib, "libvlc.lib")
#pragma comment(lib, "libvlccore.lib")
#pragma once


// CMFCVedioDlg 对话框
class CMFCVedioDlg : public CDialogEx
{
// 构造
public:
	CMFCVedioDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_VEDIO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	bool judgeVedioFile(char* temp);
	string judgeFile(char* temp);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	char* EncodeToUTF8(const char* mbcsStr);
	DECLARE_MESSAGE_MAP()
public:
	CEdit filepath;
	CString filePath;
	string pathConvert(char* ch);
	CString char_CString(char* ch);
	CSliderCtrl m_slider;
	int lineNumber=0;
private:
	CImage mOldBackgroud;
public:
	VedioPlayer* vedioPlayer = new VedioPlayer();
	void toTray();//最小化到托盘
	void DeleteTray();//删除托盘图标
	void setTransparent(float transparent);
	char* CString_char(CString str);
	TCHAR* char2TCAHR(char* str);
	BOOL IsAutoBoot();
	BOOL AutoBootSet();
	BOOL AutoBootCancel();
	CComboBox transparent;
	CButton autoStartStatus;
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedautostartstatus();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void setLoop();
	afx_msg void OnBnClickedloopplayer();
	afx_msg LRESULT OnShowTask(WPARAM wParam, LPARAM lParam);
	afx_msg void OnExitRmenu();
	afx_msg void OnClose();
	afx_msg void OnCbnSelchangetransparent();
	afx_msg void OnBnClickedSelectfile();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
