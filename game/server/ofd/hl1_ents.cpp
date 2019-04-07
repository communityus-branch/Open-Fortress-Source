//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "AI_BaseNPC.h"
#include "trains.h"
#include "ndebugoverlay.h"
#include "EntityList.h"
#include "engine/IEngineSound.h"
#include "hl1_ents.h"
#include "doors.h"
#include "soundent.h"
//#include "basegrenade.h"
#include "shake.h"
#include "globalstate.h"
#include "soundscape.h"
#include "buttons.h"
#include "sprite.h"
#include "actanimating.h"
#include "npcevent.h"
#include "func_break.h"
#include "tf_shareddefs.h"
#include "eventqueue.h"


/*********************
	TRIGGERS: trigger_auto, trigger_relay, multimanager: replaced in src by logic_auto and logic_relay
*********************/

// This trigger will fire when the level spawns (or respawns if not fire once)
// It will check a global state before firing.  
#define SF_AUTO_FIREONCE		0x0001

class CAutoTrigger : public CBaseEntity
{
	DECLARE_CLASS( CAutoTrigger, CBaseEntity );
public:
	void Spawn( void );
	void Precache( void );
	void Think( void );

	int ObjectCaps( void ) { return BaseClass::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	DECLARE_DATADESC();

private:

	COutputEvent	m_OnTrigger;
	string_t		m_globalstate;
};
LINK_ENTITY_TO_CLASS( trigger_auto, CAutoTrigger );

BEGIN_DATADESC( CAutoTrigger )
	DEFINE_KEYFIELD( m_globalstate, FIELD_STRING, "globalstate" ),

	// Outputs
	DEFINE_OUTPUT(m_OnTrigger, "OnTrigger"),
END_DATADESC()


void CAutoTrigger::Spawn( void )
{
	Precache();
}


void CAutoTrigger::Precache( void )
{
	SetNextThink( gpGlobals->curtime + 0.1f );
}


//-----------------------------------------------------------------------------
// Purpose: Checks the global state and fires targets if the global state is set.
//-----------------------------------------------------------------------------
void CAutoTrigger::Think( void )
{
	if ( !m_globalstate || GlobalEntity_GetState( m_globalstate ) == GLOBAL_ON )
	{
		m_OnTrigger.FireOutput(NULL, this);

		if ( m_spawnflags & SF_AUTO_FIREONCE )
			UTIL_Remove( this );
	}
}


#define SF_RELAY_FIREONCE		0x0001

class CTriggerRelay : public CBaseEntity
{
	DECLARE_CLASS( CTriggerRelay, CBaseEntity );
public:

	CTriggerRelay( void );

	bool KeyValue( const char *szKeyName, const char *szValue );
	void Spawn( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void RefireThink( void );

	int ObjectCaps( void ) { return BaseClass::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	DECLARE_DATADESC();

private:
	USE_TYPE	triggerType;
	float		m_flRefireInterval;
	float		m_flRefireDuration;
	float		m_flTimeRefireDone;

	USE_TYPE	m_TargetUseType;
	float		m_flTargetValue;

	COutputEvent m_OnTrigger;
};
LINK_ENTITY_TO_CLASS( trigger_relay, CTriggerRelay );

BEGIN_DATADESC( CTriggerRelay )
	DEFINE_FIELD( triggerType, FIELD_INTEGER ),
	DEFINE_KEYFIELD( m_flRefireInterval, FIELD_FLOAT, "repeatinterval" ),
	DEFINE_KEYFIELD( m_flRefireDuration, FIELD_FLOAT, "repeatduration" ),
	DEFINE_FIELD( m_flTimeRefireDone, FIELD_TIME ),
	DEFINE_FIELD( m_TargetUseType, FIELD_INTEGER ),
	DEFINE_FIELD( m_flTargetValue, FIELD_FLOAT ),

	// Function Pointers
	DEFINE_FUNCTION( RefireThink ),

	// Outputs
	DEFINE_OUTPUT(m_OnTrigger, "OnTrigger"),
END_DATADESC()

CTriggerRelay::CTriggerRelay( void )
{
	m_flRefireInterval = -1;
	m_flRefireDuration = -1;
	m_flTimeRefireDone = -1;
}

bool CTriggerRelay::KeyValue( const char *szKeyName, const char *szValue )
{
	if (FStrEq(szKeyName, "triggerstate"))
	{
		int type = atoi( szValue );
		switch( type )
		{
		case 0:
			triggerType = USE_OFF;
			break;
		case 2:
			triggerType = USE_TOGGLE;
			break;
		default:
			triggerType = USE_ON;
			break;
		}
	}
	else
	{
		return BaseClass::KeyValue( szKeyName, szValue );
	}

	return true;
}


void CTriggerRelay::Spawn( void )
{
}


void CTriggerRelay::RefireThink( void )
{
	// sending this as Activator and Caller right now. Seems the safest thing
	// since whatever fired the relay the first time may no longer exist. 
	Use( this, this, m_TargetUseType, m_flTargetValue ); 

	if( gpGlobals->curtime > m_flTimeRefireDone )
	{
		UTIL_Remove( this );
	}
	else
	{
		SetNextThink( gpGlobals->curtime + m_flRefireInterval );
	}
}

void CTriggerRelay::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	m_OnTrigger.FireOutput(pActivator, this);
	
	if ( m_spawnflags & SF_RELAY_FIREONCE )
	{
		UTIL_Remove( this );
	}

	else if( m_flRefireDuration != -1 && m_flTimeRefireDone == -1 )
	{
		// Set up to refire this target automatically
		m_TargetUseType = useType;
		m_flTargetValue = value;

		m_flTimeRefireDone = gpGlobals->curtime + m_flRefireDuration;
		SetThink( &CTriggerRelay::RefireThink );
		SetNextThink( gpGlobals->curtime + m_flRefireInterval );
	}
}


//**********************************************************
// The Multimanager Entity - when fired, will fire up to 16 targets 
// at specified times.

class CMultiManager : public CPointEntity
{
	DECLARE_CLASS( CMultiManager, CPointEntity );
public:

