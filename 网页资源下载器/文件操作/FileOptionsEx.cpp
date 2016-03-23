#include "FileOptionsEx.h"
#include "..\字符串拓展\StringEx.h"

//检测文件编码并返回特定的数值标记
int CheckFileCode(const char* str)
{
	int CodeTag = 0;
	FILE *rFile;
	string FileCodeFlag = "";
	//兼容VC 6.0
#if _MSC_VER<1400   //VS 2005版本以下            
	rFile = fopen(str, "r");
	if (rFile == NULL)
#else  //VS 2005版本以上
	errno_t Rerr = fopen_s(&rFile, str, "r");
	if (Rerr == 0)
#endif   
	{
		//char ch;
		int i = 0;
		char temp[10];
		while (!feof(rFile) && i<3)
		{
			//兼容VC 6.0
#if _MSC_VER<1400   //VS 2005版本以下            
			sprintf(temp, "%X", fgetc(rFile));
#else  //VS 2005版本以上
			sprintf_s(temp, "%X", fgetc(rFile));
#endif   
			FileCodeFlag += temp;
			//cout << temp << endl;
			//cout << fgetc(rFile) << endl;
			i++;
		}
		fclose(rFile);

		//cout << "文件头为" << FileCodeFlag.find("FFFE") << endl;
		if (FileCodeFlag.find("FFFE") >= 0 && FileCodeFlag.find("FFFE") < strlen(FileCodeFlag.c_str()))
		{
			//cout << "此文件是Unicode编码" << endl;
			CodeTag = 1200;
		}
		else if (FileCodeFlag.find("FEFF") >= 0 && FileCodeFlag.find("FEFF") < strlen(FileCodeFlag.c_str()))
		{
			//cout << "此文件是Unicode big endian编码" << endl;
			CodeTag = 1201;
		}
		else if (FileCodeFlag.find("EFBBBF") >= 0 && FileCodeFlag.find("EFBBBF") < strlen(FileCodeFlag.c_str()))
		{
			//cout << "此文件是UTF-8编码" << endl;
			CodeTag = 65001;
		}
		else
		{
			//cout << "此文件是ANSI或GB2312编码" << endl;
			CodeTag = 936;
		}


	}
	else { cout << "此文件不存在" << endl; }
	return CodeTag;

}



//读取文件里的内容
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




//将文本保存成一个文件
bool CreateFileFromText(string TextInfo, const char* FilePath)
{
	bool IsOK = false;
	if (TextInfo.length() > 0)
	{
		FILE *wFile;
		//兼容VC 6.0
#if _MSC_VER<1400   //VS 2005版本以下            
		wFile = fopen(FilePath, "w");
		if (wFile == NULL)
#else  //VS 2005版本以上
		errno_t Werr = fopen_s(&wFile, FilePath, "w"); // 定义一个文件指针, 并调用fopen()函数返回一个文件指针  
		if (Werr != 0)//文件打开成功
#endif   
		{
			IsOK = false;
			cout << "wFile打开失败" << endl;
		}
		else
		{
			fputs(TextInfo.c_str(), wFile);
			IsOK = true;
		}
		fclose(wFile);             // 关闭文件指针后, 输入流缓冲区的数据将写入到磁盘中, 一般情况下, 对文件操作完毕后, 应关闭相应的文件指针  
	}
	return IsOK;
}



//可以直接使用PathFileExistsA来替代
BOOL FileExists(LPCTSTR lpszFileName, BOOL bIsDirCheck)//该函数专门用来判断某个文件或目录是否存在的，如果第二个参数为TRUE，则检测目录，如果为FALSE，则检测文件
{
	//试图取得文件属性
	DWORD dwAttributes = ::GetFileAttributes(lpszFileName);
	
		//兼容VC 6.0
#if _MSC_VER<1400   //VS 2005版本以下            
	if (((DWORD)-1)== dwAttributes)//由于在VC 6.0中并没有定义INVALID_FILE_ATTRIBUTES，所以使用原值
#else  //VS 2005版本以上
	if (INVALID_FILE_ATTRIBUTES == dwAttributes)
#endif   
	{
		return FALSE;
	}

	//是目录
	if (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		if (bIsDirCheck) //当前检测的也是目录
		{
			return TRUE;
		}
		else
			return FALSE;
	}
	else //是文件
	{
		if (bIsDirCheck)
		{
			return FALSE;
		}
		else
			return TRUE;
	}
}




