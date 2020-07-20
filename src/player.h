#pragma once
#include "entity.h"
#include <vector>
#include "input.h"

class Player : public Entity {
public:
	struct Physics {
		/* Grounded physics */
		static constexpr double MAX_SPEED_WALK = (double)0x0180 / 0x100;
		static constexpr double MAX_SPEED_RUN = (double)0x0280 / 0x100;
		static constexpr double MAX_SPEED_SPRINT = (double)0x0380 / 0x100;
		static constexpr double FORCED_SPEED = (double)0x0140 / 0x100;  // End-of-level walk
		static constexpr double FORCED_SPEED_2 = (double)0x0200 / 0x100;  // Airship boarding cutscene
		static constexpr double ACCEL_GROUND = (double)0x000E / 0x100;
		static constexpr double DECEL_GROUND_NORMAL = (double)0x000E / 0x100;
		static constexpr double DECEL_GROUND_ICE = (double)0x0003 / 0x100;
		static constexpr double SKID_GROUND_NORMAL = (double)0x0020 / 0x100;
		static constexpr double SKID_GROUND_ICE = (double)0x000C / 0x100;
		/* Slope physics */
		static constexpr double MAX_SPEED_WALK_UPHILL = (double)0x00D0 / 0x100;
		static constexpr double MAX_SPEED_RUN_UPHILL = (double)0x0160 / 0x100;
		static constexpr double MAX_SPEED_SLIDE = (double)0x03F0 / 0x100;
		static constexpr double ACCEL_SLIDE_225 = (double)0x0010 / 0x100;
		static constexpr double ACCEL_SLIDE_45 = (double)0x0020 / 0x100;
		/* Jump physics */
		static constexpr double JUMP_VEL_STAND = (double)0x0370 / 0x100;  // vel_x_ <= 0x01.00
		static constexpr double JUMP_VEL_WALK = (double)0x0390 / 0x100;  // vel_x_ > 0x01.00
		static constexpr double JUMP_VEL_RUN = (double)0x03B0 / 0x100;  // vel_x_ > 0x02.00
		static constexpr double JUMP_VEL_SPRINT = (double)0x03F0 / 0x100;  // vel_x_ > 0x03.00
		static constexpr double STOMP_VEL_NORMAL = (double)0x0400 / 0x100;
		static constexpr double STOMP_VEL_WEAK = (double)0x0300 / 0x100;
		/* Mid-air physics */
		static constexpr double GRAVITY_LIGHT = (double)0x0010 / 0x100;  // JUMP held and vel_y_ < -0x02.00
		static constexpr double GRAVITY_HEAVY = (double)0x0050 / 0x100;
		static constexpr double VEL_TERM = (double)0x0450 / 0x100;
		static constexpr double ACCEL_AIR = (double)0x000E / 0x100;
		static constexpr double SKID_AIR = (double)0x0020 / 0x100;
	};

	Player(Sprite* sprite, double pos_x = 0, double pos_y = 0);
	~Player();

	void HandleInputs(Input& input);
	void Update(Input& input);
	int Draw(Graphics& graphics);

private:
	std::vector<int> dpad_vector_;  // Lets us cancel out opposing inputs (e.g. left + right)
	double speed_grounded_;
	double aerial_speed_cap_;
	/// <summary>
	/// The status mario has for physics and collision calculations.  This is NOT powerup status.
	/// 0 = default
	/// 1 = on ground (can jump)
	/// 2 = skidding (input is in opposite direction of motion)
	/// 4 = on ice (slippery physics)
	/// 8 = swimming
	/// 16 = invulnerable (e.g. from starman)
	/// 32 = intangible (e.g. inside pipe)
	/// 64 = uncontrollable (e.g. cutscene, inside pipe)
	/// 128 = UNUSED
	/// </summary>
	unsigned char status_;
	enum class Status {
		GROUNDED = 1,
		SKIDDING = 2,
		SLIPPING = 4,
		SWIMMING = 8,
		INVULN = 16,
		INTANG = 32,
		FIXED = 64
	};
};