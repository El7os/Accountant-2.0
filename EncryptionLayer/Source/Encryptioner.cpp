#include "../Encryptioner.h"
#include <openssl/aes.h>


std::string Encryptioner_AES::Encrypt(const std::string& Data, const std::string& Key)
{
    const unsigned char* RawData = reinterpret_cast<const unsigned char*>(Data.c_str());
    const unsigned char* KeyPtr = reinterpret_cast<const unsigned char*>(Key.c_str());
    unsigned char* CipherText = nullptr;
    AESEncrypt(RawData, Data.length(), KeyPtr, nullptr, CipherText);
    return std::string(reinterpret_cast<const char*>(CipherText), sizeof(CipherText));
}

std::string Encryptioner_AES::Decrypt(const std::string& Data, const std::string& Key)
{
    return std::string();
}

void Encryptioner_AES::AESEncrypt(const unsigned char* plaintext, int plaintext_len, const unsigned char* key, unsigned char* iv, unsigned char* ciphertext) {
    AES_KEY aes_key;
    AES_set_encrypt_key(key, 128, &aes_key);
    AES_cbc_encrypt(plaintext, ciphertext, plaintext_len, &aes_key, iv, AES_ENCRYPT);
}

void Encryptioner_AES::AESDecrypt(const unsigned char* ciphertext, int ciphertext_len, const unsigned char* key, unsigned char* iv, unsigned char* decryptedtext) {
    AES_KEY aes_key;
    AES_set_decrypt_key(key, 128, &aes_key);
    AES_cbc_encrypt(ciphertext, decryptedtext, ciphertext_len, &aes_key, iv, AES_DECRYPT);
}