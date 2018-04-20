#include "stdafx.h"
#include "SerialPort.h"
#include <iostream>
#include <string>
#include "valchange.h"
using namespace std;

int WINAPI HexToDec(char *Str)
{
	bool IsHex = FALSE;
	int result = 0;
	int i = 0, szLen = 0, nHex;
	if (Str[0] == '0')
		if ((Str[1] == 'x') || (Str[1] == 'X'))
		{
			IsHex = TRUE;
			i = 2;
		}
	szLen = strlen(Str);
	nHex = '0';
	for (; i < szLen; i++)
	{
		if (IsHex)
		{
			if ((Str[i] >= '0') && (Str[i] <= '9'))
				nHex = '0';
			else if ((Str[i] >= 'A') && (Str[i] <= 'F'))
				nHex = 'A' - 10;
			else if ((Str[i] >= 'a') && (Str[i] <= 'f'))
				nHex = 'a' - 10;
			else
				return 0;
			result = result * 16 + Str[i] - nHex;
		}
		else
		{
			if ((Str[i] < '0') || (Str[i] > '9'))
				return 0;
			result = result * 10 + Str[i] - nHex;
		}
	}
	return result;
}
int bytesToInt(byte *bytes, int size = 4)
{
	int addr = bytes[0] & 0xFF;
	addr |= ((bytes[1] << 8) & 0xFF00);
	addr |= ((bytes[2] << 16) & 0xFF0000);
	addr |= ((bytes[3] << 24) & 0xFF000000);
	return addr;
}

double SumSquare(double a, double b, double c) //��ƽ����
{
	double sumsquare;
	sumsquare = a * a + b * b + c * c;
	return sumsquare;
}

double abss(double a)
{
	if (a >= 0)
	{
		return a;
	}
	if (a < 0)
	{
		return (-1) * a;
	}
}

