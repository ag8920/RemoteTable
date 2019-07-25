#include <iostream>
#include <string>
#include <math.h>
#include <random>

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
        {cos(Az),-sin(Az),0},
        {sin(Az),cos(Az),0},
        {0,0,1}
    };

    MatByMat(My,Mx,M);
    MatByMat(M,Mz,M);
    memcpy(res,M,sizeof (M));
}
double AD (double mat[][3], int row, int col) // algebraic adjunct
{   int rowc=0; double M[2][2];
    for(int i=0;i<3;i++) if(i!=row)
        {   int colc=0;
            for(int j=0;j<3;j++) if(j!=col) { M[rowc][colc]=mat[i][j]; colc++; } // minor
            rowc++;
        }double Mdelta=M[0][0]*M[1][1]-M[1][0]*M[0][1];
    if((row+col)%2==0) Mdelta*=(-1);
    return Mdelta;//(Mdelta*pow(-1,row+col)); // Aij
}
void InvMat (double mat1[][3], double res[][3]) // A^-1=Aij^T/delta
{   double delta=0, temp[3][3];
    for(int k=0;k<3;k++) {delta+=mat1[0][k]*AD(mat1,0,k);}
    if(delta!=0.) for(int i=0;i<3;i++) for(int j=0;j<3;j++) temp[j][i]=AD(mat1,i,j)/delta;
    memcpy(res,temp,sizeof(temp));
}



void transMatrix(double matrx[][3], int numrows, int numcols)
{
    for(size_t row = 0; row < numrows - 1; row++){
        for(size_t col = row + 1; col < numcols; col++)
            std::swap(matrx[col][row], matrx[row][col]);
    }
}


double RandomAzimuth(double azimuth,double sko,std::random_device rd)
{
//    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<>normdist{0.0,sko };
    return azimuth+normdist(gen);
}
