
// MyDialogDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyDialog.h"
#include "MyDialogDlg.h"
#include "afxdialogex.h"
#include "valchange.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
bool bInLoop = true;
CSerialPort mySerialPort;
/* ����������Ķ�������ֵ */
float a = Cub_length*0.2;
float b = Cub_height*1.0;
float c = Cub_width*1.8;
GLfloat sp1[]={a,-b,-c}, sp2[]={a,b,-c},
sp3[]={a,b,c}, sp4[]={a,-b,c},
sp5[]={-a,-b,c}, sp6[]={-a,b,c},
sp7[]={-a,b,-c}, sp8[]={-a,-b,-c};

/* ����������Ķ��㷽��ֵ */
GLfloat m1[]={1.0,0.0,0.0}, m2[]={-1.0,0.0,0.0},
		m3[]={0.0,1.0,0.0}, m4[]={0.0,-1.0,0.0},
		m5[]={0.0,0.0,1.0}, m6[]={0.0,0.0,-1.0};

/* ����������Ķ�����ɫֵ */
GLfloat c1[]={1.0,0.0,0.0}, c2[]={0.0,1.0,0.0},c3[]={0.0,0.0,1.0};	
//�߳���ɱ��
bool flag=false;//�߳�1���
bool flag_p=false;//�߳�2���
bool flag1=false;//���ڳ�ʼ�����
bool flag2=false;//���ڼ������
CString str1,str2;
DWORD WINAPI ThreadFun(LPVOID pthread)						//�߳���ں���
{
	lpthread temp=(lpthread)pthread;						//�������ṹ��
	temp->progress->SetPos(temp->pos);					
	while(temp->pos<50&&flag1&&flag2)
	{
		Sleep(temp->speed);							//�����ٶ�
		temp->pos++;								//���ӽ���
		temp->progress->SetPos(temp->pos);			//���ý���������λ��
	}
	if(flag1&&flag2)
		flag_p=true;
	return true;
}
//����Ҫ��
//DWORD WINAPI ThreadFun2(LPVOID pthread)						//�߳���ں���
//{
//	//���ӽ�������Ӻ���
//	CSerialPort mySerialPort;
//	if (!mySerialPort.InitPort(7))//�����޸� if(!mySerialPort.InitPort(4))
//	{
//		str1="initPort fail !";
//		flag1=false;
//	}
//	else
//	{
//		str1="initPort success !";
//		flag1=true;
//	}
//
//	if (!mySerialPort.OpenListenThread())
//	{
//		str2="OpenListenThread fail !";
//		flag2=false;
//	}
//	else
//	{
//		str2="OpenListenThread success !";
//		flag2=true;
//	}
//	if(flag1&&flag2)
//		flag=true;
//	else
//		flag=false;
//	MessageBox(NULL,str1+"\n"+str2,"",1);
//	return true;
//}
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMyDialogDlg �Ի���


CMyDialogDlg::CMyDialogDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyDialogDlg::IDD, pParent)
{
	PixelFormat=0; 
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pitch = 0.0;//�����ʼ����
	m_roll = 0.0;
	m_yaw = 0.0;
	m_x = 0.0;
	m_y = 0.0;
	m_z = 0.0;
	m_pitch1 =0.0;  //pitch
	m_pitch2 =0.0;   //roll
	m_pitch3 =0.0;   //yaw
	m_pitch4 =0.0;  //x
	m_pitch5 =0.0;  //y
	m_pitch6 =0.0;    //z
}

void CMyDialogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SHOW, m_show);
	DDX_Text(pDX, IDC_EDIT_PITCH, m_pitch1);
	DDX_Text(pDX, IDC_EDIT_ROLL, m_pitch2);
	DDX_Text(pDX, IDC_EDIT_YAW, m_pitch3);
	DDX_Text(pDX, IDC_EDITX, m_pitch4);
	DDX_Text(pDX, IDC_EDITY, m_pitch5);
	DDX_Text(pDX, IDC_EDITZ, m_pitch6);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
}

