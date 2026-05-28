// C Header File
// Created 2/15/02; 2:21:53 PM

typedef struct {
	int card_number;
	char cards[52];
} DECK;

typedef struct {
	char cards[52];
} HAND;

void deck_init(DECK *deck, int deck_number);
void deal(DECK *deck, HAND *hand, int *order, int hand_number);
void disp_card(int x, int y, char rank, char suit, void *light, void *dark);
void disp_hand(int x, int y, HAND *hand);