double kalman(double Recdata, double ProcessNiose_Q, double MeasureNoise_R, int N)
{
	switch (N)
	{
	case 1:
	{
		double XxR = MeasureNoise_R;
		double XxQ = ProcessNiose_Q;

		static double Xx_last;

		double Xx_mid;
		double Xx_now;

		static double Xxp_last = 0.1;

		double Xxp_mid;
		double Xxp_now;
		double Xxkg;

		Xx_mid = Xx_last;							 //x_last=x(k-1|k-1),x_mid=x(k|k-1)
		Xxp_mid = Xxp_last + XxQ;					 //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����
		Xxkg = Xxp_mid / (Xxp_mid + XxR);			 //kgΪkalman filter��RΪ����
		Xx_now = Xx_mid + Xxkg * (Recdata - Xx_mid); //���Ƴ�������ֵ

		Xxp_now = (1 - Xxkg) * Xxp_mid; //����ֵ��Ӧ��covariance

		Xxp_last = Xxp_now; //����covarianceֵ
		Xx_last = Xx_now;   //����ϵͳ״ֵ̬

		return Xx_now;
	}
	break;
	case 2:
	{
		double XyR = MeasureNoise_R;
		double XyQ = ProcessNiose_Q;

		static double Xy_last;

		double Xy_mid;
		double Xy_now;

		static double Xyp_last = 0.1;

		double Xyp_mid;
		double Xyp_now;
		double Xykg;

		Xy_mid = Xy_last;							 //x_last=x(k-1|k-1),x_mid=x(k|k-1)
		Xyp_mid = Xyp_last + XyQ;					 //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����
		Xykg = Xyp_mid / (Xyp_mid + XyR);			 //kgΪkalman filter��RΪ����
		Xy_now = Xy_mid + Xykg * (Recdata - Xy_mid); //���Ƴ�������ֵ

		Xyp_now = (1 - Xykg) * Xyp_mid; //����ֵ��Ӧ��covariance

		Xyp_last = Xyp_now; //����covarianceֵ
		Xy_last = Xy_now;   //����ϵͳ״ֵ̬

		return Xy_now;
	}
	break;
	case 3:
	{
		double XzR = MeasureNoise_R;
		double XzQ = ProcessNiose_Q;

		static double Xz_last;

		double Xz_mid;
		double Xz_now;

		static double Xzp_last = 0.1;

		double Xzp_mid;
		double Xzp_now;
		double Xzkg;

		Xz_mid = Xz_last;							 //x_last=x(k-1|k-1),x_mid=x(k|k-1)
		Xzp_mid = Xzp_last + XzQ;					 //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����
		Xzkg = Xzp_mid / (Xzp_mid + XzR);			 //kgΪkalman filter��RΪ����
		Xz_now = Xz_mid + Xzkg * (Recdata - Xz_mid); //���Ƴ�������ֵ

		Xzp_now = (1 - Xzkg) * Xzp_mid; //����ֵ��Ӧ��covariance

		Xzp_last = Xzp_now; //����covarianceֵ
		Xz_last = Xz_now;   //����ϵͳ״ֵ̬

		return Xz_now;
	}
	break;
	case 4:
	{
		double YxR = MeasureNoise_R;
		double YxQ = ProcessNiose_Q;

		static double Yx_last;

		double Yx_mid;
		double Yx_now;

		static double Yxp_last = 0.1;

		double Yxp_mid;
		double Yxp_now;
		double Yxkg;

		Yx_mid = Yx_last;							 //x_last=x(k-1|k-1),x_mid=x(k|k-1)
		Yxp_mid = Yxp_last + YxQ;					 //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����
		Yxkg = Yxp_mid / (Yxp_mid + YxR);			 //kgΪkalman filter��RΪ����
		Yx_now = Yx_mid + Yxkg * (Recdata - Yx_mid); //���Ƴ�������ֵ

		Yxp_now = (1 - Yxkg) * Yxp_mid; //����ֵ��Ӧ��covariance

		Yxp_last = Yxp_now; //����covarianceֵ
		Yx_last = Yx_now;   //����ϵͳ״ֵ̬

		return Yx_now;
	}
	break;
	case 5:
	{
		double YyR = MeasureNoise_R;
		double YyQ = ProcessNiose_Q;

		static double Yy_last;

		double Yy_mid;
		double Yy_now;

		static double Yyp_last = 0.1;

		double Yyp_mid;
		double Yyp_now;
		double Yykg;

		Yy_mid = Yy_last;							 //x_last=x(k-1|k-1),x_mid=x(k|k-1)
		Yyp_mid = Yyp_last + YyQ;					 //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����
		Yykg = Yyp_mid / (Yyp_mid + YyR);			 //kgΪkalman filter��RΪ����
		Yy_now = Yy_mid + Yykg * (Recdata - Yy_mid); //���Ƴ�������ֵ

		Yyp_now = (1 - Yykg) * Yyp_mid; //����ֵ��Ӧ��covariance

		Yyp_last = Yyp_now; //����covarianceֵ
		Yy_last = Yy_now;   //����ϵͳ״ֵ̬

		return Yy_now;
	}
	break;
	case 6:
	{
		double YzR = MeasureNoise_R;
		double YzQ = ProcessNiose_Q;

		static double Yz_last;

		double Yz_mid;
		double Yz_now;

		static double Yzp_last = 0.1;

		double Yzp_mid;
		double Yzp_now;
		double Yzkg;

		Yz_mid = Yz_last;							 //x_last=x(k-1|k-1),x_mid=x(k|k-1)
		Yzp_mid = Yzp_last + YzQ;					 //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����
		Yzkg = Yzp_mid / (Yzp_mid + YzR);			 //kgΪkalman filter��RΪ����
		Yz_now = Yz_mid + Yzkg * (Recdata - Yz_mid); //���Ƴ�������ֵ

		Yzp_now = (1 - Yzkg) * Yzp_mid; //����ֵ��Ӧ��covariance

		Yzp_last = Yzp_now; //����covarianceֵ
		Yz_last = Yz_now;   //����ϵͳ״ֵ̬

		return Yz_now;
	}
	break;
	case 7:
	{
		double ZxR = MeasureNoise_R;
		double ZxQ = ProcessNiose_Q;

		static double Zx_last;

		double Zx_mid;
		double Zx_now;

		static double Zxp_last = 0.1;

		double Zxp_mid;
		double Zxp_now;
		double Zxkg;

		Zx_mid = Zx_last;							 //x_last=x(k-1|k-1),x_mid=x(k|k-1)
		Zxp_mid = Zxp_last + ZxQ;					 //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����
		Zxkg = Zxp_mid / (Zxp_mid + ZxR);			 //kgΪkalman filter��RΪ����
		Zx_now = Zx_mid + Zxkg * (Recdata - Zx_mid); //���Ƴ�������ֵ

		Zxp_now = (1 - Zxkg) * Zxp_mid; //����ֵ��Ӧ��covariance

		Zxp_last = Zxp_now; //����covarianceֵ
		Zx_last = Zx_now;   //����ϵͳ״ֵ̬

		return Zx_now;
	}
	break;
	case 8:
	{
		double ZyR = MeasureNoise_R;
		double ZyQ = ProcessNiose_Q;

		static double Zy_last;

		double Zy_mid;
		double Zy_now;

		static double Zyp_last = 0.1;

		double Zyp_mid;
		double Zyp_now;
		double Zykg;

		Zy_mid = Zy_last;							 //x_last=x(k-1|k-1),x_mid=x(k|k-1)
		Zyp_mid = Zyp_last + ZyQ;					 //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����
		Zykg = Zyp_mid / (Zyp_mid + ZyR);			 //kgΪkalman filter��RΪ����
		Zy_now = Zy_mid + Zykg * (Recdata - Zy_mid); //���Ƴ�������ֵ

		Zyp_now = (1 - Zykg) * Zyp_mid; //����ֵ��Ӧ��covariance

		Zyp_last = Zyp_now; //����covarianceֵ
		Zy_last = Zy_now;   //����ϵͳ״ֵ̬

		return Zy_now;
	}
	break;
	case 9:
	{
		double ZzR = MeasureNoise_R;
		double ZzQ = ProcessNiose_Q;

		static double Zz_last;

		double Zz_mid;
		double Zz_now;

		static double Zzp_last = 0.1;

		double Zzp_mid;
		double Zzp_now;
		double Zzkg;

		Zz_mid = Zz_last;							 //x_last=x(k-1|k-1),x_mid=x(k|k-1)
		Zzp_mid = Zzp_last + ZzQ;					 //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����
		Zzkg = Zzp_mid / (Zzp_mid + ZzR);			 //kgΪkalman filter��RΪ����
		Zz_now = Zz_mid + Zzkg * (Recdata - Zz_mid); //���Ƴ�������ֵ

		Zzp_now = (1 - Zzkg) * Zzp_mid; //����ֵ��Ӧ��covariance

		Zzp_last = Zzp_now; //����covarianceֵ
		Zz_last = Zz_now;   //����ϵͳ״ֵ̬

		return Zz_now;
	}
	break;
	} //switch
}
int N3 = 0;
double Xarry[xarrylength + 1];
double Yarry[yarrylength + 1];
double Zarry[zarrylength + 1];
double psiarry[psiarrylength + 1];
double phiarry[phiarrylength + 1];
double thetaarry[thetaarrylength + 1];

