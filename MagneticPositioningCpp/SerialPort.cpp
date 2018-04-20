#include "StdAfx.h"
#include "SerialPort.h"
#include <process.h>
#include <iostream>
#include <string>
#include "valchange.h"

double resX;
double resY;
double resZ;
double respsi, resphi, restheta2;
double resX_2;
double resY_2;
double resZ_2;
double respsi_2, resphi_2, restheta2_2;
using namespace std;
/** �߳��˳���־ */
bool CSerialPort::s_bExit = false;
/** ������������ʱ,sleep���´β�ѯ����ʱ��,��λ:�� */
const UINT SLEEP_TIME_INTERVAL = 5;

CSerialPort::CSerialPort(void)
	: m_hListenThread(INVALID_HANDLE_VALUE)
{
	m_hComm = INVALID_HANDLE_VALUE;
	m_hListenThread = INVALID_HANDLE_VALUE;

	InitializeCriticalSection(&m_csCommunicationSync);
}

CSerialPort::~CSerialPort(void)
{
	CloseListenTread();
	ClosePort();
	DeleteCriticalSection(&m_csCommunicationSync);
}

bool CSerialPort::InitPort(UINT portNo /*= 1*/, UINT baud /*= CBR_9600*/, char parity /*= 'N'*/,
						   UINT databits /*= 8*/, UINT stopsbits /*= 1*/, DWORD dwCommEvents /*= EV_RXCHAR*/)
{

	/** ��ʱ����,���ƶ�����ת��Ϊ�ַ�����ʽ,�Թ���DCB�ṹ */
	char szDCBparam[50];
	sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopsbits);

	/** ��ָ������,�ú����ڲ��Ѿ����ٽ�������,�����벻Ҫ�ӱ��� */
	if (!openPort(portNo)) //ָ�����ڲ���û�����ݣ�
	{
		return false;
	}

	/** �����ٽ�� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �Ƿ��д����� */
	BOOL bIsSuccess = TRUE;

	/** �ڴ˿���������������Ļ�������С,���������,��ϵͳ������Ĭ��ֵ.
	 *  �Լ����û�������Сʱ,Ҫע�������Դ�һЩ,���⻺�������
	 */
	/*if (bIsSuccess )
	{
		bIsSuccess = SetupComm(m_hComm,10,10);
	}*/

	/** ���ô��ڵĳ�ʱʱ��,����Ϊ0,��ʾ��ʹ�ó�ʱ���� */
	COMMTIMEOUTS CommTimeouts;
	CommTimeouts.ReadIntervalTimeout = 0;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.ReadTotalTimeoutConstant = 0;
	CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	CommTimeouts.WriteTotalTimeoutConstant = 0;
	if (bIsSuccess)
	{
		bIsSuccess = SetCommTimeouts(m_hComm, &CommTimeouts);
	}

	DCB dcb;
	if (bIsSuccess)
	{
		// ��ANSI�ַ���ת��ΪUNICODE�ַ���
		DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, NULL, 0);
		wchar_t *pwText = new wchar_t[dwNum];
		if (!MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, pwText, dwNum))
		{
			bIsSuccess = TRUE;
		}

		/** ��ȡ��ǰ�������ò���,���ҹ��촮��DCB���� */
		bIsSuccess = GetCommState(m_hComm, &dcb) && BuildCommDCBW(pwText, &dcb);
		/** ����RTS flow���� */
		dcb.fRtsControl = RTS_CONTROL_ENABLE;

		/** �ͷ��ڴ�ռ� */
		delete[] pwText;
	}

	if (bIsSuccess)
	{
		/** ʹ��DCB�������ô���״̬ */
		bIsSuccess = SetCommState(m_hComm, &dcb);
	}

	/**  ��մ��ڻ����� */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/** �뿪�ٽ�� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return bIsSuccess == TRUE;
}

bool CSerialPort::InitPort(UINT portNo, const LPDCB &plDCB)
{
	/** ��ָ������,�ú����ڲ��Ѿ����ٽ�������,�����벻Ҫ�ӱ��� */
	if (!openPort(portNo))
	{
		return false;
	}

	/** �����ٽ�� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** ���ô��ڲ��� */
	if (!SetCommState(m_hComm, plDCB))
	{
		return false;
	}

	/**  ��մ��ڻ����� */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/** �뿪�ٽ�� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

void CSerialPort::ClosePort()
{
	/** ����д��ڱ��򿪣��ر��� */
	if (m_hComm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
	}
}

bool CSerialPort::openPort(UINT portNo)
{
	/** �����ٽ�� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �Ѵ��ڵı��ת��Ϊ�豸�� */
	char szPort[50];
	sprintf_s(szPort, "COM%d", portNo);

	/** ��ָ���Ĵ��� */
	m_hComm = CreateFileA(szPort,						/** �豸��,COM1,COM2�� */
						  GENERIC_READ | GENERIC_WRITE, /** ����ģʽ,��ͬʱ��д */
						  0,							/** ����ģʽ,0��ʾ������ */
						  NULL,							/** ��ȫ������,һ��ʹ��NULL */
						  OPEN_EXISTING,				/** �ò�����ʾ�豸�������,���򴴽�ʧ�� */
						  0,
						  0);

	/** �����ʧ�ܣ��ͷ���Դ������ */
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		LeaveCriticalSection(&m_csCommunicationSync);
		return false;
	}

	/** �˳��ٽ��� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

bool CSerialPort::OpenListenThread()
{
	/** ����߳��Ƿ��Ѿ������� */
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** �߳��Ѿ����� */
		return false;
	}

	s_bExit = false;
	/** �߳�ID */
	UINT threadId;
	/** ��������ݼ����߳� */
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);
	if (!m_hListenThread)
	{
		return false;
	}
	/** �����̵߳����ȼ�,������ͨ�߳� */
	if (!SetThreadPriority(m_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))
	{
		return false;
	}

	return true;
}

