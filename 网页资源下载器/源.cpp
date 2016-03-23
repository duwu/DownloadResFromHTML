#include <string>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <vector>
#include "JSONCPP\json.h"
#include "HTMLCXX\ParserDom.h"
#include "HTMLCXX\Extensions.h"
#include "�ַ�����չ\StringEx.h"
#include "�������\WEBOptions.h"
#include "�ļ�����\FileOptionsEx.h"

//�������ؿ�
#include <UrlMon.h>
#pragma comment(lib,"UrlMon.lib")

using namespace std;
using namespace htmlcxx;

struct NodeType{//�˽ṹ������һ����ǩ�ڵ����������
	string TagName;//��ǰ�ڵ����ڵı�ǩ��
	string PropName;//��Ҫ��ȡ��ֵ��������
	vector<MyExtension::NodePropBase> PropInfo;
	//��������������<<����Ҫ������Ϊ��Ԫ����
	friend std::ostream& operator<<(std::ostream& out, NodeType& value)
	{
		if (value.TagName.length() < 1 && value.PropInfo.empty())
			out << "��ǰNodeInfoType������ȫ����ʼ��" << endl;
		else{
			if (value.TagName.length() < 1)
				out << "��ǰNodeInfoType�����е�TagNameû�г�ʼ��" << endl;
			else
				out << "��ǰ��ǩ�ڵ�����Ϊ��         " << value.TagName << endl;

			if (value.PropName.length() < 1)
				out << "��ǰNodeInfoType�����е�PropNameû�г�ʼ��" << endl;
			else
				out << "��ǰ��Ҫ��ȡ��ֵ��������Ϊ�� " << value.PropName << endl;

			if (value.PropInfo.empty())
				out << "��ǰNodeInfoType�����е�PropInfoû�г�ʼ��" << endl;
			else{
				out << "��ǰ��ǩ�ڵ�������������£�" << endl;
				for (size_t n = 0; n < value.PropInfo.size(); n++)
					out << "��" << n + 1 << "�����ԣ�" << value.PropInfo[n].PropName << "=\"" << value.PropInfo[n].PropValue << "\"" << endl;
			}
		}
		return out;
	}
};

//��ʼ���ڵ��������������Ϣ
int JsonValue2NodeType(Json::Value NodeJson, NodeType &TempNode);

void LoadFileFromURL(string DownloadURL, bool IsDownload, bool IsShowData)
{
	if (IsShowData)
		cout << "��ǰ�������أ�" << DownloadURL << "��Ӧ���ļ�" << endl;
	if (IsDownload)
	{
		string FilePath = DownloadURL.substr(DownloadURL.rfind("/") + 1);
		if (!PathFileExistsA(FilePath.c_str()))
		{
			cout << FilePath << "�����ڣ��������ء���" << endl;
			//ʹ��COM�����أ������Լ�д�������б׶ˣ�����ʵʱ�鿴���ؽ��ȣ�����ʹ��libcurl���Ľ�����
			::CoInitialize(NULL);
			::URLDownloadToFileA(NULL, DownloadURL.c_str(), FilePath.c_str(), 0, NULL);
			::CoUninitialize();
			if (PathFileExistsA(FilePath.c_str()))
				cout << FilePath << "�������" << endl;
			else
				cout << FilePath << "����ʧ��" << endl;
		}else
			cout << FilePath << "�Ѿ����ڣ���������" << endl;
	}else{
		system("pause");
		exit(1);
	}
}

//������Ӧ��ת������
//1.��TwoDArrayת��ΪNodePropBase
MyExtension::NodePropBase TwoDArray2NodePropBase_Single(JsonEx::TwoDArray Value)
{
	MyExtension::NodePropBase TempValue = { Value.one, Value.two};
	return TempValue;
}


//2.��vector<TwoDArray>ת��Ϊvector<NodePropBase>
vector<MyExtension::NodePropBase> TwoDArray2NodePropBase_Mulitple(vector<JsonEx::TwoDArray> VectorValue)
{
	vector<MyExtension::NodePropBase> TempVectorValue;
	for (size_t n = 0; n < VectorValue.size(); n++)
	{
		TempVectorValue.push_back(TwoDArray2NodePropBase_Single(VectorValue[n]));
	}
	return TempVectorValue;
}