int flagx = 0;
int flagy = 0;
int flagz = 0;
int flagpsi = 0;
int flagphi = 0;
int flagtheta = 0;

double meanX = 0;
double meanY = 0;
double meanZ = 0;
double meanpsi = 0;
double meanphi = 0;
double meantheta = 0;

int counterx = 0;
int countery = 0;
int counterz = 0;
int counterpsi = 0;
int counterphi = 0;
int countertheta = 0;
void retry(double aa[], double nn, double AA[])
{
	double Xx1, Xy1, Xz1;
	double Yx1, Yy1, Yz1;
	double Zx1, Zy1, Zz1;
	double Xx, Xy, Xz;
	double Yx, Yy, Yz;
	double Zx, Zy, Zz;
	double Px, Py, Pz, sumpxpypz;
	double rho, sn_arfa, cn_arfa, arfa1, betia1, arfa = 0;
	double f33, ff, betia, f23, f13, A1, A2, A3, X1, Y1, Z1;
	double rho1, rho2, A11a, A11b, A11c, A11, A12a, A12b, A12c, A12, A13a, A13b, A13c, A13, A23a, A23b, A23c, A23, A33a, A33b, A33c, A33;
	double psi_1, phi_1, theta_2;

	int ix;
	int iy;
	int iz;
	int ipsi;
	int iphi;
	int itheta;

	double kgx, kgy, kgz;
	double X_2, Y_2, Z_2;
	double varX, varY, varZ;
	double kgpsi, kgphi, kgtheta;
	double varpsi, varphi, vartheta;

	double psi_2, phi_2, theta_22;
	//double AA[]={0.8618,0.0301,0.0646,-0.0080,1.0461,0.0602,-0.1231,-0.0442,0.9170};
	Xx1 = aa[0];
	Xy1 = aa[1];
	Xz1 = aa[2];

	Yx1 = aa[3];
	Yy1 = aa[4];
	Yz1 = aa[5];

	Zx1 = aa[6];
	Zy1 = aa[7];
	Zz1 = aa[8];

	Xx = Xx1 * AA[0] + Xy1 * AA[1] + Xz1 * AA[2];
	Yx = Yx1 * AA[0] + Yy1 * AA[1] + Yz1 * AA[2];
	Zx = Zx1 * AA[0] + Zy1 * AA[1] + Zz1 * AA[2];

	Xy = Xx1 * AA[3] + Xy1 * AA[4] + Xz1 * AA[5];
	Yy = Yx1 * AA[3] + Yy1 * AA[4] + Yz1 * AA[5];
	Zy = Zx1 * AA[3] + Zy1 * AA[4] + Zz1 * AA[5];

	Xz = Xx1 * AA[6] + Xy1 * AA[7] + Xz1 * AA[8];
	Yz = Yx1 * AA[6] + Yy1 * AA[7] + Yz1 * AA[8];
	Zz = Zx1 * AA[6] + Zy1 * AA[7] + Zz1 * AA[8];
	Px = SumSquare(Xx, Xy, Xz);
	Py = SumSquare(Yx, Yy, Yz);
	Pz = SumSquare(Zx, Zy, Zz);
	sumpxpypz = Px + Py + Pz;

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
	/*X1=X1*100;
	Y1=Y1*1000;
	Z1=Z1*1000;*/
	//X1=kalman(X1,0.0001,nn,1)*100.0;
	//Y1=kalman(Y1,0.0001,nn,2)*100.0;
	//Z1=kalman(Z1,0.00001,nn,3)*100.0;
	if ((sn_arfa > 0) && (cn_arfa > 0))
	{
		X1 = X1;
		Y1 = Y1;
		Z1 = -Z1;
		arfa = arfa1;
	}
	if ((sn_arfa > 0) && (cn_arfa < 0))
	{
		X1 = -X1;
		Y1 = Y1;
		Z1 = -Z1;
		arfa = 180.0 - arfa1;
	}
	if ((sn_arfa < 0) && (cn_arfa < 0))
	{
		X1 = -X1;
		Y1 = -Y1;
		Z1 = -Z1;
		arfa = arfa1 + 180.0;
	}
	if ((sn_arfa < 0) && (cn_arfa > 0))
	{
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
	if ((A12 > 0) && (A11 > 0))
	{
		psi_1 = psi_1;
	}
	if ((A12 > 0) && (A11 < 0))
	{
		psi_1 = 180.0 - psi_1;
	}
	if ((A12 < 0) && (A11 < 0))
	{
		psi_1 = psi_1 + 180.0;
	}
	if ((A12 < 0) && (A11 > 0))
	{
		psi_1 = 360.0 - psi_1;
	}
	if ((A23 > 0) && (A33 > 0))
	{
		phi_1 = phi_1;
	}
	if ((A23 > 0) && (A33 < 0))
	{
		phi_1 = 180.0 - phi_1;
	}
	if ((A23 < 0) && (A33 < 0))
	{
		phi_1 = phi_1 + 180.0;
	}
	if ((A23 < 0) && (A33 > 0))
	{
		phi_1 = 360.0 - phi_1;
	}
	X1 = X1 * 1000;
	Y1 = Y1 * 1000;
	Z1 = Z1 * 1000;
	X1 = Filterx(X1);
	Y1 = Filtery(Y1);
	Z1 = Filterz(Z1);
	/* psi_1=Filterpsi(psi_1);
			 phi_1=Filterphi(phi_1);
			 theta_2=Filtertheta(theta_2);*/

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
		if (varY > 100.0)
		{
			flagy = 0;
			resY = Y1;
			resY_2 = Y1;
		}
		if (varY <= 100.0)
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
		if (varpsi > 50.0)
		{
			flagpsi = 0;
			respsi_2 = psi_1;
		}
		if (varpsi <= 50.0)
		{
			if (flagpsi == 0)
			{
				meanpsi = mean(psiarry, 31);
				//meanY=(Xarry[1]+Xarry[2]+Xarry[3]+Xarry[4])/4.0;
				flagpsi = 1;
			}
			if (flagpsi == 1)
			{
				kgpsi = mean(psiarry, 31);
				//kgy=(Xarry[1]+Xarry[2]+Xarry[3]+Xarry[4])/4.0;

				kgpsi = (abss(kgpsi) - abss(meanpsi)) * 0.1;

				psi_2 = meanpsi + kgpsi;
				respsi_2 = psi_2;
			} //flag4==1
		}	 //var<20.0

		if (varphi > 50.0)
		{
			flagphi = 0;
			resphi_2 = phi_1;
		}
		if (varphi <= 50.0)
		{
			if (flagphi == 0)
			{
				meanphi = mean(phiarry, 31);
				//meanY=(Xarry[1]+Xarry[2]+Xarry[3]+Xarry[4])/4.0;
				flagphi = 1;
			}
			if (flagphi == 1)
			{
				kgphi = mean(phiarry, 31);
				//kgy=(Xarry[1]+Xarry[2]+Xarry[3]+Xarry[4])/4.0;

				kgphi = (abss(kgphi) - abss(meanphi)) * 0.1;

				phi_2 = meanphi + kgphi;
				resphi_2 = phi_2;
			} //flag4==1
		}	 //var<20.0

		if (vartheta > 50.0)
		{
			flagtheta = 0;
			restheta2_2 = theta_2;
		}
		if (vartheta <= 50.0)
		{
			if (flagtheta == 0)
			{
				meantheta = mean(thetaarry, 31);
				//meanY=(Xarry[1]+Xarry[2]+Xarry[3]+Xarry[4])/4.0;
				flagtheta = 1;
			}
			if (flagtheta == 1)
			{
				kgtheta = mean(thetaarry, 31);
				//kgy=(Xarry[1]+Xarry[2]+Xarry[3]+Xarry[4])/4.0;

				kgtheta = (abss(kgtheta) - abss(meantheta)) * 0.1;

				theta_22 = meantheta + kgtheta;
				restheta2_2 = theta_22;
			} //flag4==1
		}	 //var<20.0
			  /*psi_1=Filterpsi(psi_1);
						psi_2=kalman(psi_1,Q,R,4);
						respsi=psi_2;
						respsi_2=psi_2;

						phi_1=Filterphi(phi_1);
						phi_2=kalman(phi_1,Q,R,5);
						resphi=phi_2;
						resphi_2=phi_2;

						theta_2=Filtertheta(theta_2);
						theta_22=kalman(theta_2,Q,R,6);
						restheta2=theta_2;
						restheta2_2=theta_2;*/
		N3 = xarrylength;

	} // ifN3==?
}
double filterpsi_buf[FILTER_Npsi + 1];
double Filterpsi(double Get_AD)
{
	int i;
	double filterpsi_sum = 0.0;
	filterpsi_buf[FILTER_Npsi] = Get_AD; //ADת����ֵ�����������һ��ֵ
	if (abs((filterpsi_buf[FILTER_Npsi] - filterpsi_buf[FILTER_Npsi - 1])) > 310.0)
	{
		filterpsi_buf[FILTER_Npsi] = filterpsi_buf[FILTER_Npsi - 1] + abs(filterpsi_buf[FILTER_Npsi] - filterpsi_buf[FILTER_Npsi - 1]) * 0.0001;
		if (filterpsi_buf[FILTER_Npsi] >= 359)
		{
			filterpsi_buf[FILTER_Npsi] = 358;
		}
	}
	for (i = 0; i < FILTER_Npsi; i++)
	{
		filterpsi_buf[i] = filterpsi_buf[i + 1]; // �����������ƣ���λ�Ե�

		filterpsi_sum += filterpsi_buf[i];
	}
	return (double)(filterpsi_sum / (FILTER_Npsi + 1));
}

