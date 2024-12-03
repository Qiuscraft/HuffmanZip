#include "XOREncryption.h"

string XOREncryption::encryptData(const string& data, const string& password) {
    // 默认不加密
    size_t passwordLength = password.length();
    if (passwordLength == 0)
        return data;
    
    // 加密过程
    string encryptedData = data;
    for (size_t i = 0; i < data.length(); ++i) {
        encryptedData[i] = data[i] ^ password[i % passwordLength];
    }
    
    return encryptedData;
}

string XOREncryption::decryptData(const string& encryptedData, const string& password) {
    // 解密过程与加密过程相同
    return encryptData(encryptedData, password);
}
