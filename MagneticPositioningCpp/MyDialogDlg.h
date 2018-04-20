
// MyDialogDlg.h : ͷ�ļ�
//
#include "glut.h"
#include "SerialPort.h"
#pragma once
#pragma comment(lib,"glut.lib")
#pragma comment(lib,"glut32.lib")
#define Cub_length  20   //��20cm
#define Cub_width   7    //��7cm
#define Cub_height  3    //��3cm
#define Shift_ratio 0.1 //λ������  %�൱�����ű���
#define Axisxs      -7.0 //������xƫ����   ��������ϵ����ڽ������ĵľ���  ���ؾ���
#define Axisys      -5.5 //������yƫ����
#define Axiszs       5.0 //������zƫ����
typedef struct Threadinfo{
	CProgressCtrl *progress;								//����������
	int speed;												//�������ٶ�
	int pos;												//������λ��
} thread,*lpthread;

// CMyDialogDlg �Ի���
class CMyDialogDlg : public CDialogEx
{
public:
	BOOL SetWindowPixelFormat(HDC hDC);   //�趨���ظ�ʽ 
	BOOL CreateViewGLContext(HDC hDC);     //View GL Context 
    void RenderScene();                                //���Ƴ��� 
	HDC hrenderDC;      //DC 
	HGLRC hrenderRC;  //RC  
	int PixelFormat; 
// ����
public:
	CMyDialogDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MYDIALOG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic m_show;
	double m_pitch;//������
	double m_roll;//������
	double m_yaw;//ƫ����
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
public://�������߳�
	thread thread1;												
	HANDLE hThread1;	
	HANDLE hThread2;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
