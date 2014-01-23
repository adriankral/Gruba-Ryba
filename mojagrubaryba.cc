#include <vector>
#include <iostream>
#include "mojagrubaryba.h"

using namespace std;

WeakPropertyVector Player::sellFor(int money)
{
	WeakPropertyVector toBeSold;
	int soldValue;
	for(auto it = properties.begin(); it != properties.end() && soldValue < money; ++it)
	{
		if(  wantSell((*it).lock()->getName())) 
		{
			soldValue += (*it).lock()->soldValue();
			toBeSold.push_back(*it);
		}
	}
	if(soldValue < money)
		return WeakPropertyVector();
	return toBeSold;
}

bool Player::canAfford(std::string const& propertyName)
{
	return game->getPropertyValue(propertyName) <= cash;
}

unsigned int Player::pay(unsigned int cost, bool volountary)
{
	if(cost <= cash)
	{
		cash -= cost;
		return cost;
	}
	WeakPropertyVector toBeSold = sellFor(cost - cash);
	if(toBeSold.empty())
	{
		if(volountary)
			return 0;
		unsigned int repayment = declareBankruptcy();
		return repayment;
	}
	for(auto it = toBeSold.begin(); it != toBeSold.end(); ++it)
	{
		cash += (*it).lock()->sell();
	}
	cash -= cost;
	return cost;
}

unsigned int Player::declareBankruptcy()
{
	unsigned int repayment = 0;
	for(auto it = properties.begin(); it != properties.end(); ++it)
	{
		repayment += (*it).lock()->sell();
	}
	properties.clear();
	active = false;
	return repayment;
}

unsigned int Property::sell()
{
	owner.reset();
	return soldValue();
}

void Property::onStep(std::shared_ptr<Player> p)
{
	if (!owner.lock())
	{
		if( p->wantBuy(name) )
		{
			if(p->pay(getValue()) == cost)
			{
				owner = p;
				p->propertyBought(shared_from_this());
			}
		}
	}
	else {
		unsigned int paid = p->pay(toPay(), false);
		paid = (paid > toPay()) ? toPay() : paid;
		owner.lock()->receive(paid);
	}
}
 
MojaGrubaRyba::MojaGrubaRyba() : players(), fields(), die(), turn(0),
	currentPlayer(0), activePlayers(0)
{
	fields.push_back(produceField(FT_START, "", 50));
	fields.push_back(produceField(FT_CORAL, "Anemonia", 160));
	fields.push_back(produceField(FT_ISLAND, "Wyspa"));
	fields.push_back(produceField(FT_CORAL, "Aporina", 220));
	fields.push_back(produceField(FT_AQUARIUM, "Akwarium", 3));
	fields.push_back(produceField(FT_PUBLIC_USE, "Grota", 300));
	fields.push_back(produceField(FT_CORAL, "Menella", 280));
	fields.push_back(produceField(FT_DEPOSITE, "Laguna", 15));
	fields.push_back(produceField(FT_PUBLIC_USE, "Statek", 250));
	fields.push_back(produceField(FT_REWARD, "Błazenki", 120));
	fields.push_back(produceField(FT_CORAL, "Pennatula", 400));
	fields.push_back(produceField(FT_PUNISHMENT, "Rekin", 180));
}

