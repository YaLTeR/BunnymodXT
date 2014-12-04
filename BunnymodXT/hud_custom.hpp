#pragma once

namespace CustomHud
{
	void Init();
	void VidInit();
	void Draw(float flTime);

	void UpdatePlayerInfo(float vel[3], float org[3]);
	void UpdatePlayerInfoInaccurate(float vel[3], float org[3]);
};

// For insertion into the HUDLIST.
class CHudCustom_Wrapper : public CHudBase
{
public:
	virtual int Init();

	virtual int VidInit()
	{
		CustomHud::VidInit();
		return 1;
	}

	virtual int Draw(float flTime)
	{
		CustomHud::Draw(flTime);
		return 1;
	}

	void InitIfNecessary()
	{
		if (!m_Initialized)
			Init();
	}

protected:
	bool m_Initialized = false;
};

class CHudCustom_Wrapper_NoVD : public CHudBase_NoVD
{
public:
	virtual int Init();

	virtual int VidInit()
	{
		CustomHud::VidInit();
		return 1;
	}

	virtual int Draw(float flTime)
	{
		CustomHud::Draw(flTime);
		return 1;
	}

	void InitIfNecessary()
	{
		if (!m_Initialized)
			Init();
	}

protected:
	bool m_Initialized = false;
};
