#include<stdio.h>
#include<cstdlib>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<string>

#define Max 1000000

struct Vertex{
	int id;
	Vertex* next;
};

Vertex* VA[Max];
bool visited[Max];
int cycle[Max];
int vertexStack[Max*10];
//int visitStack[Max*10];

void initVar(){
	for(int i=0; i<Max; i++){
		VA[i] = NULL;
		visited[i] = false;
		vertexStack[i] = 0;
	}
}

void freeAll(){
	for(int i=0; i<Max; i++){
		while(VA[i] != NULL){
			Vertex* tmp = VA[i];	
			VA[i] = VA[i]->next;
			free(tmp);
		}	
	}	
}
/*
void addEdge(int start, int dst){
	if(start >= Max || dst >= Max){
		printf("vertex id exceeds range!\n");
		exit(-1);
	}
	
	Vertex* vert = (Vertex*)malloc(sizeof(Vertex));
	vert->id = dst;	
	//Vertex* tmp = NULL;
	vert->next = VA[start];
	VA[start] = vert;
}

void addEdge1(int start, int dst){
	if(start >= Max || dst >= Max){
		printf("vertex id exceeds the range!\n");
		exit(-1);
	}
	Vertex* vert = (Vertex*)malloc(sizeof(Vertex));
	vert->id = dst;	
	vert->next = NULL;
	if(NULL == VA[start] || VA[start]->id > dst){	
		//Vertex* tmp = NULL;
		vert->next = VA[start];
		VA[start] = vert;
	}else{
		Vertex* second = VA[start];			
		Vertex* first = second;
		while(NULL != first){
			assert(first->id != dst);
			if(first->id < dst){
				second = first;
				first = first->next;
			}else	
				break;
		}
		vert->next = second->next;
		second->next = vert;
	}
}
*/
// construct graph(adjlist) from edgelist
void constrGraph(char* path){
	FILE* fp = fopen(path, "r");		
	if(NULL == fp){
		printf("%s open error! exit now\n", path);
		exit(-1);
	}
	char tmp[1024];
	char delim[]={' ', '\t', ','};
	while(fgets(tmp, 1024, fp) != NULL){
		//printf("%s\n", path);
		if(tmp[0] == '\n' || tmp[0] == '#')
			continue;
		char* vals = strtok(tmp, delim);
		if(NULL == vals)
			break;
		int start = atoi(vals);
		char* vale = strtok(NULL, delim);
		if(NULL == vale)
			break;
		int end = atoi(vale);
		//printf("%d \t %d\n", start, end);
		addEdge1(start, end);
	}	
	fclose(fp);
}
/*
void show(){
	for(int i=0; i<Max; i++){
		if(VA[i] != NULL && i%100000 == 0){
		//if(VA[i] != NULL){
			printf("id=%d-->\t", i);			
			Vertex* tmp = NULL;
			tmp = VA[i];
			do{
				printf("%d\t", tmp->id);	
				tmp = tmp->next;
			}while(tmp != NULL);
			printf("\n");
		}	
	}
}

static int cycleCount = 0;
void outputStack(int bound){
	printf("cycle %d: ", cycleCount++);
	for(int i=0; i<bound; i++){
		printf("%d\t", cycle[i]);
	}	
	printf("\n");
}
*/
FILE* fpout = NULL;
static unsigned long long cycle_count = 0;
void outputCycle(int bound){
	if(0 == cycle_count % 100000)
		printf("cycle=%llu\tstart=%d\tlen=%d\n", cycle_count, cycle[0], bound-1);
	fprintf(fpout, "cycle=%llu\tlen=%d\t", cycle_count++, bound-1);
	//fprintf(fpout, "cycle %d: ", cycle_count-1);
	for(int i=0; i<bound; i++){
		fprintf(fpout, "%d\t", cycle[i]);
	}	
	fprintf(fpout, "\n");
	fflush(fpout);
}


//int index = 0;
void dfsNonrecursive(int start){
	//avoid count the same cycle many times
	int index = 0;
	int top = 0;
	vertexStack[top] = start;	
	//cycle[index++] = start;
	int tmpvertex = 0;
	while(top >= 0){
		tmpvertex = vertexStack[top];	
		if(!visited[tmpvertex]){
			visited[tmpvertex] = true;
			cycle[index++] = tmpvertex;	
			// get the first out neighbour of tmpvertex
			Vertex* tmp = VA[tmpvertex];		
			while(tmp != NULL){
				//deepFirstSearch(tmp->id, index+1);			
				if(!visited[tmp->id] && tmp->id > start) 
					vertexStack[++top] = tmp->id; 	
				else if(visited[tmp->id]){
					if(tmp->id == start && index > 1){
						cycle[index] = start;	
						outputCycle(index+1);
					}	
				}
				tmp = tmp->next;		
			}
		}else{
			index--;	
			assert(cycle[index] == tmpvertex);
			visited[tmpvertex] = false;		
			top--;	
		}
	}
}
/*
int startVertex = 0;
void deepFirstSearch(int vertexId, int index){
	//avoid count the same cycle many times
	if(vertexId < startVertex)
		return;	
	if(visited[vertexId]){
		if(index > 0 && vertexId == startVertex){
			cycle[index] = vertexId;	
			outputStack(index+1);		
		}
	}else{
		//if(index%100 == 0)
		//	printf("in DFS vertex id=%d \t depth=%d\n", vertexId, index);
		cycle[index] = vertexId;	
		visited[vertexId] = true;
		Vertex* tmp = VA[vertexId];		
		while(tmp != NULL){
			deepFirstSearch(tmp->id, index+1);			
			tmp = tmp->next;		
		}
		visited[vertexId] = false;	
	}
}
*/
void detectCycle(){
	for(int i=0; i<Max; i++){
		if(NULL != VA[i]){	
			//printf("DFS visit vertex %d\n", i);
			memset(visited, false, sizeof(bool)*Max);
			startVertex = i; 
			//vertexStack[0] = i;
			deepFirstSearch(i, 0);	
		}	
	}	
}

void detectCycle1(){
	//for(int i=0; i<Max; i++){
	for(int i=Max-1; i>=0; i--){
		if(NULL != VA[i]){	
			//printf("dfsNonrecursive visit vertex %d\n", i);
			memset(visited, false, sizeof(bool)*Max);
			//startVertex = i; 
			//vertexStack[0] = i;
			dfsNonrecursive(i);	
		}	
	}	
}
int main(int argc, char* argv[]){
	//printf("argc=%d\t argv[0]=%s\n", argc, argv[0]);
	assert(2 == argc);
	initVar();
	printf("begin construct graph\n");
	constrGraph(argv[1]);		
	///printf("cycle detection is started \n");
	printf("adjlist of the graph:\n");
	show();
	//printf("cycle detection result(DFS):\n");
	//detectCycle();
	printf("cycle detection result(Nonrecursive):\n");
	std::string fname = argv[1];
	fname +=".result";
	char name_tmp[11];
	srand(time(NULL));
	sprintf(name_tmp, "%d", rand());
	fname += name_tmp;
	fpout = fopen(fname.c_str(), "w+");
	detectCycle1();	
	fclose(fpout);
	freeAll();
	printf("total cycles count is %llu\n", cycle_count-1);
	return 0;
}
