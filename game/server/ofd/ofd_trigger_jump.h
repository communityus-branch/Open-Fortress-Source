#ifndef OFD_TRIGGER_JUMP_H
#define OFD_TRIGGER_JUMP_H


#include "cbase.h"
#include "triggers.h"


class COFDTriggerJump : public CBaseTrigger
{
public:
	DECLARE_CLASS( COFDTriggerJump, CBaseTrigger );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	COFDTriggerJump();
	virtual void Spawn();
	void Precache();

	void StartTouch( CBaseEntity *pOther );

	void DrawDebugGeometryOverlays( void )
	{
		if ( m_debugOverlays & OVERLAY_BBOX_BIT )
		{
			NDebugOverlay::Line( GetAbsOrigin(), m_vecTarget, 0, 255, 0, true, 0.0f );
		}
		BaseClass::DrawDebugGeometryOverlays();
	}

	bool SoftLanding( void ) const { return m_iSoftLanding == 1; }

protected:
	string_t m_szTarget;
	EHANDLE m_pTarget;
	bool m_iSoftLanding;

	//float m_flApexBoost;

	COutputEvent	m_OnJump;

	CNetworkVar( Vector, m_vecTarget );
	CNetworkVar( float, m_flApexBoost );
	CNetworkVar( int, m_iSound );

	CountdownTimer m_launchCooldowns[ MAX_PLAYERS + 1 ];
};

#endif
