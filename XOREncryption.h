// 异或加密与解密模块。

#ifndef GUARD_XOREncryption_h
#define GUARD_XOREncryption_h

#include <string>

class XOREncryption {
public:
    static std::string encryptData(const std::string& data, const std::string& password);
    static std::string decryptData(const std::string& encryptedData, const std::string& password);
};

#endif
