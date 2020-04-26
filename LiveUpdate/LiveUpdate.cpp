#define _AFXDLL 
#include <afx.h>
#include <iostream>
#include <urlmon.h>
#include <mysql.h>   
#include <string>
#include <time.h>
#include <vector>
#include <fstream>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <afxwin.h>
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"libmysql.lib")
#pragma comment(lib,"urlmon.lib")
using namespace std;

//字段列数组
MYSQL_FIELD* fd;    
//存字段名二维数组
char field[32][32]; 
 //行的一个查询结果集
MYSQL_RES* res;    
//数据行的列
MYSQL_ROW column;   
MYSQL mysql;
//查询语句
char query[1000];    
//行数
static int numberRows = 0;
//列数
static int numberColumn = 0;
//新版本信息
vector<vector<string>>versionInformation;
//老版本信息
vector<vector<string>> oldVersionInformation;
//函数声明
bool ConnectDatabase();
void FreeConnect();
bool QueryDatabase();
bool DownLoadFile(string fileAddress, CString savePath);
bool CompareFile();
char* CString_char(CString str);
CString char_CString(char* ch);
void FixConfigFile();
bool Initialize();
void FixProgram();
void FixUpdateProgram();
int main()
{
    cout << "功能选择：\n1.更新\n2.修复配置文件\n3.修复程序\n4.修复更新程序配置文件" << endl;
    int choose = 0;
    cin >> choose;
    if (ConnectDatabase()) {
        cout << "连接成功！" << endl;
        cout << "开始查询数据······" << endl;
        if (!QueryDatabase()) {
            cout << "查询失败！" << endl;
        }
        else
        {
            cout << "已获得资源信息！" << endl;
            switch (choose)
            {
            case 1:
                cout << "初始化数据······" << endl;
                if (!Initialize) {
                    cout << "初始化数据失败" << endl;
                }
                else {
                    cout << "已获得相关信息，开始检测更新！" << endl;
                    if (CompareFile()) {
                        cout << "更新完成！" << endl;
                    }
                    else
                    {
                        cout << "更新失败！" << endl;
                    }
                }
                break;
            case 2:
                cout << "开始修复······" << endl;
                FixConfigFile();
                cout << "修复完成······" << endl;
                break;
            case 3:
                cout << "开始修复······" << endl;
                FixProgram();
                cout << "修复完成······" << endl;
                break;
            case 4:
                cout << "开始修复······" << endl;
                FixUpdateProgram();
                cout << "修复完成······" << endl;
                break;
            default:
                cout << "输入无效！" << endl;
                break;
            }
        }
    }
    else
    {
        cout << "连接失败！" << endl;
    }
    FreeConnect();
    system("pause");
    return 0;
}

//连接数据库
bool ConnectDatabase() {
    mysql_init(&mysql);
    const char host[] = "cdb-7qv2gt72.cd.tencentcdb.com";
    const char user[] = "everyone1";
    const char passwd[] = "li09090810";
    const char db[] = "programFile";
    unsigned int port = 10107;
    const char* unix_socket = NULL;
    unsigned long client_flag = 0;
    if (mysql_real_connect(&mysql, host, user, passwd, db, port, unix_socket, client_flag)) {
        mysql_query(&mysql, "set names utf-8");
        return true;
    }
    else {
        return false;
    }
}

void FreeConnect() {
    if (res != NULL) {
        mysql_free_result(res);
        res = NULL;
    }
    if (mysql_ping(&mysql) == 0) {
        mysql_close(&mysql);
    }
}

//查询数据
bool QueryDatabase() {
    //将数据格式化输出到字符串
    sprintf_s(query, "select * from programUp");

    if (mysql_query(&mysql, query)) {
        return false;
    }
    else
    {
        
        res = mysql_store_result(&mysql);
        // 获取列数
        numberColumn = mysql_num_fields(res);
        // 获取列数
        numberRows = mysql_num_rows(res);
        //存储字段信息
        char* str_field[64];
        versionInformation.push_back(vector<string>());
        for (int i = 0; i < numberColumn; i++) {
            str_field[i] = mysql_fetch_field(res)->name;
            versionInformation[0].push_back(str_field[i]);
        }

        if (!res) {
            return false;
        }
        int i = 1;
        while (column = mysql_fetch_row(res)) {
            versionInformation.push_back(vector<string>());
            for (int j = 0; j < numberColumn; j++) {
                versionInformation[i].push_back((char*)column[j]);
            }
            i++;
        }
        
        return true;
        
    }
    return true;
}

