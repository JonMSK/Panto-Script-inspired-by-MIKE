int tick{ 0 };
Vehicle vehArray[50]{};
Ped pedArray[50]{};
int numUsedElements{};

void pantoScript(bool enable)
{
	if (enable)
	{
		if (tick >= 1000 && numUsedElements <= 50)
		{
			Hash hash = MISC::GET_HASH_KEY("emerus");
			g_Logger->Info("Got Hash Key");
			if (!STREAMING::HAS_MODEL_LOADED(hash))
			{
				STREAMING::REQUEST_MODEL(hash);
				g_Logger->Info("Requested Model");
				Sleep(0);
			}
			NativeVector3 pos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
			g_Logger->Info("Got coords");
			float forward = -10.f;
			float heading = ENTITY::GET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID());
			float xVector = forward * sin(degToRad(heading)) * -1.f;
			float yVector = forward * cos(degToRad(heading));
			g_Logger->Info("Got other calculations");

			g_CallbackScript->AddCallback<ModelCallback>((hash), [=] {
				g_Logger->Info("Started Callback");
				*(unsigned short*)g_GameVariables->m_ModelSpawnBypass = 0x9090;
				g_Logger->Info("Bypass P1");
				Vehicle vehicle = VEHICLE::CREATE_VEHICLE(hash, pos.x + xVector, pos.y + yVector, pos.z, heading, true, false, false);
				vehArray[numUsedElements] = vehicle;
				g_Logger->Info("Created Vehicle");
				*(unsigned short*)g_GameVariables->m_ModelSpawnBypass = 0x0574;
				g_Logger->Info("Patched bypass");

				vehArray[numUsedElements] = vehicle;

				ENTITY::_SET_ENTITY_SOMETHING(vehicle, true);
				DECORATOR::DECOR_SET_INT(vehicle, "MPBitset", 0);

				Ped chaser = PED::CLONE_PED(PLAYER::PLAYER_PED_ID(), heading, true, true);
				PED::SET_PED_AS_ENEMY(chaser, true);
				Hash weaponHash = MISC::GET_HASH_KEY("weapon_appistol");
				WEAPON::GIVE_WEAPON_TO_PED(chaser, weaponHash, 3000, false, true);
				WEAPON::SET_PED_INFINITE_AMMO(chaser, true, weaponHash);
				g_Logger->Info("Cloned ped");
				PED::SET_PED_INTO_VEHICLE(chaser, vehicle, -1);
				g_Logger->Info("Set ped into vehicle");

				pedArray[numUsedElements] = chaser;

				ENTITY::SET_ENTITY_MAX_SPEED(vehicle, 10000);
				g_Logger->Info("Changed max speed");
				BRAIN::TASK_VEHICLE_CHASE(chaser, PLAYER::PLAYER_PED_ID());
				g_Logger->Info("Started chase");
			});

			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
			g_Logger->Info("Cleaned up");
			numUsedElements++;

			tick = 0;
		}
		else
		{
			tick++;
		}
	}
}

void pantoCleanup()
{
	for (int i{}; i < numUsedElements; ++i)
	{
		VEHICLE::DELETE_VEHICLE(&vehArray[i]);
		PED::DELETE_PED(&pedArray[i]);
	}

	numUsedElements = 0;
}
