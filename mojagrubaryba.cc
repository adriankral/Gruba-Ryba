#include <vector>
#include "mojagrubaryba.h"
#include "tests/grubaryba.h" //FIXME remember to change the path at the end!

using namespace std;

class Board {
	std::vector<Player> players;
	std::vector<Field> fields;
	int turn;
	int currentPlayer;
};

class Field {
public: //te maja zostac puste - jako domyślne
	virtual void onPass(Player *p){}
	virtual void onStep(Player *p){}
};

class Player {
public:
	void pay(int cost) {}; //TODO - tu domyslnie ma sie tez odbyc licytacja/przechodzenie w bankructwo
	void receive(int cost) {}; //TODO

	int position;
	int cash;
	std::vector< Property* > properties;
};

class Property : Field {
public:
	Property (int _cost) : cost{_cost} {}
	const int cost;	
	Player * owner = NULL;

	virtual int toPay();

	virtual void onStep(Player *p){
		if (owner == NULL) ; //zapytaj o kupno
		else {
			p->pay(toPay());
			owner->receive(toPay());
		}
	}
};

class Coral : Property {
public:
	virtual int toPay(){return cost/5;}	
};

class PublicUse : Property {
public:
	virtual int toPay(){return cost*2/5;}
};

