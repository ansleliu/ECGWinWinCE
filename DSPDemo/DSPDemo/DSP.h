#include "math.h"
//�û��ͬ�෨������a��b�������ϵ������yi.
double uniform(double a,double b,long *seed)
{
	double t;
	*seed=2045*(*seed)+1;
	*seed=*seed-(*seed/1048576)*1048576;
	t=(*seed)/1048576.0;
	t=a+(b-a)*t;
	return(t);
}

double gauss(double mean,double sigma,long *s)
{
	int i;double x,y;
    for(x=0,i=0;i<12;i++)
    {
		x+=uniform(0.0,1.0,s);
	}
    x=x-6.0;
	y=mean+x*sigma;
	return(y);
}

void sinwn(double a[],double f[],double ph[],int m,double fs,double snr,long seed,double x[],int n)
{
	int i,k;
	double z,pi,nsr;
	pi=4.0*atan(1.0);
	z=snr/10.0;
	z=pow(10.0,z);
	z=1.0/(2*z);
	nsr=sqrt(z);
	for(i=0;i<m;i++)
	{
		f[i]=2*pi*f[i]/fs;
		ph[i]=ph[i]*pi/180.0;
	}
	for(k=0;k<n;k++)
	{
		x[k]=0.0;
		for(i=0;i<m;i++)
		{
			x[k]=x[k]+a[i]*sin(k*f[i]+ph[i]);
		}
		x[k]=x[k]+nsr*gauss(0.0,1.0,&seed);
	}
}

//LMS����Ӧ�˲���
//x--˫����ʵ��һά���飬����n�������ź�
//d--˫����ʵ��һά���飬����n����������ź�
//y--˫����ʵ��һά���飬����n��ʵ������ź�
//n--���α����������źŵĳ���
//w--˫����ʵ��һά���飬����Ϊm������Ӧ�˲����ļ�Ȩϵ��
//m--���α���������Ӧ�˲����ĳ��ȣ�����-1��
//mu--˫����ʵ�ͱ�������������
void lms(double x[],double d[],double y[],int n,double w[],int m,double mu)
{
	int i,k;
	double e;
    for(i=0;i<m;i++)
	{
		w[i]=0.0;
	}
	for(k=0;k<m;k++)
	{
		y[k]=0.0;
		for(i=0;i<=k;i++)
		{
			y[k]+=x[k-i]*w[i];
		}
		e=d[k]-y[k];
		for(i=0;i<=k;i++)
		{
			w[i]+=2.0*mu*e*x[k-i];
		}
	}
	for(k=m;k<n;k++)
	{
		y[k]=0.0;
		for(i=0;i<m;i++)
		{
			y[k]+=x[k-i]*w[i];
		}
		e=d[k]-y[k];
		for(i=0;i<m;i++)
		{
			w[i]+=2.0*mu*e*x[k-i];
		}
	}
}

void lms(double x[],double d[],double y[],double e[],int n,double w[],int m,double mu)
{
	int i,k;
    for(i=0;i<m;i++)
	{
		w[i]=0.0;
	}
	for(k=0;k<m;k++)
	{
		y[k]=0.0;
		for(i=0;i<=k;i++)
		{
			y[k]+=x[k-i]*w[i];
		}
		e[k]=d[k]-y[k];
		for(i=0;i<=k;i++)
		{
			w[i]+=2.0*mu*e[k]*x[k-i];
		}
	}
	for(k=m;k<n;k++)
	{
		y[k]=0.0;
		for(i=0;i<m;i++)
		{
			y[k]+=x[k-i]*w[i];
		}
		e[k]=d[k]-y[k];
		for(i=0;i<m;i++)
		{
			w[i]+=2.0*mu*e[k]*x[k-i];
		}
	}
}

////////////////THIS LMS FOR CALWAV4 ////////////////////
void lms2(double x[],double d[],double e[],int n,double* v,int m,double mu)
{
	int i,k;
	double w[501],y[501];
    for(i=0;i<m;i++)
	{
		w[i]=0.0;
	}
	for(k=0;k<m;k++)
	{
		y[k]=0.0;
		for(i=0;i<=k;i++)
		{
			y[k]+=x[k-i]*w[i];
		}
		e[k]=d[k]-y[k];
		for(i=0;i<=k;i++)
		{
			w[i]+=2.0*mu*e[k]*x[k-i];
			*(v+i*501+k)=w[i];
		}
	}
	for(k=m;k<n;k++)
	{
		y[k]=0.0;
		for(i=0;i<m;i++)
		{
			y[k]+=x[k-i]*w[i];
		}
		e[k]=d[k]-y[k];
		for(i=0;i<m;i++)
		{
			w[i]+=2.0*mu*e[k]*x[k-i];
			*(v+i*501+k)=w[i];
		}
	}
}

//����H(Z)��Ƶ����Ӧ����Ƶ��Ӧ
void gain(double b[],double a[],int m,int n,double x[],double y[],int len,int sign)
//len--Ƶ����Ӧ�ĳ���
                 //��sign=0,����ʵ��Re[H(w)]���鲿Im[H(w)]
                 //��sign=1,�����Ƶ��Ӧ|H(w)|����Ƶ��Ӧ
                 //��sign=2,�����Ƶ��Ӧ|H(w)|��db��ʾ����Ƶ��Ӧ
{
	int i,k;
	double ar,ai,br,bi,zr,zi,im,re;
	double den,numr,numi,freq,temp;
	for(k=0;k<len;k++)
	{
		freq=k*0.5/(len-1);
		zr=cos(-8.0*atan(1.0)*freq);
		zi=sin(-8.0*atan(1.0)*freq);
		br=0.0;
		bi=0.0;
		for(i=m;i>0;i--)
		{
			re=br;
			im=bi;
			br=(re+b[i])*zr-im*zi;
			bi=(re+b[i])*zi+im*zr;
		}
		ar=0.0;
		ai=0.0;
		for(i=n;i>0;i--)
		{
			re=ar;
			im=ai;
			ar=(re+a[i])*zr-im*zi;
			ai=(re+a[i])*zi+im*zr;
		}
		br=br+b[0];
		ar=ar+1.0;
		numr=ar*br+ai*bi;
		numi=ar*bi-ai*br;
		den=ar*br+ai*br;
		x[k]=numr/den;
		y[k]=numi/den;
		switch(sign)
		{
		case 1:
			{
				temp=sqrt(x[k]*x[k]+y[k]*y[k]);
				y[k]=atan2(y[k],x[k]);
				x[k]=temp;
				break;
			}
		case 2:
			{
				temp=x[k]*x[k]+y[k]*y[k];
				y[k]=atan2(y[k],x[k]);
				x[k]=10.0*log10(temp);
			}
		}
	}
}