#include<iostream>
#include<cstdio>
#include<cstring>
#include<algorithm>
using namespace std;

const int maxn=9;
const double eps=1e-9;

int dx[8]={1,1,0,-1,-1,-1,0,1};
int dy[8]={0,1,1,1,0,-1,-1,-1};

inline void InitGobang(int Map[maxn][maxn])					//��ʼ������ 
{
	for(int i=0;i<maxn;i++)
	{
		for(int j=0;j<maxn;j++)
		{
			Map[i][j]=0;
		}
	}
}

inline bool InMap(int x,int y)
{
	return x>=0&&x<maxn&&y>=0&&y<maxn;
}

inline int IsSame(int x,int y,int color,int Map[maxn][maxn])		//�ж����ӵ���ɫ�������ж�������2�������Ƿ�ͬɫ 
{
	if(!InMap(x,y)) return false;
	return Map[x][y]==color;
}

inline int GetNum(int x,int y,int dir,int Map[maxn][maxn])			//����һ������ͬһ��ɫ���ӵ����� 
{
	x+=dx[dir];y+=dy[dir];
	int col=Map[x][y],sum=0;
	if(!col) return 0;
	while(IsSame(x,y,col,Map)) sum++,x+=dx[dir],y+=dy[dir];
	return sum;
}

inline int Huo4Num(int x,int y,int Map[maxn][maxn])					//����֮����ĵ����� 
{
	int sum=0,col=Map[x][y];
	for(int i=0;i<4;i++)
	{
		int temp=1,j;
		for(j=1;IsSame(x+dx[i]*j,y+dy[i]*j,col,Map);j++) temp++;
		if((!InMap(x+dx[i]*j,y+dy[i]*j)||Map[x+dx[i]*j][y+dy[i]*j])) continue;
		for(j=-1;IsSame(x+dx[i]*j,y+dy[i]*j,col,Map);j--) temp++;
		if((!InMap(x+dx[i]*j,y+dy[i]*j)||Map[x+dx[i]*j][y+dy[i]*j])) continue;
		if(temp==4) sum++;
	}
	return sum;
}

inline int Cheng5Num(int x,int y,int Map[maxn][maxn])				//����֮���������� 
{
	int sum=0,col=Map[x][y];
	for(int i=0;i<8;i++)
	{
		int temp=0,j;
		bool flag=true;
		for(j=1;IsSame(x+dx[i]*j,y+dy[i]*j,col,Map)||flag;j++)
		{
			if(!IsSame(x+dx[i]*j,y+dy[i]*j,col,Map))				//�÷���ĵ�һ����ͬɫ�ĵ㣬�����߽���߶Է����ӻ�ո�
			{
				if(Map[x+dx[i]*j][y+dy[i]*j]) temp-=10;				//�÷���ĵ�һ����ͬɫ�ĵ��ǶԷ�����,û�г����
				flag=false;
			}
			temp++;
		}
		if(!InMap(x+dx[i]*(--j),y+dy[i]*j)) continue;				//�÷���ĵ�һ����ͬɫ�ĵ��ǳ����߽�,û�г����
		for(j=-1;IsSame(x+dx[i]*j,y+dy[i]*j,col,Map);j--) temp++;
		if(temp==4) sum++;
	}
	return sum;
}

inline int Chong4Num(int x,int y,int Map[maxn][maxn])				//����֮����ĵ����� 
{
	return Cheng5Num(x,y,Map)-(Huo4Num(x,y,Map)<<1);
}

inline int Huo3Num(int x,int y,int Map[maxn][maxn])					//����֮����������� 
{
	int col=Map[x][y],sum=0,flag=2;
	for(int i=0;i<4;i++)
	{
		int temp=1,j;
		for(j=1;IsSame(x+dx[i]*j,y+dy[i]*j,col,Map);j++) temp++;
		if((!InMap(x+dx[i]*j,y+dy[i]*j)||Map[x+dx[i]*j][y+dy[i]*j])) continue;
		j++;
		if((!InMap(x+dx[i]*j,y+dy[i]*j)||Map[x+dx[i]*j][y+dy[i]*j])) flag--;
		for(j=-1;IsSame(x+dx[i]*j,y+dy[i]*j,col,Map);j--) temp++;
		if((!InMap(x+dx[i]*j,y+dy[i]*j)||Map[x+dx[i]*j][y+dy[i]*j])) continue;
		j--;
		if((!InMap(x+dx[i]*j,y+dy[i]*j)||Map[x+dx[i]*j][y+dy[i]*j])) flag--;
		if(temp==3&&flag>0) sum++;
	}
	for(int i=0;i<8;i++)
	{
		int temp=0,j;
		bool flag=true;
		for(j=1;IsSame(x+dx[i]*j,y+dy[i]*j,col,Map)||flag;j++)
		{
			if(!IsSame(x+dx[i]*j,y+dy[i]*j,col,Map))
			{
				if(flag&&!Map[x+dx[i]*j][y+dy[i]*j]) temp-=10;
				flag=false;
			}
			temp++;
		}
		if((!InMap(x+dx[i]*j,y+dy[i]*j)||Map[x+dx[i]*j][y+dy[i]*j])) continue;
		if(!Map[x+dx[i]*(--j)][y+dy[i]*j]) continue;
		for(j=1;IsSame(x+dx[i]*j,y+dy[i]*j,col,Map);j++) temp++;
		if(!Map[x+dx[i]*(--j)][y+dy[i]*j]) continue;
		if(temp==3) sum++;
	}
	return sum;
}

inline bool Changlian(int x,int y,int Map[maxn][maxn])			//�Ƿ��г������� 
{
	for(int i=0;i<4;i++)
	{
		if(GetNum(x,y,i,Map)+GetNum(x,y,i+4,Map)>4) return true;
	}
	return false;
}

inline bool IsBan(int x,int y,int Map[maxn][maxn])
{
	Map[x][y]=1;									//����õ����� 
	if(IsSame(x,y,2,Map)) return false;				//�����޽��� 
	return Huo3Num(x,y,Map)>1 || Changlian(x,y,Map) || Huo4Num(x,y,Map)+Chong4Num(x,y,Map)>1;
}

int main()
{
	freopen("chessmap.txt","r",stdin);
	freopen("ban.txt","w",stdout);
	int chessMap[maxn][maxn],px,py;
	scanf("%d%d",&px,&py);
//	printf("%d %d\n",px,py); 
	InitGobang(chessMap); 
	for(int i=0;i<maxn;i++)
		for(int j=0;j<maxn;j++)
			scanf("%d",&chessMap[i][j]);
	for(int i=0;i<maxn;i++)
		for(int j=0;j<maxn;j++)
			if(chessMap[i][j]==1) chessMap[i][j]=2;
			else if(chessMap[i][j]==2) chessMap[i][j]=1;
	if(IsBan(px,py,chessMap)) printf("1\n");
	else printf("0\n");
/*	for(int i=0;i<maxn;i++)
		for(int j=0;j<maxn;j++)
			printf("%d%c",chessMap[i][j],j==maxn-1?'\n':' ');
*/	fclose(stdin);
	fclose(stdout);
	return 0;
}
