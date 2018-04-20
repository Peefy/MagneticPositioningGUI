#ifndef VALCHANGE_H_  
#define VALCHANGE_H_  
#include <Windows.h>  

extern double resX;
extern double resY;
extern double resZ;
extern double respsi;
extern double resphi;
extern double restheta2;

extern double resX_2;
extern double resY_2;
extern double resZ_2;
extern double respsi_2;
extern double resphi_2;
extern double restheta2_2;
int WINAPI HexToDec(char *Str);
//设置x的第y位为0
#define setbit(x,y) (x)&=(0<<(y-1))
//得到x的第y位的值
#define BitGet(Number,pos) ((Number)>>(pos-1)&1) 
//string* byteToHexStr(unsigned char byte_arr[], int arr_len);
double SumSquare(double a,double b,double c);//求平方和
double abss(double a);
double kalman(double Recdata,double ProcessNiose_Q,double MeasureNoise_R,int N);
void retry(double aa[],double nn,double AA[]);
double Filter(double GetAD);
#define   pi 3.1415926
#define  constant   4433.2738356897353//1861.2//1986.7915605921357  //4433.2738356897353
#define xarrylength 30
#define yarrylength 30
#define zarrylength 30
#define psiarrylength 30
#define phiarrylength 30
#define thetaarrylength 30
#define FILTER_N 30

#define FILTER_Npsi 80
#define FILTER_Nphi 80
#define FILTER_Ntheta 80

#define FILTER_N1 5
double Filterpsi(double Get_AD) ;
double Filterphi(double Get_AD);
double Filtertheta(double Get_AD);
double Filterx(double Get_AD);
double Filtery(double Get_AD);
double Filterz(double Get_AD);
double FilterXx(double Get_AD) ;
double FilterXy(double Get_AD);
double FilterXz(double Get_AD);
double FilterYx(double Get_AD);
double FilterYy(double Get_AD);
double FilterYz(double Get_AD);
double FilterZx(double Get_AD);
double FilterZy(double Get_AD);
double FilterZz(double Get_AD);
double Filterphi2(double GetAD); 
double var(double a[],int length);
double mean(double a[],int length);
#define R 10
#define Q 0.001
#endif //VALCHANGE