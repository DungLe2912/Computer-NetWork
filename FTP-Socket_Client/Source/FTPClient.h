#pragma once

#include<afxsock.h>
#include<iostream>
#include<string>
#include<stdio.h>
#include<conio.h>
#include"Resource.h"
#include<sstream>
#include<Windows.h>
#include"stdafx.h"
#include<vector>
#include<fstream>
#include<tchar.h>

#define MAXLINE 4096 /*max text line length*/
#define BUFF_LENGTH 10000

/*Define BUFFER_LENGTH*/

using namespace std;

#define D_SCL_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
/*Define FTP code*/


///////////////////////////////////////////////////////////////////////
// Last modify: 18:00 23/5/2018
// By: Do Minh Hieu
// - thêm enum MODE
// - xóa CMD_user_response và CMD_pass_response. lý do: không cần thiết
// - xóa client_addr: chỉ dùng ip của socket control ở command PORT
// - sửa lại method Setter: void -> string
// - thêm portControl, last_portData_active, sửa ip_adrr->ip_server


enum Code_FTPClient
{
	FTPconnect_succ = 220,
	FTPlogin_succ = 230,
	FTPlogin_fail = 530,
	cmd_succ = 200,
	connect_fail = 421,
	OpenDataConnect_fail = 425,
	Ready_Transfer = 150,
	Open_PASV_Port = 227,
	Cannot_Open_Data_Connect = 425,
	Request_pass = 331,
	Transfer_complete = 226
};

enum MODE
{
	PASSIVE = 0,
	ACTIVE = 1
};

//struct sockaddr_in servaddr;

class FTPClient
{
private:
	CSocket cmdMain;
	UINT portControl;
	UINT last_portData_active;
	vector<string> parameter;
	vector<string>order_cmd;
	//properties of socket data
	string username;
	string password;
	string ip_server;
	string ip_client;
	string request;
	string respone;
	int isLogged;
	int isConnected;
	int mode;
public:
	int Login();
	int Connect();
	void Send_mess();
	int Receive_respone();
	void displayMess();
	void Full_act();
	
	int getCode();
	int getPort();
	void send_command(string);
	string Setter(string);
	//Open mode active or passive
	CSocket*Act_Mode();
	CSocket*Pass_Mode();
	/*handling string command*/
	void splitOrderString(string);
	int find_pos_order(string);
	//command ftp
	void cmd_dir(); //liet ke chi tiet
	void cmd_ls(); //liet ke don gian
	void cmd_del(string); //xoa 1 file
	void cmd_mdele(); //xoa nhieu file
	void cmd_pwd(); //hien thi duong dan hien tai tren server
	void cmd_mkdir(string); //tao thu muc tren server
	void cmd_rmdir(string); //xoa thu muc rong tren server
	void cmd_cd(string); //thay doi duong dan tren server
	void cmd_get_once_File(string); //download 1 file theo parameter truyen vao
	void cmd_get(); //download 1 file
	void cmd_mget(); //download nhieu file
	void cmd_lcd(); //thay doi duong dan duoi client
	bool cmd_put_once_File(string); //upload 1 file theo parameter truyen vao
	void cmd_put(); //upload 1 file
	void cmd_mput(); //upload nhieu file
	void cmd_quit();//thoat khoi server
	void cmd_help(); //help
	void cmd_passive();//change passive mode
	void cmd_active();//change active mode
	void cmd_open();
	void cmd_user();
	/*--------------------------------------*/
	void Interface();
	FTPClient(TCHAR*);
	~FTPClient();


};


class l_exception : exception {
private:
	wstring msg;
public:
	l_exception() : exception() {};
	l_exception(const wchar_t* s) {
		msg = s;
	}
	l_exception(const wstring s) {
		msg = s;
	}

	virtual const wchar_t * what() {
		return msg.c_str();
	}
};
/*Ham phu tro*/
void readPassword(string& pass);
string chuanhoachuoi(string tmp);