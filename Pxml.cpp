/*
Description:
	A much more quicker Learning Center Help.
	Designed to finish ALL the work in MINUTES.

TODO:
	Input your information.
	Find your class number from a txt file attached.
	Then all you need is to click "Next" and "Complete"
	DOES NOT require to fill in any blanks, just wait for less than 10sec.

Author:
	mason1900
	Great Thanks to Li Xusheng, Li Yuxuan

License:
	No restrictions for Pxml.cpp, free of any kind of use.
	TinyXML is an open source library program. License of this is written in tinyxml.cpp
*/

#include "stdafx.h"
using namespace std;

#define SchoolServer "http://222.30.60.3/"
#define CoursePrefix "Course/College_English_NEW_SecEdition_Integrated_"
#define CoursePrefix_L "Course/College_English_NEW_SecEdition_Listening_"

enum WorkModeType
{
	Single,
	Traversal
};

enum SheetStyleType
{
	Integrated,
	Listening
};
struct BookCodeType
{
	wstring szTitle;
	string Book;
	string Unit;
	string Type;
	string paperID;
};

bool parse_paper(const char* FileName, BookCodeType& BookCode);
bool parse_answer_txt(const char* FileName, vector<string>& RightAnswer);
bool gen_answer_xml(const char* FileName, const vector<string> RightAnswer,
					SheetStyleType sheet_style, const BookCodeType BookCode,const string stuID);
bool obfuscation(vector<string>& obfs, const vector<string>& RightAnswer);
//bool confirm(const wstring szTitle,string BookCode);

const char* szPaperFileName=".\\appdata\\paper\\paper.xml";
const char* szAnswerFileName=".\\appdata\\answer\\answer.xml";



bool parse_paper(const char* FileName, BookCodeType& BookCode)
{

	BookCodeType _BookCode ;
	TiXmlDocument doc;
//	TiXmlHandle docHandle(&doc);
	doc.LoadFile(FileName); 
	if (!doc.LoadFile()) {
		cerr<<"Failed to load XML file"<<endl;
//		system("pause");
		return false;
	}
	
//	load root element
	TiXmlElement* paper = doc.FirstChildElement();
	TiXmlAttribute* title = paper->FirstAttribute();
	string szMaterialName = title->Value();
	

	int len = MultiByteToWideChar(CP_UTF8, 0, &szMaterialName[0], -1, NULL, 0);	
//	WINAPI, return the required buffer size
//	Note: vector's elements are initialized to be 0 by default, so it won't overflow.
	vector<wchar_t> unicode(len);
	MultiByteToWideChar(CP_UTF8, 0, &szMaterialName[0], -1, &unicode[0], len);
	_BookCode.szTitle = &unicode[0];
	wstring szTitle = _BookCode.szTitle;                                     //Copy



//	paperID
	TiXmlAttribute* identifier = title->Next();
	_BookCode.paperID = identifier->Value();                                 //e1fea2ab-914a-4b57-a442-0d883e8bba39	

//	UNICODE to ANSI, use UNICODE for chinese characters
	int length = _BookCode.szTitle.length();
	int flagB = _BookCode.szTitle.find(L"B");
	int flagU = _BookCode.szTitle.find(L"U");
	int flagT = _BookCode.szTitle.find(L"-");
//	wstring _szShitName = szTitle.substr(0,flagB);                           //全新版第二版综合
	wstring _szBookPref = szTitle.substr(flagB,length-flagB);                //B3U1-D
	wstring _szBookName = szTitle.substr(flagB+1,flagU-flagB-1);             //3
	wstring _szUnitName = szTitle.substr(flagU+1,flagT-flagU-1);             //1
	wstring _szShitType = szTitle.substr(flagT+1,length-flagT-1);            //D
//	char ShitName[200] = "\0";
	char BookPref[200] = "\0";                                               //古语云：内存不要钱
	char BookName[200] = "\0";
	char UnitName[200] = "\0";
	char ShitType[200] = "\0";

	size_t bufsize = 100;
	size_t converted = 0;

//	wcstombs(ShitName, _szShitName.c_str(),100);
	wcstombs_s(&converted, BookPref, bufsize, _szBookPref.c_str(),_TRUNCATE);
	wcstombs_s(&converted, BookName, bufsize, _szBookName.c_str(),_TRUNCATE); 
	wcstombs_s(&converted, UnitName, bufsize, _szUnitName.c_str(),_TRUNCATE); 
	wcstombs_s(&converted, ShitType, bufsize, _szShitType.c_str(),_TRUNCATE);
	_BookCode.Book = BookName;
	_BookCode.Unit = UnitName;
	_BookCode.Type = ShitType;


	if(_BookCode.Book == BookCode.Book){
		if( _BookCode.Unit == BookCode.Unit){
			if( _BookCode.Type == BookCode.Type){
				BookCode.paperID = _BookCode.paperID;
				BookCode.szTitle = _BookCode.szTitle;
			}
		}
	}
	else{
		cerr<<"BookCode not equal"<<endl;
		return false;
	}

#ifdef _DEBUG
	wcout<<"parse_title: "<<_BookCode.szTitle<<endl;
	cout<<"parse_book: "<<_BookCode.Book<<endl;
	cout<<"parse_unit"<<_BookCode.Unit<<endl;
	cout<<"parse_type"<<_BookCode.Type<<endl;

	wcout<<"out_title"<<BookCode.szTitle<<endl;
	cout<<"out_book"<<BookCode.Book<<endl;
	cout<<"out_unit"<<BookCode.Unit<<endl;
	cout<<"out_type"<<BookCode.Type<<endl;

#endif


//	fix time
	TiXmlElement* group = paper->FirstChildElement();
	group->SetAttribute("navigationMode","nonlinear");
	while(group){
		group->SetAttribute("duration","2");
		group = group->NextSiblingElement();
	}
	doc.SaveFile();

	return true;
}

