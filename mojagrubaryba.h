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
typedef std::vector<std::weak_ptr<Property>> WeakPropertyVector;

class Player
{
protected:
	//std::string const& name; - jest w definicji klas dziedziczących po human
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
	virtual WeakPropertyVector sellFor(int money);
	WeakPropertyVector properties;
public:
	Player(int _position, int _cash, unsigned int playerID) :
		position(_position), cash(_cash), active(true),
		toWait(0), playerID(playerID) {}
	inline bool canMove() const { return active && toWait == 0; }
	inline bool isActive() const { return active; }
	inline void movePassed() { toWait = toWait > 0 ? toWait - 1 : 0; }
	inline unsigned int getPosition() { return position; }
	inline void setPosition(unsigned int _position) { position = _position; }
	inline unsigned int getToWait() { return toWait; }
	inline unsigned int getCash() { return cash; }
	// returns cost if the player can afford it; if not, declares
	// bankruptcy and returns the amount of money gained
	// through selling all properties
	unsigned int pay(unsigned int cost = 0, bool volountary = true);
	void receive(unsigned int cost) { cash += cost; }
	void wait(unsigned int delay) { toWait += delay; }
	// sells all properties, sets itself inactive and returns
	// the amount of money accumulated during the selling process
	virtual unsigned int declareBankruptcy(); 

	virtual std::string const& getName() = 0;
	virtual bool wantBuy(std::string const& propertyName) = 0;
    virtual bool wantSell(std::string const& propertyName) = 0;
};

class HumanPlayer : public Player 
{
	std::shared_ptr<Human> human;
public:
	HumanPlayer(std::shared_ptr<Human> human, int _position, int _cash, unsigned int _playerID) : Player(_position, _cash, _playerID), human(human)  {}
	std::string const& getName() {return human->getName();}
	virtual bool wantBuy(std::string const& propertyName) { return human->wantBuy(propertyName); }
	virtual bool wantSell(std::string const& propertyName) { return human->wantSell(propertyName); }
};

class ComputerPlayer : public Player 
{
	std::string const& name;
public:
	ComputerPlayer(int _position, int _cash, int _playerID) :
		Player(_position, _cash, _playerID), name("Gracz " + playerID) {  }
	std::string const& getName() {return name;}
};

class ComputerDUMB : public ComputerPlayer
{
private:
	unsigned short counter;
public:
	ComputerDUMB(int _position, int _cash, int _playerID) :
		ComputerPlayer(_position, _cash, _playerID) {}
	virtual bool wantBuy(std::string const& propertyName) { return true; }
	virtual bool wantSell(std::string const& propertyName) { return true; }
};

class ComputerSMARTASS : public ComputerPlayer
{
public:
	ComputerSMARTASS(int _position, int _cash, int _playerID) :
		ComputerPlayer(_position, _cash, _playerID) {}
	virtual bool wantBuy(std::string const& propertyName) { return true; }
	virtual bool wantSell(std::string const& propertyName) { return true; }
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

class Property : public Field
{
protected:
	const unsigned int cost;
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

class Start : public Field
{
protected:
	const int bonus;
public:
	Start(int _bonus = 50) :
		Field("Start"), bonus(_bonus) {}
	virtual void onPass(PlayerPointer p) { p->receive(bonus); }
	virtual void onStep(PlayerPointer p) { p->receive(bonus); }
};

class Prize : public Field
{
protected:
	const int bonus;
public:
	Prize(std::string const& _name, int _bonus) :
		Field(_name), bonus(_bonus) {}
	virtual inline void onStep(PlayerPointer p)
	{ p->receive(bonus); }
};

class Punishment : public Field
{
protected:
	const int malus;
public:
	Punishment(std::string const& _name, int _malus) :
		Field(_name), malus(_malus) {}
	virtual inline void onStep(PlayerPointer p)
	{ p->pay(malus); }
};

class Deposite : public Field
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

class Aquarium : public Field
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
	std::vector<std::shared_ptr<Field>> fields;
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

class Coral : public Property
{
public:
	Coral(std::string const& _name, int _cost) :
		Property(_name, _cost) {}
	virtual unsigned int const toPay() { return cost / 5; }
};

class PublicUse : public Property
{
public:
	PublicUse(std::string const& _name, int _cost) :
		Property(_name, _cost) {}
	virtual unsigned int const toPay() { return cost * 2 / 5; }
};

#endif /* MOJAGRUBARYBA_H */
