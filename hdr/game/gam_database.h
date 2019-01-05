#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/io/simpleIni/io_simpleIni.h"

typedef struct
{
	int      	maxHealth;
	float       maxSpeed;
	float    	accelerate;
	int      	score;
	int      	bounceDamage;
	bool     	canShoot;
	int      	bulletType;
	int      	chanceToShoot;
	int      	bulletDamage;
	bool     	disrupterImmune;
	float    	rechargeTime;
	int      	tokenCount;
	int			imageIndex;
	string	 	dbImageFileName;
	string    	height;
	string     	weight;

	string     	description;
	string     	className;
	string     	drive;
	string     	brain;
	string     	weapon;
	string     	sensor1;
	string     	sensor2;
	string     	sensor3;
	string     	notes;
} _dataBaseEntry;

extern vector<_dataBaseEntry>      dataBaseEntry;

// Get and build up information for droid database files
bool gam_getDBInformation();
