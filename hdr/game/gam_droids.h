#pragma once

#include "hdr/system/sys_main.h"

typedef struct
{
	bool 			isAlive;
	int 			droidType;
	int 			currentHealth;
	int 			wayPointIndex;
	int 			wayPointDirection;
	std::string 	spriteName;
	int 			currentFrame;
	float 			frameDelay;
	glm::vec2   	renderOffset;

	float 			currentSpeed;
	float           acceleration;

	cpVect 			worldPos;
	cpVect          serverWorldPos;     // Where the server thinks we are
	cpVect          viewWorldPos;
	cpVect          middlePosition;   // TODO set middle position for droids

	cpVect          destinationCoords;          // This is the line segment end point
	cpVect          destDirection;              // Which way is the droid heading
	cpVect          velocity;                   // Current speed

	/*
	cpVect acceleration;

	cpVect screenPos;
	cpVect previousWaypoints;        // This is the line segment start point


	float mass;                     // Used for collision response
//
// Weapon
	bool weaponCanFire;
	float weaponDelay;
//
// States
	bool isStopped;
	bool hasCollided;
	bool isAlive;
	bool visibleToPlayer;
	bool isExploding;
// Animation
	float currentFrameDelay;
	int currentFrame;

// Pathfinding
	int aStarPathIndex;            // Index into which path to use
	int ai_moveMode;
	int numberPathNodes;
	int currentAStarIndex;         // Index into aStarWaypoints array
	bool aStarDestinationFound;
	bool aStarInitDone;

// AI variables
	int ai_currentState;
	int ai_nextState;
	float ai_noActionCounter;
	float ai_noActionCount;
	int collidedWith;    // Who did the droid hit
	bool ignoreCollisions;
	float ignoreCollisionsCounter;
	int collisionCount;        // how many collision have occured to ignore them
	bool witnessShooting;
	bool witnessTransfer;
	float chanceToShoot;
	bool beenShotByPlayer;
	float beenShotCountdown;
	float witnessShootingCountDown;
	float witnessTransferCountDown;
	int targetIndex;        // Which droid shot this droid

	cpVect originPosition;     // Remember this to return to
	bool foundOriginPath;
	bool returnToOrigin;
	bool foundOriginPosition;
	bool onHealingTile;
	bool foundHealingTile;
	float healingCount;
	bool foundWPTile;        // Go here after healing

	bool onFleeTile;
	bool foundFleeTile;

	bool isNotPatrolling;        // Used to enter resume branch
	bool onResumeDestTile;
	bool destSet;                // Does the droid have a destination
	cpVect destinationTile;        // Where is the droid heading
	bool resumeDestFound;        // Set from running thread - aStar found destination
	bool resumeThreadStarted;    // Is the thread started and running
	ALLEGRO_THREAD *threadPathFind;        // Handle to thread running aStar function

	cpBody *body;                // Used for physics and collisions
	cpShape *shape;
	*/
} _droid;

// Setup droid information for this level
void drd_setupLevel ( std::string levelName );

// Render the droids for this level
void drd_renderThisLevel ( std::string levelName, float interpolate );

// Animate the droid
void drd_animateThisLevel ( std::string levelName );