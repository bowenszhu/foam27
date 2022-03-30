#include <fstream>
#include <iostream>
#include <math.h>
#include <iomanip>
#include <cstdlib>
#include "TEdgeSpace.h"

using namespace std;

void compute_indices(int indices[2][3], const int i, const int j, const int n) {
    indices[0][0] = (2 * j * n + i) * 3;
    indices[0][1] = ((2 * j + 1) * n + i) * 3;
    indices[0][2] = (2 * (j + 1) * n + i) * 3;
    indices[1][0] = (2 * j * n + (i + 1) % n) * 3;
    indices[1][1] = ((2 * j + 1) * n + (i + 1) % n) * 3;
    indices[1][2] = (2 * (j + 1) * n + (i + 1) % n) * 3;
}

int main(int argc, char *argv[]) {
    ofstream meshFile("blockMeshDict");
    meshFile << std::fixed << setprecision(12)
             << "/*--------------------------------*- C++ -*----------------------------------*\\\n"
                "  =========                 |\n"
                "  \\\\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox\n"
                "   \\\\    /   O peration     | Website:  https://openfoam.org\n"
                "    \\\\  /    A nd           | Version:  9\n"
                "     \\\\/     M anipulation  |\n"
                "\\*---------------------------------------------------------------------------*/\n\n"
                "FoamFile\n"
                "{\n"
                "    version     8;\n"
                "    format      ascii;\n"
                "    class       dictionary;\n"
                "    object      blockMeshDict;\n"
                "}\n"
                "// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //\n"
                "\n"
                "convertToMeters 0.001;\n"
                "\n"
                "vertices\n"
                "(\n";

    const double R_outer = 21.59; /// outer radius
    const double AR = 1.00; /// tan of the angle between
    const double lambda = 2.00; /// lambda
    const double lambda_2 = lambda / 2.; /// lambda / 2
    const double R_inner = 14.00; /// inner radius
    const double A = lambda_2 * AR;

    // read arguments if provided
    const double alpha = argc > 1 ? std::atof(argv[1]) : .15; /// angle parameter of denticle [rad]
    const int n = argc > 2 ? std::atoi(argv[2]) : 10; /// number of blocks in a circle
    const int n_R_outer = argc > 3 ? std::atoi(argv[3]) : 24; /// number of cells in R direction in outer part
    const int n_R_denticle = argc > 4 ? std::atoi(argv[4]) : 10; /// number of cells in R direction in denticle part
    const int n_theta = argc > 5 ? std::atoi(argv[5]) : 69; /// number of cells in theta direction for both parts
    const int n_z = argc > 6 ? std::atoi(argv[6]) : 7; /// number of cells in z direction
    cout << "Constructing blockMeshDict\n"
            "angle of denticle, alpha = "
         << alpha
         << " [rad]\n"
            "number of blocks in a circle = "
         << n
         << "\n"
            "number of cells in R direction in outer zone = "
         << n_R_outer
         << "\n"
            "number of cells in R direction in denticle zone = "
         << n_R_denticle
         << "\n"
            "number of cells in theta direction = "
         << n_theta
         << "\n"
            "number of cells in z direction = "
         << n_z
         << "\n";

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
    ES.calc("de", // [arg1] end cell size
            "n", // [arg2] number of cells
            R_outer - R_valley_max, // length
            (R_peak_max - R_peak_min) / n_R_denticle, // [arg1]
            n_R_outer); // [arg2]
    const double r_outer_R = ES.getr(); // expansion ratio in R direction in outer part

    for (int j = 0; j < m; j++) {
        const double z_peak = lambda * j;
        const double z_valley = z_peak + lambda_2;
        for (int i = 0; i < n; i++) {
            const double theta = ang_radian * i;
            const double c = cos(theta);
            const double s = sin(theta);
            const double x_outer = R_outer * c;
            const double y_outer = R_outer * s;
            const double x_peak_max = R_peak_max * c;
            const double y_peak_max = R_peak_max * s;
            const double x_peak_min = R_peak_min * c;
            const double y_peak_min = R_peak_min * s;
            meshFile << "    ("
                     << x_outer << " "
                     << y_outer << " "
                     << z_peak << ")\n"
                     << "    ("
                     << x_peak_max << " "
                     << y_peak_max << " "
                     << z_peak << ")\n"
                     << "    ("
                     << x_peak_min << " "
                     << y_peak_min << " "
                     << z_peak << ")\n";
        }
        for (int i = 0; i < n; i++) {
            const double theta = ang_radian * i;
            const double c = cos(theta);
            const double s = sin(theta);
            const double x_outer = R_outer * c;
            const double y_outer = R_outer * s;
            const double x_valley_max = R_valley_max * c;
            const double y_valley_max = R_valley_max * s;
            const double x_valley_min = R_valley_min * c;
            const double y_valley_min = R_valley_min * s;
            meshFile << "    ("
                     << x_outer << " "
                     << y_outer << " "
                     << z_valley << ")\n"
                     << "    ("
                     << x_valley_max << " "
                     << y_valley_max << " "
                     << z_valley << ")\n"
                     << "    ("
                     << x_valley_min << " "
                     << y_valley_min << " "
                     << z_valley << ")\n";
        }
    }
    const double z_peak_bottom = m * lambda;
    for (int i = 0; i < n; i++) {
        const double theta = ang_radian * i;
        const double c = cos(theta);
        const double s = sin(theta);
        const double x_outer = R_outer * c;
        const double y_outer = R_outer * s;
        const double x_peak_max = R_peak_max * c;
        const double y_peak_max = R_peak_max * s;
        const double x_peak_min = R_peak_min * c;
        const double y_peak_min = R_peak_min * s;
        meshFile << "    ("
                 << x_outer << " "
                 << y_outer << " "
                 << z_peak_bottom << ")\n"
                 << "    ("
                 << x_peak_max << " "
                 << y_peak_max << " "
                 << z_peak_bottom << ")\n"
                 << "    ("
                 << x_peak_min << " "
                 << y_peak_min << " "
                 << z_peak_bottom << ")\n";
    }
    meshFile << ");\n"
                "\n";
    int indices[2][3];
    meshFile << "blocks\n"
                "(\n";
    for (int j = 0; j < m; j++) {
        for (int i = 0; i < n; i++) {
            compute_indices(indices, i, j, n);
            meshFile << "     hex ("
                     << indices[0][0] << " "
                     << indices[0][0] + 1 << " "
                     << indices[0][1] + 1 << " "
                     << indices[0][1] << " "
                     << indices[1][0] << " "
                     << indices[1][0] + 1 << " "
                     << indices[1][1] + 1 << " "
                     << indices[1][1] << ") ("
                     << n_R_outer << " " << n_z << " " << n_theta << ") simpleGrading (" << r_outer_R << " 1 1)\n"
                     << "     hex ("
                     << indices[0][1] << " "
                     << indices[0][1] + 1 << " "
                     << indices[0][2] + 1 << " "
                     << indices[0][2] << " "
                     << indices[1][1] << " "
                     << indices[1][1] + 1 << " "
                     << indices[1][2] + 1 << " "
                     << indices[1][2] << ") ("
                     << n_R_outer << " " << n_z << " " << n_theta << ") simpleGrading (" << r_outer_R << " 1 1)\n"
                     << "     hex ("
                     << indices[1][0] + 1 << " "
                     << indices[1][1] + 1 << " "
                     << indices[1][1] + 2 << " "
                     << indices[1][0] + 2 << " "
                     << indices[0][0] + 1 << " "
                     << indices[0][1] + 1 << " "
                     << indices[0][1] + 1 << " "
                     << indices[0][0] + 1 << ") innerCylinder (" // cellZone
                     << n_z << " " << n_R_denticle << " " << n_theta << ") simpleGrading (1 1 1)\n"
                     << "     hex ("
                     << indices[1][1] + 1 << " "
                     << indices[1][2] + 1 << " "
                     << indices[1][2] + 2 << " "
                     << indices[1][1] + 2 << " "
                     << indices[0][1] + 1 << " "
                     << indices[0][2] + 1 << " "
                     << indices[0][2] + 1 << " "
                     << indices[0][1] + 1 << ") innerCylinder (" // cellZone
                     << n_z << " " << n_R_denticle << " " << n_theta << ") simpleGrading (1 1 1)\n";
        }
    }
    meshFile << ");\n"
                "\n";
    meshFile << "edges\n"
                "(\n";
    for (int i = 0; i < n; i++) {
        const double ang_block_1 = 2. * M_PI * (i + 1) / n;
        double x_peak_poly[N_poly];
        double y_peak_poly[N_poly];
        double x_valley_poly[N_poly];
        double y_valley_poly[N_poly];
        for (int k = 0; k < N_poly; k++) {
            const double ang_local = ang_poly * (k + 1);
            const double ang_global = ang_block_1 - ang_local;
            const double R_peak_poly = R_peak_min * exp(tan_alpha * ang_local);
            const double R_valley_poly = R_peak_poly - A;
            const double c = cos(ang_global);
            const double s = sin(ang_global);
            x_peak_poly[k] = R_peak_poly * c;
            y_peak_poly[k] = R_peak_poly * s;
            x_valley_poly[k] = R_valley_poly * c;
            y_valley_poly[k] = R_valley_poly * s;
        }
        for (int j = 0; j < m; j++) {
            compute_indices(indices, i, j, n);
            meshFile << "    arc " // outer peak
                     << indices[0][0] << " "
                     << indices[1][0] << " "
                     << ang_degree << " (0 0 1)\n"
                     << "    arc " // inner peak R_max
                     << indices[0][0] + 1 << " "
                     << indices[1][0] + 1 << " "
                     << ang_degree << " (0 0 1)\n";
            meshFile << "    polyLine " // denticle peak
                     << indices[1][0] + 2 << " "
                     << indices[0][0] + 1 << " (";
            const double z_peak = lambda * j;
            for (int k = 0; k < N_poly; k++)
                meshFile << "(" << x_peak_poly[k] << " " << y_peak_poly[k] << " " << z_peak << ") ";
            meshFile << ")\n";
            meshFile << "    arc " // outer valley
                     << indices[0][1] << " "
                     << indices[1][1] << " "
                     << ang_degree << " (0 0 1)\n"
                     << "    arc " // inner valley R_max
                     << indices[0][1] + 1 << " "
                     << indices[1][1] + 1 << " "
                     << ang_degree << " (0 0 1)\n";
            meshFile << "    polyLine " // denticle valley
                     << indices[1][1] + 2 << " "
                     << indices[0][1] + 1 << " (";
            const double z_valley = z_peak + lambda_2;
            for (int k = 0; k < N_poly; k++)
                meshFile << "(" << x_valley_poly[k] << " " << y_valley_poly[k] << " " << z_valley << ") ";
            meshFile << ")\n";
        }
        const double z_peak_bottom = m * lambda;
        const int j = m;
        compute_indices(indices, i, j, n);
        meshFile << "    arc " // bottom outer peak
                 << indices[0][0] << " "
                 << indices[1][0] << " "
                 << ang_degree << " (0 0 1)\n"
                 << "    arc " // inner peak R_max
                 << indices[0][0] + 1 << " "
                 << indices[1][0] + 1 << " "
                 << ang_degree << " (0 0 1)\n";
        meshFile << "    polyLine " // bottom denticle peak
                 << indices[1][0] + 2 << " "
                 << indices[0][0] + 1 << " (";
        for (int k = 0; k < N_poly; k++)
            meshFile << "(" << x_peak_poly[k] << " " << y_peak_poly[k] << " " << z_peak_bottom << ") ";
        meshFile << ")\n";
    }
    meshFile << ");\n"
                "\n";
    meshFile << "boundary\n"
                "(\n";
    meshFile << "    innerCylinder\n"
                "    {\n"
                "        type wall;\n"
                "        faces\n"
                "        (\n";
    for (int j = 0; j < m; j++) {
        for (int i = 0; i < n; i++) {
            compute_indices(indices, i, j, n);
            meshFile << "            ("
                     << indices[0][0] + 1 << " "
                     << indices[0][1] + 1 << " "
                     << indices[1][1] + 2 << " "
                     << indices[1][0] + 2 << ")\n"
                     << "            ("
                     << indices[0][1] + 1 << " "
                     << indices[0][2] + 1 << " "
                     << indices[1][2] + 2 << " "
                     << indices[1][1] + 2 << ")\n"
                     << "            ("
                     << indices[0][0] + 1 << " "
                     << indices[0][0] + 2 << " "
                     << indices[0][1] + 2 << " "
                     << indices[0][1] + 1 << ")\n"
                     << "            ("
                     << indices[0][1] + 1 << " "
                     << indices[0][1] + 2 << " "
                     << indices[0][2] + 2 << " "
                     << indices[0][2] + 1 << ")\n";
        }
    }
    meshFile << "        );\n"
                "    }\n";
    meshFile << "    outerCylinder\n"
                "    {\n"
                "        type wall;\n"
                "        faces\n"
                "        (\n";
    for (int j = 0; j < m; j++) {
        for (int i = 0; i < n; i++) {
            compute_indices(indices, i, j, n);
            meshFile << "            ("
                     << indices[0][1] << " "
                     << indices[0][0] << " "
                     << indices[1][0] << " "
                     << indices[1][1] << ")\n"
                     << "            ("
                     << indices[0][2] << " "
                     << indices[0][1] << " "
                     << indices[1][1] << " "
                     << indices[1][2] << ")\n";
        }
    }
    meshFile << "        );\n"
                "    }\n";
    meshFile << "    front\n"
                "    {\n";
    meshFile << "         type cyclic;\n"
                "         neighbourPatch back;\n"
                "         faces\n"
                "         (\n";
    for (int i = 0; i < n; i++) {
        compute_indices(indices, i, 0, n);
        meshFile << "            ("
                 << indices[0][0] << " "
                 << indices[0][0] + 1 << " "
                 << indices[1][0] + 1 << " "
                 << indices[1][0] << ")\n"
                 << "            ("
                 << indices[0][0] + 1 << " "
                 << indices[0][0] + 1 << " "
                 << indices[1][0] + 2 << " "
                 << indices[1][0] + 1 << ")\n";
    }
    meshFile << "         );\n"
                "    }\n";
    meshFile << "    back\n"
                "    {\n";
    meshFile << "         type cyclic;\n"
                "         neighbourPatch front;\n"
                "         faces\n"
                "         (\n";
    for (int i = 0; i < n; i++) {
        compute_indices(indices, i, m, n);
        meshFile << "            ("
                 << indices[0][0] + 1 << " "
                 << indices[0][0] << " "
                 << indices[1][0] << " "
                 << indices[1][0] + 1 << ")\n"
                 << "            ("
                 << indices[0][0] + 1 << " "
                 << indices[0][0] + 1 << " "
                 << indices[1][0] + 1 << " "
                 << indices[1][0] + 2 << ")\n";
    }
    meshFile << "         );\n"
                "    }\n";
    meshFile << ");\n"
                "\n";
    meshFile << "mergePatchPairs\n"
                "(\n"
                ");\n"
                "\n";
    meshFile << "// ************************************************************************* //\n";
    meshFile.close();
    return 0;
}