#include <fstream>
#include <iostream>
#include <math.h>
#include <iomanip>
#include <cstdlib>
#include "TEdgeSpace.h"
#include "input_parser.cpp"

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
                "    \\\\  /    A nd           | Version:  8\n"
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
    InputParser input(argc, argv);
    /// angle parameter of denticle [rad]
    const std::string &alpha_input = input.getCmdOption("-alpha");
    const double alpha = alpha_input.empty() ? 0.15 : std::stod(alpha_input);

    /// number of denticle blocks in a circle
    const std::string &n_input = input.getCmdOption("-n");
    const int n_denticle = n_input.empty() ? 10 : std::stoi(n_input);

    const int m = 1; /// number of cells (2 blocks) in z_peak_bottom direction
    const double ang_radian = M_PI * 2. / n_denticle;
    const double ang_degree = 360. / n_denticle;
    const double tan_alpha = tan(alpha);
    const double temp = tan_alpha * 4. * M_PI / n_denticle;
    const double R_peak_min = R_inner * sqrt(temp / (exp(temp) - 1.));
    const double R_peak_max = R_peak_min * exp(tan_alpha * ang_radian);
    const double R_valley_min = R_peak_min - A;
    const double R_valley_max = R_peak_max - A;
    const int N_poly = 100; /// number of points for polyline
    const double ang_poly = ang_radian / (N_poly + 1);
    const double r_outer_R = 0.2; /// expansion ratio in R dir in outer part

    const double R_max_min_gap = 2.3289556497985924; // R_max - R_min when alpha=0.15 and n=10
    /// number of cells in R direction in denticle part
    const std::string &n_R_denticle_input = input.getCmdOption("-nRDenticle");
    int n_R_denticle = n_R_denticle_input.empty() ?
                       max(5., 16 / R_max_min_gap * (R_peak_max - R_peak_min)) : std::stoi(n_R_denticle_input);

    /// number of cells in theta direction for both parts
    const std::string &n_theta_input = input.getCmdOption("-nTheta");
    int n_theta = n_theta_input.empty() ? 69 * 10 / n_denticle : std::stoi(n_theta_input);

    /// number of cells in z direction
    const std::string &n_z_input = input.getCmdOption("-nZ");
    int n_z = n_z_input.empty() ? 7 : std::stoi(n_z_input);

    TEdgeSpace ES; // a grading calculator
    ES.calc("de", // [arg1] end cell size
            "r", // [arg2] expansion ratio
            R_outer - R_valley_max, // length
            (R_peak_max - R_peak_min) / n_R_denticle, // [arg1]
            r_outer_R); // [arg2] 1 / 5
    int n_R_outer = ES.getn(); /// number of cell in R dir in outer part

    /// coarse or fine grid
    const std::string &grid_input = input.getCmdOption("-grid");
    const int grid = grid_input.empty() ? 1 : std::stoi(grid_input); // expected input: 1 or 2 or 4
    // increase the number of cells in all directions by some factor
    if (grid > 1) {
        n_R_denticle *= grid;
        n_R_outer *= grid;
        n_theta *= grid;
        n_z *= grid;
    }

    cout << "alpha: " << alpha << " # angle of denticle\n"
         << "n_denticle: " << n_denticle << " # number of blocks in a circle\n"
         << "n_R_denticle: " << n_R_denticle << " # number of cells in R direction in denticle zone\n"
         << "n_R_outer: " << n_R_outer << " # number of cells in R direction in outer part\n"
         << "r_outer_R: " << r_outer_R << " # expansion ratio in R direction in out part\n"
         << "n_theta: " << n_theta << " # number of cells in theta direction\n"
         << "n_z: " << n_z << " # number of cells in z direction\n"
         << "grid: " << grid << " # coarse or fine grid\n";

    for (int j = 0; j < m; j++) {
        const double z_peak = lambda * j;
        const double z_valley = z_peak + lambda_2;
        for (int i = 0; i < n_denticle; i++) {
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
        for (int i = 0; i < n_denticle; i++) {
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
    for (int i = 0; i < n_denticle; i++) {
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
        for (int i = 0; i < n_denticle; i++) {
            compute_indices(indices, i, j, n_denticle);
            meshFile << "    hex ("
                     << indices[0][0] << " "
                     << indices[0][0] + 1 << " "
                     << indices[0][1] + 1 << " "
                     << indices[0][1] << " "
                     << indices[1][0] << " "
                     << indices[1][0] + 1 << " "
                     << indices[1][1] + 1 << " "
                     << indices[1][1] << ") ("
                     << n_R_outer << " " << n_z << " " << n_theta << ") simpleGrading (" << r_outer_R << " 1 1)\n"
                     << "    hex ("
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
    for (int i = 0; i < n_denticle; i++) {
        const double ang_block_1 = 2. * M_PI * (i + 1) / n_denticle;
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
            compute_indices(indices, i, j, n_denticle);
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
        const int j = m;
        compute_indices(indices, i, j, n_denticle);
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
        for (int i = 0; i < n_denticle; i++) {
            compute_indices(indices, i, j, n_denticle);
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
        for (int i = 0; i < n_denticle; i++) {
            compute_indices(indices, i, j, n_denticle);
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
    meshFile << "        type cyclic;\n"
                "        neighbourPatch back;\n"
                "        faces\n"
                "        (\n";
    for (int i = 0; i < n_denticle; i++) {
        compute_indices(indices, i, 0, n_denticle);
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
    meshFile << "        type cyclic;\n"
                "        neighbourPatch front;\n"
                "        faces\n"
                "        (\n";
    for (int i = 0; i < n_denticle; i++) {
        compute_indices(indices, i, m, n_denticle);
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

    if (input.cmdOptionExists("-sampleLines")) { // create OpenFOAM function files for sampling U along 6 lines
        std::string header = "/*--------------------------------*- C++ -*----------------------------------*\\\n"
                             "  =========                 |\n"
                             "  \\\\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox\n"
                             "   \\\\    /   O peration     | Website:  https://openfoam.org\n"
                             "    \\\\  /    A nd           | Version:  8\n"
                             "     \\\\/     M anipulation  |\n"
                             "-------------------------------------------------------------------------------\n"
                             "Description\n"
                             "    Writes graph data for specified fields along a line, specified by start\n"
                             "    and end points.\n"
                             "\n"
                             "\\*---------------------------------------------------------------------------*/\n\n";
        std::string footer = "fields (U);\n" // sample the velocities
                             "\n"
                             "// Sampling and I/O settings\n"
                             "#includeEtc \"caseDicts/postProcessing/graphs/sampleDict.cfg\"\n"
                             "\n"
                             "// Override settings here, e.g.\n"
                             "/*\n"
                             "setConfig\n"
                             "{\n"
                             "    type lineCell;\n"
                             "    axis x;        // y, z, xyz\n"
                             "}\n"
                             "*/\n"
                             "\n"
                             "// Must be last entry\n"
                             "#includeEtc \"caseDicts/postProcessing/graphs/graph.cfg\"\n"
                             "\n"
                             "// ************************************************************************* //";
        ofstream graph_thetaFull_zPeak("singleGraph_thetaFull_zPeak");
        graph_thetaFull_zPeak << std::fixed << setprecision(12)
                              << header
                              << "start   (0 0 0);\n"
                              << "end     (" << R_outer << " 0 0);\n"
                              << footer;
        graph_thetaFull_zPeak.close();
        ofstream graph_thetaFull_zMid("singleGraph_thetaFull_zMid");
        graph_thetaFull_zMid << std::fixed << setprecision(12)
                             << header
                             << "start   (0 0 " << lambda_2 / 2 << ");\n"
                             << "end     (" << R_outer << " 0 " << lambda_2 / 2 << ");\n"
                             << footer;
        graph_thetaFull_zMid.close();
        ofstream graph_thetaFull_zTrough("singleGraph_thetaFull_zTrough");
        graph_thetaFull_zTrough << std::fixed << setprecision(12)
                                << header
                                << "start   (0 0 " << lambda_2 << ");\n"
                                << "end     (" << R_outer << " 0 " << lambda_2 << ");\n"
                                << footer;
        graph_thetaFull_zTrough.close();

        const double theta_mid = ang_radian / 2;
        const double x_outer_thetaMid = R_outer * cos(theta_mid);
        const double y_outer_thetaMid = R_outer * sin(theta_mid);
        ofstream graph_thetaMid_zPeak("singleGraph_thetaMid_zPeak");
        graph_thetaMid_zPeak << std::fixed << setprecision(12)
                             << header
                             << "start   (0 0 0);\n"
                             << "end     (" << x_outer_thetaMid << " " << y_outer_thetaMid << " 0);\n"
                             << footer;
        graph_thetaMid_zPeak.close();
        ofstream graph_thetaMid_zMid("singleGraph_thetaMid_zMid");
        graph_thetaMid_zMid << std::fixed << setprecision(12)
                            << header
                            << "start   (0 0 " << lambda_2 / 2 << ");\n"
                            << "end     (" << x_outer_thetaMid << " "
                            << y_outer_thetaMid << " " << lambda_2 / 2 << ");\n"
                            << footer;
        graph_thetaMid_zMid.close();
        ofstream graph_thetaMid_zTrough("singleGraph_thetaMid_zTrough");
        graph_thetaMid_zTrough << std::fixed << setprecision(12)
                               << header
                               << "start   (0 0 " << lambda_2 << ");\n"
                               << "end     (" << x_outer_thetaMid << " " << y_outer_thetaMid << " " << lambda_2
                               << ");\n"
                               << footer;
        graph_thetaMid_zTrough.close();
    }
    return 0;
}