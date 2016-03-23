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
#include <Shlwapi.h>				//����˸��ִ���·����API����
#pragma comment(lib,"Shlwapi.lib")	//����˸��ִ���·����API����

/*
PathFileExists���жϵ�ǰ·���Ƿ����
PathFindFileName����·�����ҵ��ļ��������ļ���׺��
PathFindExtension����·�����ҵ��ļ���׺��
*/

using namespace std;

/*****************************************�ļ������غ���************************************/
//�ж��ļ�����Ŀ¼�Ƿ����
BOOL FileExists(LPCTSTR lpszFileName, BOOL bIsDirCheck);

//����������ļ��Ի���ʹ��Ŷ���Զ���·������Ч
int IsFolder(const TCHAR* szBuffer);


/*****************************************�ļ�������غ���************************************/
//����ļ����벢�����ض�����ֵ��ǣ�ͨ���ļ�BOM�����
int CheckFileCode(const char* str);

//��Unicode����ת��Ϊ��������
void UnicodeToOther(const char* FilePath, const char* CodePath, int TargetCode);

//��ANSI����ת��Ϊ��������
void ANSIToOther(const char* FilePath, const char* CodePath, int TargetCode);

//��UTF-8����ת��Ϊ��������
void UTF8ToOther(const char* FilePath, const char* CodePath, int TargetCode);

//ת���ļ�����
void ConvertFileCode(const char* FilePath, const char* CodePath, int SourceCode, int TargetCode);

/*****************************************�ļ�������غ���************************************/
//�������ֿ⣨�ļ����Ʊ�Ǽ��ϣ�������ָ��Ŀ¼�µ��ļ�
bool FindFilesByNameFlag(string SearchingMD, vector<string>& FindedFilesArrary, vector<string>& NameFlagLib);

//SearchMode:��Ϊ1ʱ�����ֿ�ƥ�������ļ�·������Ϊ2ʱ�����ֿ��ƥ���ļ���׺������Ϊ3ʱ��ƥ���ļ��������׺������Ϊ4ʱ����ƥ�䲻������׺����·��
//�������ֿ�������ָ��Ŀ¼�µ��ļ�
void FindFilesByNameFlagEx(string path, vector<string>& files, vector<string>& FilterLib, int SearchMode);

/*****************************************�ļ���ȡ��غ���************************************/
//��ȡ�ļ����������Ϣ
vector<string> ReadArrInfoInFile(const char* FilePath);

//��ȡ�ļ�������ݣ����Զ�ȡ��ANSI������ļ�����Ϊ�������Զ����Ƶķ�ʽ����ȡ�ļ���
string ReadTextInFile(const char* FilePath);

//���ı������һ���ļ�
bool CreateFileFromText(string TextInfo, const char* FilePath);

//���ַ������鱣���һ���ļ�
bool CreateFileFromArrInfo(vector<string> ArrInfo, const char* FilePath);

//�������ֿ���ɸѡ������ļ�����
void FilterFileContent(const char* InPath, const char* OutPath, vector<string>& FilterLib, int FilterMode);

/*****************************************�ļ��Ի�����غ���************************************/
//ѡ��Ի���
void SelectFileOrFolderDlg(string &FolderPath);

//��ȡ�ļ���ͷn���ֽڣ�������Ϊ�ļ������ж�
string GetFileBOMByOffsetValue(const char* FilePath, int offsetcharc=3);


//���ļ�ͷ��ʼ��ȡn���ַ����������
string GetFileContentByOffsetValue(const char* FilePath, int offsetcharc=0);


//�Զ����Ƶ���ʽд�뵽������
bool CreateFileByBinary(string Text, const char*FilePath, string BOMStr="");


#endif