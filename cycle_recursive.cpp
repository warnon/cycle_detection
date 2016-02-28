#include<stdio.h>
#include<cstdlib>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<string>
#include<time.h>

#define Max 1000000

struct Vertex{
	int id;
	Vertex* next;
};

Vertex* VA[Max];
bool Visited[Max];
//vertexStack: vertices in a cycle
int vertexStack[Max];
FILE* fpout = NULL;
int Degree[Max];

void initVar(){
	for(int i=0; i<Max; i++){
		VA[i] = NULL;
		Visited[i] = false;
		vertexStack[i] = 0;
		Degree[i] = 0;
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
void addEdge(int start, int end){
	if(start >= Max || end >= Max){
		printf("vertex id exceeds range!\n");
		exit(-1);
	}
	
	Vertex* vert = (Vertex*)malloc(sizeof(Vertex));
	vert->id = end;	
	//Vertex* tmp = NULL;
	vert->next = VA[start];
	VA[start] = vert;
}
int maxvid = 0;
void addEdge1(int start, int dst){
	if(start >= Max || dst >= Max){
		printf("vertex id exceeds the range!\n");
		exit(-1);
	}
	assert(dst<=8297 && start <= 8297);
	int tmpmax = start > dst ? start : dst; 
	maxvid = tmpmax > maxvid ? tmpmax : maxvid;
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
			if(first->id == dst){
				printf("addEdge1 source %d first->id=%d\t dst=%d\n", start, first->id, dst);
				assert(first->id != dst);
					
			}
			if(first->id < dst){
				second = first;
				first = first->next;
			}else	
				break;
		}
		vert->next = second->next;
		second->next = vert;
	}
	/*
	if(172 == start){
		Vertex* tmp = VA[start];	
		printf("souce= %d ", start);
		while(tmp->next != NULL){
			printf("->%d ", tmp->id);	
			tmp = tmp->next;
		}
		printf("\n");
	}
	*/
	Degree[start]++;
	Degree[dst]++;
}
int getMaxDegreeVertex(){
	int vid = 0;
	int deg = -1;
	for(int i=0; i<Max; i++){
		if(Degree[i] > deg){
			vid = i;
			deg = Degree[i];
		}
	}	
	return vid;	
}
// assume the adjlis is sorted in ascending order!
void removeVertex(int vertid){

	for(int i=0; i<Max; i++){
		//if(NULL != VA[i]){
			//remove all inedges of vertid
			if(i != vertid && VA[i] != NULL){
				if(VA[i]->id < vertid){
					Vertex* tmp1 = VA[i];					
					Vertex* tmp2 = tmp1->next;
					while(tmp2 != NULL){
						if(tmp2->id < vertid){
							tmp1 = tmp2;
							tmp2 = tmp2->next;	
						}else if(tmp2->id > vertid){
							break;	
						}else{
							tmp1->next = tmp2->next;
							Degree[i]--;
							free(tmp2);	
							break;
						}
					}
				}else if(VA[i]->id > vertid){
					continue;
				}else{
					Vertex* tmp = VA[i];
					VA[i] = VA[i]->next;	
					Degree[i]--;
					free(tmp);
				}
			}else if(i == vertid){
				//remove all outedges of vertid
				Vertex* tmp = VA[i];	
				while(tmp != NULL){
					VA[i] = tmp->next;	
					Degree[tmp->id]--;
					free(tmp);
					tmp = VA[i];
				}
				Degree[vertid] = 0;
				assert(VA[i] == NULL);
			}
	//	}
	}	
	//Degree[vertid] = 0;
}
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
		//addEdge(start, end);
		//assert(start <= 8297 && end <= 8297);
		addEdge1(start, end);
	}	
	fclose(fp);
}

void show(){
	for(int i=0; i<Max; i++){
		if(VA[i] != NULL){
			///if(172 == i)
			//printf("id=%d--> ", i);			
			//fprintf(fpout,"id=%d--> ", i);			
			Vertex* tmp = NULL;
			tmp = VA[i];
			printf("id=%d--> ", i);			
			do{
				/*
				if(172 == i){
					printf("%d ", tmp->id);	
					assert(tmp->id <= maxvid);	
				}
				*/
				printf("%d ", tmp->id);	
				//fprintf(fpout, "%d ", tmp->id);	
				tmp = tmp->next;
			}while(tmp != NULL);
			//if(172 == i)
			printf("\n");
			//fprintf(fpout, "\n");
			//fflush(fpout);
		}	
	}
}
int max_len = 0;
static unsigned long long cycleCount = 0;
void outputStack(int bound){
	max_len = max_len < bound-1 ? bound-1 : max_len;
	if(cycleCount % 100000 == 0)
	printf("cycle=%llu \t start=%d \t cycle_len=%d\tmax_len=%d\n", cycleCount, vertexStack[0], bound-1, max_len);

	fprintf(fpout, "cycle: %llu\tlen: %d\t", cycleCount++, bound-1);
	for(int i=0; i<bound; i++){
		fprintf(fpout, "%d\t", vertexStack[i]);
	}	
	fprintf(fpout, "\n");
	fflush(fpout);
}

