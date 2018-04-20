
// MyDialogDlg.h : 头文件
//
#include "glut.h"
#include "SerialPort.h"
#pragma once
#pragma comment(lib,"glut.lib")
#pragma comment(lib,"glut32.lib")
#define Cub_length  20   //长20cm
#define Cub_width   7    //宽7cm
#define Cub_height  3    //高3cm
#define Shift_ratio 0.1 //位移因子  %相当于缩放比例
#define Axisxs      -7.0 //坐标轴x偏移量   世界坐标系相对于界面中心的距离  像素距离
#define Axisys      -5.5 //坐标轴y偏移量
#define Axiszs       5.0 //坐标轴z偏移量
typedef struct Threadinfo{
	CProgressCtrl *progress;								//进度条对象
	int speed;												//进度条速度
	int pos;												//进度条位置
} thread,*lpthread;

// CMyDialogDlg 对话框
class CMyDialogDlg : public CDialogEx
{
public:
	BOOL SetWindowPixelFormat(HDC hDC);   //设定象素格式 
	BOOL CreateViewGLContext(HDC hDC);     //View GL Context 
    void RenderScene();                                //绘制场景 
	HDC hrenderDC;      //DC 
	HGLRC hrenderRC;  //RC  
	int PixelFormat; 
// 构造
public:
	CMyDialogDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MYDIALOG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic m_show;
	double m_pitch;//俯仰角
	double m_roll;//滚动角
	double m_yaw;//偏航角
	double m_x;
	double m_y;
	double m_z;
	CProgressCtrl m_progress;
	double m_pitch1;
	double m_pitch2;
	double m_pitch3;
	double m_pitch4;
	double m_pitch5;
	double m_pitch6;
public://进度条线程
	thread thread1;												
	HANDLE hThread1;	
	HANDLE hThread2;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
