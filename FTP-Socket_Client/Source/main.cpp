// FTPClient.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "FTPClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define _WINSOCK_DEPRECATED_NO_WARNING
// The one and only application object

CWinApp theApp;

using namespace std;

///////////////////////////////////////////////////////////////////
// Function: _tmain()
// Last modify: 11:00 24/5/2018
// By: Do Minh Hieu
// Reason: sửa main thành _tmain
// thêm Client.Interface()
// sửa cout << "Can't install socket library." << endl;
// thêm system("pause") vào chỗ need 2 parametter
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			wprintf(L"Fatal Error: MFC initialization failed\n");
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			if (AfxSocketInit() == FALSE)
			{
				cout << "Can't install socket library." << endl;
				return FALSE;
			}
			//Chuong trinh bat dau
			try {
				if (argc != 2)
				{
					cout << "Need 2 parametters for program!" << endl;
					system("pause");
					exit(0);
				}
				else
				{
					FTPClient Client(argv[1]);
					Client.Interface();
				}
			
			}
			catch (l_exception& e)
			{
				cout << "ERROR: See you late!" << endl;
			}

		}
	}

	else
	{
		// TODO: change error code to suit your needs
		wprintf(L"Fatal Error: GetModuleHandle failed\n");
		nRetCode = 1;
	}

	return nRetCode;
}

