#define BUFLEN 80

class Scnner
{
public:
    char ch;
    int linelen = 0;
    int readPos = -1;
    char line[BUFLEN];
    int lineNum = 1;
    int colNUm = 0;
    char lastch = ch;

    char scan(FILE *file){
        if( !file ){
            return -1;
        }
        if(readPos == linelen-1){
            linelen = fread(line,1,BUFLEN,file);
            if(linelen == 0){
                linelen = 1;
                line[0]=-1;
            }
            readPos = -1;
        }
        readPos++;
        ch=line[readPos];
        if( lastch=='\n' ){
            lineNum++;
            colNUm=0;
        }
        if(ch==-1){
            fclose(file);
            file=NULL;
        }else if(ch != '\n'){
            colNUm++;
        }
        lastch=ch;
        return ch;
        
    }

};


enum Tag
{
    ERR,END,ID,             //错误，异常，文件结束标记
    KW_INT,KW_CHAR,KE_VOID,
    KW_EXTERN,
    NUM,CH,STR,             //常量
    ADD,SUB,MUL,DIV,MOD,     //+ - * / %
    INC,DEC,                //自加自减
    GT,GE,LT,LE,EQU,NEQU,   //>,>=,<,<=,==,!=
	AND,OR,
	LPAREN,RPAREN,			//()
	LBRACK,RBRACK,			//[]
	LBRACE,RBRACE,			//{}
	COMMA,COLON,SEMICON,	//逗号，冒号，分号
	ASSIGN,					//赋值=
	KW_IF,KW_ELSE,
	KW_SWITCH,KW_CASE,KW_DEFAULT,
	KW_WHILE,KW_DO,KW_FOR,
	KW_BREAK,KW_CONTINUE,KW_RETURN


};

