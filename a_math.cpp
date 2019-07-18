#include "a_math.h"
#include <string>
#include <math.h>


void MatByMat (double mat1[][3], double mat2[][3], double res[][3])
{   double temp[3][3];
    for(int i=0;i<3;i++) for(int j=0;j<3;j++) { temp[i][j]=0.;
            for(int k=0;k<3;k++) temp[i][j]+=mat1[i][k]*mat2[k][j]; }
    memcpy(res,temp,sizeof(temp));
}

void MyMxMz(double Roll, double Pitch, double Az, double res[][3])
{
    double M[3][3];
    double My[3][3]={
        {cos(Roll),0,-sin(Roll)},
        {0,1,0},
        {sin(Roll),0,cos(Roll)}
    };;
    double Mx[3][3]={
        {1,0,0},
        {0,cos(Pitch),sin(Pitch)},
        {0,-sin(Pitch),cos(Pitch)}
    };
    double Mz[3][3]={
        {cos(Az),sin(Az),0},
        {sin(Az),cos(Az),0},
        {0,0,1}
    };

    MatByMat(My,Mx,M);
    MatByMat(M,Mz,M);
}
