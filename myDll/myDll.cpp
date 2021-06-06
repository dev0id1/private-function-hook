#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier



int CalculateFactorial(int in, int* out);
void PrintResult(int factorial);

int CalculateFactorialImpl(int in)
{
    if (in < 0) {
        return 0;
    }
    return !in ? 1 : in * CalculateFactorialImpl(in - 1);
}

int CalculateFactorial(int in, int* out)
{
    std::cout << "CalcFactorialImpl: " << &CalculateFactorialImpl << std::endl;
    int factorial = CalculateFactorialImpl(in);
    PrintResult(factorial);
    *out = factorial;
    return 1;
}

//need for correct xor
static inline unsigned int value(char c)
{
    if (c >= '0' && c <= '9') { return c - '0'; }
    if (c >= 'a' && c <= 'f') { return c - 'a' + 10; }
    if (c >= 'A' && c <= 'F') { return c - 'A' + 10; }
    return -1;
}

//perform xor
std::string StrXor(std::string const& s1, std::string const& s2)
{
    std::string  hash1 = md5(s1);
    std::string  hash2 = md5(s2);

    assert(hash1.length() == hash2.length());
    std::string result;
    static char const alphabet[] = "0123456789abcdef";
    result.reserve(hash1.length());
    std::cout << "hash of message: " << hash1 << std::endl << "hash of secret: " << hash2 << std::endl;
    for (std::size_t i = 0; i != hash1.length(); ++i)
    {
        unsigned int v = (value(hash1[i])) ^ value((hash2[i]));
        result.push_back(alphabet[v]);
    }
    return result;
}




std::string Signature(std::string* str)
{
    //imagine this function is protected
    std::string secret = "test";
    return StrXor(*str, secret);
}

void PrintResult(int factorial)
{
    std::string message = "calculated factorial is " + std::to_string(factorial);
    std::string signature = Signature(&message);
    std::cout << "message: " << message << "\n";
    std::cout << "message hash after xor (signature): " << signature << "\n";
}


bool ValidateSignature(std::string* str, std::string* signature)
{
    if (Signature(str) == *signature)
        return true;
    else return false;
}
