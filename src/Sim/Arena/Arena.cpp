#include "Arena.h"
#include "../../RLConst.h"

Car* Arena::AddCar(Team team, const CarConfig& config) {
	Car* car = Car::_AllocateCar();
	_carsList.push_back(car);

	car->config = config;
	car->team = team;
	car->id = ++_lastCarID;

	{ // Set up rigidbody and collision shapes
		car->_childHitboxShape = new btBoxShape((config.hitboxSize * UU_TO_BT) / 2);
		car->_compoundShape = new btCompoundShape();

		btTransform hitboxOffsetTransform = btTransform();
		hitboxOffsetTransform.setIdentity();
		hitboxOffsetTransform.setOrigin(config.hitboxPosOffset * UU_TO_BT);
		car->_compoundShape->addChildShape(hitboxOffsetTransform, car->_childHitboxShape);

		btVector3 localInertia(0, 0, 0);
		car->_compoundShape->calculateLocalInertia(RLConst::CAR_MASS_BT, localInertia);

		btRigidBody::btRigidBodyConstructionInfo rbInfo
			= btRigidBody::btRigidBodyConstructionInfo(RLConst::CAR_MASS_BT, NULL, car->_compoundShape, localInertia);

		btTransform carTransform = btTransform();
		carTransform.setIdentity();
		rbInfo.m_startWorldTransform = carTransform;

		car->_rigidBody = new btRigidBody(rbInfo);
		car->_rigidBody->setDeactivationTime(FLT_MAX);
	}

	// Add rigidbody to world
	_bulletWorld->addRigidBody(car->_rigidBody);

	{ // Set up actual vehicle stuff
		car->_bulletVehicleRaycaster = new btDefaultVehicleRaycaster(_bulletWorld);

		btVehicleRL::btVehicleTuning tuning = btVehicleRL::btVehicleTuning();

		car->_bulletVehicle = new btVehicleRL(tuning, car->_rigidBody, car->_bulletVehicleRaycaster);

		// Match RL with X forward, Y right, Z up
		car->_bulletVehicle->setCoordinateSystem(1, 2, 0);

		// Set up wheel directions with RL coordinate system
		btVector3 wheelDirectionCS(0, 0, -1), wheelAxleCS(0, -1, 0);

		{ // Set up wheels
			for (int i = 0; i < 4; i++) {
				bool front = i < 2;
				bool left = i % 2;

				float x = front ? 51.25f : -33.75f;
				float y = left ? 29.5f : -29.5f;
				float rayStartZ = 20.755f;

				// These numbers??? where they from???
				float wheelRestZ = front ? -4.f : -2.3f;

				float radius = front ? 12.5f : 15.f;
				btVector3 wheelRestOffset = btVector3(x, y, wheelRestZ) * UU_TO_BT;
				btVector3 wheelRayStartOffset = btVector3(x, y, rayStartZ) * UU_TO_BT;

				float suspensionRestLength = wheelRayStartOffset.z() - wheelRestOffset.z();

				car->_bulletVehicle->addWheel(
					wheelRayStartOffset,
					wheelDirectionCS, wheelAxleCS,
					suspensionRestLength,
					radius * UU_TO_BT, tuning, true);

				{ // Fix wheel info data
					using namespace RLConst::BTVehicle;

					float suspensionScale = 
						front ? SUSPENSION_FORCE_SCALE_FRONT : SUSPENSION_FORCE_SCALE_BACK;

					btWheelInfoRL& wheelInfo = car->_bulletVehicle->m_wheelInfo[i];
					wheelInfo.m_suspensionStiffness =		SUSPENSION_STIFFNESS * suspensionScale;
					wheelInfo.m_wheelsDampingCompression =	WHEELS_DAMPING_COMPRESSION * suspensionScale;
					wheelInfo.m_wheelsDampingRelaxation =	WHEELS_DAMPING_RELAXATION * suspensionScale;
					wheelInfo.m_maxSuspensionTravelCm = (MAX_SUSPENSION_TRAVEL * UU_TO_BT) * 100; // Same for all cars (hopefully)
					wheelInfo.m_maxSuspensionForce = FLT_MAX; // Don't think there's a limit
					wheelInfo.m_bIsFrontWheel = front;

					wheelInfo.m_frictionSlip = FRICTION_SLIP_AMOUNT;
				}
			}
		}
	}

	return car;
}