bool parse_answer_txt(const char* FileName, vector<string>& RightAnswer){
	RightAnswer.clear();
	string txtFileName (FileName);
	ifstream txtFile(txtFileName.c_str());
	if (!txtFile) {
		cerr<<"answer_txt_cannot_open"<<endl;
//		system("pause");
		return false;
	}

	string temp;
//	vector<string> RightAnswer;
	while(!txtFile.eof()){
		string szTmp;
		getline(txtFile,temp);
//			txtFile.getline(temp,5000);
		int flag = temp.find("正确答案： ");
		int len = strlen("正确答案： ");
		if (flag != string::npos){
			for ( int i=flag+len; i<(signed)temp.length(); i++){
//				cout<<i<<endl;
				szTmp += temp[i];
			}
			RightAnswer.push_back(szTmp);
		}
	}
	if(RightAnswer.size() == 0){
		cerr<<"Cannot parse answer text file"<<endl;
		return false;
	}
	txtFile.close();
	return true;
}

bool gen_answerxml(const char* FileName, const vector<string> RightAnswer,
				   SheetStyleType sheet_style,const BookCodeType BookCode, 
				   const string stuID)
{
	if(RightAnswer.size() == 0){
		cerr<<"Unknown Error: gen_answerxml"<<endl;
		return false;
	}
	TiXmlDocument doc;
	TiXmlDeclaration* declare = new TiXmlDeclaration( "1.0", "", "" );
	TiXmlElement* Root = new TiXmlElement("answer");
	doc.LinkEndChild( declare);
	doc.LinkEndChild(Root);

	//set attribute
	Root->SetAttribute("paperId",BookCode.paperID.c_str());                  //Be aware of Capital letters !
	Root->SetAttribute("examineeId","");
	
	//The time number is Time in seconds since UTC 1/1/70
	time_t ltime;
	time(&ltime);
	int real_time=(int)ltime;
	srand((unsigned)time(NULL));
	static int start_time= real_time+rand()%500;
	int end_time = start_time+rand()%3000;
	real_time = end_time;

	string complete_time_str;
	string start_time_str;
	
	stringstream stream;
	stream<<start_time;
	stream>>start_time_str;
	
	stream.clear();
	stream<<end_time;
	stream>>complete_time_str;
	stream.clear();

	Root->SetAttribute("completeTime",complete_time_str.c_str());
	Root->SetAttribute("examineeName",stuID.c_str());
	Root->SetAttribute("studentId",stuID.c_str());
	Root->SetAttribute("startTime",start_time_str.c_str());

	//generate answer
	int i=0;
	for(i=0;i< (signed)RightAnswer.size();i++){
		TiXmlElement* Value = new TiXmlElement("value");
		Root->LinkEndChild(Value);
		
		if(RightAnswer[i].length() == 1){
			TiXmlElement* choice = new TiXmlElement("choice");
			Value->LinkEndChild(choice);
			char choice_str[20] = "\0";
/*
			if(RightAnswer[i] == "A")choice_str = "1";
			else if(RightAnswer[i] == "B")choice_str = "2";
			else if(RightAnswer[i] == "C")choice_str = "3";
			else choice_str = "4";
*/
			int answer_num = RightAnswer[i][0]-'A'+1; 
			_itoa_s(answer_num,choice_str,20,10);
			TiXmlText* answer = new TiXmlText(choice_str);
			choice->LinkEndChild(answer);
		}
		else{
			TiXmlElement* text_tag = new TiXmlElement("text");
			Value->LinkEndChild(text_tag);
			TiXmlText* answer = new TiXmlText(RightAnswer[i].c_str());
			text_tag->LinkEndChild(answer);
		}
	}
	if(sheet_style == Listening){
		for(int i=0;i<2;i++){		
			TiXmlElement* Value_Tag = new TiXmlElement("value");
			Root->LinkEndChild(Value_Tag);
			TiXmlElement* sound = new TiXmlElement("sound");
			Value_Tag->LinkEndChild(sound);
			TiXmlText* blank = new TiXmlText("");
			sound->LinkEndChild(blank);
		}
	}

#ifdef _DEBUG
	cout<<BookCode.paperID<<endl;
#endif

	doc.SaveFile(FileName);
	if(doc.Error()){
		cerr<<"Paper Edit Failed"<<endl;
		return false;
	}
//	doc.Clear();
	return true;
}


