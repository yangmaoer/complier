//词法记号类
class Token
{
public:
	Tag tag;
	Token(Tag t);
	virtual ~Token();
	virtual string toString();
};
//标识符记号类
class Id:public Token
{
public:
	string name;
	Id(string n,Tag tag):Token(tag){
		name = n;
	}
	~Id();
	virtual string toString();
};
//数字
class Num :public Token
{
public:
	int val;
	Num(int v,Tag tag):Token(tag){
		val = v;
	}
	virtual string toString();
};
//字符
class Char:public Token
{
public:
	char ch;
	Char(char c, Tag tag){
		
	}
	~Char();
	virtual string toString();
};
//字符串
class Str:public Token
{
public:
	string str;
	Str(string s);
	~Str();
	virtual string toString();	
};
//关键字
class Keywords
{
	//hash函数
	// struct string_hash
	// {
	// 	size_t operator()(const string &str) const{
	// 		return _stl_hash_string(str.c_str());
	// 	}
	// };
	map<string,Tag> keywords;
public:
	Keywords();
	~Keywords();
	Tag getTag(string name);//测试是否是关键字
	
};

Keywords::Keywords()
{
	keywords["int"]=KW_INT;
	keywords["char"]=KW_CHAR;
	keywords["void"]=KW_VOID;
	keywords["extern"]=KW_EXTERN;
	Keywords["if"]=KW_IF;
	keywords["else"]=KW_ELSE;
	keywords["switch"]=KW_SWITCH;
	keywords["case"]=KW_CASE;
	keywords["default"]=KW_DEFAULT;
	keywords["while"]=KW_WHILE;
	keywords["do"]=KW_DO;
	keywords["for"]=KW_FOR;
	keywords["break"]=KW_BREAK;
	keywords["continue"]=KW_CONTINUE;
	keywords["return"]KW_RETURN;
}

Tag Keywords::getTag(string name)
{
	return keywords.find(name)!=keywords.end()?keywords[name]:ID;
}