//�Զ�����NodeInfoType����ı�����ʼ���̶�����ȡ�����������ӽڵ�
vector<tree<HTML::Node>> GetSubNodeByAuto(tree<HTML::Node> ParentNode, NodeType NodeFilter,int RunFlag)
{
	vector<tree<HTML::Node>> TempNodes;
	switch (RunFlag)
	{
	case 0://��ȫ��ʼ��
	case 5://����PropNameû�г�ʼ��
		TempNodes = MyExtension::GetSubNodeByTagInfo(ParentNode, NodeFilter.TagName, NodeFilter.PropInfo); break;
	case 1://����TagName��ʼ��
	case 4://����PropInfoû�г�ʼ��
		TempNodes = MyExtension::GetSubNodeByTagName(ParentNode, NodeFilter.TagName); break;
	case 3://����PropInfo��ʼ��
	case 6://����TagNameû�г�ʼ��
		TempNodes = MyExtension::GetSubNodeByTagProp(ParentNode, NodeFilter.PropInfo); break;
	default:
		cout << "����NovelNameCurrentNodeû�г�ʼ��Ŷ" << endl; break;
	}
	return TempNodes;
}


//���������������ǰ��ҳ���ڵ�ַ��href���Ե�������ֵ�ϵĵ�ַ����ƴ�ӵ�,LMove���Զ���ƥ��ʱ�����Ʋ�����Ĭ��Ϊ1
string URLCat(string MDURL, string ChapterURL, int MatchMode, int LMove)
{
	string tempurl = "";
	switch (MatchMode)
	{
	case 0:
	{
			  //ֱ�����
			  //����http://www.dhzw.com/book/10/10751/��2908158.html�������Ҫ����http://www.dhzw.com/book/10/10751/2908158.html
			  tempurl = MDURL + ChapterURL;
	}; break;
	case 1:
	{
			  //�ֶ�ƥ�䣬MDURL����ȡ+ChapterURL
			  tempurl = MDURL.substr(0, MDURL.length() - LMove) + ChapterURL;
	}; break;
	case 2:
	{
			  //�Զ�ƥ�䣬�����Щ����ƥ���������ȴӽ�ȡ������ַ��ȡ�����ַ�����������ƥ��ԭ����ȫ���Ķ���ҳ����ַ
			  //����http://www.quledu.com/wcxs-15127/��/wcxs-15127-3883046/ �������Ҫhttp://www.quledu.com/wcxs-15127-3883046/
			  tempurl = MDURL.substr(0, MDURL.length() - MDURL.find(ChapterURL.substr(0, 3))) + ChapterURL;
	}; break;
	case 3:
	{
			  //�Զ�ƥ�䣬�����Щ��������ƥ���������������󿴣��õ���һ��/��λ����Ϣ��֮�����ȡ���ٺ��½���ַ����
			  //����http://www.woqudu.com/files/article/html/6/6647/index.html��1482559.html�������Ҫhttp://www.woqudu.com/files/article/html/6/6647/1482559.html
			  tempurl = MDURL.substr(0, MDURL.rfind("/") + 1) + ChapterURL;
	}; break;
	case 4:
	{
			  //�ֶ�ƥ�䣬MDURL����ȡ+ChapterURL
			  tempurl = MDURL + ChapterURL.substr(LMove);
	}; break;
	default:
		//MessageBoxA(NULL, "������������������", "����", MB_OK);
		cout << "��������" << endl;
		break;
	}
	return tempurl;
}


