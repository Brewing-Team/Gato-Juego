#include "Physics.h"
#include "Input.h"
#include "App.h"
#include "Log.h"
#include "math.h"
#include "Defs.h"
#include "Log.h"
#include "Render.h"
#include "Player.h"
#include "Window.h"

#ifdef __linux__
#include <SDL_keycode.h>
#include <Box2D/Box2D.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include "External/Optick/include/optick.h"
#include <Box2D/Dynamics/b2Body.h>
#elif _MSC_VER
#include "SDL/include/SDL_keycode.h"
#include "Box2D/Box2D/Box2D.h"
#include "Optick/include/optick.h"

// Tell the compiler to reference the compiled Box2D libraries
#ifdef _DEBUG
#pragma comment( lib, "../Game/Source/External/Box2D/libx86/DebugLib/Box2D.lib" )
#else
#pragma comment( lib, "../Game/Source/External/Box2D/libx86/ReleaseLib/Box2D.lib" )
#endif
#endif


Physics::Physics() : Module()
{
	// Initialise all the internal class variables, at least to NULL pointer
	world = NULL;
	mouseJoint = NULL;
}

// Destructor
Physics::~Physics()
{
	// You should do some memory cleaning here, if required
}

bool Physics::Start()
{
	LOG("Creating Physics 2D environment");

	// Create a new World
	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));

	// Set this module as a listener for contacts
	world->SetContactListener(this);

	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	return true;
}

// 
bool Physics::PreUpdate()
{
	// OPTICK PROFILIN
	OPTICK_EVENT();

	bool ret = true;

	// Step (update) the World
	// WARNING: WE ARE STEPPING BY CONSTANT 1/60 SECONDS!
	world->Step(1.0f / 60.0f, 6, 2);

	// Because Box2D does not automatically broadcast collisions/contacts with sensors, 
	// we have to manually search for collisions and "call" the equivalent to the ModulePhysics::BeginContact() ourselves...
	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		// For each contact detected by Box2D, see if the first one colliding is a sensor
		if (c->IsTouching() && c->GetFixtureA()->IsSensor())
		{
			// If so, we call the OnCollision listener function (only of the sensor), passing as inputs our custom PhysBody classes
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureB()->GetBody()->GetUserData();
			
			if (pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return ret;
}

PhysBody* Physics::CreateRectangle(int x, int y, int width, int height, bodyType type)
{
	b2BodyDef body;

	if (type == DYNAMIC) body.type = b2_dynamicBody;
	if (type == STATIC) body.type = b2_staticBody;
	if (type == KINEMATIC) body.type = b2_kinematicBody;

	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	b->ResetMassData();

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;

	return pbody;
}

PhysBody* Physics::CreateCircle(int x, int y, int radious, bodyType type)
{
	// Create BODY at position x,y
	b2BodyDef body;

	if (type == DYNAMIC) body.type = b2_dynamicBody;
	if (type == STATIC) body.type = b2_staticBody;
	if (type == KINEMATIC) body.type = b2_kinematicBody;

	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	// Add BODY to the world
	b2Body* b = world->CreateBody(&body);

	// Create SHAPE
	b2CircleShape circle;
	circle.m_radius = PIXEL_TO_METERS(radious);

	// Create FIXTURE
	b2FixtureDef fixture;
	fixture.shape = &circle;
	fixture.density = 1.0f;
	b->ResetMassData();

	// Add fixture to the BODY
	b->CreateFixture(&fixture);

	// Create our custom PhysBody class
	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = radious * 0.5f;
	pbody->height = radious * 0.5f;

	// Return our PhysBody class
	return pbody;
}

PhysBody* Physics::CreateRectangleSensor(int x, int y, int width, int height, bodyType type)
{
	// Create BODY at position x,y
	b2BodyDef body;
	if (type == DYNAMIC) body.type = b2_dynamicBody;
	if (type == STATIC) body.type = b2_staticBody;
	if (type == KINEMATIC) body.type = b2_kinematicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	// Add BODY to the world
	b2Body* b = world->CreateBody(&body);

	// Create SHAPE
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	// Create FIXTURE
	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	// Add fixture to the BODY
	b->CreateFixture(&fixture);

	// Create our custom PhysBody class
	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width;
	pbody->height = height;

	// Return our PhysBody class
	return pbody;
}

PhysBody* Physics::CreateChain(int x, int y, int* points, int size, bodyType type)
{
	// Create BODY at position x,y
	b2BodyDef body;
	if (type == DYNAMIC) body.type = b2_dynamicBody;
	if (type == STATIC) body.type = b2_staticBody;
	if (type == KINEMATIC) body.type = b2_kinematicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	// Add BODY to the world
	b2Body* b = world->CreateBody(&body);

	// Create SHAPE
	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];
	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}
	shape.CreateLoop(p, size / 2);

	// Create FIXTURE
	b2FixtureDef fixture;
	fixture.shape = &shape;

	// Add fixture to the BODY
	b->CreateFixture(&fixture);

	// Clean-up temp array
	delete p;

	// Create our custom PhysBody class
	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;

	// Return our PhysBody class
	return pbody;
}

