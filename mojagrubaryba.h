#ifndef MOJAGRUBARYBA_H
#define MOJAGRUBARYBA_H

#include <vector>

class Property;

class Player
{
private:
	int position;
	int cash;
public:
	int pay(int cost) {}; //TODO - tu domyslnie ma sie tez odbyc licytacja/przechodzenie w banructwo
	void receive(int cost) {}; //TODO
	void wait(int delay) {}; //TODO
	int position;
	int cash;
	std::vector<Property* > properties;
};

class Field
{
public:
	virtual void onPass(Player* p) {}
	virtual void onStep(Player* p) {}
};

class Property : Field
{
protected:
	const int cost;
public:
	Property(int _cost) : cost(_cost) {}
	Player* owner = NULL;

	virtual int toPay();

	virtual void onStep(Player* p);
}

class Start : Field
{
protected:
	const int bonus;
public:
	Start(int _bonus = 50) : bonus(_bonus) {}
	virtual void onPass(Player* p) { p->receive(bonus); }
	virtual void onStep(Player* p) { p->receive(bonus); }
}

class Prize : Field
{
protected:
	const int bonus;
public:
	Prize(int _bonus) : bonus(_bonus) {}
	virtual inline void onStep(Player* p) { p->receive(bonus); }
}

class Punishment : Field
{
protected:
	const int malus;
public:
	Punishment(int _malus) : malus(_malus) {}
	virtual inline void onStep(Player* p) { p->pay(malus); }
}

class Deposite : Field
{
protected:
	const int bank;
	const int payOnPass;
public:
	Deposite(int _bank = 0, int _payOnPass) : bank(_bank), payOnPass(_payOnPass) {}
	virtual inline void onStep(Player* p) { p->receive(bank); bank = 0; }
	virtual inline void onPass(Player* p) { bank += p->pay(payOnPass); }
}

class Aquarium : Field
{
protected:
	const int delay;
public:
	Aquarium(int _delay) : delay(_delay) {}
	virtual void onStep(Player* p) { p->wait(delay); }
}

class Board
{
private:
	std::vector<Player> players;
	std::vector<Field> fields;
	int turn;
	int currentPlayer;
};

class Coral : Property
{
public:
	Coral(int _cost) : Property(_cost) {}
	virtual inline int toPay() { return cost/5; }
}

class PublicUse : Property
{
public:
	PublicUse(int _cost) : Property(_cost) {}
	virtual inline int toPay() { return cost*2/5; }
}

class Anemonia : Coral(160) {};
class Island : Field {};
class Aporina : Coral(220) {};
class AqariumField : Aquarium(3) {};
class Cave : PublicUse(300) {};
class Menella : Coral(280) {};
class Laguna : Deposite(15) {};
class Ship : PublicUse(250) {};
class Nemo : Prize(120) {};
class Pennatula : Coral(400) {};
class Shark : Punishment(180) {};

#endif /* MOJAGRUBARYBA_H */
