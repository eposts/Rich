#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
struct cuboid
{
    int cid;
    int length;
    int width;
} box;
void print(cuboid s)
{
    cout<<s.cid<<" "<<s.length<<" "<<s.width<<endl;
}
bool operator < (const cuboid& lhs,const cuboid& rhs)
{
    if(lhs.cid>rhs.cid)
    {
        return false;//lhs.cid>rhs.cid;
    }
    else if(lhs.cid==rhs.cid&&lhs.length>rhs.length)
    {
        return false;//return lhs.length>rhs.length;
    }
    else if(lhs.cid==rhs.cid&&lhs.length==rhs.length&&lhs.width>rhs.width)
    {
        return false;//return lhs.width>rhs.width;
    }
    return true;
}
bool operator ==( const cuboid& lhs,const cuboid rhs )
{
    if(lhs.cid==rhs.cid&&lhs.length==rhs.length&&lhs.width==rhs.width)
    {
        return true;
    }
    else
        return false;
}
int main()
{
    vector<cuboid> a;
    int num1,num2;
    cin>>num1;
    for(int i=num1; i>0; i--)
    {
        cin>>num2;
        for(int j=num2; j>0; j--)
        {
            cin>>box.cid;
            cin>>box.length;
            cin>>box.width;
            if(box.length<box.width)
            {
                int t=box.length;
                box.length=box.width;
                box.width=t;
            }
            a.push_back(box);
        }
        sort(a.begin(),a.end());
        //È¥ÖØ
        vector<cuboid>::iterator end_unique=unique(a.begin(),a.end());
        //É¾³ýÖØ¸´
        a.erase(end_unique,a.end());
        for_each(a.begin(),a.end(),print);
        a.clear();
    }
    return 0;
}
