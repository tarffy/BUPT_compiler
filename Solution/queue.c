#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MAX 10
#typedef INT int
order *fromfile(FILE *fp1){
extern int ordernum,board[9][9];
extern node birthplace;
	int sumx=0,sumy=0,i,tem;
	//遍历一遍得到订单总数 
	while(!feof(fp1)){
		fscanf(fp1,"%d",&ordernum);
		for(i=1;i<=5;i++)fscanf(fp1,"%d",&tem);
	}
	ordernum++;
	printf("the max ordernum is %d\n",ordernum);
	fseek(fp1,0,0);
	//读入订单信息 
	order *orders=(order*)malloc(sizeof(order)*(ordernum+1));
	for(i=0;i<ordernum;i++)orders[i].id=-1;	
		for(i=0;!feof(fp1);i++) {
			fscanf(fp1,"%d%d%d%d%d%d",&orders[i].id,&orders[i].start_time,
				&orders[i].begin.x,&orders[i].begin.y,
				&orders[i].destn.x,&orders[i].destn.y);
				orders[i].begin.mode='b';orders[i].destn.mode='b';
				orders[i].step=abs(orders[i].begin.x-orders[i].destn.x)+
				abs(orders[i].begin.y-orders[i].destn.y);
				//读入并储存订单信息 
				board[orders[i].begin.x][orders[i].begin.y]=1;
				board[orders[i].destn.x][orders[i].destn.y]=2;
				//记录格子类型 用于输出 
			sumx=sumx+orders[i].begin.x+orders[i].destn.x;
			sumy=sumy+orders[i].begin.y+orders[i].destn.y;
		}
		double average1=1.0*sumx/(2*ordernum),average2=1.0*sumy/(2*ordernum);
		int xiaoshu1=(int)(10*average1)%10,xiaoshu2=(int)(10*average2)%10;
		if(xiaoshu1>=xiaoshu2)birthplace.mode='h';else birthplace.mode='s';
		birthplace.x=(int)average1;
		birthplace.y=(int)average2;
		//计算所有订单中心 作为外卖员出生点 
		printf("%lf %lf %d %d %c\n\n",average1,average2,birthplace.x,birthplace.y,birthplace.mode);
		fclose(fp1);
		return orders;
}

void copynode(node *dest,node *src){
	dest->x=src->x;
	dest->y=src->y;
	dest->mode=src->mode;
}
void initdeliver(){
	extern node birthplace;
	extern deliver **deliverpool;
	extern int delivernum;
	deliverpool[delivernum]=(deliver *)malloc(sizeof(deliver));
	deliver *tem=deliverpool[delivernum];
	copynode(&(tem->position),&birthplace);
	printnode(&(tem->position));
	tem->steps=(list *)malloc(sizeof(list));
	tem->steps->head=tem->steps->nil;
	tem->id=delivernum;
	tem->steps->lastfreshtime=0;
}
int isinside(listnode *edge1,listnode *edge2,listnode *destn){
	if(((edge1->dot.x<=destn->dot.x&&destn->dot.x<=edge2->dot.x)||(edge1->dot.x>=destn->dot.x&&destn->dot.x>=edge2->dot.x))
		&&((edge1->dot.y<=destn->dot.y&&destn->dot.y<=edge2->dot.y)||(edge1->dot.y>=destn->dot.y&&destn->dot.y>=edge2->dot.y)))
	return 1;
	else return 0;
}
int isoneside(listnode *edge1,listnode *edge2,listnode *destn){
if(((edge1->dot.x<=destn->dot.x&&destn->dot.x<=edge2->dot.x)||(edge1->dot.x>=destn->dot.x&&destn->dot.x>=edge2->dot.x))
	||((edge1->dot.y<=destn->dot.y&&destn->dot.y<=edge2->dot.y)||(edge1->dot.y>=destn->dot.y&&destn->dot.y>=edge2->dot.y)))
	return 1;
	else return 0;
}
int calculatedis1(listnode *edge1,listnode *edge2){
	int x1=edge1->dot.x,y1=edge1->dot.y,x2=edge2->dot.x,y2=edge2->dot.y;
	return abs(x1-x2+y1-y2);
}
int calculatedis2(listnode *edge1,listnode *edge2,listnode *destn){
	return calculatedis1(edge1,destn)+calculatedis1(edge2,destn);
	}
