#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
int *res;
void *fib(void *param){
	int num=atoi((char *)param);
	res=(int*)malloc(sizeof(int)*num);
	res[0]=0;
	res[1]=1;
	for(int i=2;i<num;i++)res[i]=res[i-1]+res[i-2];
}
int main(int argc,char *argv[]){
	printf("%d",-9%5); 
	pthread_t tid;
	pthread_attr_t attr;
	if(argc!=2||atoi(argv[1])<0){
		printf("wrong params\n");
		return -1;
	}
	
	pthread_attr_init(&attr);
	pthread_create(&tid,&attr,fib,argv[1]);
	pthread_join(tid,NULL);
	for(int i=0;i<atoi(argv[1]);i++){
		printf("%d ",res[i]);
	}
	printf("\n");
	return 0;
}
