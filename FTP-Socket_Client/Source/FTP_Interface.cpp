#pragma once

#include"stdafx.h"
#include"FTPClient.h"
using namespace std;

/*Chuan hoa chuoi cmd nhap vao*/
string chuanhoachuoi(string tmp)
{
	while (tmp[0] == ' ')
		tmp.erase(0, 1);
	while (tmp[tmp.size() - 1] == ' ')
		tmp.erase(tmp.size() - 1, 1);
	int pos = 0;
	for (int i = 0; i < tmp.length(); i++)
	{
		pos = i;
		if (tmp[i] == ' ')break;
		if (isalpha(tmp[i]) && isupper(tmp[i]))
			tmp[i] = tolower(tmp[i]);
	}
	for (int i = pos; i < tmp.length(); i++)
	{
		if (i < tmp.length() - 1)
		{
			if (tmp[i] == ' '&&tmp[i + 1] == ' ')
			{
				tmp.erase(i, 1);
				i--;
			}
		}
	}
	return tmp;

}
/*Cat chuoi de tao thanh order va cac parameter*/
//////////////////////////////////////////////////////////////////////////////////////////
// Method: splitOrderString()
// Last modify: 0:34 PM 25/5/2018
// Coder: Do Minh Hieu
// reason: sửa lại if (tmp.find(" ") != -1)
// nếu các lệnh dạng ls, pwd, dir thì sẽ không thêm vào parametter
// xóa xuất ra các parametter. lý do: không cần thiết.
// thêm this->parametter.clear()
// thêm trường hợp tên file có khoảng trắng
void FTPClient::splitOrderString(string tmp)
{
	int pos1 = -1;
	int pos2 = -1;
	string res = "";
	string a;
	this->parameter.clear();

	if (tmp.find("\"") != -1)
	{
		for (int i = 0; i < tmp.length(); i++)
		{
			if (tmp[i] == '\"' && pos1 == -1)
			{
				pos1 = i;
				continue;
			}
			if (tmp[i] == '\"' && pos1 != -1)
			{
				pos2 = i + 1;
			}

			if (pos1 != -1 && pos2 != -1)
			{
				parameter.push_back(tmp.substr(pos1, pos2 - pos1));
				tmp.erase(pos1 - 1, pos2 - pos1 + 1);
				pos1 = -1;
				pos2 = -1;
				i = pos1;
			}
		}
	}
	pos1 = -1;
	pos2 = 0;
	if (tmp.find(" ") != -1)
	{
		a = tmp.substr(tmp.find_first_of(' ') + 1);
		for (int i = 0; i < a.length(); i++)
		{
			if (a[i] == ' ')
				pos1 = i;
			if (i == a.length() - 1)
				pos1 = i + 1;
			if (pos1 != -1)
			{
				res = a.substr(pos2, pos1 - pos2);
				this->parameter.push_back(res);
				pos2 = pos1 + 1;
				pos1 = -1;
			}
		}
	}
}
/*Tim kiem vi tri cua oder trong bo cac order co san*/
int FTPClient::find_pos_order(string tmp)
{
	for (int i = 0; i < this->order_cmd.size(); i++)
	{
		if (tmp.compare(order_cmd.at(i)) == 0)
			return i;
	}
	return -1;
}
/*Giao dien ftp*/
///////////////////////////////////////////////////////////////
// Method: Interface()
// Last modify: 18:36 23/5/2018
// By: Do Minh Hieu
// Reason: Sửa lại phần connect + login
// xóa this->parametter.clear() ở case 3,4,5,6,7,8,9,10,11,12
// xóa các system("cls") để có thể hiện câu lệnh mà người dùng nhập
// xóa phần nhập ip của client
void FTPClient::Interface()
{
	string cmd = "";
	string order = "";
	
	system("cls");

	this->Connect();
	if (this->isConnected == true)
		this->Login();

CMD:
	cout << "ftp> ";
	getline(cin, cmd);
	if (cmd == "")
		goto CMD;
	cmd = chuanhoachuoi(cmd);
	string a = cmd;
	this->splitOrderString(a);
	order = cmd.substr(0, cmd.find_first_of(' '));
	int vt = this->find_pos_order(order);
	// vt = 0: open
	// vt = 16: quit
	// vt = 17: help
	if ((this->isConnected == false) && (vt != 0 && vt !=16 && vt != 17))
	{
		cout << "Not connected!" << endl;
		goto CMD;
	}
	// vt = 18: USER
	if ((this->isLogged == false) && vt != 0 && vt != 16 && vt != 17 && vt != 18)
	{
		cout << "Not logged in!" << endl;
		goto CMD;
	}
		
	switch (vt)
	{
	case 0:
		if (this->parameter.size() == 0)
		{
			cout << "Command Fail!" << endl;
			break;
		}
		this->cmd_open();
		break;
	case 1:
		this->cmd_ls();
		break;
	case 2:
		this->cmd_dir();
		break;
	case 3:
		if (this->parameter.size() == 0)
		{
			cout << "Command Fail!" << endl;
			break;
		}
		this->cmd_put();
		break;
	case 4:
		if (this->parameter.size() == 0)
		{
			cout << "Command Fail!" << endl;
			break;
		}
		this->cmd_get();
		break;
	case 5:
		if (this->parameter.size() == 0)
		{
			cout << "Command Fail!" << endl;
			break;
		}
		this->cmd_mput();
		break;
	case 6:
		if (this->parameter.size() == 0)
		{
			cout << "Command Fail!" << endl;
			break;
		}
		this->cmd_mget();
		break;
	case 7:
		if (this->parameter.size() == 0)
		{
			cout << "Command Fail!" << endl;
			break;
		}
		this->cmd_cd(this->parameter.at(0));
		break;
	case 8:
		if (this->parameter.size() == 0)
		{
			cout << "Command Fail!" << endl;
			break;
		}
		this->cmd_lcd();
		break;
	case 9:
		if (this->parameter.size() == 0)
		{
			cout << "Command Fail!" << endl;
			break;
		}
		this->cmd_del(this->parameter.at(0));
		break;
	case 10:
		if (this->parameter.size() == 0)
		{
			cout << "Command Fail!" << endl;
			break;
		}
		this->cmd_mdele();
		break;
	case 11:
		if (this->parameter.size() == 0)
		{
			cout << "Command Fail!" << endl;
			break;
		}
		this->cmd_mkdir(this->parameter.at(0));
		break;
	case 12:
		if (this->parameter.size() == 0)
		{
			cout << "Command Fail!" << endl;
			break;
		}
		this->cmd_rmdir(this->parameter.at(0));
		break;
	case 13:
		this->cmd_pwd();
		break;
	case 14:
		cout << "Change mode passive complete" << endl;
		this->cmd_passive();
		break;
	case 15:
		cout << "Change mode active complete" << endl;
		this->cmd_active();
		break;
	case 16:
		this->cmd_quit();
		break;
	case 17:
		this->cmd_help();
		break;
	case 18:
		this->cmd_user();
		break;
	default:
		cout << "COMMAND ERROR" << endl;
		break;
	}
	goto CMD;
}