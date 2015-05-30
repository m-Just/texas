#ifndef zcx_1
#define zcx_1
struct Card
{
	int color, val;
	Card(void)
	{
		color = val = -1;
	}
	Card(int a, int b)
	{
		color = a;
		val = b;
	}
};

bool operator <(const Card &rx1, const Card &rx2);

struct Card7 
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

Card int2card(int card_in_int);
char * Card2str(const Card &card);
int Card2int(Card); 

struct rate
{
	double first, second;
};
rate operator +(const rate &rx1, const rate &rx2);
rate dfs(bool flag[][14], const Card hand_card[], Card public_card[], const int &public_card_number, const int &player_number);
rate win_rate(const Card hand_card[], Card public_card[], const int &public_card_number, const int &player_number);
rate win_rate(const int hand_card[], int public_card[], const int &public_card_number, const int &player_number);
rate win_rate_byrandom(const int hand_card[], int public_card[], const int &public_card_number, const int &player_number);
rate make_pair(double x1, double x2);

void print_Card(Card card[], int num, char * card_type);
void print_Card(int card[], int num, char * card_type);

void read_pre_flop(double win[][52][52], double draw[][52][52]);
#endif
