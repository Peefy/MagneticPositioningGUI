// MagneticPositioning_Matlab.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "stdafx.h"  
#include <iostream>  
#include <string>  
#include <windows.h>
#include "SerialPort.h"  
#include "MagneticPositioning.h"

#include "util/openglhelper.h"

using namespace std;

int main(int argc, char ** argv)
{
    CSerialPort mySerialPort;

    //�򿪴���  
    if (!mySerialPort.InitPort(4))
    {
        std::cout << "initPort fail !" << std::endl;
    }
    else
    {
        std::cout << "initPort success !" << std::endl;
    }
   
    if (!mySerialPort.OpenListenThread())
    {
        std::cout << "OpenListenThread fail !" << std::endl;
    }
    else
    {
        std::cout << "OpenListenThread success !" << std::endl;
    }

    util::MagPositionOpenGlInit(argc, argv);

    std::cout << "ok" << endl;
    getchar();
    mySerialPort.CloseListenTread();
    return 0;
}
