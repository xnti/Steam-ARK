#include <iostream>
#include <Windows.h>


using namespace std;

int main()
{
	SetConsoleTitle(TEXT("ARK Helper"));


	string Username;
	cout << "Please enter your Username: ";
	cin >> Username;
	system("cls");
	cout << endl << "Welcome back " << Username << "!" << endl;

	cout << endl << "Please choose from the following Options: " << endl << endl;
	cout << "1. - Scanning UWorld" << endl;
	cout << "2. - Dumping GObjects, GNames" << endl;
	cout << "3. - Sig Scan UWorld, GObjects, GNames" << endl << endl;


	cout << "Option: ";
	int tt;
	cin >> tt;
	system("cls");

	int a = 1;
	int b = 2;
	int c = 3;

	int UWorld = 0;
	int GObjects = 0;
	int GNames = 0;

	if (tt == a)
	{
		cout << "Scanning UWorld...... " << endl;
		Sleep(4000);
		cout << "UWorld Adress: " << endl;
	}
	else if (tt == b)
	{
		cout << "Processing Dumps for ARK Survival Evolved" << endl;

		Sleep(2000);
		system("cls");
		cout << "GObjects and GNames were successfully created! " << endl << endl;
	}
	else if (tt == c)
	{
		system("cls");
		cout << "Scanning for UWorld, GObjects and GNames....... ";

		Sleep(4000);

		system("cls");
		cout << "Succesfully scanned all Offsets! " << endl << endl;

		cout << "UWorld: " << UWorld << endl;
		cout << "GObjects: " << GObjects << endl;
		cout << "GNames: " << GNames << endl;

	}
	else
	{
		cout << "Error! Please try it again." << endl;
	}

	system("pause");

	int o = 0; /* Option (1) */

}