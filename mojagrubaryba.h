#ifndef MOJAGRUBARYBA_H
#define MOJAGRUBARYBA_H

#include <vector>
#include <string>
#include <memory>
#include <map>
#include "tests/grubaryba.h" //FIXME remember to change the path at the end!

class Property;
class Player;
class MojaGrubaRyba;

typedef std::weak_ptr<Player> WeakPlayerPointer;
typedef std::shared_ptr<Player> PlayerPointer;
typedef std::vector<std::shared_ptr<Property>> PropertyVector;
typedef std::vector<std::weak_ptr<Property>> WeakPropertyVector;

class Player
{
protected:
	//enables two-way communication between the game and the player
	std::shared_ptr<MojaGrubaRyba> game;
	//std::string const& name; - jest w definicji klas dziedziczących po human
	unsigned int position;
	unsigned int cash;
	// true if the player is still in the game(not bankrupt)
	bool active; 
	// equals zero or number of turns to be spent involountarily passive
	unsigned int toWait;
	unsigned int playerID;
	// returns either a set of properties, selling of which would satisfy
	// money demand or an empty set, if there is no possibility
	// of satisfying aforementioned demand
	virtual WeakPropertyVector sellFor(int money);
	WeakPropertyVector properties;	
	virtual bool canAfford(std::string const& propertyName);
public:
	Player(std::shared_ptr<MojaGrubaRyba> _game, int _position, int _cash, unsigned int playerID) :
		game(_game), position(_position), cash(_cash), active(true),
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
	// through selling all properties. If volountary is set to true,
	// lack of money does not lead to bankruptcy
	unsigned int pay(unsigned int cost = 0, bool volountary = true);
	void receive(unsigned int cost) { cash += cost; }
	void wait(unsigned int delay) { toWait += delay; }
	// sells all properties, sets itself inactive and returns
	// the amount of money accumulated during the selling process
	virtual unsigned int declareBankruptcy(); 
	// confirmation from property
	virtual void propertyBought(std::weak_ptr<Property> prop) { properties.push_back(prop); }
	
	virtual std::string const& getName() = 0;
	virtual bool wantBuy(std::string const& propertyName) = 0;
    	virtual bool wantSell(std::string const& propertyName) = 0;

};

class HumanPlayer : public Player 
{
	std::shared_ptr<Human> human;
public:
	HumanPlayer(std::shared_ptr<MojaGrubaRyba> _game, std::shared_ptr<Human> _human, unsigned int _position, unsigned int _cash, unsigned int _playerID) : Player(_game, _position, _cash, _playerID), human(_human)  {}
	std::string const& getName() {return human->getName();}
	virtual bool wantBuy(std::string const& propertyName) { return human->wantBuy(propertyName); }
	virtual bool wantSell(std::string const& propertyName) { return human->wantSell(propertyName); }
};

class ComputerPlayer : public Player 
{
	std::string name;
public:
	ComputerPlayer(std::shared_ptr<MojaGrubaRyba> _game, unsigned int _position, unsigned int _cash, unsigned int _playerID) :
		Player(_game, _position, _cash, _playerID), name(std::string("Gracz ") + std::to_string(playerID)) {  }
	std::string const& getName() {return name;}
};

class ComputerDUMB : public ComputerPlayer
{
private:
	unsigned short counter;
public:
	ComputerDUMB(std::shared_ptr<MojaGrubaRyba> _game, unsigned int _position, unsigned int _cash, unsigned int _playerID) :
		ComputerPlayer(_game, _position, _cash, _playerID), counter(0) {}
	virtual bool wantBuy(std::string const& propertyName) { if(!canAfford(propertyName)) return false; counter = (counter + 1) % 3; return counter == 0; }
	virtual bool wantSell(std::string const& propertyName) { return false; }
};

class ComputerSMARTASS : public ComputerPlayer
{
public:
	ComputerSMARTASS(std::shared_ptr<MojaGrubaRyba> _game, unsigned int _position, unsigned int _cash, unsigned int _playerID) :
		ComputerPlayer(_game, _position, _cash, _playerID) {}
	virtual bool wantBuy(std::string const& propertyName) { if(!canAfford(propertyName)) return false; return true; }
	virtual bool wantSell(std::string const& propertyName) { return false; }
};

class Field
{
protected:
	std::string name;
public:
	Field(std::string const& _name) : name(_name) {}
	std::string const& getName() const { return name; }
	
	// events
	// this is a simple field. it just exists and disturbs noone.
	virtual void onPass(PlayerPointer p) {}
	virtual void onStep(PlayerPointer p) {}
};

class Property : public Field, public std::enable_shared_from_this<Property>
{
protected:
	const unsigned int cost;
public:
	Property(std::string const& _name, unsigned int _cost) :
		Field(_name), cost(_cost) {}
	WeakPlayerPointer owner;

	inline unsigned int const getValue() { return cost; }
	virtual inline unsigned int const soldValue() { return cost / 2; }

	// sets owner to empty, returns soldValue
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

class Reward : public Field
{
protected:
	const int bonus;
public:
	Reward(std::string const& _name, int _bonus) :
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
	{ p->pay(malus, false); }
};

class Deposite : public Field
{
protected:
	const unsigned int payOnPass;
	unsigned int bank;

public:
	Deposite(std::string const& _name, unsigned int _payOnPass, unsigned int _bank = 0) :
		Field(_name), payOnPass(_payOnPass), bank(_bank) {}
	virtual inline void onStep(PlayerPointer p)
	{ p->receive(bank); bank = 0; }
	virtual inline void onPass(PlayerPointer p)
	{ unsigned int paid = p->pay(payOnPass, false); bank += (paid > payOnPass) ? payOnPass : paid; }
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

class MojaGrubaRyba : public GrubaRyba, public std::enable_shared_from_this<MojaGrubaRyba>
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
	
	// required by use of std::string in wantBuy, wantSell;
	// the player doesn't have direct access to the board,
	// so he/she has to ask the game core for information
	std::map<std::string, unsigned int> propertyValueMap;

	// enumerator of field types, used in implementation of 
	// field factory function
	enum FieldType
	{
		FT_ISLAND,
		FT_START,
		FT_REWARD,
		FT_PUNISHMENT,
		FT_DEPOSITE,
		FT_AQUARIUM,
		FT_CORAL,
		FT_PUBLIC_USE
	};

	// field factory; returns a pointer to newly created field
	std::shared_ptr<Field> produceField(FieldType fType, std::string const& name, unsigned int fieldArgument = 0);
	
	// makes each player move, stopping the game if all but one go bankrupt
	void makeTurn();

	// prints game state information to the output
	void outputState();
public:
	MojaGrubaRyba();
	unsigned int getPropertyValue(std::string const& name) { return propertyValueMap[name]; }
	void setDie(std::shared_ptr<Die> _die) { if(_die) die = _die; }
	void addComputerPlayer(GrubaRyba::ComputerLevel level);
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