bool obfuscation(vector<string>& obfs, const vector<string>& RightAnswer)
{
	obfs = RightAnswer;
	
	if(RightAnswer.size()==0){
		cerr<<"Unknown ERROR"<<endl;
		return false;
	}
	
	srand((unsigned)time(NULL));
	int changed_count = 0;
	do{
		int count = rand()%10;                               //Higher than 70
		for(int i=0; i<count; i++){
			int pos = rand()%obfs.size();
			if(obfs.at(pos).length() ==1){
				int guess = rand()%2;
				char _choice = 'A'+guess;
				string temp ="";
				temp+= _choice;
				obfs.at(pos) = temp;
				if(obfs.at(pos)!=RightAnswer.at(pos))
					changed_count++;
			}
			else{
				obfs.at(pos) = "";
				changed_count++;
			}
		}
	}while(changed_count<4);
	return true;
}
			


int main(int argc, char* argv[])
{
	
	WorkModeType Mode = Single;
	if(argc == 2){
		int _arg = _stricmp(argv[1],"/q");
		if (_arg == 0)
			Mode = Traversal;
	}

	string StuID,ClassName;
	BookCodeType BookCode;
	SheetStyleType SheetStyle;

	vector<string> RightAnswer;
	
	WIN32_FIND_DATA FileData;
	HANDLE          hSearch;
	DWORD           dwAttrs;



//	setlocale(LC_ALL,"");
//	wcout calls on wcstombs(),and the character-set is default to be using C,
//	so nothing will be displayed.
//	If you do not want to change global environment, use the one below everytime 
//	you want to change locale instead.
//	std::wcout.imbue(std::locale(locale(), "chs", LC_ALL));


//	Information Input

#ifdef _DEBUG
	StuID="1210000";
	BookCode.Book = "3";
	BookCode.Unit = "4";
	BookCode.Type = "A";
	ClassName = "2012-0009-0010";
	SheetStyle = Listening;

#else
	cout<<"注意：请勿操作过于频繁，半小时内不要刷超过15套题，";
	cout<<"否则服务器会拒收试卷一天（南开的测试结果）"<<endl;
	cout<<"原因来自学校服务器"<<endl;
	cout<<endl<<endl<<endl;
	cout<<"请输入学号："<<endl;
	cin>>StuID;
	cout<<"请输入第几册："<<endl;
	cin>>BookCode.Book;
	cout<<"请输入单元号："<<endl;
	cin>>BookCode.Unit;
	
//	SheetStlye
	cout<<"请输入选项数字：0综合  1听说"<<endl;
	string choice_tmp;
	cin>>choice_tmp;
	if(choice_tmp == string("0"))
		SheetStyle = Integrated;
	else
		SheetStyle = Listening;
	
//	BookCode.Type
	if(SheetStyle == Integrated ){
		srand((unsigned)time(NULL));
		char __type = rand()%6+'A';
		string __temp = "";
		__temp+= __type;
		BookCode.Type = __temp;
	}
	else
		BookCode.Type = "A";
	
//	ClassName
	string _class_name = "";
	cout<<"请输入班号："<<endl;
	cin>>_class_name;
	ClassName = _class_name;

	cout<<endl<<endl<<"请确认以上信息是否正确，正确请输入Y，如不正确，请立即退出"<<endl;
	string _buffer;
	cin>>_buffer;
	if((_buffer != string("Y"))&&(_buffer != string("y")))
		return -1;

#endif
	
	
do{

//	Check if answer txt file exists
	string AnswerText;
	if(SheetStyle == Integrated)
		AnswerText = string(".\\Rebellion\\Integrated\\B")
			+BookCode.Book+"U"+BookCode.Unit+"-"+BookCode.Type+".txt";
	else
		AnswerText = string(".\\Rebellion\\Listening\\B")
			+BookCode.Book+"U"+BookCode.Unit+"-"+BookCode.Type+".txt";
	size_t convertedChars = 0;
	const size_t bufsize = 500;
	wchar_t AnswerTextName[bufsize];
	mbstowcs_s(&convertedChars, AnswerTextName, bufsize, AnswerText.c_str(), _TRUNCATE);

	hSearch = FindFirstFile(AnswerTextName, &FileData);
	if(hSearch == INVALID_HANDLE_VALUE ){                                    //NOT NULL!!! 0xffffffff
		cerr<<"AnswerFileNotPresent"<<AnswerText<<endl;
		system("pause");
		return -1;
	}
	

//	Parse answer txt file
	bool answer_parsed = parse_answer_txt(AnswerText.c_str(), RightAnswer);
	if(!answer_parsed){
		cerr<<"answer_parse_failure:"<<AnswerText.c_str()<<endl;
		system("pause");
		return -1;
	}
	

//	Generate process argument
	string temp = "0";
	if(BookCode.Unit.length() == 1)temp += BookCode.Unit;
	else temp = BookCode.Unit;
	string szProcessArgument;

	if(SheetStyle == Integrated){
		szProcessArgument = string("TestClient.exe unittest ")
		+SchoolServer+" "
		+StuID+" "
		+StuID+" "
		+CoursePrefix
		+BookCode.Book+"/Unit_"+temp+"/Test/B"+BookCode.Book+"U"+BookCode.Unit
		+"-"+BookCode.Type+".qzip "+ClassName;
	}
	else{
		szProcessArgument = string("TestClient.exe unittest ")
		+SchoolServer+" "
		+StuID+" "
		+StuID+" "
		+CoursePrefix_L
		+BookCode.Book+"/Unit_"+temp+"/Test/B"+BookCode.Book+"U"+BookCode.Unit
		+"-"+BookCode.Type+".qzip "+ClassName;
	}

	wchar_t szCmdLine [bufsize];
//	Reuse of bufsize & convertedchars, declared before. No use at all.
//	bufsize == 500, convertedchars == 0
	mbstowcs_s(&convertedChars, szCmdLine, bufsize, szProcessArgument.c_str(), _TRUNCATE);		

	
//	Create process
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	

//	LPTSTR szCmdline = _tcsdup(TEXT("TestClient.exe"));
//	Warning:CreateProcessW can modify the contents of this string. 
//	if this memory is read-only, the function may cause an access violation.
//	Microsoft programmer SUCKS.
	
	if(!CreateProcessW(L"TestClient.exe", szCmdLine,                         //BUG!!!!!!
		NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		cerr<<"Create process Failed"<<endl;
		system("pause");
		return	-1;
	}
	

//	Lock main thread until program starts
	HWND hWnd_start = NULL;
	while(!hWnd_start)
		hWnd_start = FindWindow(NULL,TEXT("设备测试"));

//	Parse paper.xml	
	bool parsed = parse_paper(szPaperFileName ,BookCode );
	if(!parsed){
		cerr<<"Parsing paper failed"<<endl;
		if(!TerminateProcess(pi.hProcess, 0)){
			DWORD i= GetLastError();
			cerr<<"TerminateFailed.Code"<<i<<endl;                              //Impossible, but for safety
			string __errMsg = "ERROR: Parsing paper && Terminating failed\n";
			MessageBoxA(NULL,(LPCSTR)&__errMsg[0],
				(LPCSTR)"ERROR", MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
			system("pause");
			return -1;
		}
		string _errMsg = "ERROR: Parsing paper failed\n";
		MessageBoxA(NULL,(LPCSTR)&_errMsg[0],
		(LPCSTR)"ERROR", MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);

		system("pause");
		return -1;
	}

//	Check and fix File Attribute of answer.xml
	const char* AnswerFile = szAnswerFileName;
	//bufsize == 500, convertedChars declared before
	wchar_t AnswerXMLname[bufsize];
	mbstowcs_s(&convertedChars, AnswerXMLname, bufsize, AnswerFile, _TRUNCATE);

	WIN32_FIND_DATA FileData_xml;
	hSearch = FindFirstFile(AnswerXMLname, &FileData_xml);

//	WIN32_FIND_DATA FileData;
//	HANDLE          hSearch;
//	DWORD           dwAttrs;

	if(hSearch != INVALID_HANDLE_VALUE ){ 
		dwAttrs = GetFileAttributes(AnswerXMLname);
		if (dwAttrs == INVALID_FILE_ATTRIBUTES) {
			cerr<<"Cannot get attribute."<<szAnswerFileName<<endl;
			system("pause");
			return -1; 
		}
		if (dwAttrs & FILE_ATTRIBUTE_READONLY){ 
			if(!SetFileAttributes(TEXT(".\\appdata\\answer\\answer.xml"), FILE_ATTRIBUTE_NORMAL)){
				cerr<<"Set File Attribute Failed."<<szAnswerFileName<<endl;
				system("pause");
				return -1;
			} 
		}
	}
//	Close the search handle. 
	FindClose(hSearch);

//	OBFUSCATION
	vector<string> obfs;
	bool obfs_success = obfuscation(obfs,RightAnswer);
	if(!obfs_success){
		cerr<<"Unknown ERROR"<<endl;

		if(!TerminateProcess(pi.hProcess, 0)){
			DWORD i= GetLastError();
			cerr<<"TerminateFailed.Code"<<i<<endl;                              //Impossible, but for safety
			string __errMsg = "ERROR: answer xml not written && Terminating failed\n";
			MessageBoxA(NULL,(LPCSTR)&__errMsg[0],
				(LPCSTR)"ERROR", MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
			system("pause");
			return -1;
		}
		string _errMsg = "ERROR: Unknown error:obfs";
		MessageBoxA(NULL,(LPCSTR)&_errMsg[0],
		(LPCSTR)"ERROR", MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);

		system("pause");
		return -1;
	}


//	Generate answer.xml
	bool written = gen_answerxml(szAnswerFileName, obfs,
				   SheetStyle,BookCode, StuID);
	if(!written){
		cerr<<"answer xml not written"<<endl;
		if(!TerminateProcess(pi.hProcess, 0)){
			DWORD i= GetLastError();
			cerr<<"TerminateFailed.Code"<<i<<endl;                              //Impossible, but for safety
			string __errMsg = "ERROR: answer xml not written && Terminating failed\n";
			MessageBoxA(NULL,(LPCSTR)&__errMsg[0],
				(LPCSTR)"ERROR", MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
			system("pause");
			return -1;
		}
		string _errMsg = "ERROR: answer xml not written";
		MessageBoxA(NULL,(LPCSTR)&_errMsg[0],
		(LPCSTR)"ERROR", MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);

		system("pause");
		return -1;
	}
	

//	Set to READ-ONLY
	if(!SetFileAttributes(TEXT(".\\appdata\\answer\\answer.xml"), FILE_ATTRIBUTE_READONLY)){
		cerr<<"Critical: Set to Read-only failed"<<endl;
		if(!TerminateProcess(pi.hProcess, 0)){
			DWORD i= GetLastError();
			cerr<<"TerminateFailed.Code"<<i<<endl;                              //Impossible, but for safety
			string __errMsg = "ERROR: Critical: Set to Read-only failed && Terminating failed\n";
			MessageBoxA(NULL,(LPCSTR)&__errMsg[0],
				(LPCSTR)"ERROR", MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
			system("pause");
			return -1;
		}
		string _errMsg = "ERROR: Critical: Set to Read-only failed";
		MessageBoxA(NULL,(LPCSTR)&_errMsg[0],
		(LPCSTR)"ERROR", MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);

		system("pause");
		return -1;
	}
	

//	MSGBOX UI
	if(Mode == Single){
		string _temp;
		if(SheetStyle == Integrated) _temp = "全新第二版综合";
		else
			_temp = "全新第二版听力";
		string ConfirmMessage = string("请确认以下信息：\n")
			+_temp
			+"\n第" + BookCode.Book + "册\n"
			+"第" + BookCode.Unit + "单元\n"
			+"试卷类型："+BookCode.Type;
		int confirm_flag = MessageBoxA(NULL,(LPCSTR)&ConfirmMessage[0],
			(LPCSTR)"确认信息", MB_YESNO|MB_ICONQUESTION|MB_SYSTEMMODAL);
		if(IDYES == confirm_flag){
			cout<<"Yes"<<endl;
		}
		else{
			if(!TerminateProcess(pi.hProcess, 0)){
				DWORD i= GetLastError();
				cerr<<"TerminateFailed.Code"<<i<<endl;                              //Impossible, but for safety
				system("pause");
				return -1;
			}
			WaitForSingleObject( pi.hProcess, INFINITE );
			cout<<"Terminated.User Cancelled"<<endl;
			system("pause");
			return 0;
		}
	}

//	Wait until finish
	HWND hWnd = NULL;
	while(!hWnd)
		hWnd = FindWindow(NULL,TEXT("提示"));
	cout<<"Find Finish symbol"<<endl;
	Sleep(1000);

//	The handle returned by the CreateProcess function has PROCESS_ALL_ACCESS 
//	access to the process object. OpenProcess is needless.
	
	if(!TerminateProcess(pi.hProcess, 0)){
			DWORD i= GetLastError();
			cerr<<"TerminateFailed.Code"<<i<<endl;                              //Impossible, but for safety
			string __errMsg = "ERROR: Terminating failed\n";

//	Continue if user has already exited TestClient manually.
	}

//	TerminateProcess is asynchronous
	WaitForSingleObject( pi.hProcess, INFINITE );
	
	cout<<"Terminated"<<endl;
	cout<<"B"<<BookCode.Book<<"U"<<BookCode.Unit<<"-"<<BookCode.Type<<" complete"<<endl;


	if(Mode == Traversal)
	{
		int BookNumTemp;
	
		stringstream stream;
		stream<<BookCode.Unit;
		stream>>BookNumTemp;
		stream.clear();
	
		BookNumTemp++;
		stream<<BookNumTemp;
		stream>>BookCode.Unit;
		stream.clear();
		
		if(SheetStyle == Integrated && BookNumTemp>8){
			cout<<"Traversal Mode Finished:Integrated"<<endl;
			system("pause");
			return 0;
		}
		if(SheetStyle == Listening && BookNumTemp>14){
			cout<<"Traversal Mode Finished:Listening"<<endl;
			system("pause");
			return 0;
		}
		if(SheetStyle == Integrated){
			srand((unsigned)time(NULL));
			char _type = rand()%6+'A';
			string _temp = "";
			_temp+= _type;
			BookCode.Type = _temp;
		}
		if(SheetStyle == Listening){
			BookCode.Type = "A";
		}		
	}
	
}while(Mode == Traversal);	//back to beginning



	system("pause");

	return 0;
}
