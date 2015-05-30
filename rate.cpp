#include<iostream>
#include<cstdio>
#include<algorithm>
#include<cstdlib>
#include"Card.h"
#include<string.h>
#include"constant.h"
#include<time.h>
#include<stdlib.h>
using namespace std;
#define randomcount 30

rate make_pair(double x1, double x2)
{
	rate res;
	res.first = x1; 
	res.second = x2;
	return res;
}

rate operator +(const rate &rx1, const rate &rx2)
{
	return make_pair(rx1.first + rx2.first, rx1.second + rx2.second);
}

rate dfs(bool flag[][14], const Card hand_card[], Card public_card[], const int &public_card_number, const int &player_number)
{
	if (public_card_number > 5)
	{
		printf("warning! public_card_number too large!\n");
		exit(0);
	}
	if (public_card_number == 5)
	{
		Card7 c7, tc7;
		static pair<int, int> rank[3000];
		int cnt = 0;
		
		
		static int randomnum[randomcount];
		bool f1[52];
		memset(f1, 0, sizeof(f1));
		srand(time(0));
		for (int i = 0; i < randomcount; i++)
		{
			int tmp;
			do tmp = rand() % 52; while (f1[tmp]);
			f1[tmp] = 1;
		}


		for (int i = 0; i < 5; i++) c7.card[i] = public_card[i];
		for (int i1 = 0; i1 < randomcount; i1++)
			for (int j1 = 0; j1 < i1; j1++)
			{
				int i = randomnum[i1], j = randomnum[j1];
				c7.card[5] = int2card(i);
				c7.card[6] = int2card(j);
				if (flag[c7.card[5].color][c7.card[5].val] || flag[c7.card[6].color][c7.card[6].val]) continue;
				tc7 = c7;
				tc7.get_level();
				rank[cnt++] = make_pair(tc7.level, tc7.level2);
			}
		sort(rank, rank + cnt);

		c7.card[5] = hand_card[0];
		c7.card[6] = hand_card[1];
		c7.get_level();
		int left_boundary, right_boundary;
		left_boundary = 0;
		while (left_boundary < cnt && rank[left_boundary].first < c7.level) left_boundary++;
		while (left_boundary < cnt && rank[left_boundary].first == c7.level && rank[left_boundary].second < c7.level2) left_boundary++;
		right_boundary = left_boundary;
		while (right_boundary < cnt && rank[right_boundary].first == c7.level && rank[right_boundary].second == c7.level2) right_boundary++;
		double win_rate = 1, draw_rate = 1;
		for (int i = 0; i < player_number - 1; i++) win_rate *= (double)left_boundary / cnt, draw_rate *= (double)right_boundary / cnt;
		draw_rate = draw_rate - win_rate;
		return make_pair(draw_rate, win_rate);
	}
	else
	{
#ifdef _TEST
		print_Card(public_card, public_card_number, "public card when calculating rate");
#endif
		int cnt = 0;
		rate res = make_pair((double)0, (double)0);
		for (int i = 1; i <= 4; i++)
			for (int j = 1; j <= 13; j++) if (flag[i][j] == 0)
			{
				cnt++;
				flag[i][j] = 1;
				public_card[public_card_number] = Card(i, j);
				res = res + dfs(flag, hand_card, public_card, public_card_number + 1, player_number);
				public_card[public_card_number] = Card(-1, -1);
				flag[i][j] = 0;
			}
		res.first /= cnt; res.second /= cnt;
		return res;
	}
}

rate win_rate(const Card hand_card[], Card public_card[], const int &public_card_number, const int &player_number)
{
#ifdef _TEST
	print_Card(public_card, public_card_number, "public for win rate");
#endif
	static bool flag[5][14];
	memset(flag, 0, sizeof(flag));
	for (int i = 0; i < 2; i++) flag[hand_card[i].color][hand_card[i].val] = 1;
	for (int i = 0; i < public_card_number; i++) flag[public_card[i].color][public_card[i].val] = 1;

	return dfs(flag, hand_card, public_card, public_card_number, player_number);
	
}

rate win_rate(const int hand_card[], int public_card[], const int &public_card_number, const int &player_number)
{
	Card hold[2], com[10];
	for (int i = 0; i < 2; i++) hold[i] = int2card(hand_card[i]);
	for (int i = 0; i < public_card_number; i++) com[i] = int2card(public_card[i]);
	return win_rate(hold, com, public_card_number, player_number);
}

void read_pre_flop(double win[][52][52], double draw[][52][52])
{
	FILE *fin = fopen("pre_flop_win_rate.txt", "r");
	int player_num, i, j;
	int cnt = 0;
	while (fscanf(fin, "%d %d %d ", &player_num, &i, &j) != EOF)
	{
		double a, b;
		fscanf(fin, "%lf %lf", &a, &b);
		//if (cnt % 1000 == 0)printf("%lf %lf\n", a, b);
		cnt++;
		win[player_num][j][i] = win[player_num][i][j] = a;
		draw[player_num][j][i] = draw[player_num][i][j] = b;
	}

	fclose(fin);
}
