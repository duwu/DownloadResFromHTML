#include "FileOptionsEx.h"
#include "..\�ַ�����չ\StringEx.h"

//����ļ����벢�����ض�����ֵ���
int CheckFileCode(const char* str)
{
	int CodeTag = 0;
	FILE *rFile;
	string FileCodeFlag = "";
	//����VC 6.0
#if _MSC_VER<1400   //VS 2005�汾����            
	rFile = fopen(str, "r");
	if (rFile == NULL)
#else  //VS 2005�汾����
	errno_t Rerr = fopen_s(&rFile, str, "r");
	if (Rerr == 0)
#endif   
	{
		//char ch;
		int i = 0;
		char temp[10];
		while (!feof(rFile) && i<3)
		{
			//����VC 6.0
#if _MSC_VER<1400   //VS 2005�汾����            
			sprintf(temp, "%X", fgetc(rFile));
#else  //VS 2005�汾����
			sprintf_s(temp, "%X", fgetc(rFile));
#endif   
			FileCodeFlag += temp;
			//cout << temp << endl;
			//cout << fgetc(rFile) << endl;
			i++;
		}
		fclose(rFile);

		//cout << "�ļ�ͷΪ" << FileCodeFlag.find("FFFE") << endl;
		if (FileCodeFlag.find("FFFE") >= 0 && FileCodeFlag.find("FFFE") < strlen(FileCodeFlag.c_str()))
		{
			//cout << "���ļ���Unicode����" << endl;
			CodeTag = 1200;
		}
		else if (FileCodeFlag.find("FEFF") >= 0 && FileCodeFlag.find("FEFF") < strlen(FileCodeFlag.c_str()))
		{
			//cout << "���ļ���Unicode big endian����" << endl;
			CodeTag = 1201;
		}
		else if (FileCodeFlag.find("EFBBBF") >= 0 && FileCodeFlag.find("EFBBBF") < strlen(FileCodeFlag.c_str()))
		{
			//cout << "���ļ���UTF-8����" << endl;
			CodeTag = 65001;
		}
		else
		{
			//cout << "���ļ���ANSI��GB2312����" << endl;
			CodeTag = 936;
		}


	}
	else { cout << "���ļ�������" << endl; }
	return CodeTag;

}



//��ȡ�ļ��������
string ReadTextInFile(const char* FilePath)
{

	std::ifstream in(FilePath, std::ios::in | std::ios::binary);
	if (!in) {
		std::cout << "File " << FilePath << " not found!\n";
		exit(EXIT_FAILURE);
	}

	std::string contents;
	in.seekg(0, std::ios::end);
	contents.resize((size_t)in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&contents[0], contents.size());
	in.close();
	//cout << contents << endl;
	return contents;
}




//���ı������һ���ļ�
bool CreateFileFromText(string TextInfo, const char* FilePath)
{
	bool IsOK = false;
	if (TextInfo.length() > 0)
	{
		FILE *wFile;
		//����VC 6.0
#if _MSC_VER<1400   //VS 2005�汾����            
		wFile = fopen(FilePath, "w");
		if (wFile == NULL)
#else  //VS 2005�汾����
		errno_t Werr = fopen_s(&wFile, FilePath, "w"); // ����һ���ļ�ָ��, ������fopen()��������һ���ļ�ָ��  
		if (Werr != 0)//�ļ��򿪳ɹ�
#endif   
		{
			IsOK = false;
			cout << "wFile��ʧ��" << endl;
		}
		else
		{
			fputs(TextInfo.c_str(), wFile);
			IsOK = true;
		}
		fclose(wFile);             // �ر��ļ�ָ���, �����������������ݽ�д�뵽������, һ�������, ���ļ�������Ϻ�, Ӧ�ر���Ӧ���ļ�ָ��  
	}
	return IsOK;
}



