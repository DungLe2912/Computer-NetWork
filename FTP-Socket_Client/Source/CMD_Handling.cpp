#pragma once

#include"stdafx.h"
#include"FTPClient.h"
using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////
// Method: cmd_quit()
// Last modify: 17:15 PM 23/5/2018
// Coder: Do Minh Hieu
// Use for: Thoát hoàn toàn khỏi ftp server
// Gửi command QUIT tới server và xuất ra message server thông báo lại
// đóng kết nối điều khiển
// cho isConnected = false và isLogin = false, set lại mode = passive
void FTPClient::cmd_quit()
{
	if (this->isLogged == true && this->isConnected == true)
	{
		this->request = "QUIT\r\n";
		this->Full_act();
		this->Receive_respone();
		cout << this->respone;
	}
	this->cmdMain.Close();
	this->isConnected = false;
	this->isLogged = false;
	this->mode = MODE::PASSIVE;
	this->parameter.clear();
	exit(0);
}

/*liet ke chi tiet*/
void FTPClient::cmd_dir()
{
	int pos = -1;
	CSocket*data_transfer = NULL;
	if (this->mode == 0)
		data_transfer = Pass_Mode();
	else
		data_transfer = Act_Mode();
	if ((data_transfer) && (this->getCode() == Code_FTPClient::Open_PASV_Port || this->getCode() == Code_FTPClient::cmd_succ))
	{
		this->request = "LIST\r\n";
		this->Full_act();
		
		pos = this->respone.find("226");
		if (this->getCode() == Code_FTPClient::Ready_Transfer) //=150
		{
			CSocket*curent = data_transfer;
			CSocket Connector;
			if (this->mode == MODE::ACTIVE)// active
			{
				if (data_transfer->Listen(1) == false)
				{
					cout << "Cannot listen on this port!" << endl;
					return;
				}
				if (data_transfer->Accept(Connector))
					curent = &Connector; //works in active mode
				else
					cout << "Cannot accept this connector" << endl;
			}
				int len;
				char*buffer = new char[BUFF_LENGTH];
				do
				{
					memset(buffer, 0, BUFF_LENGTH);
					len = curent->Receive(buffer, BUFF_LENGTH, 0);
					cout << buffer;
				} while (len > 0);
				delete buffer;
			}
		}
	else
	{
		cout << "Command get failed. Try again!" << endl;
	}
	if (pos==-1)
	{
		this->Receive_respone();
		this->displayMess();
	}
	data_transfer->Close();
	delete data_transfer;
}

/*liet ke don gian*/
void FTPClient::cmd_ls()
{
	int pos = -1;
	CSocket*data_transfer = NULL;
	if (this->mode == 0)
		data_transfer = Pass_Mode();
	else
		data_transfer = Act_Mode();
	if ((data_transfer) && (this->getCode() == Code_FTPClient::Open_PASV_Port || this->getCode() == Code_FTPClient::cmd_succ))
	{
		this->request = "NLST\r\n";
		this->Full_act();
		pos = this->respone.find("226");
		if (this->getCode() == Code_FTPClient::Ready_Transfer) //=150
		{
			CSocket*curent = data_transfer;
			CSocket Connector;
			if (this->mode == MODE::ACTIVE)// active
			{
				if (data_transfer->Listen(1) == false)
				{
					cout << "Cannot listen on this port!" << endl;
					return;
				}
				if (data_transfer->Accept(Connector))
					curent = &Connector; //works in active mode
				else
					cout << "Cannot accept this connector" << endl;
			}
				int len;
				char*buffer = new char[BUFF_LENGTH];
				do
				{
					memset(buffer, 0, BUFF_LENGTH);
					len = curent->Receive(buffer, BUFF_LENGTH, 0);
					cout << buffer;
				} while (len > 0);
				delete buffer;
			}
		}
	
	else
	{
		cout << "Command get failed. Try again!" << endl;
	}
	if (pos==-1)
	{
		this->Receive_respone();
		this->displayMess();
	}
	data_transfer->Close();
	delete data_transfer;
}