int calculatedis3(node *edge1,node *edge2){
	return abs(edge1->x-edge2->x+edge1->y-edge2->y);
}
int getmax(deliver *deli,listnode *tem,order *source,int time){
	int a=calculatedis3(&(deli->position),&(tem->dot)),b=source->start_time-time;
	return a>b?a:b;
}
void changesteps(deliver *deli,listnode *savenode,listnode *tokennode,int savedeltdistance,int deltdistance){
	listnode *tem=deli->steps->head;
	int temstep;
	while(tem!=savenode&&tem!=deli->steps->nil){
		temstep=tem->ddl;
		tem=tem->next;
	}
	tem=tem->next;
	tem->ddl=temstep+savedeltdistance;
	tem=tem->next;
	while(tem!=tokennode&&tem!=deli->steps->nil){
		tem->ddl+=savedeltdistance;
		temstep=tem->ddl;
		tem=tem->next;
	}
	tem=tem->next;
	tem->ddl=temstep+savedeltdistance+deltdistance;
	tem=tem->next;
	while(tem!=deli->steps->nil){
		temstep=tem->ddl;
		tem=tem->next;
	}
	deli->steps->lasttime=temstep;
}
void fresh(deliver *deli,int time){
	int delt=time-deli->steps->lastfreshtime,last;
	listnode *tem=deli->steps->head;
	while(tem!=deli->steps->nil){
		tem->ddl-=delt;
		tem=tem->next;
		last=tem->ddl;
	}
	deli->steps->lasttime=last;
	deli->steps->lastfreshtime=time;
}
int insertorder(deliver *deli,order *source,int time){
	fresh(deli,time);
	int token=0,deltdistance=999,i,savedeltdistance;
	listnode *tem=deli->steps->head,*tokennode,*temnode,*savenode;
	listnode *temstart=(listnode*)malloc(sizeof(node)),*temend=(listnode*)malloc(sizeof(node));
//	printf("%d %d %d %d\n",source->begin.x,source->begin.y,source->destn.x,source->destn.y);
//	printf("%d %d %d ")
	copynode(&(temstart->dot),&(source->begin));
	copynode(&(temend->dot),&(source->destn));
	if(tem==deli->steps->nil){
//		printf("insert 1st\n");
		deli->steps->head=temstart;
		temstart->next=temend;
		temend->next=deli->steps->nil;
		temstart->ddl=getmax(deli,temstart,source,time);
		temend->ddl=temstart->ddl+source->step;
		temstart->isbegin=source->start_time;
		temend->isbegin=0;
		return 1;
	}else{
		listnode *tem2=tem->next;
		for(i=0;i<2;i++){
		token=0;
		if(!i){
			temnode=temstart;
		}else{
			temnode=temend;
		}
		while(tem2!=deli->steps->nil){
			if(isinside(tem,tem2,temnode)){
//				printf("isinside%d\n",i);
				token=2;
				tokennode=tem;
				break;
			}else if(isoneside(tem,tem2,temnode)){
				int temdis=calculatedis2(tem,tem2,temnode);
				if(temdis<deltdistance){
					tokennode=tem;
					deltdistance=temdis;
					token=1;
				}
			}
			tem=tem2;
			tem2=tem2->next;
		}
		if(!i){
			if(token==2){
				savenode=tokennode;
				savedeltdistance=0;
				temstart->next=tokennode->next;
			}else if(token==1&&deli->steps->lasttime+deltdistance<=30){
				savenode=tokennode;
				savedeltdistance=deltdistance;
				temstart->next=tokennode->next;
			}else if(!token&&calculatedis3(&(tem->dot),&(source->begin))+source->step+deli->steps->lasttime<=30){ 
			 tem->next=temnode;
			 temnode->next=temend;
				temend->next=tem2;
				return 1;
			}else return 0;
			tem=temstart;
			tem2=temstart->next;
		}else{
			if(token==2){
				savenode->next=temstart;
				listnode *temmnode=tokennode->next;
				tokennode->next=temnode;
				temnode->next=temmnode;
				changesteps(deli,savenode,tokennode,savedeltdistance,deltdistance);
			}else if(token==1&&deli->steps->lasttime+deltdistance+savedeltdistance<=30){
				savenode->next=temstart;
				listnode *temmnode=tokennode->next;
				tokennode->next=temnode;
				temnode->next=temmnode;
				changesteps(deli,savenode,tokennode,savedeltdistance,deltdistance);
			}else if(!token&&calculatedis3(&(tem->dot),&(source->destn))+savedeltdistance<=30){
				tem->next=temend;
				temend->next=tem2;
				return 1;
			}else return 0;
		}	
	}
	}
}
void calculatemax(order *orders){
extern deliver **deliverpool;
extern int delivernum,ordernum,money;
	int i,time,tem,finishnum=0,bottom=0;
	money-=300;
	initdeliver();
	delivernum++;
	for(time=0;finishnum!=ordernum;time++){
		for(tem=bottom;tem<ordernum;tem++){
			for(i=0;i<delivernum;i++){
			if(calculatedis3(&(orders[tem].begin),&(deliverpool[0]->position))+orders[tem].step<=30){
				if(insertorder(deliverpool[i],&(orders[tem]),time)){
					finishnum++;
					if(tem==bottom+1)bottom++;
					break;
				};
			}
		}
		if(i==delivernum&&money>=300){
			money-=300;
			initdeliver();
			delivernum++;
			insertorder(deliverpool[i],&(orders[tem]),time);
		}
		}
	}
	for(i=0;i<delivernum;i++)printdeliver(deliverpool[i]);
}
//以下函数用于测试 
void printorder(order *order_array){//打印所有的订单 
extern int ordernum;
	int i;
	for(i=0;i<ordernum;i++)
	printf("%d %d %d %d %d %d %d\n",order_array[i].id,order_array[i].start_time,
				order_array[i].begin.x,order_array[i].begin.y,
				order_array[i].destn.x,order_array[i].destn.y,
				order_array[i].step);
	printf("\n");
}
void printboard(){//打印棋盘的情况 0表示无 1表示店 2表示收快递的地方 
	extern int board[9][9];
	int i,j;
	for(i=0;i<9;i++){
		for(j=0;j<9;j++){
			printf("%d ",board[i][j]);
		}
		printf("\n");
	}
}
void printnode(node *a){
	printf("%d %d %c\n",a->x,a->y,a->mode);
}
void printdeliver(deliver *a){
	printf("deliver %d position now:%d %d %c\n",a->id,a->position.x,a->position.y,a->position.mode);
	listnode *tem=a->steps->head;
	printf("deliver steps:\n");
	while(tem!=a->steps->nil){
		printf("%d ",tem->ddl); 
		printnode(&(tem->dot));
		tem=tem->next;
	}
}
