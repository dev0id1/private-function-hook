
#pragma once
#include <string>
#include <string.h> 

#ifdef MYDLL_EXPORTS
#define MYDLL_API __declspec(dllexport)
#else
#define MYDLL_API __declspec(dllimport)
#endif


extern "C" MYDLL_API int CalculateFactorial(int in, int* out);


extern "C" MYDLL_API bool ValidateSignature(std::string * str, std::string * signature);