bool CSerialPort::CloseListenTread()
{
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** ֪ͨ�߳��˳� */
		s_bExit = true;

		/** �ȴ��߳��˳� */
		Sleep(10);

		/** ���߳̾����Ч */
		CloseHandle(m_hListenThread);
		m_hListenThread = INVALID_HANDLE_VALUE;
	}
	return true;
}

UINT CSerialPort::GetBytesInCOM()
{
	DWORD dwError = 0; /** ������ */
	COMSTAT comstat;   /** COMSTAT�ṹ��,��¼ͨ���豸��״̬��Ϣ */
	memset(&comstat, 0, sizeof(COMSTAT));

	UINT BytesInQue = 0;
	/** �ڵ���ReadFile��WriteFile֮ǰ,ͨ�������������ǰ����Ĵ����־ */
	if (ClearCommError(m_hComm, &dwError, &comstat))
	{
		BytesInQue = comstat.cbInQue; /** ��ȡ�����뻺�����е��ֽ��� */
	}

	return BytesInQue;
}

UINT WINAPI CSerialPort::ListenThread(void *pParam)
{
	unsigned char status = 0;
	char flag = 0;
	unsigned char d = 0;
	unsigned char flag2 = 3;
	unsigned char a92 = 0;
	unsigned char a93 = 0;
	unsigned char a94 = 0;
	unsigned char arry[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int i = 0;
	int N = 0;
	int number = 0;
	double XX = 0;
	double XY = 0;
	double XZ = 0;
	double YX = 0;
	double YY = 0;
	double YZ = 0;
	double ZX = 0;
	double ZY = 0;
	double ZZ = 0;
	double Xx, Xy, Xz;
	double Yx, Yy, Yz;
	double Zx, Zy, Zz;
	double Px, Py, Pz, sumpxpypz;
	double sn_arfa, cn_arfa, rho, arfa, betia;
	double f33, ff, f23, f13;
	double A1, A2, A3, X1, Y1, Z1;
	double rho1, rho2, A11a, A11b, A11c, A11, A12a, A12b, A12c, A12, A13a, A13b, A13c, A13, A23a, A23b, A23c, A23, A33a, A33b, A33c, A33;
	double psi_1, phi_1, theta_2;
	double arfa1, betia1;
	double AA[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	int flag3 = 0;
	int N2 = 0;
	double NN;
	double SXx1 = 0;
	double SYx1 = 0;
	double SZx1 = 0;
	double SXy1 = 0;
	double SYy1 = 0;
	double SZy1 = 0;
	double SXz1 = 0;
	double SYz1 = 0;
	double SZz1 = 0;
	double Xxarry[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	double Yxarry[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	double Zxarry[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	double Xyarry[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	double Yyarry[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	double Zyarry[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	double Xzarry[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	double Yzarry[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	double Zzarry[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	double retryarry[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	double retryAA135[] = {0.8618, 0.0301, 0.0646, -0.0080, 1.0461, 0.0602, -0.1231, -0.0442, 0.9170};
	double retryAA315[] = {0.8297, 0.0023, -0.0257, 0.0511, 1.0394, 0.0232, 0.0263, -0.0154, 0.9292};
	double retryAA270[] = {0.8388, -0.0747, -0.0732, -0.0861, 1.0801, 0.0464, 0.0570, -0.0165, 0.9408};
	double retryAA90[] = {0.8323, -0.1342, -0.0727, -0.0742, 1.0793, -0.0660, 0.0357, 0.0613, 0.9183};
	double retryAA45[] = {0.9160, -0.1707, -0.0307, -0.0669, 0.9712, -0.0570, 0.0121, 0.0550, 0.9148};
	double retryAA225[] = {0.9418, -0.0946, -0.1446, -0.0771, 0.9771, 0.0329, 0.0817, 0.0007, 0.9354};

	double Xarry[xarrylength + 1];
	double Yarry[yarrylength + 1];
	double Zarry[zarrylength + 1];

	double psiarry[xarrylength + 1];
	double phiarry[yarrylength + 1];
	double thetaarry[zarrylength + 1];

	int ix;
	int iy;
	int iz;
	int ipsi;
	int iphi;
	int itheta;

	int N3 = 0;
	;
	int flagx = 0;
	int flagy = 0;
	int flagz = 0;
	int flagpsi = 0;
	int flagphi = 0;
	int flagtheta = 0;

	double kgx, kgy, kgz;
	double kgpsi, kgphi, kgtheta;

	double X_2, Y_2, Z_2;

	double varX, varY, varZ;
	double varpsi, varphi, vartheta;

	double meanpsi, meanphi, meantheta;
	double meanX, meanY, meanZ;

	double psi_2, phi_2, theta_22;
	AA[0] = 1.0391;
	AA[1] = -0.0763;
	AA[2] = 0.1064;
	AA[3] = 0.1638;
	AA[4] = 1.0822;
	AA[5] = -0.2057;
	AA[6] = -0.0612;
	AA[7] = -0.0599;
	AA[8] = 1.3279;
	/** �õ������ָ�� */
	CSerialPort *pSerialPort = reinterpret_cast<CSerialPort *>(pParam);

	// �߳�ѭ��,��ѯ��ʽ��ȡ��������
	while (!pSerialPort->s_bExit)
	{
		UINT BytesInQue = pSerialPort->GetBytesInCOM();
		/** ����������뻺������������,����Ϣһ���ٲ�ѯ */
		if (BytesInQue == 0)
		{
			Sleep(SLEEP_TIME_INTERVAL);
			continue;
		}

		/** ��ȡ���뻺�����е����ݲ������ʾ */
		//char cRecved = 0x00;
		unsigned char aa;
		aa = 127;
		unsigned char cRecved;
		do
		{
			cRecved = 0;
			if (pSerialPort->ReadChar(cRecved) == true)
			{

				/*resX=(double)cRecved;
				   resY=(double)cRecved;
				   resZ=(double)cRecved;*/

				//     aa=cRecved;
				int i;
				for (i = 0; i < 13; i++)
				{
					arry[i] = arry[i + 1];
				} //for
				arry[12] = cRecved;
				N = N + 1;
				if (N == 13)
				{
					N = 12;
					d = arry[0];
					if (d == 127) //HexToDec:16����ת10����
					{
						flag = 1;
					}
					if (flag == 1)
					{
						if (d == 146)
						{
							XX = (double)(arry[1] << 24) + (arry[2] << 16) + (arry[3] << 8) + arry[4];
							XY = (double)(arry[5] << 24) + (arry[6] << 16) + (arry[7] << 8) + arry[8];
							XZ = (double)(arry[9] << 24) + (arry[10] << 16) + (arry[11] << 8) + arry[12];
							flag = 0;
							a92 = 1;
						} //if 92
						if (d == 147)
						{
							YX = (double)(arry[1] << 24) + (arry[2] << 16) + (arry[3] << 8) + arry[4];
							YY = (double)(arry[5] << 24) + (arry[6] << 16) + (arry[7] << 8) + arry[8];
							YZ = (double)(arry[9] << 24) + (arry[10] << 16) + (arry[11] << 8) + arry[12];
							flag = 0;
							a93 = 1;

						} //if 93
						if (d == 148)
						{
							ZX = (double)(arry[1] << 24) + (arry[2] << 16) + (arry[3] << 8) + arry[4];
							ZY = (double)(arry[5] << 24) + (arry[6] << 16) + (arry[7] << 8) + arry[8];
							ZZ = (double)(arry[9] << 24) + (arry[10] << 16) + (arry[11] << 8) + arry[12];
							flag = 0;
							a94 = 1;

						} //if 94
						if (flag2 == (a92 + a93 + a94))
						{
							Xx = XX;
							Xy = YX;
							Xz = ZX;
							Yx = XY;
							Yy = YY;
							Yz = ZY;
							Zx = XZ;
							Zy = YZ;
							Zz = ZZ;

							/* Xx=FilterXx(Xx);Xy=FilterXy(Xy);Xz=Filter(Xz);
									 Yx=FilterYx(Yx);Yy=FilterYy(Yy);Yz=Filter(Yz);
									 Zx=FilterZx(Zx);Zy=FilterZy(Zy);Zz=Filter(Zz);*/

							Xx = Xx * AA[0] + Xy * AA[1] + Xz * AA[2];
							Yx = Yx * AA[0] + Yy * AA[1] + Yz * AA[2];
							Zx = Zx * AA[0] + Zy * AA[1] + Zz * AA[2];

							Xy = Xx * AA[3] + Xy * AA[4] + Xz * AA[5];
							Yy = Yx * AA[3] + Yy * AA[4] + Yz * AA[5];
							Zy = Zx * AA[3] + Zy * AA[4] + Zz * AA[5];

							Xz = Xx * AA[6] + Xy * AA[7] + Xz * AA[8];
							Yz = Yx * AA[6] + Yy * AA[7] + Yz * AA[8];
							Zz = Zx * AA[6] + Zy * AA[7] + Zz * AA[8];

							/*								retryarry[0]=Xx;retryarry[1]=Xy;retryarry[2]=Xz;
									retryarry[3]=Yx;retryarry[4]=Yy;retryarry[5]=Yz;
									retryarry[6]=Zx;retryarry[7]=Zy;retryarry[8]=Zz;
*/
							Px = SumSquare(Xx, Xy, Xz);
							Py = SumSquare(Yx, Yy, Yz);
							Pz = SumSquare(Zx, Zy, Zz);
							sumpxpypz = Px + Py + Pz;

							//	double xx=0.9;
							/*		double yy=0.4;
									double zz=0.325;
									double cc;
									cc=pow((pow(0.9,2)+pow(0.4,2)+pow(0.325,2)),0.5);
									cc=pow((0.66666667*sumpxpypz*pow(cc,6)),0.5);*/

							rho = 1.5 * constant * constant / sumpxpypz;
							rho = pow(rho, 0.166667);
							A1 = abss(1.1111111 * Px - 0.22222222 * Py - 0.222222222 * Pz);
							A2 = abss(1.1111111 * Py - 0.22222222 * Px - 0.2222222 * Pz);
							A3 = abss(1.1111111 * Pz - 0.22222222 * Px - 0.222222 * Py);

							X1 = (pow(rho, 4) / constant) * pow(A1, 0.5);
							Y1 = (pow(rho, 4) / constant) * pow(A2, 0.5);
							Z1 = (pow(rho, 4) / constant) * pow(A3, 0.5);

							f33 = 6 * (Zx * Zx + Zy * Zy + Zz * Zz);
							f33 = f33 / sumpxpypz;
							ff = f33 - 1.0;
							ff = abss(ff) / 3.0;
							ff = pow(ff, 0.50);

							betia = asin(ff) / pi * 180;
							f23 = 6 * (Zx * Yx + Zy * Yy + Zz * Yz);
							f23 = f23 / sumpxpypz;
							f13 = 6 * (Zx * Xx + Zy * Xy + Zz * Xz);
							f13 = f13 / sumpxpypz;
							sn_arfa = -f23 / (3 * sin(betia / 180.0 * pi) * cos(betia / 180.0 * pi));
							cn_arfa = -f13 / (3 * sin(betia / 180.0 * pi) * cos(betia / 180.0 * pi));

							rho1 = X1 * X1 + Y1 * Y1;
							rho1 = pow(rho1, 0.5);
							rho2 = X1 * X1 + Y1 * Y1 + Z1 * Z1;
							rho2 = pow(rho2, 0.5);
							arfa1 = acos(X1 / rho1) / pi * 180;
							betia1 = acos(rho1 / rho2);
							//X1=Filter(X1);
							/*NN=10;
									X1=kalman(X1,0.0001,NN,1)*100.0;
									Y1=kalman(Y1,0.0001,NN,2)*100.0;
									Z1=kalman(Z1,0.00001,NN,3)*100.0;*/

							if ((sn_arfa > 0) && (cn_arfa > 0))
							{
								/*   resX=X1;
									        resY=Y1;
									        resZ=-Z1;*/
								X1 = X1;
								Y1 = Y1;
								Z1 = -Z1;
								arfa = arfa1;
							}
							if ((sn_arfa > 0) && (cn_arfa < 0))
							{ /*
										  resX=-X1;
									      resY=Y1;
									      resZ=-Z1;*/
								X1 = -X1;
								Y1 = Y1;
								Z1 = -Z1;
								arfa = 180.0 - arfa1;
							}
							if ((sn_arfa < 0) && (cn_arfa < 0))
							{
								/*  resX=-X1;
									      resY=-Y1;
									      resZ=-Z1;*/
								X1 = -X1;
								Y1 = -Y1;
								Z1 = -Z1;
								arfa = arfa1 + 180.0;
							}
							if ((sn_arfa < 0) && (cn_arfa > 0))
							{
								/*  resX=X1;
									      resY=-Y1;
									      resZ=-Z1;*/
								X1 = X1;
								Y1 = -Y1;
								Z1 = -Z1;
								arfa = 360.0 - arfa1;
							}
							arfa1 = arfa / 180 * pi;
							A11a = (Yx * pow(rho, 3) * (cos(arfa1) * pow(cos(betia1), 2) * sin(arfa1) + 2 * cos(arfa1) * pow(cos(betia1), 4) * sin(arfa1) - 2 * cos(arfa1) * sin(arfa1) * pow(sin(betia1), 2) + 2 * cos(arfa1) * sin(arfa1) * pow(sin(betia1), 4) + 4 * cos(arfa1) * pow(cos(betia1), 2) * sin(arfa1) * pow(sin(betia1), 2))) / (constant * ((pow(cos(arfa1), 4) * pow(cos(betia1), 4) + pow(cos(arfa1), 4) * pow(sin(betia1), 4) + pow(cos(betia1), 4) * pow(sin(arfa1), 4) + pow(sin(arfa1), 4) * pow(sin(betia1), 4) + 2 * pow(cos(arfa1), 2) * pow(cos(betia1), 4) * pow(sin(arfa1), 2) + 2 * pow(cos(arfa1), 4) * pow(cos(betia1), 2) * pow(sin(betia1), 2) + 2 * pow(cos(arfa1), 2) * pow(sin(arfa1), 2) * pow(sin(betia1), 4) + 2 * pow(cos(betia1), 2) * pow(sin(arfa1), 4) * pow(sin(betia1), 2) + 4 * pow(cos(arfa1), 2) * pow(cos(betia1), 2) * pow(sin(arfa1), 2) * pow(sin(betia1), 2))));
							A11b = (Xx * pow(rho, 3) * (2 * pow(cos(arfa1), 2) * pow(sin(betia1), 2) - pow(cos(arfa1), 2) * pow(cos(betia1), 2) + 2 * pow(cos(betia1), 4) * pow(sin(arfa1), 2) + 2 * pow(sin(arfa1), 2) * pow(sin(betia1), 4) + 4 * pow(cos(betia1), 2) * pow(sin(arfa1), 2) * pow(sin(betia1), 2))) / (constant * ((pow(cos(arfa1), 4) * pow(cos(betia1), 4) + pow(cos(arfa1), 4) * pow(sin(betia1), 4) + pow(cos(betia1), 4) * pow(sin(arfa1), 4) + pow(sin(arfa1), 4) * pow(sin(betia1), 4) + 2 * pow(cos(arfa1), 2) * pow(cos(betia1), 4) * pow(sin(arfa1), 2) + 2 * pow(cos(arfa1), 4) * pow(cos(betia1), 2) * pow(sin(betia1), 2) + 2 * pow(cos(arfa1), 2) * pow(sin(arfa1), 2) * pow(sin(betia1), 4) + 2 * pow(cos(betia1), 2) * pow(sin(arfa1), 4) * pow(sin(betia1), 2) + 4 * pow(cos(arfa1), 2) * pow(cos(betia1), 2) * pow(sin(arfa1), 2) * pow(sin(betia1), 2))));
							A11c = (3 * Zx * pow(rho, 3) * cos(arfa1) * cos(betia1) * sin(betia1)) / (constant * ((pow(cos(arfa1), 2) * pow(cos(betia1), 4) + pow(cos(arfa1), 2) * pow(sin(betia1), 4) + pow(cos(betia1), 4) * pow(sin(arfa1), 2) + pow(sin(arfa1), 2) * pow(sin(betia1), 4) + 2 * pow(cos(arfa1), 2) * pow(cos(betia1), 2) * pow(sin(betia1), 2) + 2 * pow(cos(betia1), 2) * pow(sin(arfa1), 2) * pow(sin(betia1), 2))));
							A11 = A11a - A11b - A11c;
							A12a = (Xx * pow(rho, 3) * (cos(arfa1) * pow(cos(betia1), 2) * sin(arfa1) + 2 * cos(arfa1) * pow(cos(betia1), 4) * sin(arfa1) - 2 * cos(arfa1) * sin(arfa1) * pow(sin(betia1), 2) + 2 * cos(arfa1) * sin(arfa1) * pow(sin(betia1), 4) + 4 * cos(arfa1) * pow(cos(betia1), 2) * sin(arfa1) * pow(sin(betia1), 2))) / (constant * ((pow(cos(arfa1), 4) * pow(cos(betia1), 4) + pow(cos(arfa1), 4) * pow(sin(betia1), 4) + pow(cos(betia1), 4) * pow(sin(arfa1), 4) + pow(sin(arfa1), 4) * pow(sin(betia1), 4) + 2 * pow(cos(arfa1), 2) * pow(cos(betia1), 4) * pow(sin(arfa1), 2) + 2 * pow(cos(arfa1), 4) * pow(cos(betia1), 2) * pow(sin(betia1), 2) + 2 * pow(cos(arfa1), 2) * pow(sin(arfa1), 2) * pow(sin(betia1), 4) + 2 * pow(cos(betia1), 2) * pow(sin(arfa1), 4) * pow(sin(betia1), 2) + 4 * pow(cos(arfa1), 2) * pow(cos(betia1), 2) * pow(sin(arfa1), 2) * pow(sin(betia1), 2))));
							A12b = (Yx * pow(rho, 3) * (2 * pow(cos(arfa1), 2) * pow(cos(betia1), 4) - pow(cos(betia1), 2) * pow(sin(arfa1), 2) + 2 * pow(cos(arfa1), 2) * pow(sin(betia1), 4) + 2 * pow(sin(arfa1), 2) * pow(sin(betia1), 2) + 4 * pow(cos(arfa1), 2) * pow(cos(betia1), 2) * pow(sin(betia1), 2))) / (constant * ((pow(cos(arfa1), 4) * pow(cos(betia1), 4) + pow(cos(arfa1), 4) * pow(sin(betia1), 4) + pow(cos(betia1), 4) * pow(sin(arfa1), 4) + pow(sin(arfa1), 4) * pow(sin(betia1), 4) + 2 * pow(cos(arfa1), 2) * pow(cos(betia1), 4) * pow(sin(arfa1), 2) + 2 * pow(cos(arfa1), 4) * pow(cos(betia1), 2) * pow(sin(betia1), 2) + 2 * pow(cos(arfa1), 2) * pow(sin(arfa1), 2) * pow(sin(betia1), 4) + 2 * pow(cos(betia1), 2) * pow(sin(arfa1), 4) * pow(sin(betia1), 2) + 4 * pow(cos(arfa1), 2) * pow(cos(betia1), 2) * pow(sin(arfa1), 2) * pow(sin(betia1), 2))));
							A12c = (3 * Zx * pow(rho, 3) * cos(betia1) * sin(arfa1) * sin(betia1)) / (constant * ((pow(cos(arfa1), 2) * pow(cos(betia1), 4) + pow(cos(arfa1), 2) * pow(sin(betia1), 4) + pow(cos(betia1), 4) * pow(sin(arfa1), 2) + pow(sin(arfa1), 2) * pow(sin(betia1), 4) + 2 * pow(cos(arfa1), 2) * pow(cos(betia1), 2) * pow(sin(betia1), 2) + 2 * pow(cos(betia1), 2) * pow(sin(arfa1), 2) * pow(sin(betia1), 2))));
							A12 = A12a - A12b - A12c;
							A23a = -(Zy * pow(rho, 3) * (2 * pow(cos(betia1), 2) - pow(sin(betia1), 2))) / (constant * ((pow(cos(betia1), 4) + pow(sin(betia1), 4) + 2 * pow(cos(betia1), 2) * pow(sin(betia1), 2))));
							A23b = (3 * Xy * pow(rho, 3) * cos(arfa1) * cos(betia1) * sin(betia1)) / (constant * ((pow(cos(arfa1), 2) * pow(cos(betia1), 4) + pow(cos(arfa1), 2) * pow(sin(betia1), 4) + pow(cos(betia1), 4) * pow(sin(arfa1), 2) + pow(sin(arfa1), 2) * pow(sin(betia1), 4) + 2 * pow(cos(arfa1), 2) * pow(cos(betia1), 2) * pow(sin(betia1), 2) + 2 * pow(cos(betia1), 2) * pow(sin(arfa1), 2) * pow(sin(betia1), 2))));
							A23c = (3 * Yy * pow(rho, 3) * cos(betia1) * sin(arfa1) * sin(betia1)) / (constant * ((pow(cos(arfa1), 2) * pow(cos(betia1), 4) + pow(cos(arfa1), 2) * pow(sin(betia1), 4) + pow(cos(betia1), 4) * pow(sin(arfa1), 2) + pow(sin(arfa1), 2) * pow(sin(betia1), 4) + 2 * pow(cos(arfa1), 2) * pow(cos(betia1), 2) * pow(sin(betia1), 2) + 2 * pow(cos(betia1), 2) * pow(sin(arfa1), 2) * pow(sin(betia1), 2))));
							A23 = A23a - A23b - A23c;
							A33a = -(Zz * pow(rho, 3) * (2 * pow(cos(betia1), 2) - pow(sin(betia1), 2))) / (constant * ((pow(cos(betia1), 4) + pow(sin(betia1), 4) + 2 * pow(cos(betia1), 2) * pow(sin(betia1), 2))));
							A33b = (3 * Xz * pow(rho, 3) * cos(arfa1) * cos(betia1) * sin(betia1)) / (constant * ((pow(cos(arfa1), 2) * pow(cos(betia1), 4) + pow(cos(arfa1), 2) * pow(sin(betia1), 4) + pow(cos(betia1), 4) * pow(sin(arfa1), 2) + pow(sin(arfa1), 2) * pow(sin(betia1), 4) + 2 * pow(cos(arfa1), 2) * pow(cos(betia1), 2) * pow(sin(betia1), 2) + 2 * pow(cos(betia1), 2) * pow(sin(arfa1), 2) * pow(sin(betia1), 2))));
							A33c = (3 * Yz * pow(rho, 3) * cos(betia1) * sin(arfa1) * sin(betia1)) / (constant * ((pow(cos(arfa1), 2) * pow(cos(betia1), 4) + pow(cos(arfa1), 2) * pow(sin(betia1), 4) + pow(cos(betia1), 4) * pow(sin(arfa1), 2) + pow(sin(arfa1), 2) * pow(sin(betia1), 4) + 2 * pow(cos(arfa1), 2) * pow(cos(betia1), 2) * pow(sin(betia1), 2) + 2 * pow(cos(betia1), 2) * pow(sin(arfa1), 2) * pow(sin(betia1), 2))));
							A33 = A33a - A33b - A33c;
							A13a = -(Zx * pow(rho, 3) * (2 * pow(cos(betia1), 2) - pow(sin(betia1), 2))) / (constant * ((pow(cos(betia1), 4) + pow(sin(betia1), 4) + 2 * pow(cos(betia1), 2) * pow(sin(betia1), 2))));
							A13b = (3 * Xx * pow(rho, 3) * cos(arfa1) * cos(betia1) * sin(betia1)) / (constant * ((pow(cos(arfa1), 2) * pow(cos(betia1), 4) + pow(cos(arfa1), 2) * pow(sin(betia1), 4) + pow(cos(betia1), 4) * pow(sin(arfa1), 2) + pow(sin(arfa1), 2) * pow(sin(betia1), 4) + 2 * pow(cos(arfa1), 2) * pow(cos(betia1), 2) * pow(sin(betia1), 2) + 2 * pow(cos(betia1), 2) * pow(sin(arfa1), 2) * pow(sin(betia1), 2))));
							A13c = (3 * Yx * pow(rho, 3) * cos(betia1) * sin(arfa1) * sin(betia1)) / (constant * ((pow(cos(arfa1), 2) * pow(cos(betia1), 4) + pow(cos(arfa1), 2) * pow(sin(betia1), 4) + pow(cos(betia1), 4) * pow(sin(arfa1), 2) + pow(sin(arfa1), 2) * pow(sin(betia1), 4) + 2 * pow(cos(arfa1), 2) * pow(cos(betia1), 2) * pow(sin(betia1), 2) + 2 * pow(cos(betia1), 2) * pow(sin(arfa1), 2) * pow(sin(betia1), 2))));
							A13 = A13a - A13b - A13c;
							psi_1 = atan(abss(A12 / A11)) / pi * 180;
							phi_1 = atan(abss(A23 / A33)) / pi * 180;
							theta_2 = -asin(A13) / pi * 180;
							flag3 = 0;
							//restheta2=theta_2;
							if ((A12 > 0) && (A11 > 0))
							{
								//respsi=psi_1;
								psi_1 = psi_1;
							}
							if ((A12 > 0) && (A11 < 0))
							{
								//respsi=180.0-psi_1;
								psi_1 = 180.0 - psi_1;
								//flag_p=1;
							}
							if ((A12 < 0) && (A11 < 0))
							{
								//respsi=psi_1+180.0;
								psi_1 = psi_1 + 180.0;
							}
							if ((A12 < 0) && (A11 > 0))
							{
								//respsi=360.0-psi_1;
								psi_1 = 360 - psi_1;
							}
							if ((A23 > 0) && (A33 > 0))
							{
								// resphi=phi_1;
								phi_1 = phi_1;
							}
							if ((A23 > 0) && (A33 < 0))
							{
								//  resphi=180.0-phi_1;
								phi_1 = 180.0 - phi_1;
							}
							if ((A23 < 0) && (A33 < 0))
							{
								//  resphi=phi_1+180.0;
								phi_1 = 180.0 + phi_1;
							}
							if ((A23 < 0) && (A33 > 0))
							{
								//   resphi=360.0-phi_1;
								phi_1 = 360 - phi_1;
							}
							/*  if(psi_1>=355.0)
		   {
			   psi_1=0;
		   }*/
							/*   if (phi_1>=350.0)
		   {
			   phi_1=0;
		   }*/
							psi_1 = Filterpsi(psi_1);
							phi_1 = Filterphi(phi_1);
							//phi_1=Filterphi2(phi_1);
							theta_2 = Filtertheta(theta_2);

							respsi = psi_1;
							resphi = phi_1;
							restheta2 = theta_2;
							respsi_2 = psi_1;
							resphi_2 = phi_1;
							restheta2_2 = theta_2;

							/*if ((psi_1>10.0)&&(psi_1<59.0))
		   {
			   retry(retryarry,10,retryAA45);
			   flag3=1;
		   }
		   if ((psi_1>115.0)&&(psi_1<120.0))
		   {
			   retry(retryarry,10,retryAA90);
			   flag3=1;
		   }
		    if ((psi_1>=117.5)&&(psi_1<180.0))
		   {
			   retry(retryarry,10,retryAA135);
			   flag3=1;
		   }
			if ((psi_1>=190.5)&&(psi_1<260.0))
		   {
			   retry(retryarry,10,retryAA225);
			   flag3=1;
		   }
		   if((psi_1>280.0)&&(psi_1<291.0))
		   {
			   retry(retryarry,10,retryAA270);
			   flag3=1;
		   }
		    if((psi_1>=291.0)&&(psi_1<360.0))
		   {
			   retry(retryarry,10,retryAA315);
			   flag3=1;
		   }*/
							X1 = X1 * 1000;
							Y1 = Y1 * 1000;
							Z1 = Z1 * 1000;
							X1 = Filterx(X1);
							Y1 = Filtery(Y1);
							Z1 = Filterz(Z1);

							if (flag3 == 0)
							{

								for (ix = 0; ix < xarrylength; ix++)
								{
									Xarry[ix] = Xarry[ix + 1]; // �����������ƣ���λ�Ե�
								}

								Xarry[xarrylength] = X1;

								for (iy = 0; iy < yarrylength; iy++)
								{
									Yarry[iy] = Yarry[iy + 1];
								}
								Yarry[yarrylength] = Y1;

								for (iz = 0; iz < zarrylength; iz++)
								{
									Zarry[iz] = Zarry[iz + 1];
								}
								Zarry[zarrylength] = Z1;

								for (ipsi = 0; ipsi < psiarrylength; ipsi++)
								{
									psiarry[ipsi] = psiarry[ipsi + 1]; // �����������ƣ���λ�Ե�
								}
								psiarry[psiarrylength] = psi_1;

								for (iphi = 0; iphi < phiarrylength; iphi++)
								{
									phiarry[iphi] = phiarry[iphi + 1]; // �����������ƣ���λ�Ե�
								}
								phiarry[phiarrylength] = phi_1;

								for (itheta = 0; itheta < thetaarrylength; itheta++)
								{
									thetaarry[iphi] = thetaarry[iphi + 1]; // �����������ƣ���λ�Ե�
								}
								thetaarry[thetaarrylength] = theta_2;
								N3++;
								//resX=(Xarry[1]+Xarry[2]+Xarry[3]+Xarry[4])/4.0;
								if (N3 == (xarrylength + 1))
								{
									varX = var(Xarry, 31);
									varY = var(Yarry, 31);
									varZ = var(Zarry, 31);
									varpsi = var(psiarry, 31);
									varphi = var(phiarry, 31);
									vartheta = var(thetaarry, 31);
									if (varX > 50.0)
									{
										flagx = 0;
										resX = X1;
										resX_2 = X1;
									}
									if (varX <= 50.0)
									{
										if (flagx == 0)
										{
											meanX = mean(Xarry, 31);
											flagx = 1;
										}
										if (flagx == 1)
										{
											kgx = mean(Xarry, 31);
											kgx = (kgx - meanX) * 0.1;
											X_2 = meanX + kgx;
											resX = X_2;
											resX_2 = X_2;
										} //flag4==1
									}	 //varX<4
										  //	N3=xarrylength;
									if (varY > 50.0)
									{
										flagy = 0;
										resY = Y1;
										resY_2 = Y1;
									}
									if (varY <= 50.0)
									{
										if (flagy == 0)
										{
											meanY = mean(Yarry, 31);
											flagy = 1;
										}
										if (flagy == 1)
										{
											kgy = mean(Yarry, 31);

											kgy = (kgy - meanY) * 0.1;

											Y_2 = meanY + kgy;
											resY = Y_2;
											resY_2 = Y_2;
										} //flag4==1
									}	 //varY<4
									if (varZ > 50.0)
									{
										flagz = 0;
										resZ = Z1;
										resZ_2 = Z1;
									}
									if (varZ <= 50.0)
									{
										if (flagz == 0)
										{
											meanZ = mean(Zarry, 31);
											//meanY=(Xarry[1]+Xarry[2]+Xarry[3]+Xarry[4])/4.0;
											flagz = 1;
										}
										if (flagz == 1)
										{
											kgz = mean(Zarry, 31);
											//kgy=(Xarry[1]+Xarry[2]+Xarry[3]+Xarry[4])/4.0;

											kgz = (abss(kgz) - abss(meanZ)) * 0.1;

											Z_2 = meanZ + kgz;
											resZ = Z_2;
											resZ_2 = Z_2;
										} //flag4==1
									}	 //var<20.0

									//if(varpsi>5.0)
									//{
									//	flagpsi = 0;
									//	respsi_2=psi_1;
									//}
									//if(varpsi<=5.0)
									//{
									//	if(flagpsi == 0)
									//	{
									//                           meanpsi = mean(psiarry,31);
									//		//meanY=(Xarry[1]+Xarry[2]+Xarry[3]+Xarry[4])/4.0;
									//	    flagpsi  = 1;
									//	}
									//	if(flagpsi == 1)
									//	{
									//		kgpsi = mean(psiarry,31);
									//		//kgy=(Xarry[1]+Xarry[2]+Xarry[3]+Xarry[4])/4.0;
									//
									//		kgpsi = (abss(kgpsi)-abss(meanpsi))*0.1;

									//		psi_2=meanpsi+kgpsi;
									//		respsi_2=psi_2;
									//	 }//flag4==1
									//}//var<20.0

									//if(varphi>5.0)
									//{
									//	flagphi = 0;
									//	resphi_2=phi_1;
									//}
									//if(varphi<=5.0)
									//{
									//	if(flagphi == 0)
									//	{
									//                           meanphi = mean(phiarry,31);
									//		//meanY=(Xarry[1]+Xarry[2]+Xarry[3]+Xarry[4])/4.0;
									//	    flagphi  = 1;
									//	}
									//	if(flagphi == 1)
									//	{
									//		kgphi = mean(phiarry,31);
									//		//kgy=(Xarry[1]+Xarry[2]+Xarry[3]+Xarry[4])/4.0;
									//
									//		kgphi = (abss(kgphi)-abss(meanphi))*0.1;

									//		phi_2=meanphi+kgphi;
									//		resphi_2=phi_2;
									//	 }//flag4==1
									//}//var<20.0

									//  if(vartheta>5.0)
									//{
									//	flagtheta = 0;
									//	restheta2_2=theta_2;
									//}
									//if(vartheta<=5.0)
									//{
									//	if(flagtheta == 0)
									//	{
									//                           meantheta = mean(thetaarry,31);
									//		//meanY=(Xarry[1]+Xarry[2]+Xarry[3]+Xarry[4])/4.0;
									//	    flagtheta  = 1;
									//	}
									//	if(flagtheta == 1)
									//	{
									//		kgtheta = mean(thetaarry,31);
									//		//kgy=(Xarry[1]+Xarry[2]+Xarry[3]+Xarry[4])/4.0;
									//
									//		kgtheta = (abss(kgtheta)-abss(meantheta))*0.1;

									//		theta_22=meantheta+kgtheta;
									//		restheta2_2=theta_22;
									//	 }//flag4==1
									//}//var<20.0
									//          psi_1=Filterpsi(psi_1);
									//	psi_2=kalman(psi_1,Q,R,4);
									//	respsi=psi_1;
									//	respsi_2=psi_2;

									//	phi_1=Filterphi(phi_1);
									//	phi_2=kalman(phi_1,Q,R,5);
									//	resphi=phi_1;
									//	resphi_2=phi_2;

									//	theta_2=Filtertheta(theta_2);
									//	theta_22=kalman(theta_2,Q,R,6);
									//	restheta2=theta_2;
									//	restheta2_2=theta_22;

									N3 = xarrylength;
								} // ifN3==?
							}
							flag3 = 0;
							a92 = 0;
							a93 = 0;
							a94 = 0;
						} //if flag2
					}	 //flag=1
				}		  //if N=13;
			}			  //if true
		} while (--BytesInQue);
	}
	return 0;
}

bool CSerialPort::ReadChar(unsigned char &cRecved)
{
	BOOL bResult = TRUE;
	DWORD BytesRead = 0;
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/** �ٽ������� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �ӻ�������ȡһ���ֽڵ����� */
	bResult = ReadFile(m_hComm, &cRecved, 1, &BytesRead, NULL);

	if ((!bResult))
	{
		/** ��ȡ������,���Ը��ݸô�����������ԭ�� */
		DWORD dwError = GetLastError();

		/** ��մ��ڻ����� */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return false;
	}

	/** �뿪�ٽ��� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return (BytesRead == 1);
}

bool CSerialPort::WriteData(unsigned char *pData, unsigned int length)
{
	BOOL bResult = TRUE;
	DWORD BytesToSend = 0;
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/** �ٽ������� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �򻺳���д��ָ���������� */
	bResult = WriteFile(m_hComm, pData, length, &BytesToSend, NULL);
	if (!bResult)
	{
		DWORD dwError = GetLastError();
		/** ��մ��ڻ����� */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return false;
	}

	/** �뿪�ٽ��� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}
