#include <iostream>
#include <fstream>
#include <iosfwd>
#include <math.h>
using namespace std;
#include <stdio.h>
#include <errno.h>
#include <fenv.h>
#define pi acos(-1)
#include <iomanip>

int main ()
{
    bool flat = false;
	// Radius Ratio
    double eta = 0.65;
    double OR = 21.59; //22.225;
    double AR = 1.00; //2.5;  //1.00
    double L_2 = 1.00;
    double IR = 14.00;
    double A = L_2*AR;
    double MR = IR-A;
    double k = 0.00;  //-0.75
    double JJ = -3.50;  //-3.00
    double s = 2.40;
    double p = 0.00;
    double q = 0.00;
    //double IR = 14;
    int Nr = 60;
    int Nz = 40; //40 per 1 mm 
    int n =1;
    int N = 2*n+1;
    int Npoly = 100;
   // setprecision(10);
    std::ofstream output("blockMeshDict");
    output << std::fixed << setprecision(12) << endl;
    output << "/" << "*--------------------------------*- C++ -*----------------------------------*" <<"\\"  << endl;
    output << "| =========                 |                                                 |" << endl;
    output << "| \\      /  F ield         | foam-extend: Open Source CFD                    |" << endl;
    output << "|  \\    /   O peration     | Version:     3.0                                |" << endl;
    output << "|   \\  /    A nd           | Web:         http://www.extend-project.de       |" << endl;
    output << "|    \\/     M anipulation  |                                                 |" << endl;
    output << "\\" << "*---------------------------------------------------------------------------*/" << endl;
    output << "FoamFile" << endl;
    output << "{" << endl;
    output << "    version     2.0;" << endl;
    output << "    format      ascii;" << endl;
    output << "    class       dictionary;" << endl;
    output << "    object      blockMeshDict;" << endl;
    output << "}" << endl;
    output << "// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //" << endl;
    output << endl;
    output << "convertToMeters 0.001;" << endl;
    output << endl;
    // variables and calculations
    output << "//eta " << eta << ";" << endl;
    output << "//OR " << OR << ";" << endl;  // Outer Radius
    output << "//IR " << IR <<";" << endl; // Inner Radius
    output << "//AR " << AR << ";" << endl; // aspect Ratio - 1st order surface parameter
    output << "//k " << k << ";" << endl; // Curvature - 2nd order surface parameter.
    output << "//j " << JJ << ";" << endl; // Jerk - 3rd order surface parameter
    output << "//s " << s << ";" << endl; // s - 4th order surface parameter
    output << "//p " << p << ";" << endl; // s - 4th order surface parameter
    output << "//q " << q << ";" << endl; // s - 4th order surface parameter


    //output << "IR " << IR << ";" << endl;
   /* output << "PI " << pi << ";" << endl;  // number pi (3.1415 ...)
    output << "Nr " << Nr << ";" << endl;  // Radius mesh
    output << "Nz " << Nz << ";" << endl;  // Axial mesh
    output << endl;
    // size of the v-groove



    output << "lambda_2 " << 0.5 <<";" << endl;  //Magnitude of half of the wavelength
    if (flat)
    {
        output << "A #calc " << "\"0*$IR\";" << endl;
    }
    else
    {
        output << "A " << A <<";" << endl;
    };

    output << endl;
    output << "MR " << MR <<";" << endl;*/




    double Zmid[Npoly];
    double Rmid[Npoly];
    double XmidB[Npoly];
    double YmidB[Npoly];
    double XmidF[Npoly];
    double YmidF[Npoly];

    for (int i = 1 ; i < Npoly ; i ++)
    {
        Zmid[i] = i*L_2/Npoly;
        cout << Zmid[i] << endl;
      //  output << "Zmid" << i << " " << Zmid[i] << ";" << endl;
        Rmid[i] =  MR + q/(pow(L_2,5))*pow(Zmid[i],6) + (p-3.00*q)/(pow(L_2,4))*pow(Zmid[i],5) + (s-5.00*p/2+2.500*q)/(pow(L_2,3))*pow(Zmid[i],4) + (JJ-2*s+5.00*p/3)/(pow(L_2,2))*pow(Zmid[i],3) + ((2*k-3.00*JJ+2.00*s-q)/2)/L_2* pow(Zmid[i],2) +(JJ/2- (k+AR)-1.00*p/6)*(Zmid[i]) + L_2*AR;
        cout << Rmid[i] << endl;
        XmidB[i] = Rmid[i]*cos(-0.003);
        YmidB[i] = Rmid[i]*sin(-0.003);
        XmidF[i] = Rmid[i]*cos(0.003);
        YmidF[i] = Rmid[i]*sin(0.003);

     /*   output << "Rmid" << i << " " << Rmid[i] << ";" << endl;
        output << "Xmid" << i << "_B " << Rmid[i]*cos(-0.003) << ";" << endl;
        output << "Ymid" << i << "_B " << Rmid[i]*sin(-0.003) << ";"  << endl;
        output << "Xmid" << i << "_F " << Rmid[i]*cos(0.003) << ";"  << endl;
        output << "Ymid" << i << "_F " << Rmid[i]*sin(0.003) << ";"  << endl;*/
    };
    double H[N];
    double Hmid[N][Npoly];

    for (int i = 0 ; i < N ; i ++)
    {
       // output << "H" << i << " \"" << i*L_2 <<";" << endl;
        H[i] = i*L_2;
        cout << H[i] << endl;
        for (int j = 1 ; j < Npoly ; j ++)
        {
           // output << "Hmid" << i << "_" << j << " " << i*L_2+Zmid[j] << ";" << endl;
            Hmid[i][j] = i*L_2+Zmid[j];
        };

      //  output << "Hmid2" << i << " #calc \"" << i << "*$lambda_2 + 2 * $lambda_2/3\";" << endl;
    };

    float ang = 0.003;

  //  output << "Ntheta " << 1 << ";";//\"int(2*($PI*$OR*$Nr)/(($OR-$MR)))\";" << endl;


    // Mid points, used for the arc function

    double A1x = IR * 1;//cos*(0.0);
    double A1y = IR * 0;//sin(0.0);
    double A2x = OR * 1;//cos(0.0);
    double A2y = OR * 0;//sin(0.0);
    double A3x = MR * 1;//cos(0.0);
    double A3y = MR * 0;//sin(0.0);


   /* output << "A1x " << IR * cos(0.0) <<";" << endl;
    output << "A1y " << IR * sin(0.0) <<";" << endl;
    output << endl;
    output << "A2x " << OR * cos(0.0) <<";" << endl;
    output << "A2y " << OR * sin(0.0) <<";" << endl;
    output << endl;
    output << "A3x " << MR * cos(0.0) <<";" << endl;
    output << "A3y " << MR * sin(0.0) <<";" << endl;
	output << endl;*/


    double X1_1 = IR * cos(-0.003);
    double Y1_1 = IR * sin(-0.003);
    double X2_1 = OR * cos(-0.003);
    double Y2_1 = OR * sin(-0.003);
    double X2_2 = OR * cos(0.003);
    double Y2_2 = OR * sin(0.003);
    double X1_2 = IR * cos(0.003);
    double Y1_2 = IR * sin(0.003);
    double X3_1 = MR * cos(-0.003);
    double Y3_1 = MR * sin(-0.003);
    double X3_2 = MR * cos(0.003);
    double Y3_2 = MR * sin(0.003);


  /*  output << "X1_1 "  << "\"$IR * cos(-0.003)\";"  << endl;
    output << "Y1_1 "  << "\"$IR * sin(-0.003)\";"  << endl;
    output << "X2_1 "  << "\"$OR * cos(-0.003)\";"  << endl;
    output << "Y2_1 "  << "\"$OR * sin(-0.003)\";"  << endl;
    output << "X2_2 #calc "  << "\"$OR * cos(0.003)\";"  << endl;
    output << "Y2_2 #calc "  << "\"$OR * sin(0.003)\";"  << endl;
    output << "X1_2 #calc "  << "\"$IR * cos(0.003)\";"  << endl;
    output << "Y1_2 #calc "  << "\"$IR * sin(0.003)\";"  << endl;
    output << "X3_1 #calc "  << "\"$MR * cos(-0.003)\";"  << endl;
    output << "Y3_1 #calc "  << "\"$MR * sin(-0.003)\";"  << endl;
    output << "X3_2 #calc "  << "\"$MR * cos(0.003)\";"  << endl;
    output << "Y3_2 #calc "  << "\"$MR * sin(0.003)\";"  << endl;*/




    // vertices
    output << "vertices" << endl;
    output << "(" << endl;
    for (int i = 1 ; i < N+1 ; i ++)
    {
            if (i%2==1)
            {
                output << "     (" << X1_1 << " " << Y1_1 << " " << H[i-1] << ")" << endl;
                cout << H[i-1] << endl;
                output << "     (" << X2_1 << " " << Y2_1 << " " << H[i-1] << ")" << endl;
                output << "     (" << X2_2 << " " << Y2_2 << " " << H[i-1] << ")" << endl;
                output << "     (" << X1_2 << " " << Y1_2 << " " << H[i-1] << ")" << endl;
            }
            else
            {
                output << "     (" << X3_1 << " " << Y3_1 << " " << H[i-1] << ")" << endl;
                output << "     (" << X2_1 << " " << Y2_1 << " " << H[i-1] << ")" << endl;
                output << "     (" << X2_2 << " " << Y2_2 << " " << H[i-1] << ")" << endl;
                output << "     (" << X3_2 << " " << Y3_2 << " " << H[i-1] << ")" << endl;
            }

    };
    output << ");" << endl;
    output << endl;
    // blocks
    output << "blocks" << endl;
    output << "(" << endl;
    for (int i = 0 ; i < N-1 ; i ++)
    {
    //(10 1 ((0.5 0.5 2) (0.5 0.5 0.5)))
            output << "     hex (" << i*4+0 << " " << i*4+1 << " " << i*4+2 << " " << i*4+3 << " " << i*4+4 << " "<< i*4+5 << " "<< i*4+6 << " "<< i*4+7 << ") (60 1 80) simpleGrading (10 1 1)" << endl;
    }
    output << ");" << endl;
    output << endl;
    // edges
    output << "edges" << endl;
    output << "(" << endl;
    for (int i = 0 ; i < N-1 ; i ++ )
    {
        if (i%2==0)
        {
            output << "     arc " << i*4+3 << " " << i*4+0 <<  "(" << A1x << " " << A1y << " " << H[i] << ")" << endl;
            output << "     polyLine " << i * 4 + 0 << " " << i * 4 + 4 << "(";
            for (int j = 1 ; j < Npoly ; j ++)
            {
                output << " (" << XmidB[j] << " " << YmidB[j] << " " << Hmid[i][j] << ") ";
            }
            output << ")" << endl;
            output << "     polyLine " << i * 4 + 3 << " " << i * 4 + 7 << "(";
            for (int j = 1 ; j < Npoly ; j ++)
            {
                output << " (" << XmidF[j] << " " << YmidF[j] << " " << Hmid[i][j] << ") ";
            }
            output << ")" << endl;
        }
        else
        {
            output << "     arc " << i*4+0 << " " << i*4+3 <<  "(" << A3x <<" " << A3y << " " << H[i] << ")" << endl;
            output << "     polyLine " << i * 4 + 0 << " " << i * 4 + 4 << "(";
            for (int j = 1 ; j < Npoly ; j ++)
            {
               // output << " ($Xmid" << (Npoly-j) << "_B " << "$Ymid" << (Npoly-j) << "_B " << "$Hmid" << i << "_" << (j) << ") ";
                output << " (" << XmidB[Npoly-j] << " " << YmidB[Npoly-j] << " " << Hmid[i][j] << ") ";
            }
            output << ")" << endl;
            output << "     polyLine " << i * 4 + 3 << " " << i * 4 + 7 << "(";
            for (int j = 1 ; j < Npoly ; j ++)
            {
                output << " (" << XmidF[Npoly-j] << " " << YmidF[Npoly-j] << " " << Hmid[i][j] << ") ";
             //   output << " ($Xmid" << (Npoly-j) << "_F " << "$Ymid" << (Npoly-j) << "_F " << "$Hmid" << i << "_" << (j) << ") ";
            }
            output << ")" << endl;
        }

    };
    for (int i = 0 ; i < N ; i ++ )
    {
        if (i%2==0)
        {
           output << "     arc " << i*4+1 << " " << i*4+2 <<  "(" << A2x <<" " << A2y << " " << H[i] << ")" << endl;
        }
        else
        {
           output << "     arc " << i*4+1 << " " << i*4+2 <<  "(" << A2x <<" " << A2y << " " << H[i] << ")" << endl;
        }
    };
    output << ");" << endl;
    output << endl;
    // bounday
    output << "boundary" << endl;
    output << "(" << endl;
    // innnerCylinder
    output << "     innerCylinder" << endl;
    output << "     {" << endl;
    output << "         type wall;" << endl;
    output << "         faces" << endl;
    output << "         (" << endl;
    for (int i = 0 ; i < N-1 ; i ++)
    {
        output << "             (" << i*4+0 << " " << i*4+4 << " " << i*4+7 << " " << i*4+3 << ")" << endl;
    };
    output << "         );" << endl;
    output << "     }" << endl;
    // outerCylinder
    output << "     outerCylinder" << endl;
    output << "     {" << endl;
    output << "         type wall;" << endl;
    output << "         faces" << endl;
    output << "         (" << endl;
    for (int i = 0 ; i < N-1 ; i ++)
    {
        output << "             (" << i*4+2 << " " << i*4+6 << " " << i*4+5 << " " << i*4+1 << ")" << endl;
    };
    output << "         );" << endl;
    output << "     }" << endl;
    // down
    output << "     down" << endl;
    output << "     {" << endl;
    output << "         type wedge;" << endl;
   // output << "         neighbourPatch left;" << endl;
    output << "         faces" << endl;
    output << "         (" << endl;
    for (int i = 0 ; i < N-1 ; i ++)
    {
        output << "             (" << i*4+0 << " " << i*4+1 << " " << i*4+5 << " " << i*4+4 << ")" << endl;
    };
    output << "         );" << endl;
    output << "     }" << endl;
    // left
    output << "     left" << endl;
    output << "     {" << endl;
    output << "         type wedge;" << endl;
    //output << "         neighbourPatch down;" << endl;
    output << "         faces" << endl;
    output << "         (" << endl;
    for (int i = 0 ; i < N-1 ; i ++)
    {
        output << "             (" << i*4+7 << " " << i*4+6 << " " << i*4+2 << " " << i*4+3 << ")" << endl;
    };
    output << "         );" << endl;
    output << "     }" << endl;
    // front
    output << "     front" << endl;
    output << "     {" << endl;
        if (flat)
    {
        output << "         type wedge;" << endl;
        //output << "         type cyclic;" << endl;
        //output << "         neighbourPatch back;" << endl;
    }
    else
    {
        output << "         type cyclic;" << endl;
        output << "         neighbourPatch back;" << endl;
    }
    output << "         faces" << endl;
    output << "         (" << endl;
    output << "             (" << 0*4+0 << " " << 0*4+3 << " " << 0*4+2 << " " << 0*4+1 << ")" << endl;
    output << "         );" << endl;
    output << "     }" << endl;
    // back
    output << "     back" << endl;
    output << "     {" << endl;
    if (flat)
    {
        //output << "         type wall;" << endl;
        output << "         type cyclic;" << endl;
        output << "         neighbourPatch front;" << endl;
    }
    else
    {
        output << "         type cyclic;" << endl;
        output << "         neighbourPatch front;" << endl;
    }
    output << "         faces" << endl;
    output << "         (" << endl;
    output << "             (" << (N-1)*4+0 << " " << (N-1)*4+1 << " " << (N-1)*4+2 << " " << (N-1)*4+3 << ")" << endl;
    output << "         );" << endl;
    output << "     }" << endl;
    output << ");" << endl;
    // mergePatchPairs
    output << "mergePatchPairs" << endl;
    output << "(" << endl;
    output << ");" << endl;
    output << endl;
    output << "// ************************************************************************* //" << endl;
        output.close();
    return 0;

}
