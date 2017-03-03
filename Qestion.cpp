1.为什么要使用hash_map?
hash_map 在查找效率上比map要高，但是比较耗费内存，属于用空间换时间的例子
在此编译系统中，关键字数量很少，使用map 应该更好

2.子类构造函数的写法
class stu
{
    stu(int id,string name){}
}
class UPCstu
{
    UPCstu(int id,string name,int bz):stu(id,name){}
};
创建新的对象
stu s(a,b);
stu *s = new stu(a,b);

3.如何从文件中读取源码
//如果直接读取整个文件，可能需要的内存太大
使用输入流 ifstream  -->需包含 <fstream> <iostream>
char buf[100];
cin>>buf;
1.cin的输入机制是通过寻找空格来分割输入的。
另外，如果连续输入的字符串长度超过buf的存储空间，就会发生溢出
2.get(),getline(),<string>里的 getline()区别
c++编程思想 P542
3.读写原始字符，使用read()和write()

输入输出流比studio安全