void DownloadAllResInHTML(string url, const char* JSONPath)
{
	const char* DiskFile = "temp.html";
	Json::Value JSONRoot = JsonEx::GetValueByJSON(JSONPath);//�õ����˿����JSON����
	//string HTMLContent = ReadTextInFile(DiskFile);
	string HTMLContent = GeWebInfoEx(url);//֧�����ζ�������
	//cout << HTMLContent << endl;

	if (!PathFileExistsA(DiskFile) && JSONRoot["DownLoadInfo"]["SaveDisk"].asBool())
	{
		CreateFileByBinary(HTMLContent, DiskFile);
	}

	HTML::ParserDom parser;
	tree<HTML::Node> dom;

	//�����ַ�������DOM��������ͳһת��ΪASNI����
	if (IsUTF8String(HTMLContent.c_str(), HTMLContent.length()) || IsTextUTF8(HTMLContent.c_str(), HTMLContent.length()))
	{
		dom = parser.parseTree(Convert(HTMLContent, 65001, 936));
	}
	else
	{
		dom = parser.parseTree(HTMLContent);
	}

	//���ݹ��������ҵ����ڵ�
	NodeType ParentNodeInfo;
	int ParentRunFlag = JsonValue2NodeType(JSONRoot["DownLoadInfo"]["ParentNodeInfo"],ParentNodeInfo);
	std::vector<tree<HTML::Node>> ParentNodeArr = GetSubNodeByAuto(dom, ParentNodeInfo,ParentRunFlag);
	//cout << ParentNodeArr.size() << endl;
	if (ParentNodeArr.size() == 1)
	{
		//cout << ParentNodeArr[0] << endl;
		//���ݹ��������ҵ��ӽڵ�
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

			//��������Щ����Ч�����ӽ������أ�ͨ����׺������ʶ��
			bool IsDownload = JSONRoot["DownLoadInfo"]["IsDownload"].asBool();
			size_t RegIndex = DownloadURL.find(JSONRoot["DownLoadInfo"]["FileExtensionName"].asString());
			if (JSONRoot["DownLoadInfo"]["FileExtensionName"].isNull() || (RegIndex>0 && RegIndex<DownloadURL.length()))
				LoadFileFromURL(DownloadURL, IsDownload, IsShowData);
			IsEnd++;
		}
	}
}

//��������в�������
void PrintHelp()
{
	cout << "���������й��ܣ��������£�" << endl;
	cout << "-JSONPath:    �������һ��JSON�ļ���·��" << endl;
	cout << "-WebURL:      �������һ����ַ" << endl;
	cout << "ʹ��ʾ�����£�" << endl;
	cout << GetApplicationDir() << " -JSONPath:example.json -WebURL:https://ffmpeg.zeranoe.com/builds/" << endl;
}


//������
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



//��ʼ���ڵ��������������Ϣ
int JsonValue2NodeType(Json::Value NodeJson, NodeType &TempNode)
{
	//int RunFlag = 0;//����ֵ��ָ����������ִ��
	TempNode.TagName = NodeJson["TagName"].asString();
	TempNode.PropName = NodeJson["PropName"].asString();
	TempNode.PropInfo = TwoDArray2NodePropBase_Mulitple(JsonEx::ReadMultipleArraryInJSON(NodeJson["PropInfo"]));
	//cout << "��ǰTagNameΪ��" << TempNode.TagName.length() << endl;
	//cout << "��ǰPropNameΪ��" << TempNode.PropName.length() << endl;
	//cout << "��ǰPropInfoΪ��" << TempNode.PropInfo.size() << endl;
	//cout << "��ǰPropInfoΪ��" << TempNode.PropInfo.empty() << endl;

#define TAGLEN TempNode.TagName.length()
#define PROPLEN TempNode.PropName.length()
#define ISEMPTY TempNode.PropInfo.empty()

	if (TAGLEN >0 && PROPLEN < 1 && ISEMPTY)
		return 1;//����TagName��ʼ��
	if (TAGLEN < 1 && PROPLEN > 0 && ISEMPTY)
		return 2;//����PropName��ʼ��
	if (TAGLEN < 1 && PROPLEN < 1 && !ISEMPTY)
		return 3;//����PropInfo��ʼ��
	if (TAGLEN > 0 && PROPLEN > 0 && ISEMPTY)
		return 4;//����PropInfoû�г�ʼ��
	if (TAGLEN > 0 && PROPLEN < 1 && !ISEMPTY)
		return 5;//����PropNameû�г�ʼ��
	if (TAGLEN < 1 && PROPLEN > 0 && !ISEMPTY)
		return 6;//����TagNameû�г�ʼ��
	if (TAGLEN < 1 && PROPLEN < 1 && ISEMPTY)
		return 7;//��ȫû�г�ʼ��
	if (TAGLEN > 0 && PROPLEN < 1 && ISEMPTY)
		return 8;//��ȫ��ʼ��

	return 0;//��ȫ��ʼ��
}