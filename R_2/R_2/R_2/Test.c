#include "index.h"
#include <stdio.h>
#include <math.h>
#include<string.h>
#include "card.h"
FILE* fp;
int MySearchCallback(int id, void* arg) 
{
	// Note: -1 to make up for the +1 when data was inserted
	printf("Hit data rect %d\n", id-1);
	return 1; // keep going
}
float RTreeDistance(struct Rect R, struct Rect S)
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
	float min = ((A[0][0] - B[0][0])*(A[0][0] - B[0][0]) + (A[0][1] - B[0][1])*(A[0][1] - B[0][1]));
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			float x = ((A[i][0] - B[j][0])*(A[i][0] - B[j][0]) + (A[i][1] - B[j][1])*(A[i][1] - B[j][1]));
			
			if (x < min)
			{
				min = x;
			}
		}
	}
	return sqrt(min);
}
void RTreePrint2(struct Node *N)
{
    struct Node *n = N;
	if (n->level > 0) /* this is an internal node in the tree */
	{
		for (int i = 0; i < NODECARD; i++)
		{
			if (n->branch[i].child)
			{
				RTreePrint2(n->branch[i].child);
			}
		}
			
	}
	else /* this is a leaf node */
	{
		for (int i = 0; i<LEAFCARD; i++)
			if (n->branch[i].child)
			{
				fprintf(fp, "%d,", (int)n->branch[i].child - 1);
				printf("RTree:%d\n", (int)n->branch[i].child - 1);
			}
	}
}

void  RTreeSearch1(struct Branch*N1,struct Branch *N2,float len)
{
	 struct Branch *n1 = N1;
	 struct Branch *n2 = N2; // NOTE: Suspected bug was R sent in as Node* and cast to Rect* here. Fix not yet tested.
	if (n1->child->level> 0 && n2->child->level> 0)
	{
		float m = RTreeDistance(n1->rect, n2->rect);
		if (m < len)  //多出四个方向可以搜索
		{
			//四个Branch  //if 判断这条branch是否存在
			if (n1->child->branch[0].child&&n2->child->branch[1].child)
			{
				RTreeSearch1(&n1->child->branch[0], &n2->child->branch[1], len);
			}
			if (n1->child->branch[0].child&&n2->child->branch[0].child)
			{
				RTreeSearch1(&n1->child->branch[0], &n2->child->branch[0], len);
			}
			if (n1->child->branch[1].child&&n2->child->branch[0].child)
			{
				RTreeSearch1(&n1->child->branch[1], &n2->child->branch[0], len);
			}
			if (n1->child->branch[1].child&&n2->child->branch[1].child)
			{
				RTreeSearch1(&n1->child->branch[1], &n2->child->branch[1], len);
			}
		}
		//2个方向的搜索
		if (n1->child->branch[0].child&&n1->child->branch[1].child)
		{
			RTreeSearch1(&n1->child->branch[0], &n1->child->branch[1], len);
		}
		else if (n1->child->count == 1)
		{
			//printf("id:%d,%d\n", (int)n1->child->branch[0].child->branch[0].child,(int)n1->child->branch[0].child->branch[1].child);
			//struct Rect x1 = n1->child->branch[0].child->branch[1].rect; 错误的寻址
			//n1->child->branch[0].child->branch[0].child->level = -1;  错误的寻址
			//int x3 = n1->child->branch[0].child->level; 正确的寻址	
			//到达叶子节点
			printf("相似路径l----------begin\n");
			RTreePrint2(n1->child->branch[0].child);
			fprintf(fp, "\n");
			printf("相似路径l----------end\n");
		}
		if (n2->child->branch[0].child&&n2->child->branch[1].child)
		{
			RTreeSearch1(&n2->child->branch[0], &n2->child->branch[1], len);
		}
		else if (n2->child->count == 1)
		{
			//到达叶子节点
			printf("相似路径r----------begin\n");
			RTreePrint2(n2->child->branch[0].child);
			fprintf(fp, "\n");
			printf("相似路径r----------end\n");
		}
	}
	else
	{
		float m = RTreeDistance(n1->rect, n2->rect);
		if (m < len)
		{
			printf("相似路径a----------begin\n");
			RTreePrint2(n1->child);
			printf("相似路径a----------next\n");
			RTreePrint2(n2->child);
			fprintf(fp, "\n");
			printf("相似路径a----------end\n");
		}
	}
}
struct Rect rects[] = {
	{ 0, 0, 2, 2 }, // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
	{ 8, 5, 9, 6 },
	{ 7, 1, 9, 2 },
	{ 8,4,9,5 },
	{ 0,0,1,1 },
	{9,6,10,7},
	{1,1,2,2}, 
	{20,20,30,30}
};
int nrects = sizeof(rects) / sizeof(rects[0]);
struct Rect search_rect = {
	{ 0, 0, 10, 10 }, // search will find above rects that this one overlaps
};
int main()
{
	fp = fopen("1.txt", "w");
	struct Node* root = RTreeNewIndex();
	int nhits;
	printf("nrects = %d\n", nrects);
	/*
	 * Insert all the data rects.
	 * Notes about the arguments:
	 * parameter 1 is the rect being inserted,
	 * parameter 2 is its ID. NOTE: *** ID MUST NEVER BE ZERO ***, hence the +1,
	 * parameter 3 is the root of the tree. Note: its address is passed
	 * because it can change as a result of this call, therefore no other parts
	 * of this code should stash its address since it could change undernieth.
	 * parameter 4 is always zero which means to add from the root.
	 */
	//读入文件
	char filename[] = "input.txt";
	FILE *fin;
	char StrLine[1024];             //每行最大读取的字符数
	if ((fin = fopen(filename, "r")) == NULL) //判断文件是否存在及可读
	{
		printf("error!");
		return -1;
	}
	int num = 0;
	int num2 = 0;
	struct Rect file_rect[6000];
	while (!feof(fin))
	{

		fscanf(fin, "%f", &file_rect[num].boundary[num2]);
		num2++;
		char c;
		fscanf(fin, "%c", &c);
		if (c == '\n')
		{
			num++;
			num2 = 0;
		}
		//fgets(StrLine, 1024, fin);  //读取一行
		// printf("%s", StrLine); //输出
	}
	fclose(fin);                     //关闭文件

	for(int i=0; i<=num; i++)
		RTreeInsertRect(&file_rect[i], i+1, &root, 0); // i+1 is rect ID. Note: root can change
	//nhits = RTreeSearch(root, &search_rect, MySearchCallback, 0);
	//printf("Search resulted in %d hits\n", nhits);

	//float d=RTreeDistance(rects[0], rects[4]);
	//printf("dmin:%f\n", d);
	//RTreePrintNode(root, 0);
	if (root->branch[0].child&&root->branch[1].child)
		RTreeSearch1(&root->branch[0], &root->branch[1],3);

	fclose(fp);
	return 0;
}