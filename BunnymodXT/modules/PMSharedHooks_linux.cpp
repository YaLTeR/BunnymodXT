#include "../stdafx.hpp"

#include "../modules.hpp"

static bool serverside = false;

extern "C" void __cdecl PM_Move(struct playermove_s* ppmove, int server)
{
	serverside = static_cast<bool>(server);
	if (serverside)
		return ServerDLL::HOOKED_PM_Move(ppmove, server);
	else
		return ClientDLL::HOOKED_PM_Move(ppmove, server);
}

extern "C" void __cdecl PM_PlayerMove(qboolean server)
{
	if (serverside)
		return ServerDLL::HOOKED_PM_PlayerMove(server);
	else
		return ClientDLL::HOOKED_PM_PlayerMove(server);
}

extern "C" void __cdecl PM_PreventMegaBunnyJumping()
{
	if (serverside)
		return ServerDLL::HOOKED_PM_PreventMegaBunnyJumping();
	else
		return ClientDLL::HOOKED_PM_PreventMegaBunnyJumping();
}

extern "C" void __cdecl PM_Jump()
{
	if (serverside)
		return ServerDLL::HOOKED_PM_Jump();
	else
		return ClientDLL::HOOKED_PM_Jump();
}

extern "C" int __cdecl PM_ClipVelocity(float in[3], float normal[3], float out[3], float overbounce)
{
	if (serverside)
		return ServerDLL::HOOKED_PM_ClipVelocity(in, normal, out, overbounce);
	else
		return ClientDLL::HOOKED_PM_ClipVelocity(in, normal, out, overbounce);
}

extern "C" void __cdecl PM_WaterMove()
{
	if (serverside)
		return ServerDLL::HOOKED_PM_WaterMove();
	else
		return ClientDLL::HOOKED_PM_WaterMove();
}