double filterphi_buf[FILTER_Nphi + 1];
double Filterphi(double Get_AD)
{
	int i;
	double filterphi_sum = 0.0;
	filterphi_buf[FILTER_Nphi] = Get_AD; //ADת����ֵ�����������һ��ֵ
	if (abs((filterphi_buf[FILTER_Nphi] - filterphi_buf[FILTER_Nphi - 1])) > 310.0)
	{
		filterphi_buf[FILTER_Nphi] = filterphi_buf[FILTER_Nphi - 1] + abs(filterphi_buf[FILTER_Nphi] - filterphi_buf[FILTER_Nphi - 1]) * 0.0001;
		if (filterphi_buf[FILTER_Nphi] >= 359)
		{
			filterphi_buf[FILTER_Nphi] = 357;
		}
	}
	for (i = 0; i < FILTER_Nphi; i++)
	{
		filterphi_buf[i] = filterphi_buf[i + 1]; // �����������ƣ���λ�Ե�
		filterphi_sum += filterphi_buf[i];
	}
	return (double)(filterphi_sum / (FILTER_Nphi + 1));
}

double filtertheta_buf[FILTER_Ntheta + 1];
double Filtertheta(double Get_AD)
{
	int i;
	double filtertheta_sum = 0.0;
	filtertheta_buf[FILTER_Ntheta] = Get_AD; //ADת����ֵ�����������һ��ֵ
	for (i = 0; i < FILTER_Ntheta; i++)
	{
		filtertheta_buf[i] = filtertheta_buf[i + 1]; // �����������ƣ���λ�Ե�
		filtertheta_sum += filtertheta_buf[i];
	}
	return (double)(filtertheta_sum / (FILTER_Ntheta + 1));
}

