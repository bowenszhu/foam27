#include <fstream>

int main(int argc, char *argv[]) {
    std::ofstream decomposeParFile("decomposeParDict");

    const int numberOfSubdomains = argc == 2 ? std::atoi(argv[1]) : 6;

    decomposeParFile << "/*--------------------------------*- C++ -*----------------------------------*\\\n"
                        "| =========                 |                                                 |\n"
                        "| \\\\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox           |\n"
                        "|  \\\\    /   O peration     | Version:  2.3.0                                 |\n"
                        "|   \\\\  /    A nd           | Web:      www.OpenFOAM.org                      |\n"
                        "|    \\\\/     M anipulation  |                                                 |\n"
                        "\\*---------------------------------------------------------------------------*/\n"
                        "FoamFile\n"
                        "{\n"
                        "    version     2.0;\n"
                        "    format      ascii;\n"
                        "    class       dictionary;\n"
                        "    object      decomposeParDict;\n"
                        "}\n"
                        "\n"
                        "// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //\n"
                        "\n"
                        "numberOfSubdomains "
                     << numberOfSubdomains
                     << ";\n"
                        "\n"
                        "method          scotch;\n"
                        "\n"
                        "// ************************************************************************* //";
    decomposeParFile.close();
    return 0;
}