// 
bool Physics::PostUpdate()
{

	// OPTICK PROFILIN
	OPTICK_EVENT();

	bool ret = true;
	
	//  Iterate all objects in the world and draw the bodies
	if (debug)
	{
		for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
		{
			for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
			{
				switch (f->GetType())
				{
					// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					uint width, height;
					app->win->GetWindowSize(width, height);
					b2Vec2 pos = f->GetBody()->GetPosition();
					app->render->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->GetVertexCount();
					b2Vec2 prev, v;

					for (int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->GetVertex(i));
						if (i > 0)
							app->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 255, 100);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->GetVertex(0));
					app->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for (int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if (i > 0)
							app->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					app->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					app->render->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
				}
				break;
				}

				// DONE 1: If mouse button 1 is pressed ...
				// App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN
				// test if the current body contains mouse position
				if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
				{
					// test if the current body contains mouse position
					b2Vec2 p = { PIXEL_TO_METERS(app->input->GetMouseX()) + PIXEL_TO_METERS(-app->render->camera.x), PIXEL_TO_METERS(app->input->GetMouseY()) + PIXEL_TO_METERS(-app->render->camera.y) };
					if (f->GetShape()->TestPoint(b->GetTransform(), p) == true)
					{

						// If a body was selected we will attach a mouse joint to it
						// so we can pull it around

						// The variable "b2Body* mouse_body;" is defined in the header ModulePhysics.h 
						// We need to keep this body throughout several game frames; you cannot define it as a local variable here. 
						mouseBody = b;

						// Get current mouse position
						b2Vec2 mousePosition;
						mousePosition.x = p.x;
						mousePosition.y = p.y;

						// Define new mouse joint
						b2MouseJointDef def;
						def.bodyA = ground; // First body must be a static ground
						def.bodyB = mouseBody; // Second body will be the body to attach to the mouse
						def.target = mousePosition; // The second body will be pulled towards this location
						def.dampingRatio = 0.5f; // Play with this value
						def.frequencyHz = 2.0f; // Play with this value
						def.maxForce = 200.0f * mouseBody->GetMass(); // Play with this value

						// Add the new mouse joint into the World
						mouseJoint = (b2MouseJoint*)world->CreateJoint(&def);
					}
				}
			}
		}

		// If a body was selected we will attach a mouse joint to it
		// so we can pull it around
		// DONE 2: If a body was selected, create a mouse joint
		// using mouse_joint class property
		if (mouseBody != nullptr && mouseJoint != nullptr)
		{
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
			{
				// Get new mouse position and re-target mouse_joint there
				b2Vec2 mousePosition = { PIXEL_TO_METERS(app->input->GetMouseX()) + PIXEL_TO_METERS(-app->render->camera.x), PIXEL_TO_METERS(app->input->GetMouseY()) + PIXEL_TO_METERS(-app->render->camera.y) };
				mouseJoint->SetTarget(mousePosition);

				// DONE 3: If the player keeps pressing the mouse button, update
				// target position and draw a red line between both anchor points
				// Draw a red line between both anchor points
				app->render->DrawLine(METERS_TO_PIXELS(mouseBody->GetPosition().x), METERS_TO_PIXELS(mouseBody->GetPosition().y), METERS_TO_PIXELS(mousePosition.x), METERS_TO_PIXELS(mousePosition.y), 255, 0, 0);
			}
		}

		// DONE 4: If the player releases the mouse button, destroy the joint
		if (mouseBody != nullptr && mouseJoint != nullptr)
		{
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
			{
				// Tell Box2D to destroy the mouse_joint
				world->DestroyJoint(mouseJoint);

				// DO NOT FORGET THIS! We need it for the "if (mouse_body != nullptr && mouse_joint != nullptr)"
				mouseJoint = nullptr;
				mouseBody = nullptr;
			}
		}
	}


	return ret;
}

