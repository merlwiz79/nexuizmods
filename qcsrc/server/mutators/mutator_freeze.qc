void freeze_Initialize()
{
	precache_model("models/ice/ice.md3");
	warmup = time + cvar("g_start_delay") + cvar("g_freeze_warmup");
}

void freeze_CheckWinner()
{
	if(!g_ca)
		return;
	
	if(time <= game_starttime) // game didn't even start yet! nobody can win in that case.
		return;

	if(next_round || (time > warmup - cvar("g_freeze_warmup") && time < warmup))
		return; // already waiting for next round to start

	if((redalive >= 1 && bluealive >= 1) // counted in arena.qc
		|| (redalive >= 1 && yellowalive >= 1)
		|| (redalive >= 1 && pinkalive >= 1)
		|| (bluealive >= 1 && yellowalive >= 1)
		|| (bluealive >= 1 && pinkalive >= 1)
		|| (yellowalive >= 1 && pinkalive >= 1))
		return; // we still have active players on two or more teams, nobody won yet

	entity e, winner;
	string teamname;

	FOR_EACH_PLAYER(e)
	{
		if(e.freeze_frozen == 0 && e.classname == "player" && e.health >= 1) // here's one player from the winning team... good
		{
			winner = e;
			break; // break, we found the winner
		}
	}

	if(winner != world) // just in case a winner wasn't found
	{
		TeamScore_AddToTeam(winner.team, ST_SCORE, +1);
		if(winner.team == COLOR_TEAM1)
			teamname = "^1Red Team";
		else if(winner.team == COLOR_TEAM2)
			teamname = "^4Blue Team";
		else if(winner.team == COLOR_TEAM3)
			teamname = "^3Yellow Team";
		else
			teamname = "^6Pink Team";
		FOR_EACH_PLAYER(e) {
			centerprint(e, strcat(teamname, "^5 wins the round, all other teams were frozen.\n"));
		}
		bprint(teamname, "^5 wins the round since all the other teams were frozen.\n");
	}

	next_round = time + 5;
}

void freeze_Ice_Think()
{
	setorigin(self, self.owner.origin - '0 0 16');
	self.nextthink = time;

}

void freeze_Freeze()
{
		
	Portal_ClearAll(self);
	Portal_ClearAll(frag_target);
	kh_Key_DropAll(self, TRUE);
	kh_Key_DropAll(frag_attacker, TRUE);
	race_PreDie();
	DropAllRunes(self);

	if(self.ballcarried && g_nexball)
		DropBall(self.ballcarried, self.origin + self.ballcarried.origin, self.velocity);		

	if(self.flagcarried)
		DropFlag(self.flagcarried, world, world);
	
	self.freeze_frozen = 1;	

	entity ice;
	ice = spawn();
	ice.owner = self;
	ice.classname = "freeze_ice";
	ice.think = freeze_Ice_Think;
	ice.nextthink = time;
	ice.frame = floor(random() * 21); // ice model has 20 different looking frames
	setmodel(ice, "models/ice/ice.md3");

	self.movement = '0 0 0';

	// add waypoint
	WaypointSprite_Spawn("freezetag_frozen", 0, 0, self, '0 0 64', world, self.team, self, waypointsprite_attached, TRUE);
	if(self.waypointsprite_attached)
	{
		WaypointSprite_UpdateTeamRadar(self.waypointsprite_attached, RADARICON_WAYPOINT, '0.25 0.90 1');
	}
			
}

void freeze_Unfreeze()
{
	self.freeze_frozen = 0;

	// remove the ice block
	entity ice;
	for(ice = world; (ice = find(ice, classname, "freeze_ice")); ) if(ice.owner == self)
	{
		remove(ice);
		break;
	}

	// remove waypoint
	if(self.waypointsprite_attached)
		WaypointSprite_Kill(self.waypointsprite_attached);
}

MUTATOR_HOOKFUNCTION(freeze_RemovePlayer)
{
	if(self.flagcarried)
		DropFlag(self.flagcarried, world, world);
		
	if(self.freeze_frozen == 0)
	{
		if(self.team == COLOR_TEAM1)
			--redalive;
		else if(self.team == COLOR_TEAM2)
			--bluealive;
		else if(self.team == COLOR_TEAM3)
			--yellowalive;
		else if(self.team == COLOR_TEAM4)
			--pinkalive;
		--totalalive;
	}

	if(totalspawned > 2) // only check for winners if we had more than two players (one of them left, don't let the other player win just because of that)
		freeze_CheckWinner();

	freeze_Unfreeze();
		
	return 1;
}