bool Arena::RemoveCar(Car* car) {
	auto sizeBefore = _carsList.size();
	_carsList.remove(car);
	if (_carsList.size() < sizeBefore) {
		// Car was removed, free it up
		delete car;
		return true;
	} else {
		// Car wasn't found
		return false;
	}
}

Car* Arena::GetCarFromID(uint32_t id) {
	for (Car* car : _carsList)
		if (car->id == id)
			return car;

	return NULL;
}

void Arena::RegisterGoalScoreCallback(GoalScoreEventFn callbackFunc) {
	_goalScoreCallbacks.push_back(callbackFunc);
}

Arena::Arena(GameMode gameMode) {
	this->gameMode = gameMode;

	{ // Initialize ball
		ball = Ball::_AllocBall();

		float radius;
		switch (gameMode) {
		default:
			radius = RLConst::BALL_COLLISION_RADIUS_NORMAL;
			break;
		}

		ball->_collisionShape = new btSphereShape(radius);

		btRigidBody::btRigidBodyConstructionInfo constructionInfo = 
			btRigidBody::btRigidBodyConstructionInfo(RLConst::BALL_MASS_BT, NULL, ball->_collisionShape);

		constructionInfo.m_startWorldTransform.setIdentity();
		constructionInfo.m_startWorldTransform.setOrigin(btVector3(0, 0, radius));

		// TODO: Set other values in constructionInfo to match RL

		ball->_rigidBody = new btRigidBody(constructionInfo);
	}

	{ // Initialize world

		 _bulletWorldParams.collisionConfig = new btDefaultCollisionConfiguration();
		 _bulletWorldParams.collisionDispatcher = new btCollisionDispatcher(_bulletWorldParams.collisionConfig);
		 _bulletWorldParams.constraintSolver = new btSequentialImpulseConstraintSolver;
		 _bulletWorldParams.overlappingPairCache = new btDbvtBroadphase();

		_bulletWorld = new btDiscreteDynamicsWorld(
			_bulletWorldParams.collisionDispatcher,
			_bulletWorldParams.overlappingPairCache,
			_bulletWorldParams.constraintSolver,
			_bulletWorldParams.collisionConfig
		);

		_bulletWorld->setGravity(btVector3(0, 0, RLConst::GRAVITY_Z * UU_TO_BT));
	}

	// Add ball to world
	_bulletWorld->addRigidBody(ball->_rigidBody);
}

void Arena::Step(int ticksToSimulate) {
	for (int i = 0; i < ticksToSimulate; i++) {
		for (Car* car : _carsList)
			car->_PreTickUpdate();

		// Update world
		_bulletWorld->stepSimulation(TICKTIME, 0, TICKTIME);

		for (Car* car : _carsList)
			car->_PostTickUpdate();
	}
}

Arena::~Arena() {

	// Delete world first
	delete _bulletWorld;

	{ // Delete world param things
		delete _bulletWorldParams.collisionConfig;
		delete _bulletWorldParams.collisionDispatcher;
		delete _bulletWorldParams.overlappingPairCache;
		delete _bulletWorldParams.constraintSolver;
	}

	// Remove all cars
	for (Car* car : _carsList)
		delete car;

	{ // Delete collision RBs and shapes
		for (btRigidBody* colRB : _worldCollisionRBs)
			delete colRB;

		for (btCollisionShape* colObject : _worldCollisionShapes)
			delete colObject;
	}

	// Remove ball
	delete ball;
}