/*Xoa 1 file*/
void FTPClient::cmd_del(string fileName)
{
	this->request = "DELE " + fileName + "\r\n";
	this->Full_act();
}
/*Hien thi duong dan hien tai tren server*/
void FTPClient::cmd_pwd()
{
	this->request = "PWD\r\n";
	this->Full_act();
}
/*Tao thu muc tren server*/
void FTPClient::cmd_mkdir(string folderName)
{
	this->request = "MKD " + folderName + "\r\n";
	this->Full_act();
}
/*Xoa thu muc rong tren server*/
void FTPClient::cmd_rmdir(string folderName)
{
	this->request = "RMD " + folderName + "\r\n";
	this->Full_act();
}
/*Thay doi duong dan tren server*/
void FTPClient::cmd_cd(string path)
{
	this->request = "CWD " + path + "\r\n";
	this->Full_act();
}
///////////////////////////////////////////////////////////////
// Method: cmd_mdele()
// Modify: 17:24 23/5/2018
// Modifier: Do Minh Hieu
// reason: sửa lại lựa chọn.
void FTPClient::cmd_mdele()
{
	for (int i = 0; i < parameter.size(); i++)
	{
		char c;
		cout << "mdelete " << this->parameter[i] << " ? (y/n): ";
		c = getch();
		cout << c << endl;
		if (c == 'y' || c == 'Y')
			cmd_del(parameter[i]);
	}
}

///////////////////////////////////////////////////////////////
// Method: cmd_lcd()
// Modify: 0:36 25/5/2018
// Modifier: Do Minh Hieu
// Thay đổi lại toàn bộ.
void FTPClient::cmd_lcd()
{
	TCHAR buf[BUFSIZ];
	TCHAR *param = new TCHAR[parameter.at(0).size() + 1];
	param[parameter.at(0).size()] = 0;
	copy(parameter.at(0).begin(), parameter.at(0).end(), param);
	if (!SetCurrentDirectory(param))
		cout << "Set current directory false." << endl;
	else
	{
		GetCurrentDirectory(BUFSIZ, buf);
		wstring s(buf);
		string local_path(s.begin(), s.end());
		cout << "Local directory now " << local_path << endl;
	}
	delete param;
}

/*Download file tu server*/
void FTPClient::cmd_get_once_File(string fileName)
{
	int pos = -1;
	int check = -1;
	CSocket*data_transfer = NULL;
	if(this->mode==0)
		data_transfer = Pass_Mode();
	else
		data_transfer = Act_Mode();
	if ((data_transfer) && (this->getCode() == Code_FTPClient::Open_PASV_Port || this->getCode() == Code_FTPClient::cmd_succ))
	{
		this->request = "RETR " + fileName + "\r\n";
		this->Full_act();
		pos = this->respone.find("226");
		check = this->respone.find("550");
		if (this->getCode() == Code_FTPClient::Ready_Transfer) //=150
		{
			if (fileName.find("\"") != -1)
			{
				fileName.erase(0, 1);
				fileName.erase(fileName.length() - 1, 1);
			}
			ofstream os = ofstream(fileName, ios::binary);
			if (os.is_open())
			{
				CSocket*curent = data_transfer;
				CSocket Connector;
				if (this->mode == 1)// active
				{
					if (data_transfer->Listen(1) == false)
					{
						cout << "Cannot listen on this port!" << endl;
						return;
					}
					if (data_transfer->Accept(Connector))
						curent = &Connector; //works in active mode
					else
						cout << "Cannot accept this connector" << endl;
				}
				int len;
				char*buffer = new char[BUFF_LENGTH];
				do
				{
					memset(buffer, 0, BUFF_LENGTH);
					len = curent->Receive(buffer, BUFF_LENGTH, 0);
					os.write(buffer, len);
				} while (len > 0);
				os.close();
				delete buffer;
			}
			else
			{
				cout << fileName << ": File not found" << endl;
				check = 1;
			}
		}
	}
	else
	{
		cout << "Command get failed. Try again!" << endl;
	}
	if (pos == -1&&check==-1)
	{
		this->Receive_respone();
		this->displayMess();
	}
	delete data_transfer;
}

void FTPClient::cmd_get()
{
	this->cmd_get_once_File(this->parameter.at(0));
}
/*Download nhieu file*/
void FTPClient::cmd_mget()
{
	for (int i = 0; i < this->parameter.size(); i++)
	{
		char c;
		cout << "Do you wanna download file " << parameter[i] << " ? (y,n): ";
		c = getch();
		cout << c << endl;
		if (c == 'y' || c == 'Y')
			cmd_get_once_File(parameter[i]);
	}
}

