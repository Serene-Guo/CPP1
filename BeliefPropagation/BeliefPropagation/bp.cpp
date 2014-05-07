#include <iostream>
#include <vector>

using namespace std;


	double edgePotential[2][2]={1,0.5,0.5,1};
	//cout<<edgePotential[0][1]<<endl;
	double nodePotential[7][2]={0,0,0.7,0.3,0.1,0.9,0.7,0.3,0.1,0.9,0.7,0.3,0.1,0.9};
	double message[7][7][2];
	vector<vector<int>> tree(7, vector<int>(0) ); // �����Ƕ� vector ��ʼ���������ܸ�vector ��ֵ��
	 
	/*int array[2]={2,3};
	vector<int> temp(array,array+2); 
	tree[1]=temp;*/
	 

	
void calculateMarginals(double marginals[][2],int numNodes)
{

	int v;
	for(int i=1;i<=numNodes;i++)
	{
		for(int j=0;j<=1;j++)
		{// calculate the marginal of p(i=0) p(i=1)
			// cout<<"nodePotential("<<i<<"="<<j<<") : "<<nodePotential[i][j]<<endl;
			marginals[i][j]=nodePotential[i][j];
			//cout<<"initial marginals("<<i<<"="<<j<<") : "<<marginals[i][j]<<endl;
			 
			//for(int m=1;m<=numNodes;m++)
			//	{   /* if(message[m][i][j]>0)   //  �ж�������ʵ ������ôд
			//	 marginals[i][j]=marginals[i][j]*message[m][i][j];
		 //    	cout<<message[m][i][j]<<"  ";*/ 

			//}
			for(int k=0;k<tree[i].size();k++) 
		{  // visit all the neighbours of i.
			v=tree[i][k];
			marginals[i][j]=marginals[i][j]*message[v][i][j];
		//	cout<<"after * message frome"<<v<<": marginals("<<i<<"="<<j<<") : "<<marginals[i][j]<<endl;
			}
		//	cout<<"marginals("<<i<<"="<<j<<") : "<<marginals[i][j]<<endl;
			 
			
		}
	}
}
	


void calculateMessage(int from, int to)
{  
	double h[2];
	int deep;
	for(deep=0;deep<2;deep++)
	{	// ����������ֵ�µ� h��xu��
		h[deep]=nodePotential[from][deep];
		//for(int i=1;i<=6;i++)
		//{// �� ����parent֮��!!!!!!!!!!!!������neighbour ���� u��message �˻�
		//	if(i!=to&&message[i][from][deep]>0)
		//		h[deep]=h[deep]*message[i][from][deep];

		//}
		int v; // all neighbours
		for(int i=0;i<tree[from].size();i++) 
		{  // visit all the neighbours of u.
			v=tree[from][i];
			if(v!=to)
		 		h[deep]=h[deep]*message[v][from][deep];
		}

		//cout<<"h "<<h[deep]<<" ";
	}
	//cout<<endl;

	int q;
	
	  cout<<"before normalizing:****************"<<endl;
	for(deep=0;deep<2;deep++)
	{ //message u->par[u] when par[u]=p 0,1 
		 
		for(q=0;q<2;q++) // q ��u�� ����ȡֵ
			message[from][to][deep]+=edgePotential[deep][q]*h[q];
	     cout<<"message "<<from<<"->" <<to<<"("<<deep<<")="<<message[from][to][deep]<<endl;

	}
	
// normalize the message
	double denominator=message[from][to][0]+message[from][to][1];
	message[from][to][0]=message[from][to][0]/denominator;
	message[from][to][1]=message[from][to][1]/denominator;
	cout<<"after normalizing:*******************"<<endl;
	cout<<"message "<<from<<"->" <<to<<"(0)="<<message[from][to][0]<<endl;
	cout<<"message "<<from<<"->" <<to<<"(1)="<<message[from][to][1]<<endl;

}


void FirstDFS(int u,bool visit[],int par[])
{// ������ֵ����������//// ������ ���� ����������Ϊȫ�ֱ���
	// ��һ�� DFS ������uΪroot�� ���� down2up�� message
	visit[u]=true;
	int v;
	 
	for(int i=0;i<tree[u].size();i++) 
	{// visit all the neighbours of u.
		v=tree[u][i];
		 
		if(!visit[v])
			{

				par[v]=u;
				FirstDFS(v,visit,par);
			}

	}
	 
	 //calculate the message from u->par(u)
	
	int parent=par[u]; 
	if(parent==0)
		return; ////// ���� ���ڵ�  1�� par[u]=0;;;��ʼֵ
	else
		{	
			calculateMessage(u,parent);
			cout<<"calculate "<<u<<"->"<<parent<<endl;
		}
	 

}


void SecondDFS(int u,bool visit[])
{// up2down calculate 
	// visit[] �����Բ�������ȥ���ݹ������ ��Ҫ�õ� visit[], ���� ����Ϊ ȫ�ֱ�������main֮��
	visit[u]=true;
	int v;  
	 
	for(int i=0;i<tree[u].size();i++) 
	{// visit all the neighbours of u.
		v=tree[u][i];
		if(!visit[v])
			{
				 //calculate the message u->v
				cout<<"calculate "<<u<<"->"<<v<<endl;
				calculateMessage(u,v);

				SecondDFS(v,visit);
			}

	}
}


void printMarginals(double marginals[][2],int numNodes)
{
	for(int i=1;i<=numNodes;i++)
	{
		for(int j=0;j<=1;j++)
		 // calculate the marginal of p(i=0) p(i=1)
			cout<<"p("<<i<<"="<<j<<")="<<marginals[i][j]<<"  ";
		cout<<endl;
	}
}

void printMessage()
{
	for(int d=0;d<=1;d++)
	{

	}
}
void BP(int root,int numberNodes)
{ 
	
	int i=1;
	 
	bool visit[7];
	int par[7];
	for(i=1;i<=6;i++)
		{
		visit[i]=false;
		par[i]=0;
		}
	 
	FirstDFS(root,visit,par);

	for(i=1;i<=6;i++)
	  visit[i]=false;
	 
	SecondDFS(root,visit);

	printMessage();

	double marginals[7][2];
	calculateMarginals(marginals,6);
	printMarginals(marginals,6);
}


void main()
{
	tree[1].push_back(2);
	tree[1].push_back(3);
	tree[2].push_back(1);
	tree[2].push_back(4);
	tree[2].push_back(5);
	tree[3].push_back(1);
	tree[3].push_back(6);
	tree[4].push_back(2);
	tree[5].push_back(2);
	tree[6].push_back(3);
	 
	int root=2;
	int numberNodes=6;
	BP(root,6);
	
	getchar();
}







