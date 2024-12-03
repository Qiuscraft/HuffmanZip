// 异或加密与解密模块。

#include "XOREncryption.h"

// 加密函数
std::string XOREncryption::encryptData(const std::string& data, const std::string& password) {
    // 默认不加密
    size_t passwordLength = password.length();
    if (passwordLength == 0)
        return data;
    
    // 加密过程
    std::string encryptedData = data;
    for (size_t i = 0; i < data.length(); ++i) {
        encryptedData[i] = data[i] ^ password[i % passwordLength];
    }
    
    return encryptedData;
}

// 解密函数
std::string XOREncryption::decryptData(const std::string& encryptedData, const std::string& password) {
    // 解密过程与加密过程相同
    return encryptData(encryptedData, password);
}