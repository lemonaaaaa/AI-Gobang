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

struct Node									//���ؿ������ڵ���Ϣ�� 
{
	Node* fa;								//ָ��ýڵ�ĸ��� 
	int tot;								//�ýڵ㼰���ӱ����ʵĴ��� 
	int win;								//�ýڵ㼰����rollout��ʤ���Ĵ��� 
	int depth;								//�ýڵ����������е���� 
	int Map[maxn][maxn];					//�ýڵ��ʾ��������Ϣ 
	double UCB1;							//UCB����ֵ 
	vector<int> best;						//�������ڵ����һ�����ӵ�����: x=best[0], y=best[1] 
	vector<Node> child;						//��ź��ӽڵ� 
	Node()									//�չ��캯�� 
	{
		this->fa=NULL;
		tot=win=0;
		UCB1=0;
		depth=0;
	}
	Node(int Map[maxn][maxn])				//�ô�������̽��й��� 
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
	void getUCB1()								//���ù�ʽ����UCBֵ 
	{
		if(tot==0) UCB1=inf;
		else
		{
			if(this->fa->tot==0) UCB1=(double)win/(double)tot;			//���ڵ㣬������� 
			else
			{
				UCB1=(double)win/(double)tot+C*sqrt(2.0*log((double)(this->fa->tot))/(double)tot);
				if(log((double)(this->fa->tot))/(double)tot<0) printf("ERR");		//������Ϣ 
//				double temp=log((double)(this->fa->tot))/(double)tot;
//				if(temp>=0) UCB1=(double)win/(double)tot+C*sqrt(temp);
//				else UCB1=(double)win/(double)tot-C*sqrt(-temp);
			}
		}
//		cout<<win<<' '<<tot<<' '<<this->fa->tot<<' '<<UCB1<<endl;
	}
};

struct node						//����rollout�д����ʱ������Ϣ�Ľṹ������ 
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

inline unsigned long long getRand(unsigned long long l,unsigned long long r)				//��ȡһ���������� 
{
	unsigned long long x=(unsigned long long)rand()*(unsigned long long)rand();
	x%=(r-l+1);
	return l+x;
}

inline void InitGobang(int Map[maxn][maxn])			//��ʼ������ 
{
	for(int i=0;i<maxn;i++)
	{
		for(int j=0;j<maxn;j++)
		{
			Map[i][j]=0;
		}
	}
}

inline void OutputChess(int Map[maxn][maxn])			//�ڿ���̨������� 
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

inline void LabelMap(int x,int y,int Map[maxn][maxn],int opt)			//������̣��������Ӳ��� 
{
	Map[x][y]=opt;
}

inline bool JudgeIfWin(int Map[maxn][maxn],int opt)						//����rollout���ж�ĳһ���Ƿ��Ѿ�ȡ��ʤ�� 
{
	for(int i=0;i<maxn;i++)								//��ÿ���㿪ʼ�������ҡ����¡��������Ƿ������ 
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

inline bool JudgeIfDraw(int Map[maxn][maxn])				//�ж��Ƿ���� 
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

inline bool ValidDrop(int x,int y,int Map[maxn][maxn])			//�ж����ӵ��Ƿ��ڽ��� 
{
	return x<maxn&&y<maxn&&x>=0&&y>=0&&(!Map[x][y]);
}

//����������Ӳ��Խ������� 
int rollout(Node now,int player)
{ 
	vector<node> temp;
	//���ڴ�����пյ����ӵ㣬���Լӿ������ٶ� 
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
	//�õ����пյ����ӵ� 
	if(temp.empty())
	{
		if(JudgeIfWin(now.Map,player)) return 1;
		else if(JudgeIfWin(now.Map,3-player)) return -1;
		else return 0;
	}
	//������̱�ռ���ˣ�ֱ���ж�ʤ�����ߺ��� 
	if(player==1)
	{
		random_shuffle(temp.begin(),temp.end()); 
		node tt=*temp.begin();
		//���ѡȡ�Ϸ��㼯�е�һ�����������
		temp.erase(temp.begin());
		//���ӵ㼯��ɾ������� 
		LabelMap(tt.x,tt.y,now.Map,1);
		//��������ϵ������ 
		if(JudgeIfWin(now.Map,player)) return 1;
		//����Ѿ�ʤ�����򷵻ؽ�� 
	}
	//�����ǰ���������ӣ� ����һ�� 
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
		//�������岢�ж�ʤ����ƽ�� 
	}
	return 0;
}

void GetAIMove(int &px,int &py,int Map[maxn][maxn])
{
	Node root=Node(Map);
	//�Ե�ǰ������ɸ��ڵ� 
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
	//���ɵ�һ���������Ϣ 
//	int T=500000;
//	while(T--)
	int startT=clock();
	//���ó�������ʱ�� 
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
		//һֱ��UCBֵ���ĵ㣬ֱ����Ҷ�ӽڵ� 
		int ttot=0,twin=0;
		int nowPlayer=(now->depth)&1?2:1;
		if(JudgeIfWin(now->Map,nowPlayer)) twin++,ttot++;
		else if(JudgeIfWin(now->Map,3-nowPlayer)) twin--,ttot++;
		else if(JudgeIfDraw(now->Map)) ttot++;
		//�жϸ�Ҷ�ӽڵ��Ƿ��Ѿ�ʤ������ƽ�� 
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
		//����ǣ�����������չ������ڴ�ռ���˷� 
		if(now->tot==0)
		{
			//�����û�б����ʹ� 
			int nextPlayer=(now->depth)&1?1:2;
			//�ж�rollout��һ����˭���� 
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
			//��һ�η��ʵ�һ���µĵ㲻�ý�����չ
			//ֱ�����ϸ��� 
		}
		else
		{
			//����㱻���ʹ� 
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
			//�½������ĺ�̺Ϸ�״̬����Ϊ���ĺ��ӽڵ� 
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
	//�ҳ���һ��ڵ��з��ʴ������ĵ�
	//��Ϊ���ʴ�������һ�������ŵ� 
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