double filterx_buf[FILTER_N + 1];
double Filterx(double Get_AD)
{
	int i;
	double filterx_sum = 0.0;
	filterx_buf[FILTER_N] = Get_AD; //ADת����ֵ�����������һ��ֵ
	for (i = 0; i < FILTER_N; i++)
	{
		filterx_buf[i] = filterx_buf[i + 1]; // �����������ƣ���λ�Ե�
		filterx_sum += filterx_buf[i];
	}
	return (double)(filterx_sum / FILTER_N);
}

double filtery_buf[FILTER_N + 1];
double Filtery(double Get_AD)
{
	int i;
	double filtery_sum = 0.0;
	filtery_buf[FILTER_N] = Get_AD; //ADת����ֵ�����������һ��ֵ
	for (i = 0; i < FILTER_N; i++)
	{
		filtery_buf[i] = filtery_buf[i + 1]; // �����������ƣ���λ�Ե�
		filtery_sum += filtery_buf[i];
	}
	return (double)(filtery_sum / FILTER_N);
}

double filterz_buf[FILTER_N + 1];
double Filterz(double Get_AD)
{
	int i;
	double filterz_sum = 0.0;
	filterz_buf[FILTER_N] = Get_AD; //ADת����ֵ�����������һ��ֵ
	for (i = 0; i < FILTER_N; i++)
	{
		filterz_buf[i] = filterz_buf[i + 1]; // �����������ƣ���λ�Ե�
		filterz_sum += filterz_buf[i];
	}
	return (double)(filterz_sum / FILTER_N);
}