MUTATOR_HOOKFUNCTION(freeze_PlayerDies)
{		
	Portal_ClearAll(self);
	Portal_ClearAll(frag_target);
	kh_Key_DropAll(self, TRUE);
	kh_Key_DropAll(frag_attacker, TRUE);
	race_PreDie();
	DropAllRunes(self);

	if(self.ballcarried && g_nexball)
		DropBall(self.ballcarried, self.origin + self.ballcarried.origin, self.velocity);		

	if(self.flagcarried)
		DropFlag(self.flagcarried, world, world);
		
	// clear waypoints
	WaypointSprite_PlayerDead();
	
	if(self.freeze_frozen == 0)
	{
		if(self.team == COLOR_TEAM1)
			--redalive;
		else if(self.team == COLOR_TEAM2)
			--bluealive;
		else if(self.team == COLOR_TEAM3)
			--yellowalive;
		else if(self.team == COLOR_TEAM4)
			--pinkalive;
		--totalalive;

        freeze_Freeze();
	}

    if(frag_attacker.classname == STR_PLAYER)
        centerprint(frag_attacker, strcat("^2You froze ^7", frag_target.netname, ".\n"));

	if(frag_attacker == frag_target || frag_attacker == world)
	{
        if(frag_target.classname == STR_PLAYER)
            centerprint(frag_target, "^1You froze yourself.\n");
		bprint("^7", frag_target.netname, "^1 froze himself.\n");
	}
	else
	{
        if(frag_target.classname == STR_PLAYER)
            centerprint(frag_target, strcat("^1You were frozen by ^7", frag_attacker.netname, ".\n"));
		bprint("^7", frag_target.netname, "^1 was frozen by ^7", frag_attacker.netname, ".\n");
	}
	
			
	frag_target.health = cvar("g_balance_health_start"); // "respawn" the player :P
	
	freeze_CheckWinner();

	return 1;
}


MUTATOR_HOOKFUNCTION(freeze_PlayerSpawn)
{

    	freeze_Unfreeze(); // start by making sure that all ice blocks are removed

	if(totalspawned == 1 && time > game_starttime) // only one player active on server, start a new match immediately
	if(!next_round && warmup && (time < warmup - cvar("g_freeze_warmup") || time > warmup)) // not awaiting next round
	{
		next_round = time;
		return 1;
	}
	if(g_ctf && warmup && time > warmup) 
	{
		return 1;
	}
	if(warmup && time > warmup) // spawn too late, freeze player
	{
		centerprint(self, "^1You spawned after the round started, you'll spawn as frozen.\n");
		freeze_Freeze();
	}

	return 1;
}

