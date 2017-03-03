#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
/*
记录:
虚拟机部分暂时没看懂  2/27
本c4编译器不支持宏定义功能

*/
int token;            // current token
char *src, *old_src;  // pointer to source code string;
int poolsize;         // default size of text/data/stack
int line;             // line number
int *text,            //text segment
    *old_text;        //for dump text sgment
    *stack;
char *data;

int *pc,*bp,*sp,ax,cycle;

// instructions
enum { LEA ,IMM ,JMP ,CALL,JZ  ,JNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PUSH,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
       OPEN,READ,CLOS,PRTF,MALC,MSET,MCMP,EXIT };

int token_val;                // value of current token (mainly for number)
int *current_id,              // current parsed ID
    *symbols;                 // symbol table

// fields of identifier
enum {Token, Hash, Name, Type, Class, Value, BType, BClass, BValue, IdSize};

// tokens and classes (operators last and in precedence order)
enum {
  Num = 128, Fun, Sys, Glo, Loc, Id,
  Char, Else, Enum, If, Int, Return, Sizeof, While,
  Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
};

//标识符,可理解为 变量名
/*
对于语法分析而言，我们并不关心一个变量具体叫什么名字，
而只关心这个变量名代表的唯一标识。例如 int a; 定义了变量a，
而之后的语句 a = 10，我们需要知道这两个 a 指向的是同一个变量。
*/
struct identifier
{
    int token;
    int hash;//该标识符的哈希值
    char *name;//存放标识符本身的字符串，int cnt,cnt就是存在这个里面
    int class;//标识符的类别，如数字，全局变量，局部变量等等
    int type;//类型，是int,char,float等
    int value;//存放标识符的值，如果是函数，则存放函数的地址
    /*
        BXXXX：
        C 语言中标识符可以是全局的也可以是局部的，
        当局部标识符的名字与全局标识符相同时，用作保存全局标识符的信息。
    */
    int Bclass;
    int Bvalue;

};

void next() {
    char *last_pos;
    int hash;
    while( token = *src)
    {
        ++src;
        if(token == '\n') { line++; }//遇到换行符行号+1
        else if (token == "#")//遇到宏定义和include就跳过
        {
            while(*src != 0 && *src != '\n' )
            {
                src++;
            }
        }
        else if( (token>='a' && token<='z') || (token>='A' && token<='Z') || token=='_' )
        {
            //??
            last_pos = src-1;
            hash = token;
            while( (*src>='a' && *src<='z') || (*src>='A' && *src<='Z') || *src=='_' || (*src>='0' && *src<='9') )
            {
                hash = hash*146+*src;//hash函数
                src++;
            }
            current_id = symbols;
            //look for existing identifier,linear search
            while( current_id[Token] )
            {
                if (current_id[Hash] == hash &&
                !memcmp((char *)current_id[Name], last_pos, src - last_pos)) {
                    //found one, return
                    token = current_id[Token];
                    return;
                }
                current_id = current_id + IdSize;
            }

            //store new ID
            current_id[Name] = (int)last_pos;
            current_id[Hash] = hash;
            token = current_id[Token] = Id;

        }
        else if( token >= '0' && token<='9' )
        {
                token_val = token -'0';
            if(token_val > 0)//十进制数,不是以0开头
            {
                while( *src >= '0' && *src <= '9' )
                {
                    token_val = token_val*10 + *src-'0';
                    ++src;
                }
            }else //以0开头
            {
                if(*src=='x' || *src=='X')//十六进制数
                {
                    token=*++src;
                    while( token >= '0' && token<='9' )
                    {
                        token_val = token_val*16 + ()
                    }

                }
                else //八进制数
                {
                    while( *src>='0' && *src<='7')
                    {
                        token_val = token_val*8 + *src- '0';
                        ++src;
                    }
                }

            }
            token = Num;
            return;
        }
        else if( token=='"' || token=='\'' )//字符串
        {
            last_pos = data;
            while( *src != 0 && *src != token )
            {
                token_val = *src++;
                if(token_val=='\\')
                {
                    if(token_val=='n')
                    {
                        token_val = '\n';
                    }

                }
                if(token=='"')//data段是存放字符的
                {
                    *data++ = token_val;
                }
            }
            src++;
            if(token=='"')
            {
                token_val = (int)last_pos;
            }
            else
            {
                token=Num;
            }
            return;
        }
        else if( token=='/' )//注释
        {
            if(*src=='/')
            {
                //连续两个是/，可以判定为注释
                while( *src!=0 && *src!='\n' )
                {
                    ++src;
                }

            }else if( *src == '*' )//多行注释
            {
                while( *src !=0 )//没有到文件尾
                {
                    if( *src == '*' )
                    {
                        src++;
                        if(*src=='/')
                        {
                            break;
                        }
                    }
                    src++;
                }

            }else //该符号是除法
            {
                token=Div;
                return;
            }
        }
        else if (token == '=') {
            // parse '==' and '='
            if (*src == '=') {
                src ++;
                token = Eq;
            } else {
                token = Assign;
            }
            return;
        }
        else if (token == '+') {
            // parse '+' and '++'
            if (*src == '+') {
                src ++;
                token = Inc;
            } else {
                token = Add;
            }
            return;
        }
        else if (token == '-') {
            // parse '-' and '--'
            if (*src == '-') {
                src ++;
                token = Dec;
            } else {
                token = Sub;
            }
            return;
        }
        else if (token == '!') {
            // parse '!='
            if (*src == '=') {
                src++;
                token = Ne;
            }
            return;
        }
        else if (token == '<') {
            // parse '<=', '<<' or '<'
            if (*src == '=') {
                src ++;
                token = Le;
            } else if (*src == '<') {
                src ++;
                token = Shl;
            } else {
                token = Lt;
            }
            return;
        }
        else if (token == '>') {
            // parse '>=', '>>' or '>'
            if (*src == '=') {
                src ++;
                token = Ge;
            } else if (*src == '>') {
                src ++;
                token = Shr;
            } else {
                token = Gt;
            }
            return;
        }
        else if (token == '|') {
            // parse '|' or '||'
            if (*src == '|') {
                src ++;
                token = Lor;
            } else {
                token = Or;
            }
            return;
        }
        else if (token == '&') {
            // parse '&' and '&&'
            if (*src == '&') {
                src ++;
                token = Lan;
            } else {
                token = And;
            }
            return;
        }
        else if (token == '^') {
            token = Xor;
            return;
        }
        else if (token == '%') {
            token = Mod;
            return;
        }
        else if (token == '*') {
            token = Mul;
            return;
        }
        else if (token == '[') {
            token = Brak;
            return;
        }
        else if (token == '?') {
            token = Cond;
            return;
        }
        else if (token == '~' || token == ';' || token == '{' || token == '}' || token == '(' || token == ')' || token == ']' || token == ',' || token == ':') {
            // directly return the character as token;
            return;
        }

    }

    return;
}