//SearchMode:设为1时，脏字库匹配完整文件路径，设为2时，脏字库仅匹配文件后缀名，设为3时仅匹配文件名和其后缀名，设为4时，仅匹配不包含后缀名的路径
//根据脏字库来查找指定目录下的文件
void FindFilesByNameFlagEx(string path, vector<string>& files, vector<string>& FilterLib, int SearchMode)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息  
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
					//这里获得搜索所得的目录名
					FindFilesByNameFlagEx(p.assign(path).append("\\").append(fileinfo.name), files, FilterLib, SearchMode);
				}
			}
			else
			{
				//这里获得搜索到的文件名
				if (FilterStr(p.assign(path).append("\\").append(fileinfo.name), FilterLib, 2))
				{
					//cout << "else:" << p.assign(path).append("\\").append(fileinfo.name) << endl;
					//cout << p.assign(path).append("\\").append(fileinfo.name) << endl;
					string temp = p.assign(path).append("\\").append(fileinfo.name);
					//cout << temp.substr(temp.rfind(".")) << endl;//得到后缀名
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



//根据脏字库（文件名称标记集合）来查找指定目录下的文件
bool FindFilesByNameFlag(string SearchingMD, vector<string>& FindedFilesArrary, vector<string>& NameFlagLib)
{
	bool IsAvailable = false;
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;//需要#include <io.h>
	string p;
	if ((hFile = _findfirst(p.assign(SearchingMD).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					//这里获得搜索所得的目录名
					FindFilesByNameFlag(p.assign(SearchingMD).append("\\").append(fileinfo.name), FindedFilesArrary, NameFlagLib);
				}
			}
			else
			{
				//这里获得搜索到的文件名
				if (FilterStr(p.assign(SearchingMD).append("\\").append(fileinfo.name), NameFlagLib, 2))
				{
					cout << "else:" << p.assign(SearchingMD).append("\\").append(fileinfo.name) << endl;
					FindedFilesArrary.push_back(p.assign(SearchingMD).append("\\").append(fileinfo.name));
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	if (FindedFilesArrary.size() > 0)//当FindedFilesArrary的元素大于1时，说明确实找到了符合条件的文件
	{
		IsAvailable = true;
	}
	return IsAvailable;
}

//获取文件里的逐行信息
vector<string> GetArrInfoFromFile(const char* FilePath)
{
	ifstream ReadedFile;
	string info = "";
	vector<string> TextContext;
	ReadedFile.open(FilePath);
	if (!ReadedFile)
	{
		cout << "/*******************************打开文件失败！*********************************/";
	}
	else{
		while (getline(ReadedFile, info))//逐行读取文本文件
		{

			//cout << info << endl;
			TextContext.push_back(info);
		}
	}
	ReadedFile.close();
	return TextContext;
}






//将字符串数组保存成一个文件
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



//将Unicode编码转换为其他编码
void UnicodeToOther(const char* FilePath, const char* CodePath, int TargetCode)
{
	wchar_t* pUnicode = AnsiToUnicode(FilePath);
	TCHAR szBuf[1024];
	memset(szBuf, 0, sizeof(szBuf));
	FILE *pStream;
	errno_t rErr = _wfopen_s(&pStream, pUnicode, L"rb");
	//errno_t rErr = _wfopen_s(&pStream, _T("test.txt"), _T("rb"));
	fseek(pStream, 2, SEEK_SET);//由于Unicode文件编码的BOM是0XFF 0XFE，所以右移两位
	string strTemp;
	if (NULL == rErr)
	{
		while (!feof(pStream))
		{
			_fgetts(szBuf, 1024, pStream);
#ifdef UNICODE
			DWORD dwNum = WideCharToMultiByte(TargetCode, NULL, szBuf, -1, NULL, 0, NULL, FALSE);//得到UTF-8文件编码
#else
			DWORD dwNum = WideCharToMultiByte(TargetCode, NULL, AnsiToUnicode(szBuf), -1, NULL, 0, NULL, FALSE);//得到UTF-8文件编码
#endif
			//DWORD dwNum = WideCharToMultiByte(936, NULL, szBuf, -1, NULL, 0, NULL, FALSE);//得到ANSI文件编码
			char *psText;
			psText = new char[dwNum];
			if (!psText)
			{
				delete[]psText;
			}
#ifdef UNICODE
			WideCharToMultiByte(TargetCode, NULL, szBuf, -1, psText, dwNum, NULL, FALSE);//得到UTF-8文件编码
#else
			WideCharToMultiByte(TargetCode, NULL, AnsiToUnicode(szBuf), -1, psText, dwNum, NULL, FALSE);//得到UTF-8文件编码
#endif
			//WideCharToMultiByte(936, NULL, szBuf, -1, psText, dwNum, NULL, FALSE);//得到ANSI文件编码
			strTemp += psText;
			delete[]psText;
		}
	}
	free(pUnicode);//释放pUnicode的内存，一般可以忽略它，运行这条语句后，pUnicode将变为NULL了
	fclose(pStream);

	//写入文件
	fstream OutFile;
	OutFile.open(CodePath, ios::out);
	OutFile << strTemp;
	OutFile.close();
}


//将ANSI编码转换为其他编码
void ANSIToOther(const char* FilePath, const char* CodePath, int TargetCode)
{
	char buffer[1024];
	fstream ReadedFile;
	FILE *wFile;
	ReadedFile.open(FilePath, ios::in);//读取的方式打开文件
	//由于ANSI文件编码并不包含BOM，也就是文件头，所以直接读取，不需要移位
	if (!ReadedFile.is_open())
	{
		cout << "文件打开失败" << endl;
	}
	else
	{
		string FileContent("");
		while (!ReadedFile.eof())//判断是否到达了文件末尾
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
					 m_strUnicode[0] = wchar_t(0XFEFF);//Unicode编码的BOM
					 fputwc(*m_strUnicode, wFile);//这里设置了文件编码
					 fputws(AnsiToUnicode(FileContent.c_str()), wFile);
					 fclose(wFile);
		}; break;
		case 65001:
		{
					  fstream wFile;
					  wFile.open(CodePath, ios::out);//不指定二进制方式读取
					  wFile.write(Convert(FileContent, 936, 65001).c_str(), FileContent.length() + 1);
					  wFile.close();
		}; break;
		}
	}
}


//将UTF-8编码转换为其他编码
void UTF8ToOther(const char* FilePath, const char* CodePath, int TargetCode)
{
	if (TargetCode != NULL)
	{
		char buffer[1024];
		fstream InFile, OutFile;
		InFile.open(FilePath, ios::in);//读取的方式打开文件
		if (!InFile.is_open())
		{
			cout << "文件打开失败" << endl;
		}
		else
		{
			string FileContent("");
			InFile.seekg(3);//由于UTF-8文件编码的BOM是0XEF 0XBB 0XBF，所以右移三位
			//右移两位字符
			while (!InFile.eof())//判断是否到达了文件末尾
			{
				InFile.getline(buffer, 256);
				FileContent += string(buffer) + string("\r\n");
			}
			InFile.close();

			OutFile.open(CodePath, ios::out);//写入文件的方式打开
			OutFile << Convert(FileContent, 65001, 936);
			OutFile.close();//以上得到一个ANSI文件编码的文本
			if (TargetCode == 1200)
			{
				ANSIToOther(CodePath, CodePath, 1200);
			}
		}
	}
}



//转换文件编码
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


//选择对话框
void SelectFileOrFolderDlg(string &FolderPath)
{
	//打开选择对话框
	TCHAR szBuffer[MAX_PATH] = { 0 };
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = NULL;
	bi.pszDisplayName = szBuffer;
	bi.lpszTitle = _T("从下面选择文件或文件夹：");
	bi.ulFlags = BIF_BROWSEINCLUDEFILES | BIF_USENEWUI;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (NULL == idl)
	{
		return;
	}
	//cout << szBuffer << endl;
	BOOL temp = SHGetPathFromIDList(idl, szBuffer);
	//cout << "temp:" << temp << endl;//为1时，选中了文件，为0时选中了文件夹，取消时，什么都没有
	//cout << szBuffer << endl;
	//cout << UnicodeToANSI(szBuffer) << endl;
#ifdef UNICODE
	FolderPath = UnicodeToANSI(szBuffer);
#else
	FolderPath = szBuffer;
#endif
}


//仅仅能配合文件对话框使用哦，自定义路径将无效
int IsFolder(const TCHAR* szBuffer)
{
	if (GetFileAttributes(szBuffer)&FILE_ATTRIBUTE_DIRECTORY)
	{
		//cout << "此时文件夹" << endl;
		return 1;//文件夹标记
	}
	else
	{
		//cout << "此时文件" << endl;
		return 2;//文件标记
	}

	//if (GetFileAttributesA(szBuffer)&INVALID_FILE_ATTRIBUTES)
	//{
	//	cout << "此数值有效" << endl;
	//}
	//else
	//{
	//	cout << "此数值无效" << endl;
	//}
	return 0;
}


//根据脏字库来筛选所需的文件数据
void FilterFileContent(const char* InPath, const char* OutPath, vector<string>& FilterLib, int FilterMode)
{
	vector<string> FullValue = ReadArrInfoInFile(InPath);
	bool IsFull = false;
	int Choice = 0;
	//新建一个文件
	FILE *wFile;
	fopen_s(&wFile, OutPath, "w"); //以写的方式打开第二个文件
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


//获取文件里的逐行信息
vector<string> ReadArrInfoInFile(const char* FilePath)
{
	ifstream ReadedFile;
	string info = "";
	vector<string> TextContext;
	ReadedFile.open(FilePath);
	if (!ReadedFile)
	{
		cout << FilePath << "读取失败" << endl;
	}
	else{
		while (getline(ReadedFile, info))//逐行读取文本文件
		{

			//cout << info << endl;
			TextContext.push_back(info);
		}
	}
	ReadedFile.close();
	return TextContext;
}


//截取文件开头n个字节，用来作为文件类型判断
string GetFileBOMByOffsetValue(const char* FilePath, int offsetcharc)
{
	std::string contents;
	std::ifstream in(FilePath, std::ios::in | std::ios::binary);
	if (!in) {
		std::cout << "File " << FilePath << " not found!\n";
	}
	else
	{
		contents.resize((size_t)offsetcharc);//设置contents的内存大小
		in.seekg(0, std::ios::beg);//从文件流开始，偏移值为0来读取文件信息
		in.read(&contents[0], offsetcharc);//将文件开头的offsetcharc个字节读取到contents存放起来
		in.close();
	}
	//cout << contents << endl;
	return contents;
}




//从文件头开始截取n个字符后面的数据
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



//以二进制的形式写入到本地中
bool CreateFileByBinary(string Text, const char*FilePath, string BOMStr)
{

	//UTF-8："\xef\xbb\xbf"
	std::ofstream out(FilePath, std::ios::out | std::ios::binary);
	bool IsOK = true;
	if (!out) {
		std::cout << "File " << FilePath << " not found!\n";
		//exit(EXIT_FAILURE);
		IsOK = false;
	}

	//out << Text;
	//先添加UTF-8特有的BOM
	out.write(BOMStr.c_str(), BOMStr.length());
	out.write(Text.c_str(), Text.length());
	out.close();
	return IsOK;
}