	bool KeyValue( const char *szKeyName, const char *szValue );
	void Spawn ( void );
	void ManagerThink ( void );
	void ManagerUse   ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

#if _DEBUG
	void ManagerReport( void );
#endif

	bool		HasTarget( string_t targetname );
	
	int ObjectCaps( void ) { return BaseClass::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	DECLARE_DATADESC();

	int			m_cTargets;	// the total number of targets in this manager's fire list.
	int			m_index;	// Current target
	float		m_flWait;
	EHANDLE		m_hActivator;
	float		m_startTime;// Time we started firing
	string_t	m_iTargetName	[ MAX_MULTI_TARGETS ];// list if indexes into global string array
	float		m_flTargetDelay [ MAX_MULTI_TARGETS ];// delay (in seconds) from time of manager fire to target fire

	COutputEvent m_OnTrigger;

	void InputManagerTrigger( inputdata_t &data );
};
LINK_ENTITY_TO_CLASS( multi_manager, CMultiManager );

// Global Savedata for multi_manager
BEGIN_DATADESC( CMultiManager )
	DEFINE_KEYFIELD( m_flWait, FIELD_FLOAT, "wait" ),

	DEFINE_FIELD( m_cTargets, FIELD_INTEGER ),
	DEFINE_FIELD( m_index, FIELD_INTEGER ),
	DEFINE_FIELD( m_hActivator, FIELD_EHANDLE ),
	DEFINE_FIELD( m_startTime, FIELD_TIME ),
	DEFINE_ARRAY( m_iTargetName, FIELD_STRING, MAX_MULTI_TARGETS ),
	DEFINE_ARRAY( m_flTargetDelay, FIELD_FLOAT, MAX_MULTI_TARGETS ),

	// Function Pointers
	DEFINE_FUNCTION( ManagerThink ),
	DEFINE_FUNCTION( ManagerUse ),
#if _DEBUG
	DEFINE_FUNCTION( ManagerReport ),
#endif

