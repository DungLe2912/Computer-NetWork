#pragma once
#include"stdafx.h"
#include"FTPClient.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////
// Method: Login()
// Last modify: 16:26 PM 23/5/2018
// Coder: Do Minh Hieu
// Use for: Nhạp username password và login vào ftp server
// Nếu user hoặc password bị response lỗi thì isLogin = false
// Ngược lại isLogin = true
// return về isLogin
int FTPClient::Login()
{
	cout << "Username: ";
	getline(cin, this->username);
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
	return this->isLogged;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method: Connect()
// Last modify: 16:30 PM 23/5/2018
// Coder: Do Minh Hieu
// Use for: Khởi tạo kết nối điều khiển tới ftp server
// Chuyển chuỗi ip_addr qua dạng tchar* để chạy phương thức Connect
// Nếu connect thành công thì response các message từ ftp server và cho isConnected = true
// ngược lại isconnected = false
// return về isConnected
// thêm get ip_client, port control, portdata active
int FTPClient::Connect()
{
	TCHAR*param = new TCHAR[this->ip_server.size() + 1];
	param[this->ip_server.size()] = 0;
	copy(this->ip_server.begin(), this->ip_server.end(), param);
	if (cmdMain.Connect(param, 21) != 0)
	{
		cout << "Connected to " << this->ip_server << endl;
		do
		{
			this->Receive_respone();
			int ftpcode = this->getCode();
			if (this->respone.length() <= 0)
				break;
			cout << this->respone;
			if (ftpcode == 220)
				break;
		} while (1);
		this->isConnected = true;

		// get ip của client, port control
		CString host;
		this->cmdMain.GetSockName(host, portControl);
		CT2CA pszConvertedAnsiString(host);
		string ip_get_client(pszConvertedAnsiString);
		this->ip_client = ip_get_client;
		//set portData active. last_portData_active = portControl + 2 vì portData passive = portControl + 1
		last_portData_active = portControl + 2;
	}
	else
	{
		cout << "Has problem with server IP address! Use command OPEN <server ip address> to reconnect!" << endl;
		this->isConnected = false;
	}

	delete param;
	return this->isConnected;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method: Pass_Mode()
// Last modify: 16:45 PM 23/5/2018
// Coder: Do Minh Hieu
// Use for: Tạo chế độ passive cho datasocket
// Khởi tạo DataSocket và port(DataSocket) = port(cmdMain) + 1
// gửi request PASV tói server, server reesponse về port m server đang mở
// cho datasocket connect tới server ở port m
// return về con trỏ của DataSocket
CSocket*FTPClient::Pass_Mode()
{
	CSocket* DataSocket = new CSocket;
	if (DataSocket == NULL)
		return NULL;

	UINT portData = portControl + 1;
	bool check = true;
	while (!DataSocket->Create(portData))
	{
		if (check == true)
		{
			cout << "Trying to create socket transfer data..." << endl;
			check = false;
		}
	}

	this->request = "PASV\r\n";
	this->Full_act();

	UINT portServer = this->getPort();

	TCHAR *param = new TCHAR[this->ip_server.size() + 1];
	param[this->ip_server.size()] = 0;
	copy(this->ip_server.begin(), this->ip_server.end(), param);

	while (DataSocket->Connect(param, portServer) == 0);
	
	delete param;
	return DataSocket;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method: Act_Mode()
// Last modify: 01:33 PM 25/5/2018
// Coder: Do Minh Hieu
// Use for: Tạo chế độ active cho datasocket
// tạo socket data với last_portdata_active
// sửa lại IP ở PORT là ip của socket data
CSocket*FTPClient::Act_Mode()
{
	CSocket*act_data = new CSocket;
	short p1, p2;
	bool check = true;	// biến này để xuất ra thông báo tạo kênh truyền được chưa. mặc định là tạo đc.
	
	while (!act_data->Create(this->last_portData_active, SOCK_STREAM, NULL))
	{
		this->last_portData_active += 1;
		if (check == true)
		{
			cout << "Trying to create socket transfer data..." << endl;
			check = false;
		}
	}

	p1 = this->last_portData_active / 256;
	p2 = this->last_portData_active % 256;
	
	while (!act_data->Listen(1));
	
	string sIp_client = this->Setter(this->ip_client);
	this->request = "PORT " + sIp_client + "," + to_string(p1) + "," + to_string(p2) + "\r\n";
	this->Full_act();
	// tăng port data active cho lần sau sử dụng
	if (this->last_portData_active == 65535)
		this->last_portData_active = this->portControl + 2;
	else
		this->last_portData_active += 1;

	if (this->getCode() != 200)
	{
		delete act_data;
		return NULL;
	}
	
	return act_data;
}


/*Get Port from respone message from server*/
int FTPClient::getPort()
{
	stringstream tmp;
	string unusedStr;
	int unusedInt, p1, p2, a;
	char unusedChar;
	tmp = stringstream(this->respone);
	tmp >> a;
	getline(tmp, unusedStr, '(');
	tmp >> unusedInt >> unusedChar >> unusedInt >> unusedChar
		>> unusedInt >> unusedChar >> unusedInt >> unusedChar
		>> p1 >> unusedChar >> p2;
	if (tmp.fail())
		return -1;
	else
		return p1 * 256 + p2;
}


//Action for ftp command
void FTPClient::Full_act()
{
	this->Send_mess();
	this->Receive_respone();
	this->displayMess();
}
/*Send request to server*/
void FTPClient::Send_mess()
{
	this->cmdMain.Send(this->request.c_str(),this->request.length());
}

void FTPClient::send_command(string cmd)
{
	this->cmdMain.Send(cmd.c_str(), cmd.size());
}

/*Receive messeage respone from server*/
int FTPClient::Receive_respone()
{
	this->respone.clear();
	char*buffer = new char[BUFF_LENGTH];
	memset(buffer, 0, BUFF_LENGTH);
	int ret = this->cmdMain.Receive(buffer, BUFF_LENGTH, 0);
	this->respone = string(buffer).substr(0, ret);
	delete buffer;
	return ret;
}

/*Display messeage*/
void FTPClient::displayMess()
{
	cout << this->respone;
}

/*Get code in respone string from server*/
int FTPClient::getCode()
{
	string tmp = this->respone;
	string ret = "";
	ret = tmp.substr(0, tmp.find_first_of(' '));
	return atoi(ret.c_str());
}

/*read password from your keyboard*/
void readPassword(string& pass) {
	bool isEnter = false;
	pass = "";

	while (!isEnter) {
		if (_kbhit()) {
			char c = _getch();

			if (c == '\n' || c == '\r') {
				isEnter = true;
				break;
			}
			else if (c == '\b') {
				cout << "\b \b";
				if (pass.length() > 0)
					pass.pop_back();
			}
			else {
				cout << "*";
				pass.push_back(c);
			}
		}
	}
	cout << endl;
}

///////////////////////////////////////////////
// Method: Setter()
// Last modify: 01:50 25/5/2018
// By: Do Minh Hieu
// reason: để trả về chuỗi string là kết quả sau khi đổi dấu . thành dấu ,
string FTPClient::Setter(string tmp)
{
	for (int i = 0; i < tmp.length(); i++)
	{
		if (tmp[i] == '.')
			tmp[i] = ',';
	}
	return tmp;
}

/*Init class*/
FTPClient::FTPClient(TCHAR*host)
{
	if (host == NULL)
	{
		cout << "Open host: ";
		cin >> this->ip_server;
	}
	else
	{
		wstring s(host);
		string ip(s.begin(), s.end());
		this->ip_server = ip;
	}
	this->isConnected = 0;
	this->isLogged = 0;
	this->cmdMain.Create();
	this->mode = MODE::ACTIVE;
	this->order_cmd = { "open","ls","dir","put","get","mput","mget","cd","lcd","delete","mdelete","mkdir","rmdir","pwd","passive","active","quit","help","user" };
}

/*Destroy class*/
FTPClient::~FTPClient()
{
	this->cmdMain.Close();

}