/*Upload 1 file len server theo parameter truyen vao*/
bool FTPClient::cmd_put_once_File(string fileName)
{
	int pos = -1;
	int check = -1;
	CSocket*data_transfer = NULL;
	bool resultRet = true;
	ifstream put;
	
	if (fileName.find("\"") != -1)
	{
		fileName.erase(0, 1);
		fileName.erase(fileName.length() - 1, 1);
	}

	put.open(fileName.c_str(), ios::binary);
	if (put.is_open())
	{
		if (this->mode == 0)
			data_transfer = Pass_Mode();
		else
			data_transfer = Act_Mode();
		if (data_transfer && (this->getCode() == Code_FTPClient::Open_PASV_Port || this->getCode() == Code_FTPClient::cmd_succ))
		{
			this->request = "STOR " + fileName + "\r\n";
			this->Full_act();
			pos = this->respone.find("226");
			check = this->respone.find("550");
			if (this->getCode() == Code_FTPClient::Ready_Transfer) //150
			{
				CSocket*curent = data_transfer; //mac dinh la passive
				CSocket Connector;
				streamsize len;
				char *buffer = new char[BUFF_LENGTH];
				if (this->mode == 1)//active
				{
					if (data_transfer->Listen(1) == false)
					{
						cout << "Cannot listen on this port! " << endl;
						return false;
					}
					if (data_transfer->Accept(Connector))
						curent = &Connector;
					else
					{
						cout << "Cannot accept this connector!" << endl;
						return false;
					}
				}
				//tien hanh doc va gui du lieu
				while (!put.eof())
				{
					memset(buffer, 0, BUFF_LENGTH);
					put.read(buffer, BUFF_LENGTH);
					len = put.gcount();
					curent->Send(buffer, len);
				}
				put.close();
				delete buffer;
			}
		}
		else
		{
			cout << "Command Failed. Try again!" << endl;
			resultRet = false;
			check = 1;
		}
		delete data_transfer;
	}
	else
	{
		cout << fileName << ": Not found!" << endl;
		check = 1;
		resultRet = false;
	}
	if (pos == -1&&check==-1)
	{
		this->Receive_respone();
		this->displayMess();
	}
	return resultRet;
}
/*Upload 1 file*/
void FTPClient::cmd_put()
{
	bool resultRet = this->cmd_put_once_File(this->parameter.at(0));
	if (this->getCode() != Code_FTPClient::Transfer_complete&&resultRet == 1)
	{
		this->Receive_respone();
		this->displayMess();
	}
}
/*Upload nhieu file*/
void FTPClient::cmd_mput()
{
	bool resultRet;
	for (int i = 0; i < this->parameter.size(); i++)
	{
		char c;
		cout << "Do you wanna upload file " << parameter[i] << " ? (y,n): ";
		c = getch();
		cout << c << endl;
		if (c == 'y' || c == 'Y')
		{
			resultRet = this->cmd_put_once_File(this->parameter.at(i));
			if (this->getCode() != Code_FTPClient::Transfer_complete&&resultRet == 1)
			{
				this->Receive_respone();
				this->displayMess();
			}
		}
	}
}
void FTPClient::cmd_passive()
{
	this->mode = MODE::PASSIVE;
}
void FTPClient::cmd_active()
{
	this->mode = MODE::ACTIVE;
}

/*Help*/
void FTPClient::cmd_help()
{
	cout << "open\tls\tdir\tput\tget\n";
	cout << "mput\tmget\tcd\tlcd\tdelete\n";
	cout << "mdelete\tmkdir\trmdir\tpwd\tpassive\n";
	cout << "active\tquit\thelp\n";
}
/*open*/
void FTPClient::cmd_open()
{
	this->cmdMain.Close();
	this->isConnected = false;
	this->isLogged = false;
	this->mode = MODE::ACTIVE;
	this->cmdMain.Create();
	
	this->ip_server = this->parameter[0];
	
	this->Connect();
	if (this->isConnected == true)
		this->Login();
}

void FTPClient::cmd_user()
{
	this->username = this->parameter[0];
	this->request = "USER " + this->username + "\r\n";
	this->Full_act();
	if (this->getCode() == Code_FTPClient::Request_pass)
	{
		cout << "Password: ";
		readPassword(this->password);
		this->request = "PASS " + this->password + "\r\n";
		this->Full_act();
		if (this->getCode() == Code_FTPClient::FTPlogin_succ)
			this->isLogged = true;
		else
			this->isLogged = false;
	}
}