
#include <windows.h> 
#include <stdio.h> 
#include<TCHAR.H>
#include <iostream>
#include <string>
#include <filesystem>
#include <functional>
#include <algorithm>
#include "mhook/mhook-lib/mhook.h"



//typedef int(__cdecl* MYPROC)(LPWSTR);
typedef int(__cdecl* MYPROC)(int, int*);
typedef int(__cdecl* verify)(std::string*, std::string*);
typedef int(__cdecl* factorialfunc)(int);

factorialfunc CalculateFactorialImp;

//get dll path from parameter
char* GetPath(int argc, char* argv[])

{
    if (argc > 1)
        return argv[1];
    else std::cout << "add path to dll as parameter\n";
    return nullptr;
}

bool check_number(char* str) {
    for (int i = 0; i < strlen(str); i++)
        if (isdigit(str[i]) == 0)
            return true;
    return false;
}

int factorial(int x)
{
    if (x % 2 == 0)
        return -1;
    else
        return 0;
}

int main(int argc, char* argv[])
{
    HINSTANCE hinstLib;
    MYPROC CalculateFactorial;
    verify verification;
    BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

    //check if 2nd parameter is digit
    if (argc != 3)
    {
        std::cout << "invalid parameters counter, type path to dll and factorial number" << std::endl;;
        return -1;
    }
    if (check_number(argv[2]))
    {
        std::cout << "2nd parameter is not a digit, factorial must be a number" << std::endl;
        return -1;
    }


    // Get a handle to the DLL module.
    hinstLib = LoadLibraryA(GetPath(argc, argv));

    // If the handle is valid, try to get the function address.
    if (hinstLib != NULL)
    {
        CalculateFactorial = (MYPROC)GetProcAddress(hinstLib, "CalculateFactorial");

        uintptr_t CalcFactAddr = (uintptr_t)GetProcAddress(hinstLib, "CalculateFactorial");
        size_t offsetToCalcFactImpl = 0x000005d7;
        CalculateFactorialImp = (factorialfunc)(CalcFactAddr - offsetToCalcFactImpl);

        std::cout << " CalculateFactorialImp " << std::hex << CalculateFactorialImp << " CalcFactAddr: " << std::hex << CalcFactAddr << std::endl;
        // If the function address is valid, call the function.
        int* out = new int;
        if (CalculateFactorial != NULL)
        {
            fRunTimeLinkSuccess = TRUE;

            BOOL bHook = Mhook_SetHook((PVOID*)&CalculateFactorialImp,
                factorial);
            CalculateFactorial(std::stoi(argv[2]), out);
            std::cout << "calculated factorial: " << std::dec << *out;
        }
        //signature verification
        verification = (verify)GetProcAddress(hinstLib, "ValidateSignature");
        if (verification != NULL)
        {
            std::string signature;
            std::cout << std::endl << "Input signature to verify: " << std::endl;
            std::cin >> signature;
            std::string message = "calculated factorial is " + std::to_string(*out);
            bool isValid = verification(&message, &signature);
            if (!isValid)
            {
                std::cout << "Invalid signature";
            }
            else {
                std::cout << "valid signature";
            }
        }

        //Free the DLL module.

        fFreeResult = FreeLibrary(hinstLib);
    }

    // If unable to call the DLL function, use an alternative.
    if (!fRunTimeLinkSuccess)
        printf("Dll was not loaded, check your path\n");

    return 0;

}