MUTATOR_HOOKFUNCTION(freeze_PlayerPreThink)
{
    vector revive_extra_size;
    revive_extra_size = '1 1 1' * cvar("g_freeze_revive_extra_size");

    float teammate_nearby;
    FOR_EACH_PLAYER(other) if(self != other)
    {
        if(other.freeze_frozen == 0)
        {
            if(other.team == self.team)
            {
                teammate_nearby = boxesoverlap(self.absmin - revive_extra_size, self.absmax + revive_extra_size, other.absmin, other.absmax);
                if(teammate_nearby)
                    break;
            }
        }
    }

    if(teammate_nearby && self.freeze_frozen == 1) // OK, there is at least one teammate reviving us
    {
        if(self.freeze_beginrevive_time == -9999) // initialize values if this is the first frame of revival
        {
            self.freeze_beginrevive_time = time;
            self.freeze_revive_progress = 0;
        }
        else
        {
            self.freeze_revive_progress = (time - self.freeze_beginrevive_time) / cvar("g_freeze_revive_time");
            if(time - self.freeze_beginrevive_time >= cvar("g_freeze_revive_time"))
            {
                freeze_Unfreeze();
                FOR_EACH_PLAYER(other) if(self != other)
		{
			if(other.freezetag_frozen == 0)
			{
				if(other.team == self.team)
				{
					teammate_nearby = boxesoverlap(self.absmin - revive_extra_size, self.absmax + revive_extra_size, other.absmin, other.absmax);
					if(teammate_nearby)
               				if(g_ca || g_tdm || g_ctf || g_keyhunt || g_domination || g_nexball)
               				{ 
               					PlayerScore_Add(other, SP_FREEZE_REVIVALS, +1);
						PlayerScore_Add(other, SP_SCORE, +1);
						centerprint(self, strcat("^5You were revived by ^7", other.netname, ".\n"));
                				centerprint(other, strcat("^5You revived ^7", self.netname, ".\n"));
                				bprint("^7", other.netname, "^5 revived ^7", self.netname, ".\n");

                				self.freeze_beginrevive_time = -9999;
                				self.freeze_revive_progress = 0;
					}
				}
			}
		}         
	

                centerprint(self, strcat("^5You were revived by ^7", other.netname, ".\n"));
                centerprint(other, strcat("^5You revived ^7", self.netname, ".\n"));
                bprint("^7", other.netname, "^5 revived ^7", self.netname, ".\n");

                self.freeze_beginrevive_time = -9999;
                self.freeze_revive_progress = 0;
            }
        }
        // now find EVERY teammate within reviving radius, set their revive_progress values correct
        FOR_EACH_PLAYER(other) if(self != other)
        {
            if(other.freeze_frozen == 0)
            {
                if(other.team == self.team)
                {
                    teammate_nearby = boxesoverlap(self.absmin - revive_extra_size, self.absmax + revive_extra_size, other.absmin, other.absmax);
                    if(teammate_nearby)
                        other.freeze_revive_progress = self.freeze_revive_progress;
                }
            }
        }
    }
    else if(!g_ca && !teammate_nearby && self.freeze_frozen == 1) 
    {
	if(self.freeze_beginrevive_time == -9999) // initialize values if this is the first frame of revival
        {
            self.freeze_beginrevive_time = time;
            self.freeze_revive_progress = 0;
        }
        else
        {
            self.freeze_revive_progress = (time - self.freeze_beginrevive_time) / cvar("g_freeze_revive_time");
            if(time - self.freeze_beginrevive_time >= cvar("g_freeze_revive_time"))
            {
                self.freeze_beginrevive_time = -9999;
                self.freeze_revive_progress = 0;                  
                freeze_Unfreeze();
            }
        }
    }
    else if(!teammate_nearby) // only if no teammate is nearby will we reset
    {
        self.freeze_beginrevive_time = -9999;
        self.freeze_revive_progress = 0;
    }

	return 1;
}

MUTATOR_HOOKFUNCTION(freeze_PlayerPhysics)
{
	if(self.freeze_frozen)
		self.movement = '0 0 0';
	return 1;
}

MUTATOR_HOOKFUNCTION(freeze_PlayerDamage)
{
    if(g_freeze)
    {
        if(frag_target.freeze_frozen == 1)
        {
            damage_take = 0;
            damage_save = 0;
            damage_force = damage_force * cvar("g_freeze_frozen_force");
        }
    }
    return 1;
}

MUTATOR_DEFINITION(mutator_freeze)
{
	MUTATOR_HOOK(MakePlayerObserver, freeze_RemovePlayer, CBC_ORDER_ANY);
	MUTATOR_HOOK(ClientDisconnect, freeze_RemovePlayer, CBC_ORDER_ANY);
	MUTATOR_HOOK(PlayerDies, freeze_PlayerDies, CBC_ORDER_ANY);
	MUTATOR_HOOK(PlayerSpawn, freeze_PlayerSpawn, CBC_ORDER_ANY);
	MUTATOR_HOOK(PlayerPreThink, freeze_PlayerPreThink, CBC_ORDER_FIRST);
	MUTATOR_HOOK(PlayerPhysics, freeze_PlayerPhysics, CBC_ORDER_FIRST);
	MUTATOR_HOOK(PlayerDamage, freeze_PlayerDamage, CBC_ORDER_ANY);
	
	MUTATOR_ONADD
	{
		if(time > 1) // game loads at time 1
			error("This is a game type and it cannot be added at runtime.");
		if(!g_freezetag || !g_lms)
			g_freeze = 1;
			freeze_Initialize();
	}

	MUTATOR_ONREMOVE
	{
		g_freeze = 0;
		error("This is a game type and it cannot be removed at runtime.");
	}

	return 0;
}