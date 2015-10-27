#ifndef CONVERSION_H
#define CONVERSION_H

#include <string>
#include <sstream>

// converte una stringa in un numero di tipo T.
template <typename T>
T StringToNumber (const std::string &Text)
{
    std::stringstream ss(Text);
    T result;
    return ss >> result ? result : 0;
}

// funzione che converte un numero di tipo T in una stringa
template <typename T>
std::string NumberToString(const T &number)
{
   std::stringstream ss;
   ss << number;
   return ss.str();
}

#endif // CONVERSION_H
