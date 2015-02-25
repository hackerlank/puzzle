#include <iostream>
using namespace std;

#include "bitmap.h"

typedef struct _Ret
{
	float p;
	int d;
	int x;
	int y;
} Ret;

void show(const unsigned char *p)
{
	cout << (int)p[0] << " " << (int)p[1] << " " << (int)p[2] << endl;
}


float calc(const unsigned char* tt[], const unsigned char* kk[], int n)
{
	float score = 0;
	for(int c = 0; c < 3; c++)
	{
		for(int i = 0; i < n; i++)
		{
			float d = (tt[i][c] - kk[i][c])/100;
			if(d > 0)
				score += d;
			else
				score -= d;

			for(int j = i + 1; j < n; j++)
			{
				float k = ((float)tt[i][c]/tt[j][c])/((float)kk[i][c]/kk[j][c]) - 1;
				if(k > 0)
					score += k;
				else
					score -= k;

			}
		}
	}
	return score;
}

int main(int argc, char* argv[])
{
	cout << "test" << endl;

	Bitmap p = Bitmap::bitmapFromFile("res/all.png");
	cout << "pic: " << p.width() << " " << p.height() << " " << p.format() << endl;

	Bitmap t = Bitmap::bitmapFromFile("res/2.png");
	cout << "pic: " << t.width() << " " << t.height() << " " << t.format() << endl;

	
	int dir[4][4] = {
		{0, 1, 1, 1},
		{1, 1, 0, -1},
		{0, -1, 1, -1},
		{1, -1, 0, 1}
	};

	const unsigned char* kk[49];

	//int pos[13][2] = {
	//	{ 7, 23},
	//	{ 18, 14},
	//	{ 24, 15},
	//	{ 28, 7},
	//	{ 35, 15},
	//	{ 25, 20},
	//	{ 27, 28},
	//	{ 34, 29},
	//	{ 32, 37},
	//	{ 16, 38},
	//	{ 21, 29},
	//	{ 12, 25},
	//	{ 15, 18}
	//};
	int pos[13][2] = {
		{ 16, 12},
		{ 29, 12},
		{ 37, 12},
		{ 36, 18},
		{ 28, 18},
		{ 20, 19},
		{  8, 25},
		{ 16, 25},
		{ 23, 25},
		{ 16, 35},
		{ 22, 34},
		{ 34, 35},
		{ 27, 42}
	};

	int posN = 13;
	for(int i = 0; i < 13; i++)
	{
		kk[i] = t.getPixel(pos[i][0], pos[i][1]);
		pos[i][0] -= 25;
		pos[i][1] -= 25;
	}
	
	
	
	//int pos[49][2];
	//int posN = 0;
	//for(int i = 0; i < 7; i++)
	//{
	//	for(int j = 0; j < 7; j++)
	//	{
	//		int tx = 10 + i * 5;
	//		int ty = 10 + j * 5;
	//		const unsigned char* dd = t.getPixel(tx, ty);
	//		int avg = (dd[0] + dd[1] + dd[2]) / 3;
	//		int dif1 = abs(dd[0] - dd[1]);
	//		int dif2 = abs(dd[0] - dd[2]);
	//		int dif3 = abs(dd[1] - dd[2]);
	//		int pif1 = abs(dif1 - dif2);
	//		int pif2 = abs(dif1 - dif3);
	//		int pif3 = abs(dif2 - dif3);
	//		//if(avg > 30 && avg < 200 && (pif1 > 50 || pif2 > 50 || pif3 > 50))
	//		{
	//			kk[posN] = dd;
	//			pos[posN][0] = tx - 25;
	//			pos[posN][1] = ty - 25;
	//			posN++;
	//		}
	//	}
	//}
	//cout << "posN:  " << posN << endl;


	Ret ret[50];
	for(int i = 0; i < 50; i++)
		ret[i].p = 99999999;

	const unsigned char* tt[49];
	for(float s = 0.9; s < 1.1; s+=0.05)
	{
		for(int y = 25; y < p.height() - 25; y+=3)
		{
			for(int x = 25; x < p.width() - 25; x+=3)
			{
				for(int d = 0; d < 4; d++)
				{
					for(int i = 0; i < posN; i++)
						tt[i] = p.getPixel(x + pos[i][dir[d][0]]*dir[d][1]*s, y + pos[i][dir[d][2]]*dir[d][3]*s);
					float score = calc(tt, kk, posN);

					for(int m = 0; m < 50; m++)
					{
						if(score < ret[m].p)
						{
							for(int n = 50 - 1; n > m; n--)
								ret[n] = ret[n-1];
							ret[m].p = score;
							ret[m].d = d;
							ret[m].x = x;
							ret[m].y = y;
							break;
						}
					}
				}
			}
		}
	}
	
	for(int i = 0; i < 50; i++)
		cout << "[" << ret[i].p << "] " << ret[i].x << " " << ret[i].y << "   " << ret[i].d << endl;

	return 0;
}