bool DownLoadFile(string fileAddress,CString savePath)
{
    string url = fileAddress;
    size_t len = url.length();//获取字符串长度
    int nmlen = MultiByteToWideChar(CP_ACP, 0, url.c_str(), len + 1, NULL, 0);//如果函数运行成功，并且cchWideChar为零，
    //返回值是接收到待转换字符串的缓冲区所需求的宽字符数大小。
    wchar_t* buffer = new wchar_t[nmlen];
    MultiByteToWideChar(CP_ACP, 0, url.c_str(), len + 1, buffer, nmlen);

    TCHAR szfilePath[MAX_PATH + 1];
    //文件路径
    GetModuleFileName(0, szfilePath, MAX_PATH);
    //得到应用程序路径
    PathRemoveFileSpec(szfilePath);
    CString file = szfilePath;
    file += savePath;
    HRESULT hr = URLDownloadToFile(NULL, buffer, file, 0, NULL);
    if (hr == S_OK)
    {
        cout << "下载完成！" << endl;
        return true;
    }
    return false;
}

bool CompareFile()
{

    Initialize();
    
    int oldFileName = 0;
    int oldFileVersion = 0;
    int oldDownLoadPath = 0;
    int oldDownLoadLink = 0;
    for (int i = 0; i < oldVersionInformation[0].size(); i++) {
        if (strcmp(oldVersionInformation[0][i].c_str(), "filename") == 0) {
            oldFileName = i;
        }
        else if (strcmp(oldVersionInformation[0][i].c_str(), "version") == 0) {
            oldFileVersion = i;
        }
        else if (strcmp(oldVersionInformation[0][i].c_str(), "download") == 0) {
            oldDownLoadPath = i;
        }
        else if (strcmp(oldVersionInformation[0][i].c_str(), "link") == 0) {
            oldDownLoadLink = i;
        }
    }

    int fileName = 0;
    int fileVersion = 0;
    int downLoadPath = 0;
    int downLoadLink = 0;
    for (int i = 0; i < versionInformation[0].size();i++) {
        if (strcmp(versionInformation[0][i].c_str(),"filename") == 0) {
            fileName = i;
        }else if (strcmp(versionInformation[0][i].c_str(), "version") == 0) {
            fileVersion = i;
        }else if (strcmp(versionInformation[0][i].c_str(), "download") == 0) {
            downLoadPath = i;
        }else if (strcmp(versionInformation[0][i].c_str(), "link") == 0) {
            downLoadLink = i;
        }
    }
    
    for (auto j : versionInformation) {
        for (auto k : oldVersionInformation) {
            for (auto m : k) {
                if (strcmp(j[fileName].c_str(), m.c_str()) == 0) {
                    if (strcmp(j[fileVersion].c_str(), k[oldFileVersion].c_str()) == 0) {
                        break;
                    }
                    else 
                    {
                        DownLoadFile(j[downLoadLink], char_CString((char*)j[downLoadPath].c_str()));
                    }
                }
            }
        }
    }
    FixUpdateProgram();

    return true;
}

char* CString_char(CString str)
{
    //注意：以下n和len的值大小不同，n是按字符计算的，len是按字节计算的
    int n = str.GetLength();

    //获取宽字节字符的大小，大小是按字节计算的
    int len = WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), NULL, 0, NULL, NULL);

    //为多字节字符数组申请空间，数组大小为按字节计算的宽字节字节大小
    char* pFileName = new char[(unsigned __int64)len + 1];   //以字节为单位

    //宽字节编码转换成多字节编码          
    WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), pFileName, len, NULL, NULL);
    pFileName[len] = '\0';   //多字节字符以'/0'结束

    return pFileName;
}

