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
			if(p->pay(toPay()) == cost)
			{
				owner = p;
			}
		}
	}
	else {
		owner.lock()->receive(p->pay(toPay()));
	}
}
 
MojaGrubaRyba::MojaGrubaRyba() : players(), fields(), die(), turn(0),
	currentPlayer(0), activePlayers(0)
{
	fields.push_back(shared_ptr<Field>( dynamic_pointer_cast<Field>( shared_ptr<Start>(new Start(50)))));
	fields.push_back(shared_ptr<Field>( dynamic_pointer_cast<Field>( shared_ptr<Coral>(new Coral("Anemonia", 160)))));
	fields.push_back(shared_ptr<Field>( new Field("Wyspa") ));
	fields.push_back(shared_ptr<Field>( dynamic_pointer_cast<Field>( shared_ptr<Coral>(new Coral("Aporina", 220)))));
	fields.push_back(shared_ptr<Field>( dynamic_pointer_cast<Field>( shared_ptr<Aquarium>(new Aquarium("Akwarium", 3)))));
	fields.push_back(shared_ptr<Field>( dynamic_pointer_cast<Field>( shared_ptr<PublicUse>(new PublicUse("Grota", 300)))));
	fields.push_back(shared_ptr<Field>( dynamic_pointer_cast<Field>( shared_ptr<Coral>(new Coral("Menella", 280)))));
	fields.push_back(shared_ptr<Field>( dynamic_pointer_cast<Field>( shared_ptr<Deposite>(new Deposite("Laguna", 15)))));
	fields.push_back(shared_ptr<Field>( dynamic_pointer_cast<Field>( shared_ptr<PublicUse>(new PublicUse("Statek", 250)))));
	fields.push_back(shared_ptr<Field>( dynamic_pointer_cast<Field>( shared_ptr<Prize>(new Prize("Błazenki", 120)))));
	fields.push_back(shared_ptr<Field>( dynamic_pointer_cast<Field>( shared_ptr<Coral>(new Coral("Pennatula", 400)))));
	fields.push_back(shared_ptr<Field>( dynamic_pointer_cast<Field>( shared_ptr<Punishment>(new Punishment("Rekin", 180)))));
	
	
	//DUM DUM DUUUM
	/*
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
}

void MojaGrubaRyba::addComputerPlayer(MojaGrubaRyba::ComputerLevel level)
{
	if(players.size() >= MAX_PLAYERS)
		throw TooManyPlayersException(MAX_PLAYERS);
	
	shared_ptr<Player> compPlayer;
	if(level == ComputerLevel::DUMB)
		compPlayer = dynamic_pointer_cast<Player>(shared_ptr<ComputerDUMB> (new ComputerDUMB(STARTPOS, STARTCASH, ++activePlayers)));
	else if(level == ComputerLevel::SMARTASS)
		compPlayer = dynamic_pointer_cast<Player>(shared_ptr<ComputerSMARTASS> (new ComputerSMARTASS(STARTPOS, STARTCASH, ++activePlayers)));
	players.push_back(compPlayer);
	activePlayers++;
}

void MojaGrubaRyba::addHumanPlayer(std::shared_ptr<Human> human) 
{
	if(players.size() >= MAX_PLAYERS)
		throw TooManyPlayersException(MAX_PLAYERS);
	
	shared_ptr<HumanPlayer> hp(new HumanPlayer(human, STARTPOS, STARTCASH, ++activePlayers));
	shared_ptr<Player> p = dynamic_pointer_cast<Player>(hp);
	players.push_back(p);
	
}

void MojaGrubaRyba::makeTurn()
{
	for(auto it = players.begin(); it != players.end(); ++it)
	{
		if((*it)->canMove())
		{
			unsigned short moves = die->roll();
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
			}
		}
			
		(*it)->movePassed();
	}
}

/* FIXME trzeba coś zrobić z getname. Prawdopodobnie trzeba tu użyć dynamic casta. Proponuję żeby "ComputerPlayer" 
 * posiadał wirtualną metodę getName i player w zależności od tego czy jest komputerem czy graczem żeby wywoływał metodę
 * po odpowiednim caście.
 */
void MojaGrubaRyba::outputState() //FIXME
{
	for(auto it = players.begin(); it != players.end(); ++it)
	{
		shared_ptr<HumanPlayer> hp = dynamic_pointer_cast<HumanPlayer>(*it);
		shared_ptr<ComputerPlayer> cp = dynamic_pointer_cast<ComputerPlayer>(*it);

		if((*it)->canMove())
			std::cout<<(*it)->getName()<<" pole: "<<fields[(*it)->getPosition()]->getName()<<" gotowka: "<<(*it)->getCash()<<std::endl;
		else if((*it)->isActive())
			std::cout<<(*it)->getName()<<" pole: "<<fields[(*it)->getPosition()]->getName()<<" *** czekanie "<<(*it)->getToWait()<<" ***"<<std::endl;
		else
			std::cout<<(*it)->getName()<<" *** bankrut ***"<<std::endl;

/*
		if(cp)
		{
			if((*it)->canMove())
				std::cout<<cp->getName()<<" pole: "<<fields[(*it)->getPosition()]->getName()<<" gotowka: "<<(*it)->getCash()<<std::endl;
			else if((*it)->isActive())
				std::cout<<cp->getName()<<" pole: "<<fields[(*it)->getPosition()]->getName()<<" *** czekanie "<<(*it)->getToWait()<<" ***"<<std::endl;
			else
				std::cout<<cp->getName()<<" *** bankrut ***"<<std::endl;
		}
		else
		{
			if((*it)->canMove())
				std::cout<<hp->getName()<<" pole: "<<fields[(*it)->getPosition()]->getName()<<" gotowka: "<<(*it)->getCash()<<std::endl;
			else if((*it)->isActive())
				std::cout<<hp->getName()<<" pole: "<<fields[(*it)->getPosition()]->getName()<<" *** czekanie "<<(*it)->getToWait()<<" ***"<<std::endl;
			else
				std::cout<<hp->getName()<<" *** bankrut ***"<<std::endl;
		}
*/
		
	}
}


void MojaGrubaRyba::play(unsigned int rounds)
{
	if(!die)
		throw NoDieException();
	if(activePlayers < MIN_PLAYERS)
		throw TooFewPlayersException(MIN_PLAYERS);
	for(unsigned int i = 0; i < rounds; i++) //activePlayers >= MIN_PLAYERS zbedne - i tak rzucamy wyjatek w sytuacji przeciwnej
	{
		printf("Runda %u.\n", i);
		makeTurn();
		//outputState();
	}
}