// Called before quitting
bool Physics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}

// Callback function to collisions with Box2D
void Physics::BeginContact(b2Contact* contact)
{
	// Call the OnCollision listener function to bodies A and B, passing as inputs our custom PhysBody classes
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if (physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if (physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}

void Physics::EndContact(b2Contact* contact)
{
	// Call the OnCollision listener function to bodies A and B, passing as inputs our custom PhysBody classes
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if (physA && physA->listener != NULL)
		physA->listener->EndCollision(physA, physB);

	if (physB && physB->listener != NULL)
		physB->listener->EndCollision(physB, physA);

}

float Physics::lookAt(b2Vec2 source, b2Vec2 target)
{
	return atan2(
		target.x - source.x, 
		target.y - source.y
	);
}

//--------------- PhysBody

void PhysBody::GetPosition(int& x, int& y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x) - (width);
	y = METERS_TO_PIXELS(pos.y) - (height);
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = x2 - x1;
			float fy = y2 - y1;
			float dist = sqrtf((fx * fx) + (fy * fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return output.fraction * dist;
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

b2RevoluteJoint* Physics::CreateRevoluteJoint(PhysBody* A, b2Vec2 anchorA, PhysBody* B, b2Vec2 anchorB, float lowerAngle, float upperAngle, float referenceAngle, bool collideConnected, bool enableLimit)
{
	b2RevoluteJointDef revoluteJointDef;
	revoluteJointDef.bodyA = A->body;
	revoluteJointDef.bodyB = B->body;
	revoluteJointDef.collideConnected = collideConnected;
	revoluteJointDef.localAnchorA.Set(anchorA.x, anchorA.y);
	revoluteJointDef.localAnchorB.Set(anchorB.x, anchorB.y);
	revoluteJointDef.referenceAngle = DEGTORAD * referenceAngle;
	revoluteJointDef.enableLimit = enableLimit;
	revoluteJointDef.lowerAngle = -DEGTORAD * lowerAngle;
	revoluteJointDef.upperAngle = DEGTORAD * upperAngle;

	return (b2RevoluteJoint*)world->CreateJoint(&revoluteJointDef);
}
b2PrismaticJoint* Physics::CreatePrismaticJoint(PhysBody* A, b2Vec2 anchorA, PhysBody* B, b2Vec2 anchorB, b2Vec2 axys, float maxHeight, bool collideConnected, bool enableLimit)
{
	b2PrismaticJointDef prismaticJointDef;
	prismaticJointDef.bodyA = A->body;
	prismaticJointDef.bodyB = B->body;
	prismaticJointDef.collideConnected = collideConnected;
	prismaticJointDef.localAxisA.Set(axys.x, axys.y);
	prismaticJointDef.localAnchorA.Set(anchorA.x, anchorA.y);
	prismaticJointDef.localAnchorB.Set(anchorB.x, anchorB.y);
	prismaticJointDef.referenceAngle = 0;
	prismaticJointDef.enableLimit = enableLimit;
	prismaticJointDef.lowerTranslation = -0.01;
	prismaticJointDef.upperTranslation = maxHeight;

	return (b2PrismaticJoint*)world->CreateJoint(&prismaticJointDef);
}

b2WeldJoint* Physics::CreateWeldJoint(PhysBody* A, b2Vec2 anchorA, PhysBody* B, b2Vec2 anchorB, float angle, bool collideConnected, bool enableLimit)
{
	b2WeldJointDef weldJointDef;
	weldJointDef.bodyA = A->body;
	weldJointDef.bodyB = B->body;
	weldJointDef.collideConnected = collideConnected;
	weldJointDef.localAnchorA.Set(anchorA.x, anchorA.y);
	weldJointDef.localAnchorB.Set(anchorB.x, anchorB.y);
	weldJointDef.referenceAngle = 0;

	return (b2WeldJoint*)world->CreateJoint(&weldJointDef);
}

b2Body** Physics::CreateRope(int length){

	b2Body** segments = new b2Body*[length];
	b2RevoluteJoint** joints = new b2RevoluteJoint*[length - 1];
	b2RopeJoint** ropeJoints = new b2RopeJoint*[length - 1];

	b2BodyDef* bodyDef = new b2BodyDef();
	bodyDef->type = b2_dynamicBody;

	float width = 0.1f, height = 0.25f;

	b2PolygonShape* shape = new b2PolygonShape();
	shape->SetAsBox(width / 2, height / 2);

	for (int i = 0; i < length; i++)
	{
		segments[i] = world->CreateBody(bodyDef);
		segments[i]->CreateFixture(shape, 1.0f);
	}

	delete shape;
	shape = nullptr;

	b2RevoluteJointDef* jointDef = new b2RevoluteJointDef();
	jointDef->localAnchorA.y = -height / 2;
	jointDef->localAnchorB.y = height / 2;

	for (int i = 0; i < length - 1; i++)
	{
		jointDef->bodyA = segments[i];
		jointDef->bodyB = segments[i + 1];
		joints[i] = (b2RevoluteJoint*)world->CreateJoint(jointDef);
	}

	b2RopeJointDef* ropeJointDef = new b2RopeJointDef();
	ropeJointDef->localAnchorA.Set(0, -height / 2);
	ropeJointDef->localAnchorB.Set(0, height / 2);
	ropeJointDef->maxLength = height;

	for (int i = 0; i < length - 1; i++)
	{
		ropeJointDef->bodyA = segments[i];
		ropeJointDef->bodyB = segments[i + 1];
		ropeJoints[i] = (b2RopeJoint*)world->CreateJoint(ropeJointDef);
	}

	return segments;
}

b2Body** Physics::CreateRope(int length, b2Vec2 startPos){
	    b2Body** segments = new b2Body*[length];
    b2RevoluteJoint** joints = new b2RevoluteJoint*[length + 1];
    b2RopeJoint** ropeJoints = new b2RopeJoint*[length - 1];

    b2BodyDef* bodyDef = new b2BodyDef();
    bodyDef->type = b2_dynamicBody;

    float width = 0.1f, height = height = 0.25f;

    b2PolygonShape* shape = new b2PolygonShape();
    shape->SetAsBox(width / 2, height / 2);

    for (int i = 0; i < length; i++)
    {
        segments[i] = world->CreateBody(bodyDef);
        segments[i]->CreateFixture(shape, 1.0f);
    }

    delete shape;
    shape = nullptr;

    b2BodyDef anchorBodyDef;
    anchorBodyDef.type = b2_staticBody;

    anchorBodyDef.position = startPos;
    b2Body* startAnchor = world->CreateBody(&anchorBodyDef);

    b2RevoluteJointDef* jointDef = new b2RevoluteJointDef();
    jointDef->localAnchorA.y = -height / 2;
    jointDef->localAnchorB.y = height / 2;

    jointDef->bodyA = startAnchor;
    jointDef->bodyB = segments[0];
    joints[0] = (b2RevoluteJoint*)world->CreateJoint(jointDef);

    for (int i = 0; i < length - 1; i++)
    {
        jointDef->bodyA = segments[i];
        jointDef->bodyB = segments[i + 1];
        joints[i + 1] = (b2RevoluteJoint*)world->CreateJoint(jointDef);
    }

    b2RopeJointDef* ropeJointDef = new b2RopeJointDef();
    ropeJointDef->localAnchorA.Set(0, -height / 2);
    ropeJointDef->localAnchorB.Set(0, height / 2);
    ropeJointDef->maxLength = height;

    for (int i = 0; i < length - 1; i++)
    {
        ropeJointDef->bodyA = segments[i];
        ropeJointDef->bodyB = segments[i + 1];
        ropeJoints[i] = (b2RopeJoint*)world->CreateJoint(ropeJointDef);
    }

    return segments;
}

b2Body** Physics::CreateRope(int length, b2Vec2 startPos, b2Vec2 endPos)
{
    b2Body** segments = new b2Body*[length];
    b2RevoluteJoint** joints = new b2RevoluteJoint*[length + 1];
    b2RopeJoint** ropeJoints = new b2RopeJoint*[length - 1];

    b2BodyDef* bodyDef = new b2BodyDef();
    bodyDef->type = b2_dynamicBody;

    float width = 0.1f, height = (endPos - startPos).Length() / length;

    b2PolygonShape* shape = new b2PolygonShape();
    shape->SetAsBox(width / 2, height / 2);

    for (int i = 0; i < length; i++)
    {

        bodyDef->position = startPos + (endPos - startPos);
		bodyDef->position *= (i / (float)length);
        segments[i] = world->CreateBody(bodyDef);
        segments[i]->CreateFixture(shape, 1.0f);
    }

    delete shape;
    shape = nullptr;

    b2BodyDef anchorBodyDef;
    anchorBodyDef.type = b2_staticBody;

    anchorBodyDef.position = startPos;
    b2Body* startAnchor = world->CreateBody(&anchorBodyDef);

    anchorBodyDef.position = endPos;
    b2Body* endAnchor = world->CreateBody(&anchorBodyDef);

    b2RevoluteJointDef* jointDef = new b2RevoluteJointDef();
    jointDef->localAnchorA.y = -height / 2;
    jointDef->localAnchorB.y = height / 2;

    jointDef->bodyA = startAnchor;
    jointDef->bodyB = segments[0];
    joints[0] = (b2RevoluteJoint*)world->CreateJoint(jointDef);

    for (int i = 0; i < length - 1; i++)
    {
        jointDef->bodyA = segments[i];
        jointDef->bodyB = segments[i + 1];
        joints[i + 1] = (b2RevoluteJoint*)world->CreateJoint(jointDef);
    }

    jointDef->bodyA = segments[length - 1];
    jointDef->bodyB = endAnchor;
    joints[length] = (b2RevoluteJoint*)world->CreateJoint(jointDef);

    b2RopeJointDef* ropeJointDef = new b2RopeJointDef();
    ropeJointDef->localAnchorA.Set(0, -height / 2);
    ropeJointDef->localAnchorB.Set(0, height / 2);
    ropeJointDef->maxLength = height;

    for (int i = 0; i < length - 1; i++)
    {
        ropeJointDef->bodyA = segments[i];
        ropeJointDef->bodyB = segments[i + 1];
        ropeJoints[i] = (b2RopeJoint*)world->CreateJoint(ropeJointDef);
    }

    return segments;
}