double filterXx_buf[FILTER_N + 1];
double FilterXx(double Get_AD)
{
	int i;
	double filterXx_sum = 0.0;
	filterXx_buf[FILTER_N] = Get_AD; //ADת����ֵ�����������һ��ֵ
	for (i = 0; i < FILTER_N; i++)
	{
		filterXx_buf[i] = filterXx_buf[i + 1]; // �����������ƣ���λ�Ե�
		filterXx_sum += filterXx_buf[i];
	}
	return (double)(filterXx_sum / FILTER_N);
}

double filterYx_buf[FILTER_N + 1];
double FilterYx(double Get_AD)
{
	int i;
	double filterYx_sum = 0.0;
	filterYx_buf[FILTER_N] = Get_AD; //ADת����ֵ�����������һ��ֵ
	for (i = 0; i < FILTER_N; i++)
	{
		filterYx_buf[i] = filterYx_buf[i + 1]; // �����������ƣ���λ�Ե�
		filterYx_sum += filterYx_buf[i];
	}
	return (double)(filterYx_sum / FILTER_N);
}

double filterZx_buf[FILTER_N + 1];
double FilterZx(double Get_AD)
{
	int i;
	double filterZx_sum = 0.0;
	filterZx_buf[FILTER_N] = Get_AD; //ADת����ֵ�����������һ��ֵ
	for (i = 0; i < FILTER_N; i++)
	{
		filterZx_buf[i] = filterZx_buf[i + 1]; // �����������ƣ���λ�Ե�
		filterZx_sum += filterZx_buf[i];
	}
	return (double)(filterZx_sum / FILTER_N);
}

