//
//  grubaryba_example.cc
//  grubaryba
//

#include <iostream>
#include <memory>
#include "mojagrubaryba.h"

class ExampleDie : public Die {
public:
    unsigned short roll() const {
        static unsigned int rollNum = 0;
        unsigned short rolls[] = { 1, 1, 1, 2, 1, 3 };
        return rolls[rollNum++ % 6];
    }
    std::shared_ptr<Die> clone() const {
        return std::shared_ptr<Die>(new ExampleDie());
    }
};

class VeryDumbHuman : public Human {
    std::string name;
public:
    VeryDumbHuman(std::string const& name_): name(name_) {}

    std::string const& getName() const { return name; }
    bool wantBuy(std::string const& property) { return false; }
    bool wantSell(std::string const& property) { return false; }
    std::shared_ptr<Human> clone() const {
        return std::shared_ptr<Human>(new VeryDumbHuman(name));
    }
};

int main(int argc, const char * argv[]) {
    std::shared_ptr<MojaGrubaRyba> mgr = std::shared_ptr<MojaGrubaRyba>(new MojaGrubaRyba());
    std::shared_ptr<Die> die = std::shared_ptr<Die>(new ExampleDie());
    std::shared_ptr<Human> albin = std::shared_ptr<Human>(new VeryDumbHuman("Albina"));
    mgr->setDie(die);
    mgr->addComputerPlayer(GrubaRyba::ComputerLevel::DUMB);
    mgr->addComputerPlayer(GrubaRyba::ComputerLevel::SMARTASS);
    mgr->addHumanPlayer(albin);
    mgr->play(100);
    return 0;
}