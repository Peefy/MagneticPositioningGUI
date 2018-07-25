#include "StdAfx.h"    
#include "SerialPort.h"    
#include <process.h>    
#include <iostream>  
#include"MagneticPositioning.h"

/** 线程退出标志 */
bool CSerialPort::s_bExit = false;
/** 当串口无数据时,sleep至下次查询间隔的时间,单位:秒 */
const UINT SLEEP_TIME_INTERVAL = 10;
using namespace std;
unsigned char arry[49] = {};
int N = 0;
unsigned char status = 0;
unsigned char d = 0;
unsigned char ctl;
double N93[4][3];

double reXx, reXy, reXz;
double reYx, reYy, reYz;
double reZx, reZy, reZz;
double *repose = new double[6];

double re_9_elements[3][3];
double F42[3][3] = { { 1,1,1 },{ 1,1,1 },{ 1,1,1 } };
double F4_set[3][3] = { { 1,1,-1 },{ 1,1,-1 },{ -1,-1,-1 } };
int counter = 0;
int flag = 0;
int flag2 = 0;

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

    /** 临时变量,将制定参数转化为字符串形式,以构造DCB结构 */
    char szDCBparam[50];
    sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopsbits);

    /** 打开指定串口,该函数内部已经有临界区保护,上面请不要加保护 */
    if (!openPort(portNo))
    {
        return false;
    }

    /** 进入临界段 */
    EnterCriticalSection(&m_csCommunicationSync);

    /** 是否有错误发生 */
    BOOL bIsSuccess = TRUE;

    /** 在此可以设置输入输出的缓冲区大小,如果不设置,则系统会设置默认值.
    *  自己设置缓冲区大小时,要注意设置稍大一些,避免缓冲区溢出
    */
    /*if (bIsSuccess )
    {
    bIsSuccess = SetupComm(m_hComm,10,10);
    }*/

    /** 设置串口的超时时间,均设为0,表示不使用超时限制 */
    COMMTIMEOUTS  CommTimeouts;
    CommTimeouts.ReadIntervalTimeout = 0;
    CommTimeouts.ReadTotalTimeoutMultiplier = 0;
    CommTimeouts.ReadTotalTimeoutConstant = 0;
    CommTimeouts.WriteTotalTimeoutMultiplier = 0;
    CommTimeouts.WriteTotalTimeoutConstant = 0;
    if (bIsSuccess)
    {
        bIsSuccess = SetCommTimeouts(m_hComm, &CommTimeouts);
    }

    DCB  dcb;
    if (bIsSuccess)
    {
        // 将ANSI字符串转换为UNICODE字符串    
        DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, NULL, 0);
        wchar_t *pwText = new wchar_t[dwNum];
        if (!MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, pwText, dwNum))
        {
            bIsSuccess = TRUE;
        }

        /** 获取当前串口配置参数,并且构造串口DCB参数 */
        bIsSuccess = GetCommState(m_hComm, &dcb) && BuildCommDCB(pwText, &dcb);
        /** 开启RTS flow控制 */
        dcb.fRtsControl = RTS_CONTROL_ENABLE;

        /** 释放内存空间 */
        delete[] pwText;
    }

    if (bIsSuccess)
    {
        /** 使用DCB参数配置串口状态 */
        bIsSuccess = SetCommState(m_hComm, &dcb);
    }

    /**  清空串口缓冲区 */
    PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

    /** 离开临界段 */
    LeaveCriticalSection(&m_csCommunicationSync);

    return bIsSuccess == TRUE;
}

bool CSerialPort::InitPort(UINT portNo, const LPDCB& plDCB)
{
    /** 打开指定串口,该函数内部已经有临界区保护,上面请不要加保护 */
    if (!openPort(portNo))
    {
        return false;
    }

    /** 进入临界段 */
    EnterCriticalSection(&m_csCommunicationSync);

    /** 配置串口参数 */
    if (!SetCommState(m_hComm, plDCB))
    {
        return false;
    }

    /**  清空串口缓冲区 */
    PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

    /** 离开临界段 */
    LeaveCriticalSection(&m_csCommunicationSync);

    return true;
}

