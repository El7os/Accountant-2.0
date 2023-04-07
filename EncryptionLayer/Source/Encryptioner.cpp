#include "../Encryptioner.h"
#include <iostream>
#include <openssl/aes.h>


std::string Encryptioner_AES::Encrypt(const std::string& Data, const std::string& Key)
{
    /*std::string EncryptedData;
    AES_KEY AesKey;
    if (AES_set_encrypt_key((const unsigned char*)Key.c_str(), 128, &AesKey) < 0) {
        throw std::runtime_error("Failed to set AES key");
    }
    EncryptedData.resize(Data.size() + AES_BLOCK_SIZE);
    unsigned char* EncryptedDataPtr = (unsigned char*)EncryptedData.data();
    const unsigned char* DataPtr = (const unsigned char*)Data.data();
    int EncryptedDataLength = Data.length();
    AES_cbc_encrypt(DataPtr, EncryptedDataPtr, Data.size(), &AesKey, EncryptedDataPtr, AES_ENCRYPT);
    EncryptedData.resize(EncryptedDataLength);
    return EncryptedData;*/
    std::string EncryptedData;
    AES_KEY AesKey;
    if (AES_set_encrypt_key((const unsigned char*)Key.c_str(), 128, &AesKey) < 0) {
        throw std::runtime_error("Failed to set AES key");
    }
    EncryptedData.resize(Data.size() + AES_BLOCK_SIZE);
    unsigned char* EncryptedDataPtr = (unsigned char*)EncryptedData.data();
    const unsigned char* DataPtr = (const unsigned char*)Data.data();
    int EncryptedDataLength = Data.length();
    AES_cbc_encrypt(DataPtr, EncryptedDataPtr, Data.size(), &AesKey, EncryptedDataPtr, AES_ENCRYPT);
    EncryptedData.resize(EncryptedDataLength);
    return EncryptedData;
}

std::string Encryptioner_AES::Decrypt(const std::string& EncryptedData, const std::string& Key)
{
    /*std::string DecryptedData;
    AES_KEY AesKey;
    if (AES_set_decrypt_key((const unsigned char*)Key.c_str(), 128, &AesKey) < 0) {
        throw std::runtime_error("Failed to set AES key");
    }
    DecryptedData.resize(Data.size());
    unsigned char* DecryptedDataPtr = (unsigned char*)DecryptedData.data();
    const unsigned char* EncryptedDataPtr = (const unsigned char*)Data.data();
    AES_cbc_encrypt(EncryptedDataPtr, DecryptedDataPtr, Data.size(), &AesKey, DecryptedDataPtr, AES_DECRYPT);
    DecryptedData.resize(strlen(DecryptedData.c_str()));
    return DecryptedData;*/

    std::string DecryptedData;
    AES_KEY AesKey;
    if (AES_set_decrypt_key((const unsigned char*)Key.c_str(), 128, &AesKey) < 0) {
        throw std::runtime_error("Failed to set AES key");
    }
    DecryptedData.resize(EncryptedData.size());
    unsigned char* DecryptedDataPtr = (unsigned char*)DecryptedData.data();
    const unsigned char* EncryptedDataPtr = (const unsigned char*)EncryptedData.data();
    int DecryptedDataLength = EncryptedData.size();
    AES_cbc_encrypt(EncryptedDataPtr, DecryptedDataPtr, EncryptedData.size(), &AesKey, DecryptedDataPtr, AES_DECRYPT);
    DecryptedData.resize(DecryptedDataLength);
    return DecryptedData;
}
