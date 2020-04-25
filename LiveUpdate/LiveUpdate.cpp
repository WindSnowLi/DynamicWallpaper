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

MYSQL_FIELD* fd;    //字段列数组
char field[32][32]; //存字段名二维数组
MYSQL_RES* res;     //行的一个查询结果集
MYSQL_ROW column;   //数据行的列
char query[1000];    //查询语句
MYSQL mysql;
//函数声明
bool ConnectDatabase();
void FreeConnect();
bool QueryDatabase();
bool DownLoadFile(string fileAddress, CString savePath);
bool CompareFile();
char* CString_char(CString str);
CString char_CString(char* ch);
void FixConfigFile();
int main()
{
    cout << "功能选择：\n1.更新\n2.修复配置文件"<<endl;
    int i = 0;
    cin >> i;
    if (i == 1) {
        if (ConnectDatabase()) {
            cout << "连接成功！" << endl;
            cout << "开始查询数据······" << endl;
            if (!QueryDatabase()) {
                cout << "查询失败！" << endl;
            }
            else
            {
                cout << "已获得相关信息，开始对比！" << endl;
                if (CompareFile()) {
                    cout << "检测到可更新选项，开始下载！" << endl;
                    char* downAddress = column[2];
                    if (DownLoadFile(downAddress,_T("\\DynamicWallpaper.exe"))) {
                        cout << "下载完成！" << endl;
                        cout << "更新完成！" << endl;
                    }
                    else
                    {
                        cout << "下载失败！更新失败！" << endl;
                    }
                }
            }
        }
        else
        {
            cout << "连接失败！" << endl;
        }
        FreeConnect();
    }
    else if (i == 2) {
        cout << "开始修复······" << endl;
        FixConfigFile();
        cout << "修复完成······" << endl;
    }
    else 
    {
        cout << "输入无效！" << endl;
    }
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
        mysql_query(&mysql, "set names gbk");
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
    sprintf_s(query, "select * from programUp where 文件名='DynamicWallpaper.exe'");

    if (mysql_query(&mysql, query)) {
        return false;
    }
    else
    {
        res = mysql_store_result(&mysql);
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
        return true;
    }
    return false;
}

bool CompareFile()
{
    TCHAR szfilePath[MAX_PATH + 1];
    //文件路径
    GetModuleFileName(0, szfilePath, MAX_PATH);
    //得到应用程序路径
    PathRemoveFileSpec(szfilePath);
    CString versionInformation_xml_Path = szfilePath;
    versionInformation_xml_Path += _T("\\config\\VersionInformation.xml");
    vector<string> versioninformation;
    //获取以前的版本信息
    ifstream file_VersionInformation_xml(versionInformation_xml_Path);
    boost::archive::xml_iarchive iVersionInformation(file_VersionInformation_xml);
    iVersionInformation& BOOST_SERIALIZATION_NVP(versioninformation);

    //获取当前时间
    CString presentTime;
    presentTime.Format(_T("%d-%d-%d"),
        CTime::GetCurrentTime().GetYear(),
        CTime::GetCurrentTime().GetMonth(),
        CTime::GetCurrentTime().GetDay());
    char* tempCheckTime = CString_char(presentTime);
    string checkTime = tempCheckTime;

    versioninformation.erase(versioninformation.begin());
    versioninformation.insert(versioninformation.begin(), checkTime);

    column = mysql_fetch_row(res);
    char* version = column[1];
    delete tempCheckTime;

    std::ofstream file(versionInformation_xml_Path);
    boost::archive::xml_oarchive oa(file);
    oa& BOOST_SERIALIZATION_NVP(versioninformation);

    if (strcmp(version, versioninformation[2].c_str())) {
        return true;
    }
    return false;
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
    return CString();
}

void FixConfigFile()
{
    DownLoadFile("https://gitee.com/wzmmdsnn/windSnowLi/raw/master/ProgramFile/config/VersionInformation.xml", _T("\\config\\VersionInformation.xml"));
    DownLoadFile("https://gitee.com/wzmmdsnn/windSnowLi/raw/master/ProgramFile/config/ARecentVideo.xml", _T("\\config\\ARecentVideo.xml"));
    DownLoadFile("https://gitee.com/wzmmdsnn/windSnowLi/raw/master/ProgramFile/config/VideoDirectory.xml", _T("\\config\\VideoDirectory.xml"));
}

