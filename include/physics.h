/*
 * physics.h
 *
 *  Created on: Dec 10, 2019
 *      Author: little
 */

#ifndef LIB_INCLUDE_PHYSICS_H_
#define LIB_INCLUDE_PHYSICS_H_






namespace little
{


/*
class Physics
{

};
*/


static auto VerticalVelocity = [](float dt)
{
	const float G = 9.8f; // Acceleration of gravity

	return G * dt;
};


}



#endif /* LIB_INCLUDE_PHYSICS_H_ */
