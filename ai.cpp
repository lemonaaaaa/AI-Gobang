#include<iostream>
#include<cstdio>
#include<cstring>
#include<cmath>
#include<algorithm>
#include<vector>
#include<ctime>
#include<map>
using namespace std;

const int maxn=9;
const int inf=0x7f7f7f7f;
const double C=1.96;
const double eps=1e-9;

struct Node									//蒙特卡洛树节点信息类 
{
	Node* fa;								//指向该节点的父亲 
	int tot;								//该节点及孩子被访问的次数 
	int win;								//该节点及孩子rollout中胜利的次数 
	int depth;								//该节点在搜索树中的深度 
	int Map[maxn][maxn];					//该节点表示的棋盘信息 
	double UCB1;							//UCB估价值 
	vector<int> best;						//存放这个节点的上一步走子的坐标: x=best[0], y=best[1] 
	vector<Node> child;						//存放孩子节点 
	Node()									//空构造函数 
	{
		this->fa=NULL;
		tot=win=0;
		UCB1=0;
		depth=0;
	}
	Node(int Map[maxn][maxn])				//用传入的棋盘进行构造 
	{
		this->fa=NULL;
		tot=win=0;
		UCB1=0;
		depth=0;
		for(int i=0;i<maxn;i++)
		{
			for(int j=0;j<maxn;j++)
			{
				this->Map[i][j]=Map[i][j];
			}
		}
	}
	void getUCB1()								//利用公式计算UCB值 
	{
		if(tot==0) UCB1=inf;
		else
		{
			if(this->fa->tot==0) UCB1=(double)win/(double)tot;			//根节点，无需计算 
			else
			{
				UCB1=(double)win/(double)tot+C*sqrt(2.0*log((double)(this->fa->tot))/(double)tot);
				if(log((double)(this->fa->tot))/(double)tot<0) printf("ERR");		//调试信息 
//				double temp=log((double)(this->fa->tot))/(double)tot;
//				if(temp>=0) UCB1=(double)win/(double)tot+C*sqrt(temp);
//				else UCB1=(double)win/(double)tot-C*sqrt(-temp);
			}
		}
//		cout<<win<<' '<<tot<<' '<<this->fa->tot<<' '<<UCB1<<endl;
	}
};

struct node						//用于rollout中存放临时坐标信息的结构体类型 
{
	int x,y;
	node(int _x,int _y)
	{
		x=_x;
		y=_y;
	}
//	bool operator < (node xx) const
//	{
//		return x==xx.x?y<xx.y:x<xx.x;
//	}
};

inline unsigned long long getRand(unsigned long long l,unsigned long long r)				//获取一个大的随机数 
{
	unsigned long long x=(unsigned long long)rand()*(unsigned long long)rand();
	x%=(r-l+1);
	return l+x;
}

inline void InitGobang(int Map[maxn][maxn])			//初始化棋盘 
{
	for(int i=0;i<maxn;i++)
	{
		for(int j=0;j<maxn;j++)
		{
			Map[i][j]=0;
		}
	}
}

inline void OutputChess(int Map[maxn][maxn])			//在控制台输出棋盘 
{
	printf("  ");
	for(int i=0;i<maxn;i++) printf("%d ",i+1);
//	printf("Y\n");
	printf("\n"); 
	for(int i=0;i<maxn;i++)
	{
		printf("%d ",i+1);
		for(int j=0;j<maxn;j++)
		{
			if(Map[i][j]==0) printf("_ ");
			if(Map[i][j]==1) printf("x ");
			if(Map[i][j]==2) printf("o ");
		}
		printf("\n");
	}
//	printf("X\n");
}

inline void LabelMap(int x,int y,int Map[maxn][maxn],int opt)			//标记棋盘，进行落子操作 
{
	Map[x][y]=opt;
}

