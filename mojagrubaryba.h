#ifndef MOJAGRUBARYBA_H
#define MOJAGRUBARYBA_H

#include <vector>

class Property;

class Die
{
public:
	virtual unsigned short roll() const;
	virtual std::shared_ptr<Die> clone() const;
}

class Player
{
private:
	unsigned int position;
	unsigned int cash;
protected:
	virtual std::vector<std::shared_ptr<Property> > sellFor(int money);
public:
	unsigned int pay(unsigned int cost) {}; //TODO - tu domyslnie ma sie tez odbyc licytacja/przechodzenie w banructwo
	void receive(unsigned int cost) {}; //TODO
	void wait(unsigned int delay) {}; //TODO
	virtual unsigned int declareBancruptcy();
	std::vector<std::shared_ptr<Property> > properties;
};

class Field
{
private:
	std::string const& name;
public:
	Field(std::string const& _name) : name(_name) {}
	virtual void onPass(std::shared_ptr<Player> p) {}
	virtual void onStep(std::shared_ptr<Player> p) {}
};

class Property : Field
{
protected:
	const unsigned int cost;
public:
	Property(std::string const& _name, unsigned int _cost) : Field(_name), cost(_cost) {}
	std::shared_ptr<Player> owner;

	inline unsigned int getValue() { return cost; }
	virtual inline unsigned int soldValue() { return cost / 2; }
	virtual unsigned int sell();

	virtual unsigned int toPay();

	virtual void onStep(Player> p);
}

class Start : Field
{
protected:
	const int bonus;
public:
	Start(std::string const& _name, int _bonus = 50) : Field(_name), bonus(_bonus) {}
	virtual void onPass(std::shared_ptr<Player> p) { p->receive(bonus); }
	virtual void onStep(std::shared_ptr<Player> p) { p->receive(bonus); }
}

class Prize : Field
{
protected:
	const int bonus;
public:
	Prize(std::string const& _name, int _bonus) : Field(_name), bonus(_bonus) {}
	virtual inline void onStep(std::shared_ptr<Player> p) { p->receive(bonus); }
}

class Punishment : Field
{
protected:
	const int malus;
public:
	Punishment(std::string const& _name, int _malus) : Field(_name), malus(_malus) {}
	virtual inline void onStep(std::shared_ptr<Player> p) { p->pay(malus); }
}

class Deposite : Field
{
protected:
	const int bank;
	const int payOnPass;
public:
	Deposite(std::string const& _name, int _bank = 0, int _payOnPass) : Field(_name), bank(_bank), payOnPass(_payOnPass) {}
	virtual inline void onStep(std::shared_ptr<Player> p) { p->receive(bank); bank = 0; }
	virtual inline void onPass(std::shared_ptr<Player> p) { bank += p->pay(payOnPass); }
}

class Aquarium : Field
{
protected:
	const int delay;
public:
	Aquarium(std::string const& _name, int _delay) : Field(_name), delay(_delay) {}
	virtual void onStep(std::shared_ptr<Player> p) { p->wait(delay); }
}

class MojaGrubaRyba
{
private:
	std::vector<Player> players;
	std::vector<Field> fields;
	std::shared_ptr<Die> die;
	int turn;
	int currentPlayer;
public:
	void setDie(std::shared_ptr<Die> _die) { die = _die; }
	void addComputerPlayer(GrubaRyba::ComputerLevel level); //TODO
	void addHumanPlayer(std::shared_ptr<Human> human);
};

class Coral : Property
{
public:
	Coral(std::string const& _name, int _cost) : Property(_name, _cost) {}
	virtual inline int toPay() { return cost / 5; }
}

class PublicUse : Property
{
public:
	PublicUse(std::string const& _name, int _cost) : Property(_name, _cost) {}
	virtual inline int toPay() { return cost * 2 / 5; }
}

class Anemonia : Coral("Anemonia", 160) {};
class Island : Field("Wyspa") {};
class Aporina : Coral("Aporina", 220) {};
class AqariumField : Aquarium("Akwarium", 3) {};
class Cave : PublicUse("Grota", 300) {};
class Menella : Coral("Menella", 280) {};
class Laguna : Deposite("Laguna", 15) {};
class Ship : PublicUse("Statek", 250) {};
class Nemo : Prize("Błazenki", 120) {};
class Pennatula : Coral("Pennatula", 400) {};
class Shark : Punishment("Rekin", 180) {};

#endif /* MOJAGRUBARYBA_H */
