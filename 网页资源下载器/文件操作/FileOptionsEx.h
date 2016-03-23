#ifndef __FILEOPTIONSEX_H
#define __FILEOPTIONSEX_H
#include <io.h>
#include <vector>
#include <string>
#include <tchar.h>
#include <fstream>
#include "Shlobj.h"
#include "CommDlg.h"
#include <iostream>
#include <windows.h>
#include <Shlwapi.h>				//存放了各种处理路径的API函数
#pragma comment(lib,"Shlwapi.lib")	//存放了各种处理路径的API函数

/*
PathFileExists：判断当前路径是否存在
PathFindFileName：在路径中找到文件名及其文件后缀名
PathFindExtension：在路径中找到文件后缀名
*/

using namespace std;

/*****************************************文件检测相关函数************************************/
//判断文件或者目录是否存在
BOOL FileExists(LPCTSTR lpszFileName, BOOL bIsDirCheck);

//仅仅能配合文件对话框使用哦，自定义路径将无效
int IsFolder(const TCHAR* szBuffer);


/*****************************************文件编码相关函数************************************/
//检测文件编码并返回特定的数值标记，通过文件BOM来检测
int CheckFileCode(const char* str);

//将Unicode编码转换为其他编码
void UnicodeToOther(const char* FilePath, const char* CodePath, int TargetCode);

//将ANSI编码转换为其他编码
void ANSIToOther(const char* FilePath, const char* CodePath, int TargetCode);

//将UTF-8编码转换为其他编码
void UTF8ToOther(const char* FilePath, const char* CodePath, int TargetCode);

//转换文件编码
void ConvertFileCode(const char* FilePath, const char* CodePath, int SourceCode, int TargetCode);

/*****************************************文件搜索相关函数************************************/
//根据脏字库（文件名称标记集合）来查找指定目录下的文件
bool FindFilesByNameFlag(string SearchingMD, vector<string>& FindedFilesArrary, vector<string>& NameFlagLib);

//SearchMode:设为1时，脏字库匹配完整文件路径，设为2时，脏字库仅匹配文件后缀名，设为3时仅匹配文件名和其后缀名，设为4时，仅匹配不包含后缀名的路径
//根据脏字库来查找指定目录下的文件
void FindFilesByNameFlagEx(string path, vector<string>& files, vector<string>& FilterLib, int SearchMode);

/*****************************************文件读取相关函数************************************/
//获取文件里的逐行信息
vector<string> ReadArrInfoInFile(const char* FilePath);

//读取文件里的内容，可以读取非ANSI编码的文件，因为它就是以二进制的方式来读取文件的
string ReadTextInFile(const char* FilePath);

//将文本保存成一个文件
bool CreateFileFromText(string TextInfo, const char* FilePath);

//将字符串数组保存成一个文件
bool CreateFileFromArrInfo(vector<string> ArrInfo, const char* FilePath);

//根据脏字库来筛选所需的文件数据
void FilterFileContent(const char* InPath, const char* OutPath, vector<string>& FilterLib, int FilterMode);

/*****************************************文件对话框相关函数************************************/
//选择对话框
void SelectFileOrFolderDlg(string &FolderPath);

//截取文件开头n个字节，用来作为文件类型判断
string GetFileBOMByOffsetValue(const char* FilePath, int offsetcharc=3);


//从文件头开始截取n个字符后面的数据
string GetFileContentByOffsetValue(const char* FilePath, int offsetcharc=0);


//以二进制的形式写入到本地中
bool CreateFileByBinary(string Text, const char*FilePath, string BOMStr="");


#endif