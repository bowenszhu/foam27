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
    double AR = 1.00; /// tan of the angle between 08 and 04
    double L_2 = 1.00; /// lambda / 2 (lambda is the distance between 0 and 8)
    double IR = 14.00; /// inner radius
    double A = L_2 * AR;
    double MR = IR - A; /// radius in the middle
    int n = 1; /// number of blocks (with 12 vertices)
    int N = 2 * n + 1; /// number of surface levels
    double ang = 0.003;

    double H[N]; /// z of surface levels (used for defining arcs)
    for (int i = 0; i < N; i++)
        H[i] = i * L_2;

    double cos_ang = cos(ang);
    double sin_ang = sin(ang);

    double X1_2 = IR * cos_ang;
    double Y1_2 = IR * sin_ang;
    double X1_1 = X1_2;
    double Y1_1 = -Y1_2;
    double X2_2 = OR * cos_ang;
    double Y2_2 = OR * sin_ang;
    double X2_1 = X2_2;
    double Y2_1 = -Y2_2;
    double X3_2 = MR * cos_ang;
    double Y3_2 = MR * sin_ang;
    double X3_1 = X3_2;
    double Y3_1 = -Y3_2;

    int v_i = 0; /// vertex index
    for (int i = 0; i < N; i++) {
        if (i % 2) {
            meshFile << "    (" << X3_1 << " " << Y3_1 << " " << H[i] << ") // vertex " << v_i++ << "\n";
            meshFile << "    (" << X2_1 << " " << Y2_1 << " " << H[i] << ") // vertex " << v_i++ << "\n";
            meshFile << "    (" << X2_2 << " " << Y2_2 << " " << H[i] << ") // vertex " << v_i++ << "\n";
            meshFile << "    (" << X3_2 << " " << Y3_2 << " " << H[i] << ") // vertex " << v_i++ << "\n";
        } else {
            meshFile << "    (" << X1_1 << " " << Y1_1 << " " << H[i] << ") // vertex " << v_i++ << "\n";
            meshFile << "    (" << X2_1 << " " << Y2_1 << " " << H[i] << ") // vertex " << v_i++ << "\n";
            meshFile << "    (" << X2_2 << " " << Y2_2 << " " << H[i] << ") // vertex " << v_i++ << "\n";
            meshFile << "    (" << X1_2 << " " << Y1_2 << " " << H[i] << ") // vertex " << v_i++ << "\n";
        }
    }
    meshFile << ");\n"
                "\n";
    meshFile <<"blocks\n"
               "(\n";
    for (int i = 0; i < N - 1; i++) {
        int i4 = i * 4;
        meshFile << "     hex ("
                 << i4 + 0 << " "
                 << i4 + 1 << " "
                 << i4 + 2 << " "
                 << i4 + 3 << " "
                 << i4 + 4 << " "
                 << i4 + 5 << " "
                 << i4 + 6 << " "
                 << i4 + 7 << ") (60 1 80) simpleGrading (10 1 1)\n";
    }
    meshFile << ");\n"
                "\n";
    meshFile << "edges\n"
                "(\n";
    double A1x = IR;
    double A2x = OR;
    double A3x = MR;
    double Ay = 0.;
    for (int i = 0; i < N; i++) {
        int i4 = i * 4;
        if (i % 2)
            meshFile << "    arc " << i4 << " " << i4 + 3 << " (" << A3x << " " << Ay << " " << H[i] << ")\n";
        else
            meshFile << "    arc " << i4 << " " << i4 + 3 << " (" << A1x << " " << Ay << " " << H[i] << ")\n";
        meshFile << "    arc " << i4 + 1 << " " << i4 + 2 << " (" << A2x << " " << Ay << " " << H[i] << ")\n";
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
    for (int i = 0; i < N - 1; i++) {
        int i4 = i * 4;
        meshFile << "            (" << i4 << " " << i4 + 4 << " " << i4 + 7 << " " << i4 + 3 << ")\n";
    }
    meshFile << "        );\n"
                "    }\n";

    meshFile << "    outerCylinder\n"
                "    {\n"
                "        type wall;\n"
                "        faces\n"
                "        (\n";
    for (int i = 0; i < N - 1; i++) {
        int i4 = i * 4;
        meshFile << "            (" << i4 + 1 << " " << i4 + 2 << " " << i4 + 6 << " " << i4 + 5 << ")\n";
    }
    meshFile << "        );\n"
                "    }\n";

    meshFile << "    down\n"
                "    {\n"
                "        type wedge;\n"
                "        faces\n"
                "        (\n";
    for (int i = 0; i < N - 1; i++) {
        int i4 = i * 4;
        meshFile << "            (" << i4 << " " << i4 + 1 << " " << i4 + 5 << " " << i4 + 4 << ")\n";
    }
    meshFile << "        );\n"
                "    }\n";

    meshFile << "    left\n"
                "    {\n"
                "        type wedge;\n"
                "        faces\n"
                "        (\n";
    for (int i = 0; i < N - 1; i++) {
        int i4 = i * 4;
        meshFile << "            (" << i4 + 7 << " " << i4 + 6 << " " << i4 + 2 << " " << i4 + 3 << ")\n";
    }
    meshFile << "        );\n"
                "    }\n";

    meshFile << "    front\n"
                "    {\n"
                "        type cyclic;\n"
                "        neighbourPatch back\n"
                "        faces\n"
                "        (\n";
    meshFile << "            (" << 0 << " " << 3 << " " << 2 << " " << 1 << ")\n";
    meshFile << "        );\n"
                "    }\n";

    meshFile << "    back \n"
                "    {\n"
                "        type cyclic;\n"
                "        neighbourPatch front\n"
                "        faces\n"
                "        (\n";
    int N14 = (N - 1) * 4;
    meshFile << "            (" << N14 << " " << N14 + 1 << " " << N14 + 2 << " " << N14 + 3 << ")\n";
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