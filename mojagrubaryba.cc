#include <vector>
#include "mojagrubaryba.h"
#include "tests/grubaryba.h" //FIXME remember to change the path at the end!

using namespace std;

std::vector<std::shared_ptr<Property> > Player::sellFor(int money)
{
	std::vector<std::shared_ptr<Property> > toBeSold;
	int soldValue;
	for(std::vector<std::shared_ptr<Property> >::iterator it = properties.begin();
			it != properties.end() && soldValue < money; ++it)
	{
		if(wantSell(it->getName()))
		{
			soldValue += it->getSellValue();
			toBeSold.push_back(it);
		}
	}
	if(soldValue < money)
		return std::vector<std::shared_ptr<Property> >();
	return toBeSold;
}

unsigned int Player::pay(unsigned int cost)
{
	if(cost <= cash)
	{
		cash -= cost;
		return cost;
	}
	std::vector<std::shared_ptr<Property> > toBeSold = sellFor(cost - cash);
	if(toBeSold.empty())
	{
		unsigned int repayment = declareBankruptcy();
		return repayment;
	}
	for(std::vector<std::shared_ptr<Property> >::iterator it = toBeSold.begin();
			it != toBeSold.end(); ++it)
	{
		cash += it->sell();
	}
	cash -= cost;
	return cost;
}

unsigned int Player::declareBankruptcy()
{
	unsigned int repayment = 0;
	for(std::vector<std::shared_ptr<Property> >::it = properties.begin();
			it != properties.end(); ++it)
	{
		repayment += it->sell();
	}
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
	if (!owner)
	{
		if(p->wantBuy(name))
		{
			if(p->pay(toPay()))
				owner = p;
		}
	}
	else {
		owner->receive(p->pay(toPay()));
	}
}

MojaGrubaRyba::MojaGrubaRyba() : players(), fields(), die(), turn(0),
	currentPlayer(0), playersCount(0)
{

}

void MojaGrubaRyba::addComputerPlayer(MojaGrubaRyba::ComputerLevel level)
{
	if(players.size() >= MAX_PLAYERS)
		throw TooManyPlayersException();
	std::string newName = "Gracz ";
	newName += std::string(++playersCount);
	players.push_back(std::shared_ptr<Player>(new ComputerPlayer(newName, level)));
}

void MojaGrubaRyba::addHumanPlayer(std::shared_ptr<Human> human)
{
	if(players.size() >= MAX_PLAYERS)
		throw TooManyPlayersException();
	players.push_back(human);
}

void MojaGrubaRyba::makeTurn()
{
	for(std::vector<std::shared_ptr<Player> > it = players.begin();
			it != players.end(); ++i)
	{
		if(it->canMove())
		{
			it->movePassed();
			unsigned short moves = die.roll();
			for(unsigned short i = 0; it->canMove() && i < moves - 1; i++)
			{
				it->setPosition((it->getPosition() + 1) % fields.size());
				fields[it->getPosition()]->onPass(it);
			}
			if(it->canMove())
			{
				it->setPosition((it->getPosition() + 1) % fields.size());
				fields[it->getPosition()]->onStay(it);
			}
			if(!it->isActive())
			{
				--activePlayers;
			}
		} else
			it->movePassed();
	}
}

void MojaGrubaRyba::outputState()
{
	for(std::vector<std::shared_ptr<Player> > it = players.begin();
			it != players.end(); ++it)
	{
		if(it->canMove())
			printf("%s pole: %s gotowka: %u\n", it->getName(), fields[it->getPosition()]->getName(), it->getCash());
		else if(it->isActive())
			printf("%s pole: %s *** czekanie %u ***\n", it->getName(), fields[it->getPosition()]->getName(), it->getToWait());
		else
			printf("%s *** bankrut ***\n", it->getName());
	}
}

void MojaGrubaRyba::play(unsigned int rounds)
{
	if(!die)
		throw NoDieException();
	for(unsigned int i = 0; activePlayers >= MIN_PLAYERS && i < rounds; i++)
	{
		printf("Runda %u.\n", i);
		makeTurn();
		outputState();
	}
}
