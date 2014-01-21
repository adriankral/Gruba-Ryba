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
		unsigned int repayment = declareBancruptcy();
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
		p->pay(toPay());
		owner->receive(toPay());
	}
}

