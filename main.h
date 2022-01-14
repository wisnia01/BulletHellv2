#pragma once

void CheckPlanePosition(SDL_Rect& planeC);

void BulletMovingCircleEnemy(int  circleEnemyBulletRadius[20], SDL_Rect  circleEnemyBulletC[20][20], SDL_Rect& circleEnemyC);

void BulletMovingNonstaticEnemy(SDL_Rect  nonstaticEnemyBulletC[20], SDL_Rect  nonstaticEnemyBulletDest[20], SDL_Rect& nonstaticEnemyC, double worldTime, SDL_Rect& planeC);

void BulletMovingMovingEnemy(SDL_Rect  movingEnemyBulletC[20], double worldTime, bool& splash, SDL_Rect& splashSpot, SDL_Rect& movingEnemyC, int& movingEnemySplashRadius, SDL_Rect  movingEnemySplashC[20]);

void DestroyCircleEnemy(int circleEnemyHealth, bool& destroyedSwitch, double& enemyDestroyedTimer, bool& immunity, double& immunityTimer, int& enemyCurrentAnim, SDL_Rect& nonstaticEnemyC, SDL_Rect  nonstaticEnemyBulletC[20], SDL_Rect  nonstaticEnemyBulletDest[20], int& nonstaticEnemyHealth, int& stage);

void DestroyNonstaticEnemy(int nonstaticEnemyHealth, bool& destroyedSwitch, double& enemyDestroyedTimer, bool& immunity, double& immunityTimer, int& enemyCurrentAnim, SDL_Rect& movingEnemyC, SDL_Rect  movingEnemyBulletC[20], int& movingEnemyHealth, int& stage);

void DestroyMovingEnemy(int movingEnemyHealth, bool& destroyedSwitch, double& enemyDestroyedTimer, bool& immunity, double& immunityTimer, int& enemyCurrentAnim, int& menu);

void EntityMoving(int slowdown, SDL_Rect& planeC, int velocityX, int velocityY, int& planeCurrentAnim, int stage, SDL_Rect& movingEnemyC, int& movingVelocityX);

void CollisionCircleEnemy(SDL_Rect& planeCenter, SDL_Rect& circleEnemyC, bool& immunity, double& immunityTimer, int& planeHealth, int& points, SDL_Rect  circleEnemyBulletC[20][20], SDL_Rect  planeBulletC[20], int& circleEnemyHealth);

void CollisionNonstaticEnemy(SDL_Rect& planeCenter, SDL_Rect& nonstaticEnemyC, bool& immunity, double& immunityTimer, int& planeHealth, int& points, SDL_Rect  nonstaticEnemyBulletC[20], SDL_Rect  planeBulletC[20], int& nonstaticEnemyHealth);

void CreateCircleAbility(bool& circleUltimateAbilityTimerReset, double& circleUltimateAbilityTimer, SDL_Rect& circleUltimateArea, SDL_Surface* screen, SDL_Rect& camera, int czerwony, SDL_Rect& planeCenter, bool& circleUltimateAbility);

void CreateBonusHealth(bool& bonusHealthTimerReset, double& bonusHealthTimer, SDL_Rect& bonusHealthArea, SDL_Surface* screen, SDL_Rect& camera, int czerwony, int zielony, SDL_Rect& planeCenter, int& planeHealth, bool& bonusHealth);

void CheckActualLetter(int points, int& gradeCurrentAnim, char& gradeLetter);

void CollisionMovingEnemy(SDL_Rect& planeCenter, SDL_Rect& movingEnemyC, bool& immunity, double& immunityTimer, int& planeHealth, int& points, SDL_Rect  movingEnemyBulletC[20], SDL_Rect  planeBulletC[20], int& movingEnemyHealth);