void CSerialPort::ClosePort()
{
    /** 如果有串口被打开，关闭它 */
    if (m_hComm != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hComm);
        m_hComm = INVALID_HANDLE_VALUE;
    }
}

bool CSerialPort::openPort(UINT portNo)
{
    /** 进入临界段 */
    EnterCriticalSection(&m_csCommunicationSync);

    /** 把串口的编号转换为设备名 */
    char szPort[50];
    sprintf_s(szPort, "COM%d", portNo);

    /** 打开指定的串口 */
    m_hComm = CreateFileA(szPort,  /** 设备名,COM1,COM2等 */
        GENERIC_READ | GENERIC_WRITE, /** 访问模式,可同时读写 */
        0,                            /** 共享模式,0表示不共享 */
        NULL,                         /** 安全性设置,一般使用NULL */
        OPEN_EXISTING,                /** 该参数表示设备必须存在,否则创建失败 */
        0,
        0);

    /** 如果打开失败，释放资源并返回 */
    if (m_hComm == INVALID_HANDLE_VALUE)
    {
        LeaveCriticalSection(&m_csCommunicationSync);
        return false;
    }

    /** 退出临界区 */
    LeaveCriticalSection(&m_csCommunicationSync);

    return true;
}

bool CSerialPort::OpenListenThread()
{
    /** 检测线程是否已经开启了 */
    if (m_hListenThread != INVALID_HANDLE_VALUE)
    {
        /** 线程已经开启 */
        return false;
    }

    s_bExit = false;
    /** 线程ID */
    UINT threadId;
    /** 开启串口数据监听线程 */
    m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);
    if (!m_hListenThread)
    {
        return false;
    }
    /** 设置线程的优先级,高于普通线程 */
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
        /** 通知线程退出 */
        s_bExit = true;

        /** 等待线程退出 */
        Sleep(10);

        /** 置线程句柄无效 */
        CloseHandle(m_hListenThread);
        m_hListenThread = INVALID_HANDLE_VALUE;
    }
    return true;
}

UINT CSerialPort::GetBytesInCOM()
{
    DWORD dwError = 0;  /** 错误码 */
    COMSTAT  comstat;   /** COMSTAT结构体,记录通信设备的状态信息 */
    memset(&comstat, 0, sizeof(COMSTAT));

    UINT BytesInQue = 0;
    /** 在调用ReadFile和WriteFile之前,通过本函数清除以前遗留的错误标志 */
    if (ClearCommError(m_hComm, &dwError, &comstat))
    {
        BytesInQue = comstat.cbInQue; /** 获取在输入缓冲区中的字节数 */
    }

    return BytesInQue;
}

