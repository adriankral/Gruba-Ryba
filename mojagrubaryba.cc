#include <vector>
#include "mojagrubaryba.h"
#include "tests/grubaryba.h" //FIXME remember to change the path at the end!

using namespace std;

void Property::onStep(Player *p)
{
	if (owner == NULL) ; //zapytaj o kupno
	else {
		p->pay(toPay());
		owner->receive(toPay());
	}
}

