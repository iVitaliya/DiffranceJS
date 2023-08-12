#include "v8.h"

#include "./src/diffrancejs.hpp"

int main(int argc, char *argv[])
{
    char *filename = argv[1];
    auto *diffrancejs = new DiffranceJS();
    std::unique_ptr<v8::Platform> platform = diffrancejs->InitialiseV8(argc, argv);

    diffrancejs->initialiseVM();
    diffrancejs->InitialiseProgram(filename);
    diffrancejs->Shutdown();

    return 0;
}