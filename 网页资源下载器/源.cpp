#include <string>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <vector>
#include "JSONCPP\json.h"
#include "HTMLCXX\ParserDom.h"
#include "HTMLCXX\Extensions.h"
#include "字符串拓展\StringEx.h"
#include "网络操作\WEBOptions.h"
#include "文件操作\FileOptionsEx.h"

//引入下载库
#include <UrlMon.h>
#pragma comment(lib,"UrlMon.lib")

using namespace std;
using namespace htmlcxx;

struct NodeType{//此结构体存放了一个标签节点的所有属性
	string TagName;//当前节点所在的标签名
	string PropName;//需要获取数值的属性名
	vector<MyExtension::NodePropBase> PropInfo;
	//重载输出流运算符<<，需要声明其为友元函数
	friend std::ostream& operator<<(std::ostream& out, NodeType& value)
	{
		if (value.TagName.length() < 1 && value.PropInfo.empty())
			out << "当前NodeInfoType变量完全不初始化" << endl;
		else{
			if (value.TagName.length() < 1)
				out << "当前NodeInfoType变量中的TagName没有初始化" << endl;
			else
				out << "当前标签节点名称为：         " << value.TagName << endl;

			if (value.PropName.length() < 1)
				out << "当前NodeInfoType变量中的PropName没有初始化" << endl;
			else
				out << "当前需要获取数值的属性名为： " << value.PropName << endl;

			if (value.PropInfo.empty())
				out << "当前NodeInfoType变量中的PropInfo没有初始化" << endl;
			else{
				out << "当前标签节点的属性罗列如下：" << endl;
				for (size_t n = 0; n < value.PropInfo.size(); n++)
					out << "第" << n + 1 << "个属性：" << value.PropInfo[n].PropName << "=\"" << value.PropInfo[n].PropValue << "\"" << endl;
			}
		}
		return out;
	}
};

//初始化节点属性里的所有信息
int JsonValue2NodeType(Json::Value NodeJson, NodeType &TempNode);

void LoadFileFromURL(string DownloadURL, bool IsDownload, bool IsShowData)
{
	if (IsShowData)
		cout << "当前正在下载：" << DownloadURL << "对应的文件" << endl;
	if (IsDownload)
	{
		string FilePath = DownloadURL.substr(DownloadURL.rfind("/") + 1);
		if (!PathFileExistsA(FilePath.c_str()))
		{
			cout << FilePath << "不存在，即将下载……" << endl;
			//使用COM来下载，无需自己写，不过有弊端，不能实时查看下载进度，建议使用libcurl来改进下载
			::CoInitialize(NULL);
			::URLDownloadToFileA(NULL, DownloadURL.c_str(), FilePath.c_str(), 0, NULL);
			::CoUninitialize();
			if (PathFileExistsA(FilePath.c_str()))
				cout << FilePath << "下载完成" << endl;
			else
				cout << FilePath << "下载失败" << endl;
		}else
			cout << FilePath << "已经存在，跳过……" << endl;
	}else{
		system("pause");
		exit(1);
	}
}

//定义相应的转换函数
//1.将TwoDArray转换为NodePropBase
MyExtension::NodePropBase TwoDArray2NodePropBase_Single(JsonEx::TwoDArray Value)
{
	MyExtension::NodePropBase TempValue = { Value.one, Value.two};
	return TempValue;
}


//2.将vector<TwoDArray>转换为vector<NodePropBase>
vector<MyExtension::NodePropBase> TwoDArray2NodePropBase_Mulitple(vector<JsonEx::TwoDArray> VectorValue)
{
	vector<MyExtension::NodePropBase> TempVectorValue;
	for (size_t n = 0; n < VectorValue.size(); n++)
	{
		TempVectorValue.push_back(TwoDArray2NodePropBase_Single(VectorValue[n]));
	}
	return TempVectorValue;
}


//自动根据NodeInfoType里面的变量初始化程度来获取符合条件的子节点
vector<tree<HTML::Node>> GetSubNodeByAuto(tree<HTML::Node> ParentNode, NodeType NodeFilter,int RunFlag)
{
	vector<tree<HTML::Node>> TempNodes;
	switch (RunFlag)
	{
	case 0://完全初始化
	case 5://仅有PropName没有初始化
		TempNodes = MyExtension::GetSubNodeByTagInfo(ParentNode, NodeFilter.TagName, NodeFilter.PropInfo); break;
	case 1://仅有TagName初始化
	case 4://仅有PropInfo没有初始化
		TempNodes = MyExtension::GetSubNodeByTagName(ParentNode, NodeFilter.TagName); break;
	case 3://仅有PropInfo初始化
	case 6://仅有TagName没有初始化
		TempNodes = MyExtension::GetSubNodeByTagProp(ParentNode, NodeFilter.PropInfo); break;
	default:
		cout << "参数NovelNameCurrentNode没有初始化哦" << endl; break;
	}
	return TempNodes;
}