double filterXy_buf[FILTER_N + 1];
double FilterXy(double Get_AD)
{
	int i;
	double filterXy_sum = 0.0;
	filterXy_buf[FILTER_N] = Get_AD; //ADת����ֵ�����������һ��ֵ
	for (i = 0; i < FILTER_N; i++)
	{
		filterXy_buf[i] = filterXy_buf[i + 1]; // �����������ƣ���λ�Ե�
		filterXy_sum += filterXy_buf[i];
	}
	return (double)(filterXy_sum / FILTER_N);
}

double filterYy_buf[FILTER_N + 1];
double FilterYy(double Get_AD)
{
	int i;
	double filterYy_sum = 0.0;
	filterYy_buf[FILTER_N] = Get_AD; //ADת����ֵ�����������һ��ֵ
	for (i = 0; i < FILTER_N; i++)
	{
		filterYy_buf[i] = filterYy_buf[i + 1]; // �����������ƣ���λ�Ե�
		filterYy_sum += filterYy_buf[i];
	}
	return (double)(filterYy_sum / FILTER_N);
}

double filterZy_buf[FILTER_N + 1];
double FilterZy(double Get_AD)
{
	int i;
	double filterZy_sum = 0.0;
	filterZy_buf[FILTER_N] = Get_AD; //ADת����ֵ�����������һ��ֵ
	for (i = 0; i < FILTER_N; i++)
	{
		filterZy_buf[i] = filterZy_buf[i + 1]; // �����������ƣ���λ�Ե�
		filterZy_sum += filterZy_buf[i];
	}
	return (double)(filterZy_sum / FILTER_N);
}

