#ifndef GUARD_XOREncryption_h
#define GUARD_XOREncryption_h

#include <string>

namespace XOREncryption {
    std::string encryptData(const std::string& data, const std::string& password);
    std::string decryptData(const std::string& encryptedData, const std::string& password);
}

#endif