inline bool JudgeIfWin(int Map[maxn][maxn],int opt)						//用于rollout中判断某一方是否已经取得胜利 
{
	for(int i=0;i<maxn;i++)								//从每个点开始，看向右、向下、向右下是否成五连 
	{
		for(int j=0;j<maxn;j++)
		{
			if(Map[i][j]!=opt) continue;
			bool flag;
			if(j+4<maxn)
			{
				flag=true;
				for(int k=j;k<=j+4;k++)
				{
					flag&=Map[i][k]==opt?true:false;
				}
				if(flag) return true;
			}
			if(i+4<maxn)
			{
				flag=true;
				for(int k=i;k<=i+4;k++)
				{
					flag&=Map[k][j]==opt?true:false;
				}
				if(flag) return true;
				flag=true;
				for(int k=1;k<=4;k++)
				{
					flag&=Map[i+k][j+k]==opt?true:false;
				}
				if(flag) return true;
			}
		}
	}
	return false;
}

inline bool JudgeIfDraw(int Map[maxn][maxn])				//判断是否和棋 
{
	for(int i=0;i<maxn;i++)
	{
		for(int j=0;j<maxn;j++)
		{
			if(!Map[i][j]) return false;
		}
	}
	return true;
}

inline bool ValidDrop(int x,int y,int Map[maxn][maxn])			//判断落子点是否在界内 
{
	return x<maxn&&y<maxn&&x>=0&&y>=0&&(!Map[x][y]);
}

//采用随机走子策略进行走子 
int rollout(Node now,int player)
{ 
	vector<node> temp;
	//用于存放所有空的落子点，简略加快搜索速度 
	for(int i=0;i<maxn;i++)
	{
		for(int j=0;j<maxn;j++)
		{
			if(ValidDrop(i,j,now.Map))
			{
				temp.push_back(node(i,j));
			}
		}
	}
	//得到所有空的落子点 
	if(temp.empty())
	{
		if(JudgeIfWin(now.Map,player)) return 1;
		else if(JudgeIfWin(now.Map,3-player)) return -1;
		else return 0;
	}
	//如果棋盘被占满了，直接判断胜负或者和棋 
	if(player==1)
	{
		random_shuffle(temp.begin(),temp.end()); 
		node tt=*temp.begin();
		//随机选取合法点集中的一个点进行走子
		temp.erase(temp.begin());
		//并从点集中删除这个点 
		LabelMap(tt.x,tt.y,now.Map,1);
		//标记棋盘上的这个点 
		if(JudgeIfWin(now.Map,player)) return 1;
		//如果已经胜利，则返回结果 
	}
	//如果当前是先手走子， 先走一步 
	while(true)
	{
		if(temp.empty()) return 0;
		random_shuffle(temp.begin(),temp.end());
		node tt=*temp.begin();
		temp.erase(temp.begin());
		LabelMap(tt.x,tt.y,now.Map,2);
		if(JudgeIfWin(now.Map,player)) return 1;
		if(JudgeIfWin(now.Map,3-player)) return -1;
		
		if(temp.empty()) return 0;
		random_shuffle(temp.begin(),temp.end());
		tt=*temp.begin();
		temp.erase(temp.begin());
		LabelMap(tt.x,tt.y,now.Map,1);
		if(JudgeIfWin(now.Map,player)) return 1;
		if(JudgeIfWin(now.Map,3-player)) return -1;
		//轮流下棋并判断胜负、平局 
	}
	return 0;
}