	// Outputs
	DEFINE_OUTPUT(m_OnTrigger, "OnTrigger"),
	DEFINE_INPUTFUNC( FIELD_VOID, "Trigger", InputManagerTrigger ),
END_DATADESC()

void CMultiManager::InputManagerTrigger( inputdata_t &data )
{
	ManagerUse ( NULL, NULL, USE_TOGGLE, 0 );
}

bool CMultiManager::KeyValue( const char *szKeyName, const char *szValue )
{
	if ( BaseClass::KeyValue( szKeyName, szValue ) )
	{
		return true;
	}
	else // add this field to the target list
	{
		// this assumes that additional fields are targetnames and their values are delay values.
		if ( m_cTargets < MAX_MULTI_TARGETS )
		{
			char tmp[128];

			UTIL_StripToken( szKeyName, tmp );
			m_iTargetName [ m_cTargets ] = AllocPooledString( tmp );
			m_flTargetDelay [ m_cTargets ] = atof (szValue);
			m_cTargets++;
		}
		else
		{
			return false;
		}
	}

	return true;
}


void CMultiManager::Spawn( void )
{
	SetSolid( SOLID_NONE );
	SetUse ( &CMultiManager::ManagerUse );
	SetThink ( &CMultiManager::ManagerThink);

	// Sort targets
	// Quick and dirty bubble sort
	int swapped = 1;

	while ( swapped )
	{
		swapped = 0;
		for ( int i = 1; i < m_cTargets; i++ )
		{
			if ( m_flTargetDelay[i] < m_flTargetDelay[i-1] )
			{
				// Swap out of order elements
				string_t name = m_iTargetName[i];
				float delay = m_flTargetDelay[i];
				m_iTargetName[i] = m_iTargetName[i-1];
				m_flTargetDelay[i] = m_flTargetDelay[i-1];
				m_iTargetName[i-1] = name;
				m_flTargetDelay[i-1] = delay;
				swapped = 1;
			}
		}
	}
}


bool CMultiManager::HasTarget( string_t targetname )
{ 
	for ( int i = 0; i < m_cTargets; i++ )
		if ( FStrEq(STRING(targetname), STRING(m_iTargetName[i])) )
			return true;
	
	return false;
}


// Designers were using this to fire targets that may or may not exist -- 
// so I changed it to use the standard target fire code, made it a little simpler.
void CMultiManager::ManagerThink ( void )
{
	float	t;

	t = gpGlobals->curtime - m_startTime;

	while ( m_index < m_cTargets && m_flTargetDelay[ m_index ] <= t )
	{
		FireTargets( STRING( m_iTargetName[ m_index ] ), m_hActivator, this, USE_TOGGLE, 0 );
		m_index++;
	}

	if ( m_index >= m_cTargets )// have we fired all targets?
	{
		SetThink( NULL );
		SetUse ( &CMultiManager::ManagerUse );// allow manager re-use 
	}
	else
	{
		SetNextThink( m_startTime + m_flTargetDelay[ m_index ] );
	}
}


// The USE function builds the time table and starts the entity thinking.
void CMultiManager::ManagerUse ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	m_hActivator = pActivator;
	m_index = 0;
	m_startTime = gpGlobals->curtime;

	m_OnTrigger.FireOutput(pActivator, this);
	
	// Calculate the time to re-enable the multimanager - just after the last output is fired.
	// dvsents2: need to disable multimanager until last output is fired
	//m_fEnableTime = gpGlobals->curtime + m_OnTrigger.GetMaxDelay();

	SetUse( NULL );// disable use until all targets have fired

