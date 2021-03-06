#include <cmath> // sqrt

#include "Distribution.h"
#include "Random.h"
#include "Particle.h"
#include "Source.h"
#include "Point.h"


Particle_t Point_Source::getSource()
{
  	Point_t p = dist_dir->sample();
	p.normalize();
	
	Particle_t P( pos, p, dist_enrg->sample(), dist_time->sample() );
	return P;
}


Particle_t DiskX_Source::getSource()
{
  	Point_t p = dist_dir->sample();
	p.normalize();

	double z,y;
	// Rejection sampling: square --> circle
	do
	{
		z = 2.0 * Urand() - 1.0;
		y = 2.0 * Urand() - 1.0;
	}
	while ( z*z + y*y > 1.0 );

	z = z * r;
	y = y * r;
	Point_t pos( x0, y0 + y, z0 + z );
	
	Particle_t P( pos, p, dist_enrg->sample(), dist_time->sample() );
	return P;
}


Particle_t DiskZ_Source::getSource()
{
  	Point_t p = dist_dir->sample();
	p.normalize();

	double x,y;
	// Rejection sampling: square --> circle
	do
	{
		x = 2.0 * Urand() - 1.0;
		y = 2.0 * Urand() - 1.0;
	}
	while ( x*x + y*y > 1.0 );

	x = x * r;
	y = y * r;
	Point_t pos( x0 + x, y0 + y, z0 );
	
	Particle_t P( pos, p, dist_enrg->sample(), dist_time->sample() );
	return P;
}


Particle_t Sphere_Shell_Source::getSource()
{
  	Point_t p = dist_dir->sample();
	p.normalize();

	double x,y,z;
	// Rejection sampling: Cube --> Spherical shell
	do
	{
		x = 2.0 * Urand() - 1.0;
		y = 2.0 * Urand() - 1.0;
		z = 2.0 * Urand() - 1.0;
	}
	while ( ( x*x + y*y + z*z > 1.0 ) || ( x*x + y*y + z*z < risq ) );

	x = x * ro;
	y = y * ro;
	z = z * ro;

	Point_t pos( x0 + x, y0 + y, z + z0 );

	Particle_t P( pos, p, dist_enrg->sample(), dist_time->sample() );
	return P;
}


Particle_t Generic_Source::getSource() 
{
  	Point_t p = dist_dir->sample();
	p.normalize();
	
	Particle_t P ( dist_pos->sample(), p, dist_enrg->sample(), dist_time->sample() );
  	return P;
}

Eigenvalue_Source::Eigenvalue_Source( const int SW, const std::shared_ptr< Distribution_t<Point_t> > pos, const std::shared_ptr< Distribution_t<Point_t> > dir,
				const std::shared_ptr< Distribution_t<double> > enrg, const std::shared_ptr< Distribution_t<double> > time )
{
	for(int i = 0; i < SW; i++)
	{
		Point_t p = dir->sample();
		p.normalize();
		source_particle_bank.emplace( pos->sample(), p, enrg->sample(), time->sample() );
	}

	source_weight = SW;

	std::cout << "source_weight: " << source_weight << std::endl;

	source_particle_weight = 1.0;
}

void Eigenvalue_Source::update( std::stack <Particle_t>& F )
{
	source_particle_bank.swap( F );
	//std::cout << "new source bank size: " << source_particle_bank.size() << std::endl;
	source_particle_weight = source_weight / (double) source_particle_bank.size();
	//std::cout << "new source particle weight: " << source_particle_weight << std::endl;
	return;
}

Particle_t Eigenvalue_Source::getSource()
{
	Particle_t P = source_particle_bank.top();
	source_particle_bank.pop();
	P.setWeight(source_particle_weight);
	return P;
}