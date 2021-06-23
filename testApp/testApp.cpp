
#include "pch.h"

#define JUMP_INSTRUCTION_SIZE 5  // 0x86 jump instruction is 5 bytes total (opcode + 4 byte address).
#define offsetToCalcFactImpl 0x000005cd 

//typedef int(__cdecl* MYPROC)(LPWSTR);
typedef int(__cdecl* MYPROC)(int, int*);
typedef int(__cdecl* verify)(const std::string*, const std::string*);
typedef int(__cdecl* factorialfunc)(int);

factorialfunc CalculateFactorialImp;

bool ApplyPatch(const factorialfunc CalculateFactorialImp, const void* factorialFunc1)
{
    int64_t* targetFunc = (int64_t*)CalculateFactorialImp;
    int64_t* replacementFunc = (int64_t*)factorialFunc1;

    SYSTEM_INFO system;
    GetSystemInfo(&system);
    int pageSize = system.dwAllocationGranularity;

    uintptr_t startAddress = (uintptr_t)targetFunc;
    uintptr_t endAddress = startAddress + 1;
    uintptr_t pageStart = startAddress & -pageSize;

    // Mark page containing the target function implementation as writable so we can inject our own instruction.
    DWORD permissions = 0;
    BOOL value = VirtualProtect((LPVOID)pageStart, endAddress - pageStart, PAGE_EXECUTE_READWRITE, &permissions);

    if (!value)
    {
        return false;
    }

    int32_t jumpOffset = (int64_t)replacementFunc - ((int64_t)targetFunc + JUMP_INSTRUCTION_SIZE);

    char instruction[5];
    instruction[0] = '\xE9'; // E9 = "Jump near (relative)" opcode
    ((int32_t*)(instruction + 1))[0] = jumpOffset; // Next 4 bytes = jump offset

    // Replace first instruction in target target function with our unconditional jump to replacement function.
    char* functionImplementation = (char*)targetFunc;
    for (int i = 0; i < JUMP_INSTRUCTION_SIZE; i++)
    {
        functionImplementation[i] = instruction[i];
    }
}


//get dll path from parameter
char* GetPath(int argc, char* argv[])

{
    if (argc > 1)
        return argv[1];
    else std::cout << "add path to dll as parameter\n";
    return nullptr;
}

bool check_number(const char* str) {
    for (int i = 0; i < strlen(str); i++)
        if (isdigit(str[i]) == 0)
            return true;
    return false;
}

//function to replace private one from dll
int factorialFunc1(int x)
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
    BOOL fFreeResult = FALSE, fRunTimeLinkSuccess = FALSE;

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
        CalculateFactorialImp = (factorialfunc)((uintptr_t)CalculateFactorial - offsetToCalcFactImpl);
        std::cout << " CalculateFactorialImp(private) " << std::hex << CalculateFactorialImp << " CalcFactAddr: " << std::hex << CalculateFactorial << std::endl;

        // If the function address is valid, call the function.
        int out;
        if (CalculateFactorial != NULL)
        {
            fRunTimeLinkSuccess = TRUE;

            bool x = ApplyPatch(CalculateFactorialImp, factorialFunc1);
            if (x)
                CalculateFactorial(std::stoi(argv[2]), &out);
            std::cout << "calculated factorial: " << std::dec << out;
        }

        //signature verification
        verification = (verify)GetProcAddress(hinstLib, "ValidateSignature");
        if (verification != NULL)
        {
            std::string signature;
            std::cout << std::endl << "Input signature to verify: " << std::endl;
            std::cin >> signature;
            std::string message = "calculated factorial is " + std::to_string(out);

            bool isValid = verification(&message, &signature);
            if (!isValid)
                std::cout << "Invalid signature";
            else 
                std::cout << "valid signature";
        }

        //Free the DLL module.

        fFreeResult = FreeLibrary(hinstLib);
        if (!fFreeResult)
        {
            std::cout << "error on dll unload" << std::endl;
            return -1;
        }
            
    }

    // If unable to call the DLL function, use an alternative.
    if (!fRunTimeLinkSuccess)
        printf("Dll was not loaded, check your path\n");

    return 0;

}