shared_ptr<Field> MojaGrubaRyba::produceField(FieldType fType, std::string const& name, unsigned int fieldArgument)
{
	switch(fType)
	{
		case FT_ISLAND:
			return shared_ptr<Field>(new Field(name));
		case FT_START:
			return shared_ptr<Field>(dynamic_pointer_cast<Field>(shared_ptr<Start>(new Start(fieldArgument))));
		case FT_REWARD:
			return shared_ptr<Field>(dynamic_pointer_cast<Field>(shared_ptr<Reward>(new Reward(name, fieldArgument))));
		case FT_PUNISHMENT:
			return shared_ptr<Field>(dynamic_pointer_cast<Field>(shared_ptr<Punishment>(new Punishment(name, fieldArgument))));
		case FT_DEPOSITE:
			return shared_ptr<Field>(dynamic_pointer_cast<Field>(shared_ptr<Deposite>(new Deposite(name, fieldArgument))));
		case FT_AQUARIUM:
			return shared_ptr<Field>(dynamic_pointer_cast<Field>(shared_ptr<Aquarium>(new Aquarium(name, fieldArgument))));
		case FT_CORAL:
			propertyValueMap.insert(std::make_pair(std::string(name), fieldArgument));
			return shared_ptr<Field>(dynamic_pointer_cast<Field>(shared_ptr<Coral>(new Coral(name, fieldArgument))));
		case FT_PUBLIC_USE:
			propertyValueMap.insert(std::make_pair(std::string(name), fieldArgument));
			return shared_ptr<Field>(dynamic_pointer_cast<Field>(shared_ptr<PublicUse>(new PublicUse(name, fieldArgument))));
	}
	return shared_ptr<Field>();
}

void MojaGrubaRyba::addComputerPlayer(MojaGrubaRyba::ComputerLevel level)
{
	if(players.size() >= MAX_PLAYERS)
		throw TooManyPlayersException(MAX_PLAYERS);
	
	shared_ptr<Player> compPlayer;
	if(level == ComputerLevel::DUMB)
		compPlayer = dynamic_pointer_cast<Player>(shared_ptr<ComputerDUMB> (new ComputerDUMB(shared_from_this(), STARTPOS, STARTCASH, ++activePlayers)));
	else if(level == ComputerLevel::SMARTASS)
		compPlayer = dynamic_pointer_cast<Player>(shared_ptr<ComputerSMARTASS> (new ComputerSMARTASS(shared_from_this(), STARTPOS, STARTCASH, ++activePlayers)));
	players.push_back(compPlayer);
}

void MojaGrubaRyba::addHumanPlayer(std::shared_ptr<Human> human) 
{
	if(players.size() >= MAX_PLAYERS)
		throw TooManyPlayersException(MAX_PLAYERS);
	
	shared_ptr<HumanPlayer> hp(new HumanPlayer(shared_from_this(), human, STARTPOS, STARTCASH, ++activePlayers));
	shared_ptr<Player> p = dynamic_pointer_cast<Player>(hp);
	players.push_back(p);
	
}

void MojaGrubaRyba::makeTurn()
{
	for(auto it = players.begin(); it != players.end(); ++it)
	{
		(*it)->movePassed();
		if((*it)->canMove())
		{
			unsigned short moves = die->roll() + die->roll();
			for(unsigned short i = 0; i < moves - 1; i++)
			{
				(*it)->setPosition(((*it)->getPosition() + 1) % fields.size());
				fields[(*it)->getPosition()]->onPass(*it);
			}
			(*it)->setPosition(((*it)->getPosition() + 1) % fields.size());
			fields[(*it)->getPosition()]->onStep(*it);
			if(!(*it)->isActive())
			{
				--activePlayers;
				if(activePlayers < 2)
					return;
			}
		}
			
	}
}

void MojaGrubaRyba::outputState()
{
	for(auto it = players.begin(); it != players.end(); ++it)
	{
		if((*it)->canMove())
			std::cout<<(*it)->getName()<<" pole: "<<fields[(*it)->getPosition()]->getName()<<" gotowka: "<<(*it)->getCash()<<std::endl;
		else if((*it)->isActive())
			std::cout<<(*it)->getName()<<" pole: "<<fields[(*it)->getPosition()]->getName()<<" *** czekanie "<<(*it)->getToWait()<<" ***"<<std::endl;
		else
			std::cout<<(*it)->getName()<<" *** bankrut ***"<<std::endl;
	}
}


void MojaGrubaRyba::play(unsigned int rounds)
{
	if(!die)
		throw NoDieException();
	if(activePlayers < MIN_PLAYERS)
		throw TooFewPlayersException(MIN_PLAYERS);
	for(unsigned int i = 1; activePlayers > 1 && i <= rounds; i++)
	{
		printf("Runda %u.\n", i);
		makeTurn();
		outputState();
	}
}
