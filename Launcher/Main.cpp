
#include "..\EncryptionLayer\Encryptioner.h"
#include <iostream>

int main()
{
	Encryptioner_AES Encryptioner;

	std::string Key = "Yahya";

	std::string EncryptedData = Encryptioner.Encrypt("Benim Emin Yahya", Key);
	std::string DecryptedData = Encryptioner.Decrypt(EncryptedData, Key);

	std::cout << "Encryped Data : " << EncryptedData << std::endl;
	std::cout << "Decryped Data : " << DecryptedData << std::endl;
	
	

	return 0;
}