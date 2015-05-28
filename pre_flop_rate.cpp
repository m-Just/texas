#include"Card.h"
#include<stdio.h>
#include<algorithm>
#include<cstdlib>
using namespace std;

const int c_50_5 = 2118760;
const int c_52_5 = 2598960;
double win[10][52][52], draw[10][52][52];

int toat = 0;

void pre_dfs(bool flag[][14], Card public_card[], const int &public_card_number)
{
	if (public_card_number > 5)
	{
		printf("warning! public_card_number too large!\n");
		exit(0);
	}
	if (public_card_number == 5)
	{
		toat++;
		if (toat % 100 == 0)printf("%lf%\n", (double)toat / c_52_5);
		Card7 c7;
		static Card7 c7_in_order[3000];
		int cnt = 0;

		for (int i = 0; i < 5; i++) c7.card[i] = public_card[i];
		for (int i = 0; i < 52; i++)
		{
			c7.card[5] = int2card(i);
			if (flag[c7.card[5].color][c7.card[5].val]) continue;
			for (int j = 0; j < i; j++)
			{
				c7.card[6] = int2card(j);
				if (flag[c7.card[6].color][c7.card[6].val]) continue;
				c7_in_order[cnt++] = c7;
				c7_in_order[cnt - 1].get_level();
				c7_in_order[cnt - 1].card[5].val = i;
				c7_in_order[cnt - 1].card[6].val = j;
			}
		}
		sort(c7_in_order, c7_in_order + cnt);

		int left = 0, right = 0;
		for (int i = 0; i < cnt; i++)
		{
			while (left < cnt && (c7_in_order[i].level != c7_in_order[left].level || c7_in_order[i].level2 != c7_in_order[left].level2)) left++;
			while (right < cnt && c7_in_order[i].level == c7_in_order[right].level && c7_in_order[i].level2 == c7_in_order[right].level2) right++;

			int a = c7_in_order[i].card[5].val, b = c7_in_order[i].card[6].val;
			double win_t = 1, draw_t = 1;
			for (int j = 1; j < 8; j++)
			{
				win_t *= (double)left / cnt;
				draw_t *= (double)right / cnt;
				win[j][a][b] += win_t;
				draw[j][a][b] += draw_t - win_t;
			}
		}
	}
	else
	{
		int last;
		if (public_card_number == 0) last = 52; 
			else last = Card2int(public_card[public_card_number - 1]);
		for (int i = 0; i < last; i++)
		{
			Card ct = int2card(i);
			if (flag[ct.color][ct.val]) continue;
			flag[ct.color][ct.val] = 1;
			public_card[public_card_number] = ct;
			pre_dfs(flag, public_card, public_card_number + 1);
			flag[ct.color][ct.val] = 0;
			if (public_card_number == 0)
			{
				char str[100];
				sprintf(str, "preflop%d.txt", i+ 1);
				FILE *fout = fopen(str, "w");
				fprintf(fout, "playernum i j winrate drawrate");
				for (int i = 0; i < 52; i++)
					for (int j = 0; j < i; j++)
					{
						for (int k = 1; k < 8; k++) fprintf(fout, "%d %d %d %lf %lf\n", k, i, j, win[k][i][j] / c_50_5, draw[k][i][j] / c_50_5);
					}
				fclose(fout);
			}
		}
	}
}

int main()
{
	bool flag[10][14];
	Card card[10];
	memset(flag, 0, sizeof(flag));

	for (int i = 0; i < 52; i++)
		for (int j = 0; j < 52; j++)
			for (int k = 0; k < 10; k++) win[k][i][j] = draw[k][i][j] = 0;
	pre_dfs(flag, card, 0);

	return 0; 
}