	SetThink ( &CMultiManager::ManagerThink );
	SetNextThink( gpGlobals->curtime );
}


#if _DEBUG
void CMultiManager::ManagerReport ( void )
{
	int	cIndex;

	for ( cIndex = 0 ; cIndex < m_cTargets ; cIndex++ )
	{
		Msg( "%s %f\n", STRING(m_iTargetName[cIndex]), m_flTargetDelay[cIndex] );
	}
}
#endif


/**********************
	    Pendulum
/**********************/

#define SF_PENDULUM_SWING 2

LINK_ENTITY_TO_CLASS( func_pendulum, CPendulum );

BEGIN_DATADESC( CPendulum )
	DEFINE_FIELD( m_flAccel, FIELD_FLOAT ),
	DEFINE_FIELD( m_flTime, FIELD_TIME ),
	DEFINE_FIELD( m_flMaxSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( m_flDampSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( m_vCenter, FIELD_VECTOR ),
	DEFINE_FIELD( m_vStart, FIELD_VECTOR ),
	
	DEFINE_KEYFIELD( m_flMoveDistance, FIELD_FLOAT, "pendistance" ),
	DEFINE_KEYFIELD( m_flDamp, FIELD_FLOAT, "damp" ),
	DEFINE_KEYFIELD( m_flBlockDamage, FIELD_FLOAT, "dmg" ),
	
	DEFINE_FUNCTION( PendulumUse ),
	DEFINE_FUNCTION( Swing ),
	DEFINE_FUNCTION( Stop ),
	DEFINE_FUNCTION( RopeTouch ),

	DEFINE_FIELD( m_hEnemy, FIELD_EHANDLE ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Activate", InputActivate ),
END_DATADESC()

void CPendulum::Spawn( void )
{
	CBaseToggle::AxisDir();

	m_flDamp *=  0.001;
	
	if ( FBitSet ( m_spawnflags, SF_DOOR_PASSABLE ) )
		SetSolid( SOLID_NONE );
	else
		SetSolid( SOLID_BBOX );

	SetMoveType( MOVETYPE_PUSH );
	SetModel( STRING(GetModelName()) );

	if ( m_flMoveDistance != 0 )
	{
		if ( m_flSpeed == 0 )
			 m_flSpeed = 100;

		m_flAccel = ( m_flSpeed * m_flSpeed ) / ( 2 * fabs( m_flMoveDistance ));	// Calculate constant acceleration from speed and distance
		m_flMaxSpeed = m_flSpeed;
		m_vStart = GetAbsAngles();
		m_vCenter = GetAbsAngles() + ( m_flMoveDistance * 0.05 ) * m_vecMoveAng;

		if ( FBitSet( m_spawnflags, SF_BRUSH_ROTATE_START_ON ) )
		{		
			SetThink( &CBaseEntity::SUB_CallUseToggle );
			SetNextThink( gpGlobals->curtime + 0.1f );
		}

		m_flSpeed = 0;
		SetUse( &CPendulum::PendulumUse );

		VPhysicsInitShadow( false, false );
		///VPhysicsGetObject()->SetPosition( GetAbsOrigin(), pev->absangles );
	}

	if ( FBitSet( m_spawnflags, SF_PENDULUM_SWING ) )
	{
		SetTouch ( &CPendulum::RopeTouch );
	}
}


void CPendulum::PendulumUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( m_flSpeed )		// Pendulum is moving, stop it and auto-return if necessary
	{
		if ( FBitSet( m_spawnflags, SF_BRUSH_ROTATE_START_ON ) )
		{		
			float	delta;

			delta = CBaseToggle::AxisDelta( m_spawnflags, GetAbsAngles(), m_vStart );

			SetLocalAngularVelocity( m_flMaxSpeed * m_vecMoveAng );
			SetNextThink( gpGlobals->curtime + delta / m_flMaxSpeed);
			SetThink( &CPendulum::Stop );
		}
		else
		{
			m_flSpeed = 0;		// Dead stop
			SetThink( NULL );
			SetLocalAngularVelocity( QAngle( 0, 0, 0 ) );
		}
	}
	else
	{
		SetNextThink( gpGlobals->curtime + 0.1f );		// Start the pendulum moving
		m_flTime = gpGlobals->curtime;		// Save time to calculate dt
		SetThink( &CPendulum::Swing );
		m_flDampSpeed = m_flMaxSpeed;
	}
}

void CPendulum::InputActivate( inputdata_t &inputdata )
{
	SetNextThink( gpGlobals->curtime + 0.1f );		// Start the pendulum moving
	m_flTime = gpGlobals->curtime;				// Save time to calculate dt
	SetThink( &CPendulum::Swing );
	m_flDampSpeed = m_flMaxSpeed;
}

void CPendulum::Stop( void )
{
	SetAbsAngles( m_vStart );
	m_flSpeed = 0;
	SetThink( NULL );
	SetLocalAngularVelocity( QAngle ( 0, 0, 0 ) );
}


void CPendulum::Blocked( CBaseEntity *pOther )
{
	m_flTime = gpGlobals->curtime;
}

void CPendulum::Swing( void )
{
	float delta, dt;
	
	delta = CBaseToggle::AxisDelta( m_spawnflags, GetAbsAngles(), m_vCenter );
	dt = gpGlobals->curtime - m_flTime;	// How much time has passed?
	m_flTime = gpGlobals->curtime;		// Remember the last time called

	if ( delta > 0 && m_flAccel > 0 )
		m_flSpeed -= m_flAccel * dt;	// Integrate velocity
	else 
		m_flSpeed += m_flAccel * dt;

	if ( m_flSpeed > m_flMaxSpeed )
		 m_flSpeed = m_flMaxSpeed;
	else if ( m_flSpeed < -m_flMaxSpeed )
		m_flSpeed = -m_flMaxSpeed;

	// scale the destdelta vector by the time spent traveling to get velocity
	SetLocalAngularVelocity( m_flSpeed * m_vecMoveAng );

	// Call this again
	SetNextThink( gpGlobals->curtime + 0.1f );
	SetMoveDoneTime( 0.1 );
	
	if ( m_flDamp )
	{
		m_flDampSpeed -= m_flDamp * m_flDampSpeed * dt;
		if ( m_flDampSpeed < 30.0 )
		{
			SetAbsAngles( m_vCenter );
			m_flSpeed = 0;
			SetThink( NULL );
			SetLocalAngularVelocity( QAngle( 0, 0, 0 ) );
		}
		else if ( m_flSpeed > m_flDampSpeed )
			m_flSpeed = m_flDampSpeed;
		else if ( m_flSpeed < -m_flDampSpeed )
			m_flSpeed = -m_flDampSpeed;
	}
}

void CPendulum::Touch ( CBaseEntity *pOther )
{
	if ( m_flBlockDamage <= 0 )
		 return;

	// we can't hurt this thing, so we're not concerned with it
	if ( !pOther->m_takedamage )
		  return;

	// calculate damage based on rotation speed
	float damage = m_flBlockDamage * m_flSpeed * 0.01;

	if ( damage < 0 )
		 damage = -damage;

	pOther->TakeDamage( CTakeDamageInfo( this, this, damage, DMG_CRUSH ) );
	
	Vector vNewVel = (pOther->GetAbsOrigin() - GetAbsOrigin());
	
	VectorNormalize( vNewVel );

	pOther->SetAbsVelocity( vNewVel * damage );
}

void CPendulum::RopeTouch ( CBaseEntity *pOther )
{
	if ( !pOther->IsPlayer() )
	{// not a player!
		DevMsg ( 2, "Not a client\n" );
		return;
	}

	if ( pOther == GetEnemy() )
		 return;
	
	m_hEnemy = pOther;
	pOther->SetAbsVelocity( Vector ( 0, 0, 0 ) );
	pOther->SetMoveType( MOVETYPE_NONE );
}


/*********************
	    MORTARS
*********************/

class CFuncMortarField : public CBaseToggle
{
	DECLARE_CLASS( CFuncMortarField, CBaseToggle );
public:
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *pkvd );

	// Bmodels don't go across transitions
	virtual int	ObjectCaps( void ) { return CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

/*	virtual int	Save( CSave &save );
	virtual int	Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];*/

	//void EXPORT FieldUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	void InputTrigger ( inputdata_t &inputdata );

	DECLARE_DATADESC();

	string_t	m_iszXController;
	string_t	m_iszYController;
	float		m_flSpread;
	int			m_iCount;
	int			m_fControl;
};

LINK_ENTITY_TO_CLASS( func_mortar_field, CFuncMortarField );

BEGIN_DATADESC( CFuncMortarField )
	DEFINE_KEYFIELD( m_iszXController, FIELD_STRING, "m_iszXController" ),
	DEFINE_KEYFIELD( m_iszYController, FIELD_STRING, "m_iszYController" ),
	DEFINE_KEYFIELD( m_flSpread,		FIELD_FLOAT,   "m_flSpread" ),
	DEFINE_KEYFIELD( m_iCount,		FIELD_INTEGER, "m_iCount" ),
	DEFINE_KEYFIELD( m_fControl,		FIELD_INTEGER, "m_fControl" ),
	
	DEFINE_INPUTFUNC( FIELD_VOID,		"Trigger",		InputTrigger ),
END_DATADESC()


// Drop bombs from above
void CFuncMortarField::Spawn( void )
{
	SetSolid( SOLID_NONE );
	SetModel( STRING(GetModelName()) );    // set size and link into world
	SetMoveType( MOVETYPE_NONE );
	AddEffects( EF_NODRAW );
//	SetUse( FieldUse );
	Precache();
}


void CFuncMortarField::Precache( void )
{
	PrecacheModel( "sprites/lgtning.vmt" );

	PrecacheScriptSound( "MortarField.Trigger" );
}

void CFuncMortarField::InputTrigger( inputdata_t &inputdata )
{
	Vector vecStart;
	CollisionProp()->RandomPointInBounds( Vector( 0, 0, 1 ), Vector( 1, 1, 1 ), &vecStart );

	switch( m_fControl )
	{
	case 0:	// random
		break;
	case 1: // Trigger Activator
		if (inputdata.pActivator != NULL)
		{
			vecStart.x = inputdata.pActivator->GetAbsOrigin().x;
			vecStart.y = inputdata.pActivator->GetAbsOrigin().y;
		}
		break;
	case 2: // table
		{
			CBaseEntity *pController;

			if ( m_iszXController != NULL_STRING )
			{
				pController = gEntList.FindEntityByName( NULL, STRING(m_iszXController) );
				if (pController != NULL)
				{
					if ( FClassnameIs( pController, "momentary_rot_button" ) )
					{
						CMomentaryRotButton *pXController = static_cast<CMomentaryRotButton*>( pController );
						Vector vecNormalizedPos( pXController->GetPos( pXController->GetLocalAngles() ), 0.0f, 0.0f );
						Vector vecWorldSpace;
						CollisionProp()->NormalizedToWorldSpace( vecNormalizedPos, &vecWorldSpace );
						vecStart.x = vecWorldSpace.x;
					}
					else
					{
						DevMsg( "func_mortarfield has X controller that isn't a momentary_rot_button.\n" );
					}
				}
			}
			if ( m_iszYController != NULL_STRING )
			{
				pController = gEntList.FindEntityByName( NULL, STRING(m_iszYController) );
				if (pController != NULL)
				{
					if ( FClassnameIs( pController, "momentary_rot_button" ) )
					{
						CMomentaryRotButton *pYController = static_cast<CMomentaryRotButton*>( pController );
						Vector vecNormalizedPos( 0.0f, pYController->GetPos( pYController->GetLocalAngles() ), 0.0f );
						Vector vecWorldSpace;
						CollisionProp()->NormalizedToWorldSpace( vecNormalizedPos, &vecWorldSpace );
						vecStart.y = vecWorldSpace.y;
					}
					else
					{
						DevMsg( "func_mortarfield has Y controller that isn't a momentary_rot_button.\n" );
					}
				}
			}
		}
		break;
	}

	CPASAttenuationFilter filter( this, ATTN_NONE );
	EmitSound( filter, entindex(), "MortarField.Trigger" );	

	float t = 2.5;
	for (int i = 0; i < m_iCount; i++)
	{
		Vector vecSpot = vecStart;
		vecSpot.x += random->RandomFloat( -m_flSpread, m_flSpread );
		vecSpot.y += random->RandomFloat( -m_flSpread, m_flSpread );

		trace_t tr;
		UTIL_TraceLine( vecSpot, vecSpot + Vector( 0, 0, -1 ) * MAX_TRACE_LENGTH, MASK_SOLID_BRUSHONLY, this,  COLLISION_GROUP_NONE, &tr );

		CBaseEntity *pMortar = Create( "monster_mortar", tr.endpos, QAngle( 0, 0, 0 ), inputdata.pActivator );
		pMortar->SetNextThink( gpGlobals->curtime + t );
		t += random->RandomFloat( 0.2, 0.5 );

		if (i == 0)
			CSoundEnt::InsertSound ( SOUND_DANGER, tr.endpos, 400, 0.3 );
	}
}

#ifdef HL1_DLL

class CMortar : public CHL1BaseGrenade
{
	DECLARE_CLASS( CMortar, CHL1BaseGrenade );

public:
	void Spawn( void );
	void Precache( void );

	void MortarExplode( void );

	int m_spriteTexture;

	DECLARE_DATADESC();
};

BEGIN_DATADESC( CMortar )
	DEFINE_THINKFUNC( MortarExplode ),
	//DEFINE_FIELD( m_spriteTexture, FIELD_INTEGER ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( monster_mortar, CMortar );

void CMortar::Spawn( )
{
	SetMoveType( MOVETYPE_NONE );
	SetSolid( SOLID_NONE );

	SetDamage( 200 );
	SetDamageRadius( GetDamage() * 2.5 );

	SetThink( &CMortar::MortarExplode );
	SetNextThink( TICK_NEVER_THINK );

	Precache( );
}


void CMortar::Precache( )
{
	m_spriteTexture = PrecacheModel( "sprites/lgtning.vmt" );
}

void CMortar::MortarExplode( void )
{
	Vector vecStart	= GetAbsOrigin();
	Vector vecEnd	= vecStart;
	vecEnd.z += 1024;

	UTIL_Beam( vecStart, vecEnd, m_spriteTexture, 0, 0, 0, 0.5, 4.0, 4.0, 100, 0, 255, 160, 100, 128, 0 );

	trace_t tr;
	UTIL_TraceLine( GetAbsOrigin() + Vector( 0, 0, 1024 ), GetAbsOrigin() - Vector( 0, 0, 1024 ), MASK_ALL, this, COLLISION_GROUP_NONE, &tr );


	Explode( &tr, DMG_BLAST | DMG_MISSILEDEFENSE );
	UTIL_ScreenShake( tr.endpos, 25.0, 150.0, 1.0, 750, SHAKE_START );
}

#endif

//=========================================================
// Dead HEV suit prop
//=========================================================
class CNPC_DeadHEV : public CAI_BaseNPC
{
	DECLARE_CLASS( CNPC_DeadHEV, CAI_BaseNPC );
public:
	void Spawn( void );
	Class_T	Classify ( void ) { return	CLASS_NONE; }
	float MaxYawSpeed( void ) { return 8.0f; }

	bool KeyValue( const char *szKeyName, const char *szValue );

	int	m_iPose;// which sequence to display	-- temporary, don't need to save
	static char *m_szPoses[4];
};

char *CNPC_DeadHEV::m_szPoses[] = { "deadback", "deadsitting", "deadstomach", "deadtable" };

bool CNPC_DeadHEV::KeyValue( const char *szKeyName, const char *szValue )
{
	if ( FStrEq( szKeyName, "pose" ) )
		m_iPose = atoi( szValue );
	else 
		CAI_BaseNPC::KeyValue( szKeyName, szValue );

	return true;
}

LINK_ENTITY_TO_CLASS( monster_hevsuit_dead, CNPC_DeadHEV );

//=========================================================
// ********** DeadHEV SPAWN **********
//=========================================================
void CNPC_DeadHEV::Spawn( void )
{
	PrecacheModel("models/player.mdl");
	SetModel( "models/player.mdl" );

	ClearEffects();
	SetSequence( 0 );
	m_nBody				= 1;
	m_bloodColor		= BLOOD_COLOR_RED;

	SetSequence( LookupSequence( m_szPoses[m_iPose] ) );

	if ( GetSequence() == -1 )
	{
		Msg ( "Dead hevsuit with bad pose\n" );
		SetSequence( 0 );
		ClearEffects();
		AddEffects( EF_BRIGHTLIGHT );
	}

	// Corpses have less health
	m_iHealth			= 8;

	NPCInitDead();
}


//
// Render parameters trigger
//
// This entity will copy its render parameters (renderfx, rendermode, rendercolor, renderamt)
// to its targets when triggered.
//


// Flags to indicate masking off various render parameters that are normally copied to the targets
#define SF_RENDER_MASKFX	(1<<0)
#define SF_RENDER_MASKAMT	(1<<1)
#define SF_RENDER_MASKMODE	(1<<2)
#define SF_RENDER_MASKCOLOR	(1<<3)

class CRenderFxManager : public CBaseEntity
{
	DECLARE_CLASS( CRenderFxManager, CBaseEntity );
public:
	void	Spawn( void );
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	// Input handlers.
	void	InputActivate( inputdata_t &inputdata );

	DECLARE_DATADESC();
};

BEGIN_DATADESC( CRenderFxManager )
	DEFINE_INPUTFUNC( FIELD_VOID, "Activate", InputActivate ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( env_render, CRenderFxManager );


void CRenderFxManager::Spawn( void )
{
	AddSolidFlags( FSOLID_NOT_SOLID );
	SetMoveType( MOVETYPE_NONE );
	AddEffects( EF_NODRAW );
}


void CRenderFxManager::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( m_target != NULL_STRING )
	{
		CBaseEntity *pEntity = NULL;
		while ( ( pEntity = gEntList.FindEntityByName( pEntity, STRING( m_target ) ) ) != NULL )
		{
			if ( !HasSpawnFlags( SF_RENDER_MASKFX ) )
				pEntity->m_nRenderFX = m_nRenderFX;
			if ( !HasSpawnFlags( SF_RENDER_MASKAMT ) )
				pEntity->SetRenderColorA( GetRenderColor().a );
			if ( !HasSpawnFlags( SF_RENDER_MASKMODE ) )
				pEntity->m_nRenderMode = m_nRenderMode;
			if ( !HasSpawnFlags( SF_RENDER_MASKCOLOR ) )
				pEntity->m_clrRender = m_clrRender;
		}
	}
}


void CRenderFxManager::InputActivate( inputdata_t &inputdata )
{
	Use( inputdata.pActivator, inputdata.pCaller, USE_ON, 0 );
}


// Link env_sound to soundscape system
LINK_ENTITY_TO_CLASS( env_sound, CEnvSoundscape );

//=========================================================
// WaitTillLand - in order to emit their meaty scent from
// the proper location, gibs should wait until they stop 
// bouncing to emit their scent. That's what this function
// does.
//=========================================================
void CHL1Gib::WaitTillLand ( void )
{
	if ( !IsInWorld() )
	{
		UTIL_Remove( this );
		return;
	}

	if ( GetAbsVelocity() == vec3_origin )
	{
	/*	SetRenderColorA( 255 );
		m_nRenderMode = kRenderTransTexture;
		AddSolidFlags( FSOLID_NOT_SOLID );*/
		
		SetNextThink( gpGlobals->curtime + m_lifeTime );
		SetThink ( &CBaseEntity::SUB_FadeOut );

		// If you bleed, you stink!
	/*	if ( m_bloodColor != DONT_BLEED )
		{
			// ok, start stinkin!
			CSoundEnt::InsertSound ( bits_SOUND_MEAT, pev->origin, 384, 25 );
		}*/
	}
	else
	{
		// wait and check again in another half second.
		SetNextThink( gpGlobals->curtime + 0.5 );
	}
}

//
// Gib bounces on the ground or wall, sponges some blood down, too!
//
void CHL1Gib::BounceGibTouch ( CBaseEntity *pOther )
{
	Vector	vecSpot;
	trace_t	tr;
	
	if ( GetFlags() & FL_ONGROUND)
	{
		SetAbsVelocity( GetAbsVelocity() * 0.9 );

		SetAbsAngles( QAngle( 0, GetAbsAngles().y, 0 ) );
		SetLocalAngularVelocity( QAngle( 0, GetLocalAngularVelocity().y, 0 ) );
	}
	else
	{
		if ( m_cBloodDecals > 0 && m_bloodColor != DONT_BLEED )
		{
			vecSpot = GetAbsOrigin() + Vector ( 0 , 0 , 8 );//move up a bit, and trace down.
			UTIL_TraceLine ( vecSpot, vecSpot + Vector ( 0, 0, -24 ),  MASK_SOLID_BRUSHONLY, this, COLLISION_GROUP_NONE, &tr);

			UTIL_BloodDecalTrace( &tr, m_bloodColor );

			m_cBloodDecals--; 
		}

		if ( m_material != matNone && random->RandomInt( 0, 2 ) == 0 )
		{
			float volume;
			float zvel = fabs( GetAbsVelocity().z );
		
			volume = 0.8 * min(1.0, ((float)zvel) / 450.0);

			CBreakable::MaterialSoundRandom( entindex(), (Materials)m_material, volume );
		}
	}
}

//
// Sticky gib puts blood on the wall and stays put. 
//
void CHL1Gib::StickyGibTouch ( CBaseEntity *pOther )
{
	Vector	vecSpot;
	trace_t	tr;
	
	SetThink ( &CHL1Gib::SUB_Remove );
	SetNextThink( gpGlobals->curtime + 10 );

	if ( !FClassnameIs( pOther, "worldspawn" ) )
	{
		SetNextThink( gpGlobals->curtime );
		return;
	}

	UTIL_TraceLine ( GetAbsOrigin(), GetAbsOrigin() + GetAbsVelocity() * 32,  MASK_SOLID_BRUSHONLY, this, COLLISION_GROUP_NONE, &tr );

	UTIL_BloodDecalTrace( &tr, m_bloodColor );

	SetAbsVelocity( tr.plane.normal * -1 );

	QAngle qAngle;

	VectorAngles( GetAbsVelocity(), qAngle );
	SetAbsAngles( qAngle );

	SetAbsVelocity( vec3_origin );
	SetLocalAngularVelocity( QAngle( 0, 0, 0 ) );
	SetMoveType( MOVETYPE_NONE );
}

//
// Throw a chunk
//
void CHL1Gib::Spawn( const char *szGibModel )
{
	SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE );

	SetFriction( 0.55 ); // deading the bounce a bit
	
	// sometimes an entity inherits the edict from a former piece of glass,
	// and will spawn using the same render FX or rendermode! bad!
	SetRenderColorA( 255 );
	m_nRenderMode = kRenderNormal;
	m_nRenderFX = kRenderFxNone;
	SetSolid( SOLID_BBOX );
	AddSolidFlags( FSOLID_NOT_STANDABLE );
	SetClassname( "gib" );

	SetModel( szGibModel );
	UTIL_SetSize( this, Vector( 0, 0, 0), Vector(0, 0, 0));

	SetNextThink( gpGlobals->curtime + 4 );

	m_lifeTime = 25;

	SetThink ( &CHL1Gib::WaitTillLand );
	SetTouch ( &CHL1Gib::BounceGibTouch );

	m_material = matNone;
	m_cBloodDecals = 5;// how many blood decals this gib can place (1 per bounce until none remain). 
}

LINK_ENTITY_TO_CLASS( hl1gib, CHL1Gib );

BEGIN_DATADESC( CHL1Gib )
	// Function Pointers
	DEFINE_FUNCTION( BounceGibTouch ),
	DEFINE_FUNCTION( StickyGibTouch ),
	DEFINE_FUNCTION( WaitTillLand ),

	DEFINE_FIELD( m_bloodColor, FIELD_INTEGER ),
	DEFINE_FIELD( m_cBloodDecals, FIELD_INTEGER ),
	DEFINE_FIELD( m_material, FIELD_INTEGER ),
	DEFINE_FIELD( m_lifeTime, FIELD_FLOAT ),
END_DATADESC()

#define SF_ENDSECTION_USEONLY		0x0001

class CTriggerEndSection : public CBaseEntity
{
	DECLARE_CLASS( CTriggerEndSection, CBaseEntity );

public:
	void Spawn( void );
	void InputEndSection( inputdata_t &data  );
	
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS( trigger_endsection, CTriggerEndSection );

BEGIN_DATADESC( CTriggerEndSection )
	DEFINE_INPUTFUNC( FIELD_VOID, "EndSection", InputEndSection ),
END_DATADESC()

void CTriggerEndSection::Spawn( void )
{
	if ( gpGlobals->deathmatch )
	{
		UTIL_Remove( this );
		return;
	}
}

void CTriggerEndSection::InputEndSection( inputdata_t &data )
{
	CBaseEntity *pPlayer = UTIL_GetLocalPlayer();

	if ( pPlayer )
	{
		//HACKY MCHACK - This works, but it's nasty. Alfred is going to fix a
		//bug in gameui that prevents you from dropping to the main menu after
		// calling disconnect.
		 engine->ClientCommand ( pPlayer->edict(), "toggleconsole;disconnect\n");
	}

	UTIL_Remove( this );
}
