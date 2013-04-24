/*
 *strtok函数会破坏被分解字符串的完整，调用前和调用后的s已经不一样了。如果
 *要保持原字符串的完整，可以使用strchr和sscanf的组合等。
 */
//C code
#include <string.h>
#include <stdio.h>
int main(void)
{
    char input[16] = "abc,d";
    char *p;
    /**/ /* strtok places a NULL terminator
            in front of the token, if found */
    p = strtok(input, ",");
    if (p) printf("%s\n", p);
    /**/ /* A second call to strtok using a NULL
            as the first parameter returns a pointer
            to the character following the token */
    p = strtok(NULL, ",");
    if (p) printf("%s\n", p);
    return 0;
}


//C++ code
#include <iostream>
#include <cstring>
using namespace std;
int main()
{
    char sentence[]="This is a sentence with 7 tokens";
    cout<<"The string to be tokenized is:\n"<<sentence<<"\n\nThe tokens are:\n\n";
    char *tokenPtr=strtok(sentence," ");
    while(tokenPtr!=NULL)
    {
        cout<<tokenPtr<<'\n';
        tokenPtr=strtok(NULL," ");
    }
    //cout<<"After strtok, sentence = "<<tokenPtr<<endl;
    return 0;
}
/*
 *函数第一次调用需设置两个参数。第一次分割的结果，返回串中第一个 ',' 之前的字符串,也就是上面的程序第一次输出abc。
 *第二次调用该函数strtok(NULL,","),第一个参数设置为NULL。结果返回分割依据后面的字串，即第二次输出d。
 *strtok是一个线程不安全的函数，因为它使用了静态分配的空间来存储被分割的字符串位置
 *线程安全的函数叫strtok_r,ca
 *运用strtok来判断ip或者mac的时候务必要先用其他的方法判断'.'或':'的个数，因为用strtok截断的话，比如："192..168.0...8..."这个字符串，strtok只会截取四次，中间的...无论多少都会被当作一个key
 */