BEGIN_MESSAGE_MAP(CMyDialogDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CMyDialogDlg::OnBnClickedStart)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CMyDialogDlg ��Ϣ�������

BOOL CMyDialogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_progress.SetRange(0,50);//���ý�������Χ
	///////////////////////OPENGL INIT///////////////////////// 
	CWnd *wnd=GetDlgItem(IDC_SHOW); 
    hrenderDC=::GetDC(wnd->m_hWnd); 
	if(SetWindowPixelFormat(hrenderDC)==FALSE) 
		return 0; 
	
	if(CreateViewGLContext(hrenderDC)==FALSE) 
		return 0; 
	glPolygonMode(GL_FRONT,GL_FILL); 
	glPolygonMode(GL_BACK,GL_FILL); 
	/////////////////////////////////////////// 
	CRect ImageRect;
	m_show.GetWindowRect(ImageRect);
	ScreenToClient(ImageRect);
	int width=(ImageRect.right-ImageRect.left)/2*2;
	int height=(ImageRect.bottom-ImageRect.top)/2*2;
	glEnable(GL_TEXTURE_2D); 
	glShadeModel(GL_SMOOTH); 
	glViewport(0,0,width,height); 
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	if (ImageRect.Width() <= ImageRect.Height())		
		glOrtho(-8.0, 8.0, -8.0*(GLfloat)ImageRect.Height()/(GLfloat)ImageRect.Width(),		
		8.0*(GLfloat)ImageRect.Height()/(GLfloat)ImageRect.Width(), -8.0, 8.0);	
	else		
		glOrtho(-8.0*(GLfloat)ImageRect.Width()/(GLfloat)ImageRect.Height(),		
		8.0*(GLfloat)ImageRect.Width()/(GLfloat)ImageRect.Height(), -8.0, 8.0, -8.0, 8.0);
	//gluPerspective(45,1,0.1,100.0); 
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity(); 
	gluLookAt(2.0,0.5,2.0, 0.0,0.0,0.0, 0.0,1.0,0.0);
	glShadeModel(GL_SMOOTH);       // Enable Smooth Shading 
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);    // Black Background 
	glClearDepth(1.0f);         // Depth Buffer Setup 
	glEnable(GL_DEPTH_TEST);       // Enables Depth Testing 
	glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do 
	///////////////////////////////////////////////////////////////////////// 
	glEnableClientState(GL_VERTEX_ARRAY); 
	glEnableClientState(GL_TEXTURE_COORD_ARRAY); 

	//�����ı���������Ĵ�С
	CFont *m_Font;
        m_Font = new CFont; 
        m_Font->CreateFont(50,25,0,0,80,
        FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_SWISS,_T("Arial"));
		//X-edit��
        CEdit *m_Edit_X=(CEdit *)GetDlgItem(IDC_EDIT_PITCH);
        m_Edit_X->SetFont(m_Font,FALSE);
        GetDlgItem(IDC_EDIT_PITCH)->SetFont(m_Font);
		//Y-edit ��
		CEdit *m_Edit_Y=(CEdit *)GetDlgItem(IDC_EDIT_ROLL);
        m_Edit_Y->SetFont(m_Font,FALSE);
        GetDlgItem(IDC_EDIT_ROLL)->SetFont(m_Font);
		//Z-edit ��
		CEdit *m_Edit_Z=(CEdit *)GetDlgItem(IDC_EDIT_YAW);
        m_Edit_Z->SetFont(m_Font,FALSE);
        GetDlgItem(IDC_EDIT_YAW)->SetFont(m_Font);
			//psi-edit��
        CEdit *m_Edit_psi=(CEdit *)GetDlgItem(IDC_EDITX);
        m_Edit_psi->SetFont(m_Font,FALSE);
        GetDlgItem(IDC_EDITX)->SetFont(m_Font);
		//phi-edit ��
		CEdit *m_Edit_phi=(CEdit *)GetDlgItem(IDC_EDITY);
        m_Edit_phi->SetFont(m_Font,FALSE);
        GetDlgItem(IDC_EDITY)->SetFont(m_Font);
		//theta-edit ��
		CEdit *m_Edit_theta=(CEdit *)GetDlgItem(IDC_EDITZ);
        m_Edit_theta->SetFont(m_Font,FALSE);
        GetDlgItem(IDC_EDITZ)->SetFont(m_Font);


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMyDialogDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMyDialogDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMyDialogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
BOOL CMyDialogDlg::SetWindowPixelFormat(HDC hDC) 
{ 
	PIXELFORMATDESCRIPTOR pixelDesc; 
	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR); 
	pixelDesc.nVersion = 1; 
	pixelDesc.dwFlags = PFD_DRAW_TO_WINDOW | 
		PFD_SUPPORT_OPENGL | 
		PFD_DOUBLEBUFFER | 
		PFD_TYPE_RGBA; 
	pixelDesc.iPixelType = PFD_TYPE_RGBA; 
	pixelDesc.cColorBits = 32; 
	pixelDesc.cRedBits = 0; 
	pixelDesc.cRedShift = 0; 
	pixelDesc.cGreenBits = 0; 
	pixelDesc.cGreenShift = 0; 
	pixelDesc.cBlueBits = 0; 
	pixelDesc.cBlueShift = 0; 
	pixelDesc.cAlphaBits = 0; 
	pixelDesc.cAlphaShift = 0; 
	pixelDesc.cAccumBits = 0; 
	pixelDesc.cAccumRedBits = 0; 
	pixelDesc.cAccumGreenBits = 0; 
	pixelDesc.cAccumBlueBits = 0; 
	pixelDesc.cAccumAlphaBits = 0; 
	pixelDesc.cDepthBits = 0; 
	pixelDesc.cStencilBits = 1; 
	pixelDesc.cAuxBuffers = 0; 
	pixelDesc.iLayerType = PFD_MAIN_PLANE; 
	pixelDesc.bReserved = 0; 
	pixelDesc.dwLayerMask = 0; 
	pixelDesc.dwVisibleMask = 0; 
	pixelDesc.dwDamageMask = 0; 
	PixelFormat = ChoosePixelFormat(hDC,&pixelDesc); 
	if(PixelFormat==0) // Choose default 
	{ 
		PixelFormat = 1; 
		if(DescribePixelFormat(hDC,PixelFormat, 
			sizeof(PIXELFORMATDESCRIPTOR),&pixelDesc)==0) 
		{ 
			return FALSE; 
		} 
	} 
	if(SetPixelFormat(hDC,PixelFormat,&pixelDesc)==FALSE) 
	{ 
		return FALSE; 
	} 
	return TRUE; 
} 

