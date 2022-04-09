#include <fstream>
#include <iostream>
#include <math.h>
#include <iomanip>
#include <cstdlib>
#include "TEdgeSpace.h"

int main(int argc, char *argv[]) {
    const double R_outer = 21.59; /// outer radius
    const double AR = 1.00; /// tan of the angle between
    const double lambda = 2.00; /// lambda
    const double lambda_2 = lambda / 2.; /// lambda / 2
    const double R_inner = 14.00; /// inner radius
    const double A = lambda_2 * AR;
    const double alpha = argc > 1 ? std::atof(argv[1]) : .15; /// angle parameter of denticle [rad]
    const int n = argc > 2 ? std::atoi(argv[2]) : 10; /// number of blocks in a circle
    const int n_R_outer = argc > 3 ? std::atoi(argv[3]) : 24; /// number of cells in R direction in outer part
//    const int n_R_denticle = argc > 4 ? std::atoi(argv[4]) : 10; /// number of cells in R direction in denticle part
    const int n_theta = argc > 5 ? std::atoi(argv[5]) : 69; /// number of cells in theta direction for both parts
    const int n_z = argc > 6 ? std::atoi(argv[6]) : 7; /// number of cells in z direction
    const int m = 1; /// number of cells (2 blocks) in z_peak_bottom direction
    const double ang_radian = M_PI * 2. / n;
    const double ang_degree = 360. / n;
    const double tan_alpha = tan(alpha);
    const double temp = tan_alpha * 4. * M_PI / n;
    const double R_peak_min = R_inner * sqrt(temp / (exp(temp) - 1.));
    const double R_peak_max = R_peak_min * exp(tan_alpha * ang_radian);
    const double R_valley_min = R_peak_min - A;
    const double R_valley_max = R_peak_max - A;
    const int N_poly = 100; /// number of points for polyline
    const double ang_poly = ang_radian / (N_poly + 1);

    TEdgeSpace ES; // a grading calculator

    ofstream gradingFile("grading.txt");

    for (int n_R_denticle = 10; n_R_denticle <= 40; n_R_denticle++) {
        ES.calc("de", // [arg1] end cell size
                "n", // [arg2] number of cells
                R_outer - R_valley_max, // length
                (R_peak_max - R_peak_min) / n_R_denticle, // [arg1]
                n_R_outer);
        double r_outer_R = ES.getr();
        gradingFile << n_R_denticle << " " << r_outer_R << "\n";
    }


    gradingFile.close();
    return 0;
}
