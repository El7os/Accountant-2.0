
#include "..\EncryptionLayer\Encryptioner.h"
#include "..\DatabaseLayer\DatabaseController.h"
#include <iostream>

int main()
{
	DatabaseController& Controller = DatabaseController::GetController();

	

	
	std::filesystem::path Path("C:\\Users\\Pro_y\\Documents\\Github\\Accountant-2.0");
	Controller.SetPath(Path);
	DatabaseController::EConnectionResult Result = Controller.InitializeConnection();
	std::cout << "Result : " << Result << std::endl;

	return 0;
}