void GetAIMove(int &px,int &py,int Map[maxn][maxn])
{
	Node root=Node(Map);
	//以当前棋局生成根节点 
	root.depth=0;
	Node* now=&root;
	for(int i=0;i<maxn;i++)
	{
		for(int j=0;j<maxn;j++)
		{
			if(!ValidDrop(i,j,root.Map)) continue;
			Node temp=Node(root.Map);
			temp.best.push_back(i);
			temp.best.push_back(j);
			LabelMap(i,j,temp.Map,2);
			temp.fa=&root;
			temp.depth=1;
			root.child.push_back(temp);
		}
	}
	//生成第一层的棋盘信息 
//	int T=500000;
//	while(T--)
	int startT=clock();
	//设置程序运行时间 
	while(clock()-startT<10000)
	{
		now=&root;
		while(!(now->child.empty()))
		{
			int maxP=0;
			double maxV=-eps;
			for(unsigned i=0;i<now->child.size();i++)
			{
				now->child[i].getUCB1();
				if(now->child[i].UCB1>maxV)
				{
					maxV=now->child[i].UCB1;
					maxP=i;
				}
			}
//			printf("%lf\n",maxV);
			now=&(now->child[maxP]);
		}
		//一直找UCB值最大的点，直到到叶子节点 
		int ttot=0,twin=0;
		int nowPlayer=(now->depth)&1?2:1;
		if(JudgeIfWin(now->Map,nowPlayer)) twin++,ttot++;
		else if(JudgeIfWin(now->Map,3-nowPlayer)) twin--,ttot++;
		else if(JudgeIfDraw(now->Map)) ttot++;
		//判断该叶子节点是否已经胜利或者平局 
		if(ttot)
		{
			now->tot+=ttot;
			now->win+=twin;
			if(now!=&root) now->getUCB1();
			while(now->fa!=NULL)
			{
				Node* temp=now->fa;
				temp->tot+=ttot;
				temp->win-=twin;
				twin=-twin;
				if(temp!=&root) temp->getUCB1();
				now=temp;
			}
			continue;
		}
		//如果是，则不用向下扩展，造成内存空间的浪费 
		if(now->tot==0)
		{
			//这个点没有被访问过 
			int nextPlayer=(now->depth)&1?1:2;
			//判断rollout第一层是谁走子 
			int twin=-rollout(*now,nextPlayer);
			now->win+=twin;
			now->tot+=1;
			if(now!=&root) now->getUCB1();
			while(now->fa!=NULL)
			{
				Node* temp=now->fa;
				temp->tot+=1;
				temp->win-=twin;
				twin=-twin;
				if(temp!=&root) temp->getUCB1();
				now=temp;
			}
			//第一次访问到一个新的点不用进行扩展
			//直接向上更新 
		}
		else
		{
			//这个点被访问过 
			for(int i=0;i<maxn;i++)
			{
				for(int j=0;j<maxn;j++)
				{
					if(!ValidDrop(i,j,now->Map)) continue;
					Node temp=Node(now->Map);
					temp.best.push_back(i);
					temp.best.push_back(j);
					if(now->depth&1) LabelMap(i,j,temp.Map,1);
					else LabelMap(i,j,temp.Map,2);
					temp.fa=now;
					temp.depth=now->depth+1;
					now->child.push_back(temp);
				}
			}
			//新建这个点的后继合法状态，作为它的孩子节点 
			int randP=rand()%now->child.size();
			now->child[randP];
			int nextPlayer=(now->depth)&1?1:2;
			int twin=-rollout(*now,nextPlayer);
			now->win+=twin;
			now->tot+=1;
			if(now!=&root) now->getUCB1();
			while(now->fa!=NULL)
			{
				Node* temp=now->fa;
				temp->tot+=1;
				temp->win-=twin;
				twin=-twin;
				if(temp!=&root) temp->getUCB1();
				now=temp;
			}
		}
	}
	int allMaxV=0;
	for(unsigned i=0;i<root.child.size();i++)
	{
		if(root.child[i].tot>allMaxV)
		{
//			printf("%d %d %d\n",root.child[i].tot,root.child[i].best[0]+1,root.child[i].best[1]+1);
			allMaxV=root.child[i].tot;
			px=root.child[i].best[0];
			py=root.child[i].best[1];
		}
	}
	//找出第一层节点中访问次数最多的点
	//因为访问次数最多的一定是最优的 
}

int main()
{
//	cout<<sizeof(Node);
	freopen("chessmap.txt","r",stdin);
	freopen("chesspos.txt","w",stdout);
	srand((unsigned)time(NULL));
	int chessMap[maxn][maxn],px,py;
	InitGobang(chessMap); 
	for(int i=0;i<maxn;i++)
		for(int j=0;j<maxn;j++)
			scanf("%d",&chessMap[i][j]);
	for(int i=0;i<maxn;i++)
		for(int j=0;j<maxn;j++)
			if(chessMap[i][j]==1) chessMap[i][j]=2;
			else if(chessMap[i][j]==2) chessMap[i][j]=1;
	GetAIMove(px,py,chessMap);
	printf("%d,%d",px,py);
	fclose(stdin);
	fclose(stdout);
	return 0;
}
