#include<iostream>
#include<cstdio>
#include<algorithm>
#include<cstdlib>
#include"Card.h"
using namespace std;

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
	if (public_card_number == 5)
	{
		Card7 c7;
		Card7 c7_in_order[1300];
		int cnt = 0;

		for (int i = 0; i < 5; i++) c7.card[i] = public_card[i];
		for (int i = 0; i < 52; i++)
			for (int j = 0; j < i; j++)
			{
				c7.card[5] = int2card(i);
				c7.card[6] = int2card(j);
				if (flag[c7.card[5].color][c7.card[5].val] || flag[c7.card[6].color][c7.card[6].val]) continue;
				c7_in_order[cnt++] = c7;
				c7_in_order[cnt - 1].get_level();
			}
		sort(c7_in_order, c7_in_order + cnt);

		c7.card[5] = hand_card[0];
		c7.card[6] = hand_card[1];
		c7.get_level();
		int left_boundary, right_boundary;
		left_boundary = 0;
		while (!(c7_in_order[left_boundary].level == c7.level && c7_in_order[left_boundary].level2 == c7.level2)) left_boundary++;
		right_boundary = left_boundary;
		while (c7_in_order[right_boundary].level == c7.level && c7_in_order[right_boundary].level2 == c7.level2) right_boundary++;
		double win_rate = 1, draw_rate = 1;
		for (int i = 0; i < player_number - 1; i++) win_rate *= (double)left_boundary / cnt, draw_rate *= (double)right_boundary / cnt;
		draw_rate = draw_rate - win_rate;
		return make_pair(draw_rate, win_rate);
		//return make_pair((double)left_boundary / cnt, (double)(right_boundary - left_boundary) / cnt);
	}
	else
	{
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
	bool flag[5][14] = { 0 };
	for (int i = 0; i < 2; i++) flag[hand_card[i].color][hand_card[i].val] = 1;
	for (int i = 0; i < public_card_number; i++) flag[public_card[i].color][public_card[i].val] = 1;

	return dfs(flag, hand_card, public_card, public_card_number, player_number);
	
}