//这个函数用来将当前网页所在地址和href属性的属性数值上的地址进行拼接的,LMove是自定义匹配时的左移参数，默认为1
string URLCat(string MDURL, string ChapterURL, int MatchMode, int LMove)
{
	string tempurl = "";
	switch (MatchMode)
	{
	case 0:
	{
			  //直接相加
			  //比如http://www.dhzw.com/book/10/10751/和2908158.html，最后需要的是http://www.dhzw.com/book/10/10751/2908158.html
			  tempurl = MDURL + ChapterURL;
	}; break;
	case 1:
	{
			  //手动匹配，MDURL被截取+ChapterURL
			  tempurl = MDURL.substr(0, MDURL.length() - LMove) + ChapterURL;
	}; break;
	case 2:
	{
			  //自动匹配，针对那些左向匹配的情况。先从截取到的网址上取三个字符出来，用来匹配原来的全文阅读网页的网址
			  //比如http://www.quledu.com/wcxs-15127/和/wcxs-15127-3883046/ ，最后需要http://www.quledu.com/wcxs-15127-3883046/
			  tempurl = MDURL.substr(0, MDURL.length() - MDURL.find(ChapterURL.substr(0, 3))) + ChapterURL;
	}; break;
	case 3:
	{
			  //自动匹配，针对那些部分整块匹配的情况，从右往左看，得到第一个/的位置信息，之后将其截取，再和章节网址并接
			  //比如http://www.woqudu.com/files/article/html/6/6647/index.html和1482559.html，最后需要http://www.woqudu.com/files/article/html/6/6647/1482559.html
			  tempurl = MDURL.substr(0, MDURL.rfind("/") + 1) + ChapterURL;
	}; break;
	case 4:
	{
			  //手动匹配，MDURL被截取+ChapterURL
			  tempurl = MDURL + ChapterURL.substr(LMove);
	}; break;
	default:
		//MessageBoxA(NULL, "参数爆表，请重新设置", "警告", MB_OK);
		cout << "参数爆表" << endl;
		break;
	}
	return tempurl;
}


void DownloadAllResInHTML(string url, const char* JSONPath)
{
	const char* DiskFile = "temp.html";
	Json::Value JSONRoot = JsonEx::GetValueByJSON(JSONPath);//得到过滤库里的JSON对象
	//string HTMLContent = ReadTextInFile(DiskFile);
	string HTMLContent = GeWebInfoEx(url);//支持三次断网重连
	//cout << HTMLContent << endl;

	if (!PathFileExistsA(DiskFile) && JSONRoot["DownLoadInfo"]["SaveDisk"].asBool())
	{
		CreateFileByBinary(HTMLContent, DiskFile);
	}

	HTML::ParserDom parser;
	tree<HTML::Node> dom;

	//根据字符串生成DOM树，将其统一转换为ASNI编码
	if (IsUTF8String(HTMLContent.c_str(), HTMLContent.length()) || IsTextUTF8(HTMLContent.c_str(), HTMLContent.length()))
	{
		dom = parser.parseTree(Convert(HTMLContent, 65001, 936));
	}
	else
	{
		dom = parser.parseTree(HTMLContent);
	}

	//根据过滤条件找到父节点
	NodeType ParentNodeInfo;
	int ParentRunFlag = JsonValue2NodeType(JSONRoot["DownLoadInfo"]["ParentNodeInfo"],ParentNodeInfo);
	std::vector<tree<HTML::Node>> ParentNodeArr = GetSubNodeByAuto(dom, ParentNodeInfo,ParentRunFlag);
	//cout << ParentNodeArr.size() << endl;
	if (ParentNodeArr.size() == 1)
	{
		//cout << ParentNodeArr[0] << endl;
		//根据过滤条件找到子节点
		NodeType SubNodeInfo;
		int SubRunFlag = JsonValue2NodeType(JSONRoot["DownLoadInfo"]["SubNodeInfo"], SubNodeInfo);
		std::vector<tree<HTML::Node>> SubNodeArr = GetSubNodeByAuto(ParentNodeArr[0], SubNodeInfo, SubRunFlag);

		//cout << SubNodeArr.size() << endl;
		size_t IsEnd = 0;
		while (IsEnd<SubNodeArr.size()){
			MyExtension::NodePropBase PropInfo;
			MyExtension::GetPropValueByPropName(SubNodeArr[IsEnd].begin(), SubNodeInfo.PropName, PropInfo);

			string DownloadURL = URLCat(url, PropInfo.PropValue, JSONRoot["MatchURL"]["MatchMode"].asInt(), JSONRoot["MatchURL"]["nMoveUnit"].asInt());
			bool IsShowData = JSONRoot["MatchURL"]["IsShowData"].asBool();

			//仅仅对那些有有效的链接进行下载，通过后缀名进行识别
			bool IsDownload = JSONRoot["DownLoadInfo"]["IsDownload"].asBool();
			size_t RegIndex = DownloadURL.find(JSONRoot["DownLoadInfo"]["FileExtensionName"].asString());
			if (JSONRoot["DownLoadInfo"]["FileExtensionName"].isNull() || (RegIndex>0 && RegIndex<DownloadURL.length()))
				LoadFileFromURL(DownloadURL, IsDownload, IsShowData);
			IsEnd++;
		}
	}
}