BOOL CMyDialogDlg::CreateViewGLContext(HDC hDC) 
{ 
	hrenderRC = wglCreateContext(hDC); 
	if(hrenderRC==NULL) 
		return FALSE; 
	if(wglMakeCurrent(hDC,hrenderRC)==FALSE) 
		return FALSE; 
	return TRUE; 
} 
void CMyDialogDlg::RenderScene()   
{ 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	////����������
	glBegin(GL_LINES);
	glColor3f(1.0f,1.0f,1.0f);
	// y-axis
    glVertex3f( Axisxs, Axisys, Axiszs);
    glVertex3f( Axisxs+4.0f, Axisys, Axiszs);
	// y-axis arrow
	glVertex3f(Axisxs+4.0f, Axisys, Axiszs);
	glVertex3f( Axisxs+3.9f, Axisys+0.1f, Axiszs);
	glVertex3f(Axisxs+4.0f, Axisys, Axiszs);
	glVertex3f( Axisxs+3.9f, Axisys-0.1f, Axiszs);
	//z-axis
	glVertex3f( Axisxs, Axisys, Axiszs);
    glVertex3f( Axisxs, Axisys-2.0f, Axiszs);
	// z-axis arrow
	glVertex3f( Axisxs, Axisys-2.0f, Axiszs);
	glVertex3f( Axisxs+0.1f, Axisys-1.9f, Axiszs);
	glVertex3f( Axisxs, Axisys-2.0f, Axiszs);
	glVertex3f( Axisxs-0.1f, Axisys-1.9f, Axiszs);
	//x-axis
	glVertex3f( Axisxs, Axisys, Axiszs );
    glVertex3f( Axisxs, Axisys, Axiszs-2.0f );
	//// z-axis arrow
	glVertex3f( Axisxs, Axisys, Axiszs-2.0f );
	glVertex3f( Axisxs, Axisys+0.1f, Axiszs-1.9f );
	glVertex3f( Axisxs, Axisys, Axiszs-2.0f );
	glVertex3f( Axisxs, Axisys-0.1f, Axiszs-1.9f );
	glEnd();

	glPushMatrix();
	glScalef(0.3,0.3,0.3);
	//��ת
	glRotatef(m_yaw,0.0,1.0,0.0);
	glRotatef(m_pitch,0.0,0.0,1.0);
	glRotatef(m_roll,1.0,0.0,0.0);
	//ƽ��
	glTranslatef(-m_z*Shift_ratio,m_x*Shift_ratio,-m_y*Shift_ratio);
	//���ƶ���ı���
	glBegin (GL_QUADS); 
	//��1����
	glColor3fv(c1);
	glNormal3fv(m1);
	glVertex3fv(sp1);
	glVertex3fv(sp2);
	glVertex3fv(sp3);
	glVertex3fv(sp4);
	//��2����
	glColor3fv(c1);
	glNormal3fv(m2);
	glVertex3fv(sp5);
	glVertex3fv(sp6);
	glVertex3fv(sp7);
	glVertex3fv(sp8);
	//��3����
	glColor3fv(c2);
	glNormal3fv(m3);
	glVertex3fv(sp2);
	glVertex3fv(sp3);
	glVertex3fv(sp6);
	glVertex3fv(sp7);
	//��4����
	glColor3fv(c2);
	glNormal3fv(m4);
	glVertex3fv(sp1);
	glVertex3fv(sp4);
	glVertex3fv(sp5);
	glVertex3fv(sp8);
	//��5����
	glColor3fv(c3);
	glNormal3fv(m5);
	glVertex3fv(sp3);
	glVertex3fv(sp4);
	glVertex3fv(sp5);
	glVertex3fv(sp6);
	//��6����
	glColor3fv(c3);
	glNormal3fv(m6);
	glVertex3fv(sp1);
	glVertex3fv(sp2);
	glVertex3fv(sp7);
	glVertex3fv(sp8);
	glEnd();
	//////����������
	glBegin(GL_LINES);
	glColor3f(1.0f,0.0f,1.0f);
	// x-axis
    glVertex3f( 0.0f, 0.0f, 0.0f);
    glVertex3f( 14.0f, 0.0f, 0.0f);
	//y-axis
	glVertex3f( 0.0f, 0.0f, 0.0f);
    glVertex3f( 0.0f, 3.0f, 0.0f);
	//z-axis
	glVertex3f( 0.0f, 0.0f, 0.0f );
    glVertex3f( 0.0f, 0.0f, 9.0f );
	glEnd();
	glPopMatrix();
	glFlush();
	SwapBuffers(hrenderDC); 
} 
LPTHREAD_START_ROUTINE startlop(void*)
{

 while (bInLoop)//�߼�����
 {

 // MessageBoxW(NULL,L"hello",L"test",MB_OK);
  mySerialPort.OpenListenThread();
  //xx=resX;

  Sleep(1000);
 }
 return 0;
}
void CMyDialogDlg::OnBnClickedStart()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_START)->EnableWindow(FALSE);	
	DWORD ThreadID1,ThreadID2;
	DWORD code1,code2;
	//�����̲߳���
	thread1.progress=&m_progress;//����������
	thread1.speed=60;//�ٶ�
	thread1.pos=0;//��ʼλ��
	if(!GetExitCodeThread(hThread1,&code1)||(code1!=STILL_ACTIVE))
	{
		hThread1=CreateThread(NULL,0,ThreadFun,&thread1,0,&ThreadID1);//��������ʼ�߳�
	}
	//if(!GetExitCodeThread(hThread2,&code2)||(code2!=STILL_ACTIVE))//����Ҫ��
	//{
	//	hThread2=CreateThread(NULL,0,ThreadFun2,NULL,0,&ThreadID2);//��������ʼ�߳�
	//}
	//CSerialPort mySerialPort;
	if (!mySerialPort.InitPort(7))//�����޸� if(!mySerialPort.InitPort(4))
	{
		str1="initPort fail !";
		flag1=false;
	}
	else
	{
		str1="initPort success !";
		flag1=true;
	}

	if (!mySerialPort.OpenListenThread())
	{
		str2="OpenListenThread fail !";
		flag2=false;
	}
	else
	{
		str2="OpenListenThread success !";
		flag2=true;
	}
	if(flag1&&flag2)
		flag=true;
	else
		flag=false;
	MessageBox(str1+"\n"+str2);
	//���������߳�
	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)startlop,NULL,0,0);

	SetTimer(1,20,0);
}

void CMyDialogDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(flag&&flag_p)//flag_p��ʾ�����ʼ���Ƿ�ɹ��� flag��ʾ���ݳ���ʱ��׼���ɹ�
	{
		DWORD code;
		if(GetExitCodeThread(hThread1,&code))
			if(code==STILL_ACTIVE)									//�����ǰ�̻߳��
			{
				TerminateThread(hThread1,0);						//ǰЩ��ֹ�߳�
				CloseHandle(hThread1);								//�����߳̾��
			}
		//����Ҫ��
		//DWORD code1;
		//if(GetExitCodeThread(hThread2,&code1))
		//	if(code1==STILL_ACTIVE)									//�����ǰ�̻߳��
		//	{
		//		TerminateThread(hThread2,0);						//ǰЩ��ֹ�߳�
		//		CloseHandle(hThread2);								//�����߳̾��
		//	}
		GetDlgItem(IDC_PROGRESS1)->ShowWindow(FALSE);//ȡ����������ʾ
		RenderScene(); 
		/************************************************************************
		/*�˴����ӳ���
		*m_pitch:�����Ǹ�ֵ
		*m_roll�������Ǹ�ֵ
		*m_yaw��ƫ���Ǹ�ֵ
		*m_x:ģ������x����
		*m_y:ģ������y����
		*m_z:ģ������z����
		*Example:*/
		         m_pitch=restheta2;//(��)
		         m_roll=resphi;
		         m_yaw=-respsi;
		         m_x=0;//(cm)
		         m_y=0;
		         m_z=0;	
		         
				 m_pitch1=resphi_2;
				 m_pitch2=restheta2_2;
				 m_pitch3=respsi_2;
				 m_pitch4=resX_2;
				 m_pitch5=resY_2;
				 m_pitch6=resZ_2;


		//m_pitch+=5;
		//m_roll+=3;
		//m_yaw+=2;
		//m_x+=5;//(cm)
		//m_y-=5;
		//m_z+=1;
		

	}
	UpdateData(FALSE);
	CDialogEx::OnTimer(nIDEvent);
}


HBRUSH CMyDialogDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	//CFont m_font;
	//m_font.CreatePointFont(200,"����");
	//if((pWnd->GetDlgCtrlID()==IDC_EDITX)||(pWnd->GetDlgCtrlID()==IDC_EDITY)||(pWnd->GetDlgCtrlID()==IDC_EDITZ)
	//	||(pWnd->GetDlgCtrlID()==IDC_EDIT_PITCH)||(pWnd->GetDlgCtrlID()==IDC_EDIT_YAW)||(pWnd->GetDlgCtrlID()==IDC_EDIT_ROLL))
	//{
	//	pDC->SelectObject(&m_font);
	//}
	//// TODO:  Return a different brush if the default is not desired
	return hbr;
}
