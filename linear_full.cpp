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
    meshFile << std::fixed << setprecision(12);
    meshFile
            << "/*--------------------------------*- C++ -*----------------------------------*\\\n"
               "  =========                 |\n"
               "  \\\\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox\n"
               "   \\\\    /   O peration     | Website:  https://openfoam.org\n"
               "    \\\\  /    A nd           | Version:  9\n"
               "     \\\\/     M anipulation  |\n"
               "\\*---------------------------------------------------------------------------*/\n\n"
               "FoamFile\n"
               "{\n"
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

    double OR = 21.59; /// outer radius
    double AR = 1.00; /// tan of the angle between
    double lambda = 2.00; /// lambda
    double lambda_2 = lambda / 2.; /// lambda / 2
    double IR = 14.00; /// inner radius
    double A = lambda_2 * AR;
    double MR = IR - A; /// radius in the middle
    int n = 30; /// number of blocks in a circle
    int m = 20; /// number of cells (2 blocks) in z direction
    double ang_radian = M_PI * 2. / n;
    double ang_degree = 360. / n;

    int v_i = 0; /// vertex index
    for (int j = 0; j < m; j++) {
        double z1 = lambda * j;
        double z2 = z1 + lambda_2;
        for (int i = 0; i < n; i++) {
            double theta = ang_radian * i;
            double c = cos(theta);
            double s = sin(theta);
            double Ox = OR * c; // outer
            double Oy = OR * s;
            double Ix = IR * c; // inner
            double Iy = IR * s;
            meshFile << "    (" << Ox << " " << Oy << " " << z1 << ") // vertex " << v_i++ << "\n"
                     << "    (" << Ix << " " << Iy << " " << z1 << ") // vertex " << v_i++ << "\n";
        }
        for (int i = 0; i < n; i++) {
            double theta = ang_radian * i;
            double c = cos(theta);
            double s = sin(theta);
            double Ox = OR * c; // outer
            double Oy = OR * s;
            double Mx = MR * c; // middle
            double My = MR * s;
            meshFile << "    (" << Ox << " " << Oy << " " << z2 << ") // vertex " << v_i++ << "\n"
                     << "    (" << Mx << " " << My << " " << z2 << ") // vertex " << v_i++ << "\n";
        }
    }
    double z = m * lambda;
    for (int i = 0; i < n; i++) {
        double theta = ang_radian * i;
        double c = cos(theta);
        double s = sin(theta);
        double Ox = OR * c; // outer
        double Oy = OR * s;
        double Ix = IR * c; // inner
        double Iy = IR * s;
        meshFile << "    (" << Ox << " " << Oy << " " << z << ") // vertex " << v_i++ << "\n"
                 << "    (" << Ix << " " << Iy << " " << z << ") // vertex " << v_i++ << "\n";
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
                     << indices[1][1] << ") (60 80 1) simpleGrading (10 1 1)\n"
                     << "     hex ("
                     << indices[0][1] << " "
                     << indices[0][1] + 1 << " "
                     << indices[0][2] + 1 << " "
                     << indices[0][2] << " "
                     << indices[1][1] << " "
                     << indices[1][1] + 1 << " "
                     << indices[1][2] + 1 << " "
                     << indices[1][2] << ") (60 80 1) simpleGrading (10 1 1)\n";
        }
    }
    meshFile << ");\n"
                "\n";
    meshFile << "edges\n"
                "(\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            compute_indices(indices, i, j, n);
            meshFile << "    arc "
                     << indices[0][0] << " "
                     << indices[1][0] << " "
                     << ang_degree << " (0 0 1)\n"
                     << "    arc "
                     << indices[0][0] + 1 << " "
                     << indices[1][0] + 1 << " "
                     << ang_degree << " (0 0 1)\n"
                     << "    arc "
                     << indices[0][1] << " "
                     << indices[1][1] << " "
                     << ang_degree << " (0 0 1)\n"
                     << "    arc "
                     << indices[0][1] + 1 << " "
                     << indices[1][1] + 1 << " "
                     << ang_degree
                     << " (0 0 1)\n";
        }
        int j = m;
        compute_indices(indices, i, j, n);
        meshFile << "    arc "
                 << indices[0][0] << " "
                 << indices[1][0] << " "
                 << ang_degree << " (0 0 1)\n"
                 << "    arc "
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