//����ֱ��ʹ��PathFileExistsA�����
BOOL FileExists(LPCTSTR lpszFileName, BOOL bIsDirCheck)//�ú���ר�������ж�ĳ���ļ���Ŀ¼�Ƿ���ڵģ�����ڶ�������ΪTRUE������Ŀ¼�����ΪFALSE�������ļ�
{
	//��ͼȡ���ļ�����
	DWORD dwAttributes = ::GetFileAttributes(lpszFileName);
	
		//����VC 6.0
#if _MSC_VER<1400   //VS 2005�汾����            
	if (((DWORD)-1)== dwAttributes)//������VC 6.0�в�û�ж���INVALID_FILE_ATTRIBUTES������ʹ��ԭֵ
#else  //VS 2005�汾����
	if (INVALID_FILE_ATTRIBUTES == dwAttributes)
#endif   
	{
		return FALSE;
	}

	//��Ŀ¼
	if (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		if (bIsDirCheck) //��ǰ����Ҳ��Ŀ¼
		{
			return TRUE;
		}
		else
			return FALSE;
	}
	else //���ļ�
	{
		if (bIsDirCheck)
		{
			return FALSE;
		}
		else
			return TRUE;
	}
}




//SearchMode:��Ϊ1ʱ�����ֿ�ƥ�������ļ�·������Ϊ2ʱ�����ֿ��ƥ���ļ���׺������Ϊ3ʱ��ƥ���ļ��������׺������Ϊ4ʱ����ƥ�䲻������׺����·��
//�������ֿ�������ָ��Ŀ¼�µ��ļ�
void FindFilesByNameFlagEx(string path, vector<string>& files, vector<string>& FilterLib, int SearchMode)
{
	//�ļ����  
	long   hFile = 0;
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					//�������������õ�Ŀ¼��
					FindFilesByNameFlagEx(p.assign(path).append("\\").append(fileinfo.name), files, FilterLib, SearchMode);
				}
			}
			else
			{
				//���������������ļ���
				if (FilterStr(p.assign(path).append("\\").append(fileinfo.name), FilterLib, 2))
				{
					//cout << "else:" << p.assign(path).append("\\").append(fileinfo.name) << endl;
					//cout << p.assign(path).append("\\").append(fileinfo.name) << endl;
					string temp = p.assign(path).append("\\").append(fileinfo.name);
					//cout << temp.substr(temp.rfind(".")) << endl;//�õ���׺��
					switch (SearchMode)
					{
					case 1:
					{
							  //cout << temp << endl;
							  files.push_back(temp);
					}; break;
					case 2:
					{
							  string EndStr = temp.substr(temp.rfind("."));
							  //cout << EndStr << endl;
							  if (FilterStr(EndStr, FilterLib, 2))
							  {
								  files.push_back(temp);
							  }
					}; break;
					case 3:
					{
							  string FileName = temp.substr(temp.rfind("\\") + 1, temp.rfind("."));
							  //cout << FileName << endl;
							  if (FilterStr(FileName, FilterLib, 1))
							  {
								  files.push_back(temp);
							  }
					}; break;
					case 4:
					{
							  string PathAndName = temp.substr(0, temp.rfind("."));
							  //cout << PathAndName << endl;
							  if (FilterStr(PathAndName, FilterLib, 1))
							  {
								  files.push_back(temp);
							  }
					}; break;
					}
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}



//�������ֿ⣨�ļ����Ʊ�Ǽ��ϣ�������ָ��Ŀ¼�µ��ļ�
bool FindFilesByNameFlag(string SearchingMD, vector<string>& FindedFilesArrary, vector<string>& NameFlagLib)
{
	bool IsAvailable = false;
	//�ļ����  
	long   hFile = 0;
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;//��Ҫ#include <io.h>
	string p;
	if ((hFile = _findfirst(p.assign(SearchingMD).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					//�������������õ�Ŀ¼��
					FindFilesByNameFlag(p.assign(SearchingMD).append("\\").append(fileinfo.name), FindedFilesArrary, NameFlagLib);
				}
			}
			else
			{
				//���������������ļ���
				if (FilterStr(p.assign(SearchingMD).append("\\").append(fileinfo.name), NameFlagLib, 2))
				{
					cout << "else:" << p.assign(SearchingMD).append("\\").append(fileinfo.name) << endl;
					FindedFilesArrary.push_back(p.assign(SearchingMD).append("\\").append(fileinfo.name));
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	if (FindedFilesArrary.size() > 0)//��FindedFilesArrary��Ԫ�ش���1ʱ��˵��ȷʵ�ҵ��˷����������ļ�
	{
		IsAvailable = true;
	}
	return IsAvailable;
}

//��ȡ�ļ����������Ϣ
vector<string> GetArrInfoFromFile(const char* FilePath)
{
	ifstream ReadedFile;
	string info = "";
	vector<string> TextContext;
	ReadedFile.open(FilePath);
	if (!ReadedFile)
	{
		cout << "/*******************************���ļ�ʧ�ܣ�*********************************/";
	}
	else{
		while (getline(ReadedFile, info))//���ж�ȡ�ı��ļ�
		{

			//cout << info << endl;
			TextContext.push_back(info);
		}
	}
	ReadedFile.close();
	return TextContext;
}






//���ַ������鱣���һ���ļ�
bool CreateFileFromArrInfo(vector<string> ArrInfo, const char* FilePath)
{
	bool IsOK = false;
	vector<string> TextContext;
	ofstream WriteFile;
	WriteFile.open(FilePath);
	if (ArrInfo.size() > 0)
	{
		for (size_t n = 0; n < ArrInfo.size(); n++)
		{
			WriteFile << ArrInfo[n] << endl;
		}
		WriteFile.close();
		IsOK = true;
	}
	return IsOK;
}



//��Unicode����ת��Ϊ��������
void UnicodeToOther(const char* FilePath, const char* CodePath, int TargetCode)
{
	wchar_t* pUnicode = AnsiToUnicode(FilePath);
	TCHAR szBuf[1024];
	memset(szBuf, 0, sizeof(szBuf));
	FILE *pStream;
	errno_t rErr = _wfopen_s(&pStream, pUnicode, L"rb");
	//errno_t rErr = _wfopen_s(&pStream, _T("test.txt"), _T("rb"));
	fseek(pStream, 2, SEEK_SET);//����Unicode�ļ������BOM��0XFF 0XFE������������λ
	string strTemp;
	if (NULL == rErr)
	{
		while (!feof(pStream))
		{
			_fgetts(szBuf, 1024, pStream);
#ifdef UNICODE
			DWORD dwNum = WideCharToMultiByte(TargetCode, NULL, szBuf, -1, NULL, 0, NULL, FALSE);//�õ�UTF-8�ļ�����
#else
			DWORD dwNum = WideCharToMultiByte(TargetCode, NULL, AnsiToUnicode(szBuf), -1, NULL, 0, NULL, FALSE);//�õ�UTF-8�ļ�����
#endif
			//DWORD dwNum = WideCharToMultiByte(936, NULL, szBuf, -1, NULL, 0, NULL, FALSE);//�õ�ANSI�ļ�����
			char *psText;
			psText = new char[dwNum];
			if (!psText)
			{
				delete[]psText;
			}
#ifdef UNICODE
			WideCharToMultiByte(TargetCode, NULL, szBuf, -1, psText, dwNum, NULL, FALSE);//�õ�UTF-8�ļ�����
#else
			WideCharToMultiByte(TargetCode, NULL, AnsiToUnicode(szBuf), -1, psText, dwNum, NULL, FALSE);//�õ�UTF-8�ļ�����
#endif
			//WideCharToMultiByte(936, NULL, szBuf, -1, psText, dwNum, NULL, FALSE);//�õ�ANSI�ļ�����
			strTemp += psText;
			delete[]psText;
		}
	}
	free(pUnicode);//�ͷ�pUnicode���ڴ棬һ����Ժ�������������������pUnicode����ΪNULL��
	fclose(pStream);

	//д���ļ�
	fstream OutFile;
	OutFile.open(CodePath, ios::out);
	OutFile << strTemp;
	OutFile.close();
}


//��ANSI����ת��Ϊ��������
void ANSIToOther(const char* FilePath, const char* CodePath, int TargetCode)
{
	char buffer[1024];
	fstream ReadedFile;
	FILE *wFile;
	ReadedFile.open(FilePath, ios::in);//��ȡ�ķ�ʽ���ļ�
	//����ANSI�ļ����벢������BOM��Ҳ�����ļ�ͷ������ֱ�Ӷ�ȡ������Ҫ��λ
	if (!ReadedFile.is_open())
	{
		cout << "�ļ���ʧ��" << endl;
	}
	else
	{
		string FileContent("");
		while (!ReadedFile.eof())//�ж��Ƿ񵽴����ļ�ĩβ
		{
			ReadedFile.getline(buffer, 256);
			FileContent += string(buffer) + string("\r\n");
		}
		ReadedFile.close();
		switch (TargetCode)
		{
		case 1200:
		{
					 errno_t wErr = _wfopen_s(&wFile, AnsiToUnicode(CodePath), L"w+b");
					 wchar_t m_strUnicode[1];
					 m_strUnicode[0] = wchar_t(0XFEFF);//Unicode�����BOM
					 fputwc(*m_strUnicode, wFile);//�����������ļ�����
					 fputws(AnsiToUnicode(FileContent.c_str()), wFile);
					 fclose(wFile);
		}; break;
		case 65001:
		{
					  fstream wFile;
					  wFile.open(CodePath, ios::out);//��ָ�������Ʒ�ʽ��ȡ
					  wFile.write(Convert(FileContent, 936, 65001).c_str(), FileContent.length() + 1);
					  wFile.close();
		}; break;
		}
	}
}


//��UTF-8����ת��Ϊ��������
void UTF8ToOther(const char* FilePath, const char* CodePath, int TargetCode)
{
	if (TargetCode != NULL)
	{
		char buffer[1024];
		fstream InFile, OutFile;
		InFile.open(FilePath, ios::in);//��ȡ�ķ�ʽ���ļ�
		if (!InFile.is_open())
		{
			cout << "�ļ���ʧ��" << endl;
		}
		else
		{
			string FileContent("");
			InFile.seekg(3);//����UTF-8�ļ������BOM��0XEF 0XBB 0XBF������������λ
			//������λ�ַ�
			while (!InFile.eof())//�ж��Ƿ񵽴����ļ�ĩβ
			{
				InFile.getline(buffer, 256);
				FileContent += string(buffer) + string("\r\n");
			}
			InFile.close();

			OutFile.open(CodePath, ios::out);//д���ļ��ķ�ʽ��
			OutFile << Convert(FileContent, 65001, 936);
			OutFile.close();//���ϵõ�һ��ANSI�ļ�������ı�
			if (TargetCode == 1200)
			{
				ANSIToOther(CodePath, CodePath, 1200);
			}
		}
	}
}



//ת���ļ�����
void ConvertFileCode(const char* FilePath, const char* CodePath, int SourceCode, int TargetCode)
{
	switch (SourceCode)
	{
	case 936://ANSI---->Unicode;ANSI---->UTF-8
	{
				 ANSIToOther(FilePath, CodePath, TargetCode);
	}; break;
	case 65001://UTF-8---->Unicode;UTF-8---->ANSI
	{
				   UTF8ToOther(FilePath, CodePath, TargetCode);
	}; break;
	case 1200://Unicode---->ANSI;Unicode---->UTF-8
	{
				  UnicodeToOther(FilePath, CodePath, TargetCode);
	}; break;
	}
}


//ѡ��Ի���
void SelectFileOrFolderDlg(string &FolderPath)
{
	//��ѡ��Ի���
	TCHAR szBuffer[MAX_PATH] = { 0 };
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = NULL;
	bi.pszDisplayName = szBuffer;
	bi.lpszTitle = _T("������ѡ���ļ����ļ��У�");
	bi.ulFlags = BIF_BROWSEINCLUDEFILES | BIF_USENEWUI;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (NULL == idl)
	{
		return;
	}
	//cout << szBuffer << endl;
	BOOL temp = SHGetPathFromIDList(idl, szBuffer);
	//cout << "temp:" << temp << endl;//Ϊ1ʱ��ѡ�����ļ���Ϊ0ʱѡ�����ļ��У�ȡ��ʱ��ʲô��û��
	//cout << szBuffer << endl;
	//cout << UnicodeToANSI(szBuffer) << endl;
#ifdef UNICODE
	FolderPath = UnicodeToANSI(szBuffer);
#else
	FolderPath = szBuffer;
#endif
}


//����������ļ��Ի���ʹ��Ŷ���Զ���·������Ч
int IsFolder(const TCHAR* szBuffer)
{
	if (GetFileAttributes(szBuffer)&FILE_ATTRIBUTE_DIRECTORY)
	{
		//cout << "��ʱ�ļ���" << endl;
		return 1;//�ļ��б��
	}
	else
	{
		//cout << "��ʱ�ļ�" << endl;
		return 2;//�ļ����
	}

	//if (GetFileAttributesA(szBuffer)&INVALID_FILE_ATTRIBUTES)
	//{
	//	cout << "����ֵ��Ч" << endl;
	//}
	//else
	//{
	//	cout << "����ֵ��Ч" << endl;
	//}
	return 0;
}


//�������ֿ���ɸѡ������ļ�����
void FilterFileContent(const char* InPath, const char* OutPath, vector<string>& FilterLib, int FilterMode)
{
	vector<string> FullValue = ReadArrInfoInFile(InPath);
	bool IsFull = false;
	int Choice = 0;
	//�½�һ���ļ�
	FILE *wFile;
	fopen_s(&wFile, OutPath, "w"); //��д�ķ�ʽ�򿪵ڶ����ļ�
	for (size_t i = 0; i < FullValue.size(); i++)
	{
		switch (FilterMode)
		{
		case 1:
		{
				  IsFull = false;
				  Choice = 1;
		}; break;
		case 2:
		{
				  IsFull = false;
				  Choice = 2;
		}; break;
		case 3:
		{
				  IsFull = true;
				  Choice = 1;
		}; break;
		case 4:
		{
				  IsFull = true;
				  Choice = 2;
		}; break;
		}

		if (FilterStr(FullValue[i], FilterLib, Choice) != IsFull)
		{
			fputs(FullValue[i].c_str(), wFile);
			fputs("\n", wFile);
		}
	}
	fclose(wFile);
}


//��ȡ�ļ����������Ϣ
vector<string> ReadArrInfoInFile(const char* FilePath)
{
	ifstream ReadedFile;
	string info = "";
	vector<string> TextContext;
	ReadedFile.open(FilePath);
	if (!ReadedFile)
	{
		cout << FilePath << "��ȡʧ��" << endl;
	}
	else{
		while (getline(ReadedFile, info))//���ж�ȡ�ı��ļ�
		{

			//cout << info << endl;
			TextContext.push_back(info);
		}
	}
	ReadedFile.close();
	return TextContext;
}


//��ȡ�ļ���ͷn���ֽڣ�������Ϊ�ļ������ж�
string GetFileBOMByOffsetValue(const char* FilePath, int offsetcharc)
{
	std::string contents;
	std::ifstream in(FilePath, std::ios::in | std::ios::binary);
	if (!in) {
		std::cout << "File " << FilePath << " not found!\n";
	}
	else
	{
		contents.resize((size_t)offsetcharc);//����contents���ڴ��С
		in.seekg(0, std::ios::beg);//���ļ�����ʼ��ƫ��ֵΪ0����ȡ�ļ���Ϣ
		in.read(&contents[0], offsetcharc);//���ļ���ͷ��offsetcharc���ֽڶ�ȡ��contents�������
		in.close();
	}
	//cout << contents << endl;
	return contents;
}




//���ļ�ͷ��ʼ��ȡn���ַ����������
string GetFileContentByOffsetValue(const char* FilePath, int offsetcharc)
{
	std::ifstream in(FilePath, std::ios::in | std::ios::binary);
	if (!in) {
		std::cout << "File " << FilePath << " not found!\n";
		exit(EXIT_FAILURE);
	}

	std::string contents;
	in.seekg(0, std::ios::end);
	contents.resize((size_t)in.tellg());
	in.seekg(offsetcharc, std::ios::beg);
	in.read(&contents[0], contents.size() - offsetcharc);
	in.close();
	//cout << contents << endl;
	return contents;
}



//�Զ����Ƶ���ʽд�뵽������
bool CreateFileByBinary(string Text, const char*FilePath, string BOMStr)
{

	//UTF-8��"\xef\xbb\xbf"
	std::ofstream out(FilePath, std::ios::out | std::ios::binary);
	bool IsOK = true;
	if (!out) {
		std::cout << "File " << FilePath << " not found!\n";
		//exit(EXIT_FAILURE);
		IsOK = false;
	}

	//out << Text;
	//�����UTF-8���е�BOM
	out.write(BOMStr.c_str(), BOMStr.length());
	out.write(Text.c_str(), Text.length());
	out.close();
	return IsOK;
}

