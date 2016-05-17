#include<iostream>
#include<cstring>
#include<unordered_map>
#include<vector>
#include<fstream>
#include<unordered_set>
using namespace std;
#define NUM 6000   //���еĹ켣�����Ŀ 
#define LEN 3.0
//�ָ��ַ���
vector<string> split(const string& src, string separate_character)
{
	vector<string> strs;
	int separate_characterLen = separate_character.size();//�ָ��ַ������ȣ������Ϳ���֧�ֶ��� �����ķָ���  
	int last_position = 0, index = -1;
	while (-1 != (index = src.find(separate_character, last_position)))
	{
		strs.push_back(src.substr(last_position, index - last_position));
		last_position = index + separate_characterLen;
	}
	string lastString = src.substr(last_position);
	if (!lastString.empty())
		strs.push_back(lastString);
	return strs;
}
struct Rect
{
	float boundary[4]; /* xmin,ymin,...,xmax,ymax,... */
};
int  RTreeDistance( Rect R,  Rect S)  //�����Ƿ�����Ҫ�� 
{
	struct Rect *r = &R, *s = &S;
	float A[4][2];
	float B[4][2];
	A[0][0] = r->boundary[0];
	A[0][1] = r->boundary[1];

	A[1][0] = r->boundary[2];
	A[1][1] = r->boundary[3];

	A[2][0] = r->boundary[0];
	A[2][1] = r->boundary[3];

	A[3][0] = r->boundary[2];
	A[3][1] = r->boundary[1];

	B[0][0] = s->boundary[0];
	B[0][1] = s->boundary[1];

	B[1][0] = s->boundary[2];
	B[1][1] = s->boundary[3];

	B[2][0] = s->boundary[0];
	B[2][1] = s->boundary[3];

	B[3][0] = s->boundary[2];
	B[3][1] = s->boundary[1];
	float max = ((A[0][0] - B[0][0])*(A[0][0] - B[0][0]) + (A[0][1] - B[0][1])*(A[0][1] - B[0][1]));
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			float x = ((A[i][0] - B[j][0])*(A[i][0] - B[j][0]) + (A[i][1] - B[j][1])*(A[i][1] - B[j][1]));
			
			if (x > LEN*LEN)
			{
				return 0;
			}
		}
	}
	return 1;
}
void Cal(char*file1,char* file2)
{
	unordered_map<int,unordered_set<int>>mymap;
	ifstream fin(file1);
	for(int i=0;i<NUM;i++)
		mymap.insert(pair<int,unordered_set<int>>(i,unordered_set<int>())); 
		
	string s = "";
	int line = 0;
	int nodeNum=INT_MIN;
	//����label
	while (!fin.eof())
	{
		vector<string>vec;	
		getline(fin, s);//getline ����string	
		vec=split(s,",");
		for(int i=0;i<vec.size();i++)
		{
			if(atoi(vec[i].c_str())>nodeNum) //Ѱ�ҽڵ������� 
			{
				nodeNum=atoi(vec[i].c_str()); 
			}
			unordered_set<int>*tempset=&mymap[atoi(vec[i].c_str())];
			for(int j=0;j<vec.size();j++)
			{
				if(vec[i]!=vec[j])
				{
					tempset->insert(atoi(vec[j].c_str()));
				}
			}
		}
		line++;
	}
	fin.close();
	//ͨ��R�� ��ӡ�õ������еĽ��Ƶ�·�� 
	for(int i=0;i<=nodeNum;i++)
	{
		cout<<i<<" ����: "; 
		for (const int& x: mymap[i]) 
		{
			cout << " " << x;
		}
		cout<<endl;
	}
	//Ȼ����bitmap ��ȷ�������еĽ���·�� 
	int **F=new int*[nodeNum+1];
	for(int i=0;i<nodeNum+1;i++)
	{
		F[i]=new int[nodeNum+1];
	}
	//�������������� 0 û�м��� ����-1 ������ ����1 
	
	//��ʼ��-1
	for(int i=0;i<nodeNum+1;i++)
	{
		for(int j=0;j<nodeNum+1;j++)
			F[i][j]=-1;	
	} 
	//��ȡ·�������� 
	unordered_map<int,Rect>mylocation;
	int tnum=0;
	fin.open(file2);
	while (!fin.eof())
	{
		vector<string>vec;	
		getline(fin, s);//getline ����string	
		vec=split(s,",");
		if(vec.size()>0)
		{
			Rect rect;
			rect.boundary[0]=atof(vec[0].c_str());
			rect.boundary[1]=atof(vec[1].c_str());
			rect.boundary[2]=atof(vec[2].c_str());
			rect.boundary[3]=atof(vec[3].c_str());
			mylocation.insert(pair<int,Rect>(tnum,rect));
			tnum++;
		}
	
    }
    fin.close();
    //��ʼ����
	for(int i=0;i<=nodeNum;i++)
	{
		//cout<<mylocation[i].boundary[0]<<" " <<mylocation[i].boundary[1]<<" " <<mylocation[i].boundary[2]<<" " <<mylocation[i].boundary[3]<<" " ;
		//cout<<endl;
		for (const int& x: mymap[i]) 
		{
			if(F[i][x]==-1||F[x][i]==-1)//û�м����
			{
				F[i][x]=F[x][i]=RTreeDistance(mylocation[i],mylocation[x]);
			} 
		}
	} 
    //���ݼ������յĽ��
	for(int i=0;i<=nodeNum;i++)
	{
		cout<<i<<" ����·����";
		for(int j=0;j<=nodeNum;j++)
		{
			if(F[i][j]==1)
			{
				cout<<" "<<j;
			}
		}
		cout<<endl;
	}	 
    
 } 
int main()
{
	 
	Cal("1.txt","input.txt");
	return 0;
}