UINT WINAPI CSerialPort::ListenThread(void* pParam)
{
    /** 得到本类的指针 */
    CSerialPort *pSerialPort = reinterpret_cast<CSerialPort*>(pParam);

    // 线程循环,轮询方式读取串口数据    
    while (!pSerialPort->s_bExit)
    {
        //UINT BytesInQue = pSerialPort->GetBytesInCOM();
        ///** 如果串口输入缓冲区中无数据,则休息一会再查询 */
        //if (BytesInQue == 0)
        //{
        //    Sleep(SLEEP_TIME_INTERVAL);
        //    continue;
        //}

        /** 读取输入缓冲区中的数据并输出显示 */
        unsigned char cRecved;
        cRecved = 0;   
        auto num = pSerialPort->GetBytesInCOM();
        cout << "buffer:" << num << endl;
        Sleep(SLEEP_TIME_INTERVAL);
        if (num > 0)
        {
            pSerialPort->ReadChar(cRecved);
            switch (status)
            {
            case 0:
                if (cRecved == 127)
                {
                    status = 7;
                    std::cout << "ok" << endl;
                }
                break;
            case 7:
                if (cRecved == 146)
                {
                    num = pSerialPort->GetBytesInCOM();
                    if (num <= 48)
                    {
                        status = 0;
                    }
                    for (auto i = 0; i < 48; ++i)
                    {
                        pSerialPort->ReadChar(cRecved);
                        arry[i] = cRecved;
                    }
                    ctl = 0;
                    for (int i = 0; i < 4; i++)
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            N93[i][j] = (double)((arry[ctl] << 24) + (arry[ctl + 1] << 16) + (arry[ctl + 2] << 8) + arry[ctl + 3]) / 100.0;
                            // std::cout << N93[i][j] << endl;
                            ctl = ctl + 4;
                        }
                    }
                    //校正
                    reXx = N93[0][0] * 6.892 / 4.228; reYx = N93[0][1] * 6.892 / 5.545; reZx = N93[0][2];
                    reXy = N93[1][0] * 6.892 / 4.228; reYy = N93[1][1] * 6.892 / 5.545; reZy = N93[1][2];
                    reXz = N93[2][0] * 6.892 / 4.228; reYz = N93[2][1] * 6.892 / 5.545; reZz = N93[2][2];
                    
                    re_9_elements[0][0] = reXx;  re_9_elements[0][1] = reYx;   re_9_elements[0][2] = reZx;
                    re_9_elements[1][0] = reXy;  re_9_elements[1][1] = reYy;   re_9_elements[1][2] = reZy;
                    re_9_elements[2][0] = reXz;  re_9_elements[2][1] = reYz;   re_9_elements[2][2] = reZz;
                
                    counter++;
                    std::cout << counter << endl;
                    if ((counter >= 500) && (flag == 0))
                    {
                        // 预置点 alpha=45°； beta =10° 
                        for (int i = 0; i < 3; i++)
                        {
                            for (int j = 0; j < 3; j++)
                            {
                                if (re_9_elements[i][j] * F4_set[i][j] < 0)
                                {
                                    F42[i][j] = -F42[i][j];
                                }
                                else
                                {
                                    F42[i][j] = F42[i][j];
                                }
                            }
                        }
                        flag = 1;
                        flag2 = 1;

                    }
                    for (int i = 0; i < 3; i++)
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            re_9_elements[i][j] = F42[i][j] * re_9_elements[i][j];
                        }

                    }

                    repose = pose(re_9_elements);

                    std::cout << "X:" << repose[0] << endl;
                    std::cout << "Y:" << repose[1] << endl;
                    std::cout << "Z:" << repose[2] << endl;
                    std::cout << "psi:" << repose[3] << endl;
                    std::cout << "theta:" << repose[4] << endl;
                    std::cout << "phi:" << repose[5] << endl;

                }
                break;
            default: break;
            }//switch
        }
    }
    return 0;
}

bool CSerialPort::ReadChar(unsigned char &cRecved)
{
    BOOL  bResult = TRUE;
    DWORD BytesRead = 0;
    if (m_hComm == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    /** 临界区保护 */
    EnterCriticalSection(&m_csCommunicationSync);

    /** 从缓冲区读取一个字节的数据 */
    bResult = ReadFile(m_hComm, &cRecved, 1, &BytesRead, NULL);
    if ((!bResult))
    {
        /** 获取错误码,可以根据该错误码查出错误原因 */
        DWORD dwError = GetLastError();

        /** 清空串口缓冲区 */
        PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
        LeaveCriticalSection(&m_csCommunicationSync);

        return false;
    }

    /** 离开临界区 */
    LeaveCriticalSection(&m_csCommunicationSync);

    return (BytesRead == 1);

}

bool CSerialPort::WriteData(unsigned char* pData, unsigned int length)
{
    BOOL   bResult = TRUE;
    DWORD  BytesToSend = 0;
    if (m_hComm == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    /** 临界区保护 */
    EnterCriticalSection(&m_csCommunicationSync);

    /** 向缓冲区写入指定量的数据 */
    bResult = WriteFile(m_hComm, pData, length, &BytesToSend, NULL);
    if (!bResult)
    {
        DWORD dwError = GetLastError();
        /** 清空串口缓冲区 */
        PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
        LeaveCriticalSection(&m_csCommunicationSync);

        return false;
    }

    /** 离开临界区 */
    LeaveCriticalSection(&m_csCommunicationSync);

    return true;
}