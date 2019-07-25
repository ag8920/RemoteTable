#ifndef A_MATH_H
#define A_MATH_H

//class ThreePosAlg
//{
//public:
//    ThreePosAlg();
//private:
//    void turnSensorData();
//    void recalcDVt();
//    double recalcG();
//    double recalcPitchBIP();
//    double recalcRollBIP();
//    double recalcAzimuthBIP();

//};

void MatByMat(double mat1[][3], double mat2[][3], double res[][3]);
void MyMxMz(double Roll, double Pitch, double Az, double res[][3]);
double AD (double mat[][3], int row, int col);
void InvMat (double mat1[][3], double res[][3]);
void transMatrix(double matrx[][3], int numrows, int numcols);
#endif // A_MATH_H
