#include "Tire.h"

Tire::Tire(b2World* world) {
    b2BodyDef bodyDef;
    bodyDef.position.Set(XPOS/RATIO, YPOS/RATIO);
    bodyDef.type = b2_dynamicBody;
    m_body = world->CreateBody(&bodyDef);

    b2PolygonShape polygonShape;
    polygonShape.SetAsBox( 0.5f*CAR_WIDTH/RATIO, 1.25f*CAR_WIDTH/RATIO, b2Vec2(0,0), -90*DEGTORAD);
    b2Fixture* fixture = m_body->CreateFixture(&polygonShape, 1);//shape, density
    fixture->SetUserData( new CarTireFUD() );

    m_body->SetUserData("Tire");

    m_currentTraction = 1;
}

Tire::~Tire() {
    m_body->GetWorld()->DestroyBody(m_body);
}

void Tire::setCharacteristics(float maxForwardSpeed, 
                                float maxBackwardSpeed, 
                                float maxDriveForce, 
                                float maxLateralImpulse) {
    m_maxForwardSpeed   = maxForwardSpeed;
    m_maxBackwardSpeed  = maxBackwardSpeed;
    m_maxDriveForce     = maxDriveForce;
    m_maxLateralImpulse = maxLateralImpulse;
}

void Tire::updateFriction() {
    b2Vec2 impulse = m_body->GetMass() * -getLateralVelocity();
    if (impulse.Length() > m_maxLateralImpulse) {
        impulse *= m_maxLateralImpulse / impulse.Length();
    }
    m_body->ApplyLinearImpulse( m_currentTraction * impulse, m_body->GetWorldCenter(), 1);
    m_body->ApplyAngularImpulse( m_currentTraction * 0.1f * m_body->GetInertia() * -m_body->GetAngularVelocity(), 1 );
    b2Vec2 currentForwardNormal = getForwardVelocity();
    float currentForwardSpeed = currentForwardNormal.Normalize();
    float dragForceMagnitude = -2 * currentForwardSpeed;
    m_body->ApplyForce( m_currentTraction * dragForceMagnitude * currentForwardNormal, m_body->GetWorldCenter(), 1 );
}

void Tire::updateDrive(int controlState) {
    float desiredSpeed = 0;
    switch (controlState & (TDC_UP|TDC_DOWN)) {
        case UP:   desiredSpeed = m_maxForwardSpeed;  break;
        case DOWN: desiredSpeed = m_maxBackwardSpeed; break;
        default: return;//do nothing
    }
    
    b2Vec2 currentForwardNormal = m_body->GetWorldVector( b2Vec2(-1,0) );
    float currentSpeed = b2Dot( getForwardVelocity(), currentForwardNormal );
    float force = 0;
    if (desiredSpeed > currentSpeed) {
        force = m_maxDriveForce;
    } else {
        if (desiredSpeed < currentSpeed) {
            force = -m_maxDriveForce;
        } else {
            return; 
        }
    }
    m_body->ApplyForce( m_currentTraction * force * currentForwardNormal, m_body->GetWorldCenter(), 1 );
}

void Tire::updateTurn(int controlState) {
    float desiredTorque = 0;
    switch ( controlState & (TDC_LEFT|TDC_RIGHT) ) {
        case LEFT:  desiredTorque = 15;  break;
        case RIGHT: desiredTorque = -15; break;
        default: ;//nothing
    }
    m_body->ApplyTorque( desiredTorque, 1 );
}

b2Vec2 Tire::getLateralVelocity() {
    b2Vec2 currentRightNormal = m_body->GetWorldVector( b2Vec2(0,1) );
    return b2Dot( currentRightNormal, m_body->GetLinearVelocity() ) * currentRightNormal;
}

b2Vec2 Tire::getForwardVelocity() {
    b2Vec2 currentForwardNormal = m_body->GetWorldVector( b2Vec2(-1, 0) );
    return b2Dot( currentForwardNormal, m_body->GetLinearVelocity() ) * currentForwardNormal;
}