double filterXz_buf[FILTER_N + 1];
double FilterXz(double Get_AD)
{
	int i;
	double filterXz_sum = 0.0;
	filterXz_buf[FILTER_N] = Get_AD; //ADת����ֵ�����������һ��ֵ
	for (i = 0; i < FILTER_N; i++)
	{
		filterXz_buf[i] = filterXz_buf[i + 1]; // �����������ƣ���λ�Ե�
		filterXz_sum += filterXz_buf[i];
	}
	return (double)(filterXz_sum / FILTER_N);
}

double filterYz_buf[FILTER_N + 1];
double FilterYz(double Get_AD)
{
	int i;
	double filterYz_sum = 0.0;
	filterYz_buf[FILTER_N] = Get_AD; //ADת����ֵ�����������һ��ֵ
	for (i = 0; i < FILTER_N; i++)
	{
		filterYz_buf[i] = filterYz_buf[i + 1]; // �����������ƣ���λ�Ե�
		filterYz_sum += filterYz_buf[i];
	}
	return (double)(filterYz_sum / FILTER_N);
}

double filterZz_buf[FILTER_N + 1];
double FilterZz(double Get_AD)
{
	int i;
	double filterZz_sum = 0.0;
	filterZz_buf[FILTER_N] = Get_AD; //ADת����ֵ�����������һ��ֵ
	for (i = 0; i < FILTER_N; i++)
	{
		filterZz_buf[i] = filterZz_buf[i + 1]; // �����������ƣ���λ�Ե�
		filterZz_sum += filterZz_buf[i];
	}
	return (double)(filterZz_sum / FILTER_N);
}
double var(double a[], int length)
{
	int i;
	double arry_sum = 0.0;
	double arry_mean = 0.0;
	double arry_var = 0.0;
	for (i = 0; i < length; i++)
	{
		arry_sum += a[i];
	}
	arry_mean = arry_sum / length;
	for (i = 0; i < length; i++)
	{
		arry_var += (a[i] - arry_mean) * (a[i] - arry_mean);
	}
	return arry_var = arry_var / length;
}
double mean(double a[], int length)
{
	int i;
	double arry_sum = 0.0;
	double arry_mean = 0.0;
	for (i = 0; i < length; i++)
	{
		arry_sum += a[i];
	}
	return arry_mean = arry_sum / length;
}

int counterphi2 = 0;
double filterphi2_buf[FILTER_N1 + 1];
double Filterphi2(double GetAD)
{
	int i, j;
	int k;
	double filter_temp, filter_sum = 0;

	for (k = 0; k < FILTER_N1; k++)
	{
		filterphi2_buf[k] = filterphi2_buf[k + 1]; // �����������ƣ���λ�Ե�
	}
	filterphi2_buf[FILTER_N1] = GetAD; //ADת����ֵ�����������һ��ֵ
	counterphi2++;
	if (counterphi2 == FILTER_N1 + 1)
	{
		if (abs((filterphi2_buf[FILTER_N1] - filterphi2_buf[FILTER_N1 - 1])) > 310.0)
		{
			filterphi2_buf[FILTER_N1] = filterphi2_buf[FILTER_N1 - 1];
		}

		for (i = 0; i < FILTER_N1 + 1; i++)
		{
			filter_sum += filterphi2_buf[i];
		}

		counterphi2 = FILTER_N1;
		return filter_sum / (FILTER_N1 + 1);
	}
}