CString char_CString(char* ch)
{
    CString temp;
    if (ch == NULL) {
        temp = _T("空");
    }
    else {
        int charLen = strlen(ch);
        //计算多字节字符的大小，按字符计算。
        int len = MultiByteToWideChar(CP_ACP, 0, ch, charLen, NULL, 0);
        //为宽字节字符数组申请空间，数组大小为按字节计算的多字节字符大小
        TCHAR* buf = new TCHAR[(unsigned __int64)len + 1];
        //多字节编码转换成宽字节编码
        MultiByteToWideChar(CP_ACP, 0, ch, charLen, buf, len);
        buf[len] = '\0'; //添加字符串结尾，注意不是len+1
        //将TCHAR数组转换为CString
        temp.Append(buf);
        //删除缓冲区
        delete[]buf;
    }
    return temp;
}

void FixConfigFile()
{
    int downLoadPath = 0;
    int downLoadLink = 0;
    int typeFile = 0;
    for (int i = 0; i < versionInformation[0].size(); i++) {
        if (strcmp(versionInformation[0][i].c_str(), "filetype") == 0) {
            typeFile = i;
        }
        else if (strcmp(versionInformation[0][i].c_str(), "download") == 0) {
            downLoadPath = i;
        }
        else if (strcmp(versionInformation[0][i].c_str(), "link") == 0) {
            downLoadLink = i;
        }
    }
    for (auto j : versionInformation) {
        if (strcmp(j[typeFile].c_str(), "config") == 0) {
            DownLoadFile(j[downLoadLink], char_CString((char*)j[downLoadPath].c_str()));
        }
    }
}

bool Initialize()
{
    TCHAR szfilePath[MAX_PATH + 1];
    //文件路径
    GetModuleFileName(0, szfilePath, MAX_PATH);
    //得到应用程序路径
    PathRemoveFileSpec(szfilePath);
    CString versionInformation_xml_Path = szfilePath;
    versionInformation_xml_Path += _T("\\config\\VersionInformation.xml");
    //获取以前的版本信息
    ifstream file_VersionInformation_xml(versionInformation_xml_Path);
    boost::archive::xml_iarchive iVersionInformation(file_VersionInformation_xml);
    iVersionInformation& BOOST_SERIALIZATION_NVP(oldVersionInformation);
    oldVersionInformation.pop_back();
    return true;
}

void FixProgram()
{
    int downLoadPath = 0;
    int downLoadLink = 0;
    int typeFile = 0;   
    for (int i = 0; i < versionInformation[0].size(); i++) {
        if (strcmp(versionInformation[0][i].c_str(), "download") == 0) {
            downLoadPath = i;
        }
        else if (strcmp(versionInformation[0][i].c_str(), "link") == 0) {
            downLoadLink = i;
        }
    }
    for (auto j : versionInformation) {
        DownLoadFile(j[downLoadLink], char_CString((char*)j[downLoadPath].c_str()));
    }
}

void FixUpdateProgram()
{
    TCHAR szfilePath[MAX_PATH + 1];
    //文件路径
    GetModuleFileName(0, szfilePath, MAX_PATH);
    //得到应用程序路径
    PathRemoveFileSpec(szfilePath);
    //获取当前时间
    CString presentTime;
    presentTime.Format(_T("%d-%d-%d"),
        CTime::GetCurrentTime().GetYear(),
        CTime::GetCurrentTime().GetMonth(),
        CTime::GetCurrentTime().GetDay());
    char* tempCheckTime = CString_char(presentTime);
    string checkTime = tempCheckTime;
    versionInformation.push_back(vector<string>());
    versionInformation.back().push_back(tempCheckTime);
    CString versionInformation_xml_Path111 = szfilePath;
    versionInformation_xml_Path111 += _T("\\config\\VersionInformation.xml");

    std::ofstream file(versionInformation_xml_Path111);
    boost::archive::xml_oarchive oa(file);
    oa& BOOST_SERIALIZATION_NVP(versionInformation);
}

