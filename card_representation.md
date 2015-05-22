#IMPORTANT
This is only for internal representation.  
For representation in communication with the server, please view the "程序通讯协议"

##Formation
An integer(0~51): n = 4*(point-1)+(color)

##Point representation
1 for Ace  
2 for Two  
...  
11 for Jack  
12 for Queen  
13 for King

##Color representation
0 for Hearts  
1 for Diamonds  
2 for Spades  
3 for Clubs

##Properties
point = n/4 + 1  
color = n%4

##Examples
0 for Ace of Hearts  
9 for Three of Diamonds  
51 for King of Clubs  
