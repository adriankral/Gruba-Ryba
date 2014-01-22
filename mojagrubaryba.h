#ifndef MOJAGRUBARYBA_H
#define MOJAGRUBARYBA_H

#include <vector>
#include <string>
#include <memory>
#include "tests/grubaryba.h" //FIXME remember to change the path at the end!


class Property;
class Player;

typedef std::weak_ptr<Player> WeakPlayerPointer;
typedef std::shared_ptr<Player> PlayerPointer;
typedef std::vector<std::shared_ptr<Property>> PropertyVector;

class Player
{
protected:
	std::string const& name;
	unsigned int position;
	unsigned int cash;
	// true if the player is still in the game
	bool active; 
	// equals zero or number of turns to be spent involountarily passive
	unsigned int toWait;
	unsigned int playerID;
	// returns either a set of properties, selling of which would satisfy
	// money demand or an empty set, if there is no possibility
	// of satisfying aforementioned demand
	virtual PropertyVector sellFor(int money);
public:
	Player(std::string const& _name, int _position, int _cash) :
		name(_name), position(_position), cash(_cash), active(true),
		toWait(0) {}
	inline bool canMove() const { return active && toWait != 0; }
	inline bool isActive() const { return active; }
	inline void movePassed() { toWait = toWait > 0 ? toWait - 1 : 0; }
	inline unsigned int getPosition() { return position; }
	inline void setPosition(unsigned int _position) { position = _position; }
	// returns cost if the player can afford it; if not, declares
	// bankruptcy and returns the amount of money gained
	// through selling all properties
	unsigned int pay(unsigned int cost = 0, bool volountary = true);
	void receive(unsigned int cost) { cash += cost; }
	void wait(unsigned int delay) { toWait += delay; }
	virtual std::string const& getName();
	// sells all properties, sets itself inactive and returns
	// the amount of money accumulated during the selling process
	virtual unsigned int declareBankruptcy(); 
	PropertyVector properties;
};

class HumanPlayer : Player, Human 
{
	//SOMETHING.
};

class ComputerPlayer : Player 
{
	//SOMETHING
};

class ComputerDUMB : Player //ComputerPlayer ?
{
private:
	unsigned short counter;
public:
	ComputerDUMB(std::string const& _name, int _position, int _cash) :
		Player(_name, _position, _cash) {}
	bool wantBuy(std::string const& property) const;
	bool wantSell(std::string const& property) const { return true; }
};

class ComputerSMARTASS : Player //ComputerPlayer ?
{
public:
	ComputerSMARTASS(std::string const& _name, int _position, int _cash) :
		Player(_name, _position, _cash) {}
	bool wantBuy(std::string const& property) const { return true; }
	bool wantSell(std::string const& property) const { return true; }
};

class Field
{
protected:
	std::string const& name;
public:
	Field(std::string const& _name) : name(_name) {}
	std::string const& getName() const { return name; }
	virtual void onPass(PlayerPointer p) {}
	virtual void onStep(PlayerPointer p) {}
};

class Property : Field
{
protected:
	const unsigned int cost;
	bool hasOwner=false;
public:
	Property(std::string const& _name, unsigned int _cost) :
		Field(_name), cost(_cost) {}
	WeakPlayerPointer owner;

	inline unsigned int const getValue() { return cost; }
	virtual inline unsigned int const soldValue() { return cost / 2; }
	virtual unsigned int sell();
	
	// returns the amount of money to be paid to owner on stay
	virtual unsigned int const toPay() = 0;

	virtual void onStep(PlayerPointer p);
};

class Start : Field
{
protected:
	const int bonus;
public:
	Start(std::string const& _name, int _bonus = 50) :
		Field(_name), bonus(_bonus) {}
	virtual void onPass(PlayerPointer p) { p->receive(bonus); }
	virtual void onStep(PlayerPointer p) { p->receive(bonus); }
};

class Prize : Field
{
protected:
	const int bonus;
public:
	Prize(std::string const& _name, int _bonus) :
		Field(_name), bonus(_bonus) {}
	virtual inline void onStep(PlayerPointer p)
	{ p->receive(bonus); }
};

class Punishment : Field
{
protected:
	const int malus;
public:
	Punishment(std::string const& _name, int _malus) :
		Field(_name), malus(_malus) {}
	virtual inline void onStep(PlayerPointer p)
	{ p->pay(malus); }
};

class Deposite : Field
{
protected:
	const int payOnPass;
	int bank;

public:
	Deposite(std::string const& _name, int _payOnPass, int _bank = 0) :
		Field(_name), payOnPass(_payOnPass), bank(_bank) {}
	virtual inline void onStep(PlayerPointer p)
	{ p->receive(bank); bank = 0; }
	virtual inline void onPass(PlayerPointer p)
	{ bank += p->pay(payOnPass); }
};

class Aquarium : Field
{
protected:
	const int delay;
public:
	Aquarium(std::string const& _name, int _delay) :
		Field(_name), delay(_delay) {}
	virtual void onStep(PlayerPointer p) { p->wait(delay); }
};

class MojaGrubaRyba : GrubaRyba
{
private:
	std::vector<PlayerPointer> players;
	std::vector<std::shared_ptr<Field> > fields;
	std::shared_ptr<Die> die;
	int turn;
	int currentPlayer;
	unsigned int activePlayers = 0;

	const unsigned int MIN_PLAYERS = 2;
	const unsigned int MAX_PLAYERS = 8;
	const unsigned int STARTPOS = 0;
	const unsigned int STARTCASH = 1000;

	void makeTurn();
	void outputState();
public:
	MojaGrubaRyba();
	void setDie(std::shared_ptr<Die> _die) { if(_die) die = _die; }
	void addComputerPlayer(GrubaRyba::ComputerLevel level); //TODO
	void addHumanPlayer(std::shared_ptr<Human> human);
	void play(unsigned int rounds);
};

class Coral : Property
{
public:
	Coral(std::string const& _name, int _cost) :
		Property(_name, _cost) {}
	virtual inline int toPay() const { return cost / 5; }
};

class PublicUse : Property
{
public:
	PublicUse(std::string const& _name, int _cost) :
		Property(_name, _cost) {}
	virtual inline int toPay() const { return cost * 2 / 5; }
};

/* za dużo to gówno wyrzuca :P
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
*/

#endif /* MOJAGRUBARYBA_H */