int startVertex = 0;
//int index = 0;
void deepFirstSearch(int vertexId, int index){
	//avoid count the same cycle many times
	if(vertexId < startVertex)
		return;	
	//printf("DFS vid=%d \t index=%d\n", vertexId, index);
	if(Visited[vertexId]){
		if(index > 0 && vertexId == startVertex){
			vertexStack[index] = vertexId;	
			outputStack(index+1);		
		}
	}else{
		vertexStack[index] = vertexId;	
		Visited[vertexId] = true;
		Vertex* tmp = VA[vertexId];		
		while(tmp != NULL){
			deepFirstSearch(tmp->id, index+1);			
			tmp = tmp->next;		
		}
		Visited[vertexId] = false;	
	}
}

void detectCycle(){
	//for(int i=0; i<Max; i++){
	for(int i=Max-1; i>=0; i--){
		if(NULL != VA[i]){	
			memset(Visited, false, sizeof(bool)*Max);
			startVertex = i; 
			//printf("cycle start=%d\n", i);
			//vertexStack[0] = i;
			deepFirstSearch(i, 0);	
		}	
	}	
}
void writeDegree(){
	for(int i=0; i<Max; i++){
		if(VA[i] != NULL){
			//printf("id=%d-->\t", i);			
			fprintf(fpout, "id= %d\t", i);
			int degree = 0;
			Vertex* tmp = NULL;
			tmp = VA[i];
			do{
				degree++;
				//printf("%d\t", tmp->id);	
				tmp = tmp->next;
			}while(tmp != NULL);
			//printf("\n");
			fprintf(fpout, "out_degree= %d\n", degree);
		}	
		fflush(fpout);
	}

}
void checkAdjlist(int* array, int len){
	for(int i=0; i<len; i++){
		for(int j=0; j<Max; j++){
			if(NULL != VA[j]){
				Vertex* tmp = VA[j];	
				while(tmp != NULL){
					if(tmp->id == array[i]){	
						//assert(tmp->id != array[i]);
						printf("source id=%d\ttmp->id=%d\t array_i=%d\n", j, tmp->id, array[i]);
						assert(tmp->id != array[i]);
					}			
					tmp = tmp->next;
				}
			}	
		}		
	}	
}
void orderCheck(){
	for(int i=0; i<Max; i++){
		if(NULL != VA[i]){
			Vertex* tmp = VA[i];	
			while(tmp->next != NULL){
				assert(tmp->id < tmp->next->id);	
				tmp = tmp->next;
			}		
		}
	}
}
int main(int argc, char* argv[]){
	//printf("argc=%d\t argv[0]=%s\n", argc, argv[0]);
	assert(argc >= 2);
	initVar();
	//printf("begin construct graph\n");
	constrGraph(argv[1]);		
	orderCheck();
	printf("maxvid=%d\n", maxvid);
	//printf("cycle detection is started \n");
	//printf("adjlist of the graph:\n");
	//show();
	//printf("cycle detection result:\n");
	std::string fname = argv[1];
	fname += ".remove_result";
	srand(time(NULL));
	//int random = rand();
	char str_rd[11];
	sprintf(str_rd, "%d", rand());
	fname +=str_rd;
	fpout = fopen(fname.c_str(), "w+");
	assert(fpout != NULL);
	//writeDegree();
	int top = 100;
	int* topn = (int*)malloc(sizeof(int)*top);
	int* edges =(int*)malloc(sizeof(int)*top);	
	FILE* fpdeg = fopen((fname+"deg").c_str(), "w+");
	int sumdeg = 0;
	/*
	for(int i=0; i<top; i++){
		int vertexid = getMaxDegreeVertex();
		topn[i] = vertexid;
		edges[i] = Degree[vertexid];
		sumdeg += edges[i];
		fprintf(fpdeg, "id= %d\tdegree= %d\n", vertexid, edges[i]);
		//printf("top=%d\tmaxdegree_vertex id=%d\tdegree=%d\n", i, vertexid, Degree[vertexid]);
		removeVertex(vertexid);
		printf("%d\t", topn[i]);
	}
	fprintf(fpdeg, "sum_deg= %d\n", sumdeg);
	fflush(fpdeg);
	fclose(fpdeg);
	printf("\n");	
	*/	
	//show();
	//checkAdjlist(topn, top);
	free(topn);
	free(edges);
	topn = NULL;
	printf("cycle detection begins\n");
	detectCycle();
	fclose(fpout);
	freeAll();
	return 0;
}
