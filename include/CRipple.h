#ifndef  CRIPPLE_H_
#define CRIPPLE_H_

#ifdef CRIPPLE_EXPORTS
#define CRIPPLE_API __declspec(dllexport)
#else
#define CRIPPLE_API __declspec(dllimport)
#endif

// 此类是从 dll 导出的
class CRIPPLE_API CRipple {
public:
	CRipple();
	virtual ~CRipple();
	//[成员函数]
public:
	//获取水波位图宽
	int GetRippleWidth() { return m_iBmpWidth; }
	//获取水波位图高
	int GetRippleHeight() { return m_iBmpHeight; }
	//初始化水波对象
	bool InitRipple(HWND hWnd, HBITMAP hBmp, UINT uiSpeed);
	//开始定时器
	void startTimer();
	//取消定时器
	void cancelTimer();
	//释放水波对象
	void FreeRipple();
	//刷新到屏幕DC
	void UpdateFrame(HDC hDc);
	//扔石子（设定波源）
	void DropStone(int x, int y, int stoneSize, int stoneWeight);
	//水波扩散
	void WaveSpread();
	//根据波幅渲染水波
	void WaveRender();
	//目标窗口句柄
	HWND workerw;
	//[成员变量]
private:
	HWND				m_hWnd;						//窗口句柄
	HDC					m_hRenderDC;				//内存渲染DC句柄
	HBITMAP				m_hRenderBmp;				//渲染位图句柄
	int					m_iBmpWidth;				//水波背景图片宽
	int					m_iBmpHeight;				//水波背景图片高
	int					m_iBytesPerWidth;			//位图每一行的字节数
	int* m_pWaveBuf1;				//波能缓冲区1
	int* m_pWaveBuf2;				//波能缓冲区2
	BYTE* m_pBmpSource;				//水波位图原始数据
	BYTE* m_pBmpRender;				//水波位图渲染数据
	BITMAPINFO			m_stBitmapInfo;			    //位图信息结构
};

//全局水波纹对象
CRIPPLE_API extern CRipple* g_Ripple;

#endif // ! CRIPPLE_H_