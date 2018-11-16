/*
  ==============================================================================

    Spring.cpp
    Created: 3 Aug 2018 3:44:01pm
    Author:  Theo

  ==============================================================================
*/

#include "Spring.h"
#include "../JuceLibraryCode/JuceHeader.h"

Spring::Spring(Particle* firstPoint, Particle* secondPoint, double length, double str, int index) :
	a(firstPoint),
	b(secondPoint),
	restingLength(length),
	strength(str),
	intervalIndex(index)
{

}

Particle* Spring::getA()
{
	return a;
}

Particle* Spring::getB()
{
	return b;
}

double Spring::getStrength()
{
	return strength;
}

int Spring::getIntervalIndex()
{
	return intervalIndex;
}

Spring::Ptr Spring::copy(void)
{
    Spring::Ptr copySpring = new Spring(a, b, restingLength, strength, intervalIndex);
	return copySpring;
}

bool Spring::compare(Spring* that)
{
	return (a->compare(that->getA()) &&
		b->compare(that->getB()) &&
		strength == that->getStrength());
}

void Spring::print()
{
	DBG("Point A:");
	a->print();
	DBG("\nPoint B:");
	b->print();
	DBG("Strength: " + String(strength));
}

void Spring::setStrength(double newStrength)
{
	strength = newStrength;
}

void Spring::satisfyConstraints(void)
{
    //int ao = a->getOctave(), bo = b->getOctave();
    
    double diff = b->getX() - a->getX();
    length = abs(diff);
    
    if (intervalIndex != 12) diff = fmod(diff, 1200.0);
    
	if (diff == 0.0) return;
    
    double maxStiffness = 0.1;
    double minStiffness = 0.0001;
    double meanStiffness = 0.0001 + 0.0999 * stiffness;

	double actualStrength = Utilities::clip(minStiffness, (meanStiffness * strength) / (1.0 - strength), maxStiffness);

    diff *= ( (diff - restingLength) / diff ) * actualStrength;

    if (!a->getLocked())
    {
        a->addX(diff);
    }

    if (!b->getLocked())
    {
        b->subX(diff);
    }

}

void Spring::update()
{
	/*
	Verlet function
	protected void update(boolean applyConstraints) {
        Vec2D delta = b.sub(a);
        // add minute offset to avoid div-by-zero errors
        float dist = delta.magnitude() + EPS;
        float normDistStrength = (dist - restLength)
                / (dist * (a.invWeight + b.invWeight)) * strength;
        if (!a.isLocked && !isALocked) {
            a.addSelf(delta.scale(normDistStrength * a.invWeight));
            if (applyConstraints) {
                a.applyConstraints();
            }
        }
        if (!b.isLocked && !isBLocked) {
            b.addSelf(delta.scale(-normDistStrength * b.invWeight));
            if (applyConstraints) {
                b.applyConstraints();
            }
        }
    }

	*/
}
