#include <fstream>
#include <iostream>
#include <math.h>
#include <iomanip>

using namespace std;

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
    meshFile << "blocks\n"
                "(\n";
    for (int j = 0; j < m; j++) {
        for (int i = 0; i < n; i++) {
            int index11 = (2 * j * n + i) * 2;
            int index12 = ((2 * j + 1) * n + i) * 2;
            int index13 = (2 * (j + 1) * n + i) * 2;
            int index21 = (2 * j * n + (i + 1) % n) * 2;
            int index22 = ((2 * j + 1) * n + (i + 1) % n) * 2;
            int index23 = (2 * (j + 1) * n + (i + 1) % n) * 2;
            meshFile << "     hex ("
                     << index11 << " "
                     << index11 + 1 << " "
                     << index12 + 1 << " "
                     << index12 << " "
                     << index21 << " "
                     << index21 + 1 << " "
                     << index22 + 1 << " "
                     << index22 << ") (60 80 1) simpleGrading (10 1 1)\n"
                     << "     hex ("
                     << index12 << " "
                     << index12 + 1 << " "
                     << index13 + 1 << " "
                     << index13 << " "
                     << index22 << " "
                     << index22 + 1 << " "
                     << index23 + 1 << " "
                     << index23 << ") (60 80 1) simpleGrading (10 1 1)\n";
        }
    }
    meshFile << ");\n"
                "\n";
    meshFile << "edges\n"
                "(\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            int index11 = (2 * j * n + i) * 2;
            int index12 = ((2 * j + 1) * n + i) * 2;
            int index21 = (2 * j * n + (i + 1) % n) * 2;
            int index22 = ((2 * j + 1) * n + (i + 1) % n) * 2;
            meshFile << "    arc " << index11 << " " << index21 << " " << ang_degree << " (0 0 1)\n"
                     << "    arc " << index11 + 1 << " " << index21 + 1 << " " << ang_degree << " (0 0 1)\n"
                     << "    arc " << index12 << " " << index22 << " " << ang_degree << " (0 0 1)\n"
                     << "    arc " << index12 + 1 << " " << index22 + 1 << " " << ang_degree << " (0 0 1)\n";
        }
        int j = m;
        int index11 = (2 * j * n + i) * 2;
        int index21 = (2 * j * n + (i + 1) % n) * 2;
        meshFile << "    arc " << index11 << " " << index21 << " " << ang_degree << " (0 0 1)\n"
                 << "    arc " << index11 + 1 << " " << index21 + 1 << " " << ang_degree << " (0 0 1)\n";
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
            int index11 = (2 * j * n + i) * 2;
            int index12 = ((2 * j + 1) * n + i) * 2;
            int index13 = (2 * (j + 1) * n + i) * 2;
            int index21 = (2 * j * n + (i + 1) % n) * 2;
            int index22 = ((2 * j + 1) * n + (i + 1) % n) * 2;
            int index23 = (2 * (j + 1) * n + (i + 1) % n) * 2;
            meshFile << "            ("
                     << index11 + 1 << " "
                     << index12 + 1 << " "
                     << index22 + 1 << " "
                     << index21 + 1 << ")\n"
                     << "            ("
                     << index12 + 1 << " "
                     << index13 + 1 << " "
                     << index23 + 1 << " "
                     << index22 + 1 << ")\n";
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
            int index11 = (2 * j * n + i) * 2;
            int index12 = ((2 * j + 1) * n + i) * 2;
            int index13 = (2 * (j + 1) * n + i) * 2;
            int index21 = (2 * j * n + (i + 1) % n) * 2;
            int index22 = ((2 * j + 1) * n + (i + 1) % n) * 2;
            int index23 = (2 * (j + 1) * n + (i + 1) % n) * 2;
            meshFile << "            (" << index12 << " " << index11 << " " << index21 << " " << index22 << ")\n"
                     << "            (" << index13 << " " << index12 << " " << index22 << " " << index23 << ")\n";
        }
    }
    meshFile << "        );\n"
                "    }\n";

//    meshFile << "    down\n"
//                "    {\n"
//                "        type wedge;\n"
//                "        faces\n"
//                "        (\n";
//    for (int i = 0; i < N - 1; i++) {
//        int i4 = i * 4;
//        meshFile << "            (" << i4 << " " << i4 + 1 << " " << i4 + 5 << " " << i4 + 4 << ")\n";
//    }
//    meshFile << "        );\n"
//                "    }\n";
//
//    meshFile << "    left\n"
//                "    {\n"
//                "        type wedge;\n"
//                "        faces\n"
//                "        (\n";
//    for (int i = 0; i < N - 1; i++) {
//        int i4 = i * 4;
//        meshFile << "            (" << i4 + 7 << " " << i4 + 6 << " " << i4 + 2 << " " << i4 + 3 << ")\n";
//    }
//    meshFile << "        );\n"
//                "    }\n";

//    meshFile << "    front\n"
//                "    {\n"
//                "        type cyclic;\n"
//                "        neighbourPatch back\n"
//                "        faces\n"
//                "        (\n";
//    for (int i = 0; i < n; i++) {
//        int index1 = 4 * i;
//        int index2 = 4 * ((i + 1) % n);
//        meshFile << "            (" << index1 << " " << index1 + 1 << " " << index2 + 1 << " " << index2 << ")\n";
//    }
//    meshFile << "        );\n"
//                "    }\n";
//    meshFile << "    back \n"
//                "    {\n"
//                "        type cyclic;\n"
//                "        neighbourPatch front\n"
//                "        faces\n"
//                "        (\n";
//    for (int i = 0; i < n; i++) {
//        int index1 = 4 * (m * n + i);
//        int index2 = 4 * (m * n + (i + 1) % n);
//        meshFile << "            (" << index1 + 1 << " " << index1 << " " << index2 << " " << index2 + 1 << ")\n";
//    }
//    meshFile << "        );\n"
//                "    }\n";
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