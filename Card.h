#ifndef zcx_1
#define zcx_1
struct Card
{
	int color, val;
	Card(void)
	{
		color = val = -1;
	}
};

bool operator <(const Card &rx1, const Card &rx2);

struct Card7 //将牌组赋值给card7前必须排序 
{
	Card card[7];
	int level; // range: 1 ~ 9 
	int level2; 
	
	Card7(void)
	{
		level = -1; 
	} 
	
	int get_level(void);
};

bool operator <(const Card7 &rx1, const Card7 &rx2);
void sort_card7(Card7 card_array[], int N);

#endif
