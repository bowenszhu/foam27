#include <fstream>
#include <iostream>
#include <math.h>
#include <iomanip>

using namespace std;

void compute_indices(int indices[2][3], const int i, const int j, const int n) {
    indices[0][0] = (2 * j * n + i) * 2;
    indices[0][1] = ((2 * j + 1) * n + i) * 2;
    indices[0][2] = (2 * (j + 1) * n + i) * 2;
    indices[1][0] = (2 * j * n + (i + 1) % n) * 2;
    indices[1][1] = ((2 * j + 1) * n + (i + 1) % n) * 2;
    indices[1][2] = (2 * (j + 1) * n + (i + 1) % n) * 2;
}

int main() {
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

    double R_outer = 21.59; /// outer radius
    double AR = 1.00; /// tan of the angle between
    double lambda = 2.00; /// lambda
    double lambda_2 = lambda / 2.; /// lambda / 2
    double R_peak_inner = 14.00; /// inner radius
    double A = lambda_2 * AR;
    double R_valley_inner = R_peak_inner - A;
    int n = 10; /// number of blocks in a circle
    int m = 1; /// number of cells (2 blocks) in z_peak_bottom direction
    double ang_radian = M_PI * 2. / n;
    double ang_degree = 360. / n;

    int v_i = 0; /// vertex index
    for (int j = 0; j < m; j++) {
        double z_peak = lambda * j;
        double z_valley = z_peak + lambda_2;
        for (int i = 0; i < n; i++) {
            double theta = ang_radian * i;
            double c = cos(theta);
            double s = sin(theta);
            double x_outer = R_outer * c;
            double y_outer = R_outer * s;
            double x_peak_inner = R_peak_inner * c;
            double y_peak_inner = R_peak_inner * s;
            meshFile << "    ("
                     << x_outer << " "
                     << y_outer << " "
                     << z_peak << ") // vertex " << v_i++ << "\n"
                     << "    ("
                     << x_peak_inner << " "
                     << y_peak_inner << " "
                     << z_peak << ") // vertex " << v_i++ << "\n";
        }
        for (int i = 0; i < n; i++) {
            double theta = ang_radian * i;
            double c = cos(theta);
            double s = sin(theta);
            double x_outer = R_outer * c;
            double y_outer = R_outer * s;
            double x_valley_inner = R_valley_inner * c;
            double y_valley_inner = R_valley_inner * s;
            meshFile << "    ("
                     << x_outer << " "
                     << y_outer << " "
                     << z_valley << ") // vertex " << v_i++ << "\n"
                     << "    ("
                     << x_valley_inner << " "
                     << y_valley_inner << " "
                     << z_valley << ") // vertex " << v_i++ << "\n";
        }
    }
    double z_peak_bottom = m * lambda;
    for (int i = 0; i < n; i++) {
        double theta = ang_radian * i;
        double c = cos(theta);
        double s = sin(theta);
        double x_outer = R_outer * c;
        double y_outer = R_outer * s;
        double x_peak_inner = R_peak_inner * c;
        double y_peak_inner = R_peak_inner * s;
        meshFile << "    ("
                 << x_outer << " "
                 << y_outer << " "
                 << z_peak_bottom << ") // vertex " << v_i++ << "\n"
                 << "    ("
                 << x_peak_inner << " "
                 << y_peak_inner << " "
                 << z_peak_bottom << ") // vertex " << v_i++ << "\n";
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
                     << indices[1][1] << ") (10 20 30) simpleGrading (0.5 3 1)\n"
                     << "     hex ("
                     << indices[0][1] << " "
                     << indices[0][1] + 1 << " "
                     << indices[0][2] + 1 << " "
                     << indices[0][2] << " "
                     << indices[1][1] << " "
                     << indices[1][1] + 1 << " "
                     << indices[1][2] + 1 << " "
                     << indices[1][2] << ") (10 20 30) simpleGrading (0.5 3 1)\n";
        }
    }
    meshFile << ");\n"
                "\n";
    meshFile << "edges\n"
                "(\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            compute_indices(indices, i, j, n);
            meshFile << "    arc " // outer peak
                     << indices[0][0] << " "
                     << indices[1][0] << " "
                     << ang_degree << " (0 0 1)\n"
                     << "    arc " // inner peak
                     << indices[0][0] + 1 << " "
                     << indices[1][0] + 1 << " "
                     << ang_degree << " (0 0 1)\n"
                     << "    arc " // outer valley
                     << indices[0][1] << " "
                     << indices[1][1] << " "
                     << ang_degree << " (0 0 1)\n"
                     << "    arc " // inner valley
                     << indices[0][1] + 1 << " "
                     << indices[1][1] + 1 << " "
                     << ang_degree << " (0 0 1)\n";
        }
        int j = m;
        compute_indices(indices, i, j, n);
        meshFile << "    arc " // bottom outer peak
                 << indices[0][0] << " "
                 << indices[1][0] << " "
                 << ang_degree << " (0 0 1)\n"
                 << "    arc " // inner peak R_max
                 << indices[0][0] + 1 << " "
                 << indices[1][0] + 1 << " "
                 << ang_degree << " (0 0 1)\n";
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
                     << indices[1][1] + 1 << " "
                     << indices[1][0] + 1 << ")\n"
                     << "            ("
                     << indices[0][1] + 1 << " "
                     << indices[0][2] + 1 << " "
                     << indices[1][2] + 1 << " "
                     << indices[1][1] + 1 << ")\n";
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
                 << indices[1][0] << ")\n";
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
                 << indices[1][0] + 1 << ")\n";
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