void expression(int level) {
    // do nothing
}

void program() {
    next();                  // get next token
    while (token > 0) {
        printf("token is: %c\n", token);
        next();
    }
}

int eval() { // 虚拟机
    int op, *tmp;
    while (1) {
        if (op == IMM)       {ax = *pc++;}                                     // load immediate value to ax
        else if (op == LC)   {ax = *(char *)ax;}                               // load character to ax, address in ax
        else if (op == LI)   {ax = *(int *)ax;}                                // load integer to ax, address in ax
        else if (op == SC)   {ax = *(char *)*sp++ = ax;}                       // save character to address, value in ax, address on stack
        else if (op == SI)   {*(int *)*sp++ = ax;}
        else if (op == PUSH) {*--sp = ax;}
        else if (op == JMP)  {pc = (int*)*pc;}                           // save integer to address, value in ax, address on stack
        else if (op == JZ)   {pc = ax ? pc+1 : (int *)*pc;}
        else if (op == JNZ)  {pc = ax ? (int *)*pc : pc+1;}


    }

    return 0;
}

int main(int argc, char **argv)
{
    int i, fd;

    argc--;
    argv++;

    poolsize = 256 * 1024; // arbitrary size
    line = 1;

    if ((fd = open(*argv, 0)) < 0) {
        printf("could not open(%s)\n", *argv);
        return -1;
    }

    if (!(src = old_src = malloc(poolsize))) {
        printf("could not malloc(%d) for source area\n", poolsize);
        return -1;
    }

    // read the source file
    if ((i = read(fd, src, poolsize-1)) <= 0) {
        printf("read() returned %d\n", i);
        return -1;
    }
    src[i] = 0; // add EOF character
    close(fd);

    // allocate memory for virtual machine
    if (!(text = old_text = malloc(poolsize))) {
        printf("could not malloc(%d) for text area\n", poolsize);
        return -1;
    }
    if (!(data = malloc(poolsize))) {
        printf("could not malloc(%d) for data area\n", poolsize);
        return -1;
    }
    if (!(stack = malloc(poolsize))) {
        printf("could not malloc(%d) for stack area\n", poolsize);
        return -1;
    }

    memset(text, 0, poolsize);
    memset(data, 0, poolsize);
    memset(stack, 0, poolsize);

    bp=sp=(int*)((int)stack+poolsize);
    ax=0;

    program();
    return eval();

}