//输出命令行参数帮助
void PrintHelp()
{
	cout << "启动命令行功能，参数如下：" << endl;
	cout << "-JSONPath:    这里放置一个JSON文件的路径" << endl;
	cout << "-WebURL:      这里放置一个网址" << endl;
	cout << "使用示例如下：" << endl;
	cout << GetApplicationDir() << " -JSONPath:example.json -WebURL:https://ffmpeg.zeranoe.com/builds/" << endl;
}


//主函数
void main(int argc, char* argv[])
{
	if (argc > 1 && string(argv[1]) == string("-Help"))
		PrintHelp();
	else {
		if (argc == 3)
		{
			string tempstr1 = string(argv[1]);
			string tempstr2 = string(argv[2]);
			size_t index1 = tempstr1.find("JSONPath:");
			size_t index2 = tempstr2.find("WebURL:");
			if ((index1 > 0 && index1 < tempstr1.length()) && (index2 > 0 && index2 < tempstr2.length()))
				DownloadAllResInHTML(tempstr2.substr(strlen("-WebURL:")), (tempstr1.substr(strlen("-JSONPath:"))).c_str());
		}
		else
			PrintHelp();
	}

	system("pause");
}



//初始化节点属性里的所有信息
int JsonValue2NodeType(Json::Value NodeJson, NodeType &TempNode)
{
	//int RunFlag = 0;//返回值，指导后面语句的执行
	TempNode.TagName = NodeJson["TagName"].asString();
	TempNode.PropName = NodeJson["PropName"].asString();
	TempNode.PropInfo = TwoDArray2NodePropBase_Mulitple(JsonEx::ReadMultipleArraryInJSON(NodeJson["PropInfo"]));
	//cout << "当前TagName为：" << TempNode.TagName.length() << endl;
	//cout << "当前PropName为：" << TempNode.PropName.length() << endl;
	//cout << "当前PropInfo为：" << TempNode.PropInfo.size() << endl;
	//cout << "当前PropInfo为：" << TempNode.PropInfo.empty() << endl;

#define TAGLEN TempNode.TagName.length()
#define PROPLEN TempNode.PropName.length()
#define ISEMPTY TempNode.PropInfo.empty()

	if (TAGLEN >0 && PROPLEN < 1 && ISEMPTY)
		return 1;//仅有TagName初始化
	if (TAGLEN < 1 && PROPLEN > 0 && ISEMPTY)
		return 2;//仅有PropName初始化
	if (TAGLEN < 1 && PROPLEN < 1 && !ISEMPTY)
		return 3;//仅有PropInfo初始化
	if (TAGLEN > 0 && PROPLEN > 0 && ISEMPTY)
		return 4;//仅有PropInfo没有初始化
	if (TAGLEN > 0 && PROPLEN < 1 && !ISEMPTY)
		return 5;//仅有PropName没有初始化
	if (TAGLEN < 1 && PROPLEN > 0 && !ISEMPTY)
		return 6;//仅有TagName没有初始化
	if (TAGLEN < 1 && PROPLEN < 1 && ISEMPTY)
		return 7;//完全没有初始化
	if (TAGLEN > 0 && PROPLEN < 1 && ISEMPTY)
		return 8;//完全初始化

	return 0;//完全初始化
}