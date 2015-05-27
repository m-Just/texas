#include<cstdlib>
#include<algorithm> 
#include"Card.h"
using namespace std;

Card int2card(int card_in_int)
{
	Card res;
	res.val = card_in_int / 4 + 1;
	res.color = card_in_int % 4 + 1;
	return res;
}

int Card7::get_level(void)
	{

#ifdef TEST
		printf("calculating the level of Card7:\n");
		for (int i = 0; i < 7; i++) printf("(%d, %d) ", card[i].color, card[i].val);
		printf("\n");
#endif
		sort(card, card + 6);
		level = 1; // High card
		level2 = 0;
		for (int i = 0;i < 7; i++) level2 = level2 * 13 + card[i].val; 
		
		int cnt[14] = {0}; 
		for (int i = 0; i < 7; i++) cnt[card[i].val]++;
		
		int pair_cnt = 0;
		int three_max = -1;
		int pair_max = -1;
		for (int i = 1; i <= 13; i++)
		{
			if (cnt[i] == 4) //Four of a kind
			{
				level = 8;
				int tt = 6;
				while (card[tt].val == i) tt--;
				level2 = i * 13 + card[tt].val;
			}
			if (level <= 4 && cnt[i] == 3) // Three of a kind
			{
				level = 4;
				three_max = i;
				int tt = 6;
				while (card[tt].val == i) tt--;
				level2 = i * 13 + card[tt].val;
				tt--;
				while(card[tt].val == i) tt--;
				level2 = level2 * 13 + card[tt].val;
			}
			if (cnt[i] == 2) 
			{
				pair_cnt++;
				pair_max = i;
			}
		}
		
		if (level == 4 && pair_cnt >= 1) //Full house
		{
			level = 7;
			level2 = three_max * 13 + pair_max;
		}
		if (level < 2 && pair_cnt == 1) // One pair
		{
			level = 2;
			level2 = pair_max;
		}
		if (level < 3 && pair_cnt > 1) // Two pair
		{
			level = 3;
			int pair1, pair2, single;
			pair1 = pair2 = single = 13;
			while (cnt[pair1] < 2) pair1--;
			pair2 = pair1 - 1;
			while (cnt[pair2] < 2) pair2--;
			while (cnt[single] == 0 || single == pair1 || single == pair2) single--;
			level2 = (pair1 * 13 + pair2) * 13 + single;
		}
		
		int straight_cnt = 0;
		for (int i = 1; i <= 13; i++)
		{
			if (cnt[i] >= 1) straight_cnt++;
			else straight_cnt = 0;
			if (straight_cnt >= 5) // Straight
			{
				level = 5; 
				level2 = i; 
			}
		}
		
		if (level < 7)
		{
			
			
			int c[5][8] = {0};
			for (int i = 0; i < 7; i++) c[card[i].color][++c[card[i].color][0]] = card[i].val;
			for (int i = 1; i <= 4; i++) if (c[i][0] >= 5)
			{
				if (level <= 6)//Flush
				{
					level = 6;
					level2 = c[i][c[i][0]];
				}
				int straight_cnt = 0;
				for (int j = 1; j < c[i][0]; j++)
				{
					if (c[i][j] + 1 == c[i][j + 1]) straight_cnt++;
					else straight_cnt = 0;
					if (straight_cnt >= 4) 
					{
						level = 9; // Straight Flush
						level2 = c[i][j + 1];
					}
				}
			}
		}
		
#ifdef TEST
		printf("level:%d %d", level1, level2);
#endif
		return level;
	}

bool operator <(const Card &rx1, const Card &rx2) 
{
	return rx1.val < rx2.val;
 } 

bool operator <(const Card7 &rx1, const Card7 &rx2)
{
	return (rx1.level != rx2.level)?(rx1.level < rx2.level):(rx1.level2 < rx2.level2); 
} 

void sort_card7(Card7 card_array[], int N)
{
	sort(card_array, card_array + N);
}

