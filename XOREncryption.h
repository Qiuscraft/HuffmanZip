#ifndef GUARD_XOREncryption_h
#define GUARD_XOREncryption_h

#include <string>

using std::string;

namespace XOREncryption {
    string encryptData(const string& data, const string& password);
    string decryptData(const string& encryptedData, const string& password);
}

#endif
