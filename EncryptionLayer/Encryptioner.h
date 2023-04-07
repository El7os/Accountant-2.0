#pragma once

//class string;
#include <string>

class Encryptioner
{
public:

	virtual std::string Encrypt(const std::string& Data, const std::string& Key) = 0;

	virtual std::string Decrypt(const std::string& Data, const std::string& Key) = 0;
};

class Encryptioner_AES final : public Encryptioner
{
public:

	std::string Encrypt(const std::string& Data, const std::string& Key) override;

	std::string Decrypt(const std::string& Data, const std::string& Key) override;

private:

	void AESEncrypt(const unsigned char* plaintext, int plaintext_len, const unsigned char* key, unsigned char* iv, unsigned char* ciphertext);

	void AESDecrypt(const unsigned char* ciphertext, int ciphertext_len, const unsigned char* key, unsigned char* iv, unsigned char* decryptedtext);
};
