#pragma once
#include "entity.h"
#include <vector>
#include "input.h"

class Player : public Entity {
public:
	struct Physics {
		/* Grounded physics */
		static constexpr double MAX_SPEED_WALK = 0x0180 / 256.0;
		static constexpr double MAX_SPEED_RUN = 0x0280 / 256.0;
		static constexpr double MAX_SPEED_SPRINT = 0x0380 / 256.0;
		static constexpr double FORCED_SPEED = 0x0140 / 256.0;  // End-of-level walk
		static constexpr double FORCED_SPEED_2 = 0x0200 / 256.0;  // Airship boarding cutscene
		static constexpr double ACCEL_GROUND = 0x000E / 256.0;
		static constexpr double DECEL_GROUND_NORMAL = 0x000E / 256.0;
		static constexpr double DECEL_GROUND_ICE = 0x0003 / 256.0;
		static constexpr double SKID_GROUND_NORMAL = 0x0020 / 256.0;
		static constexpr double SKID_GROUND_ICE = 0x000C / 256.0;
		/* Slope physics */
		static constexpr double MAX_SPEED_WALK_UPHILL = 0x00D0 / 256.0;
		static constexpr double MAX_SPEED_RUN_UPHILL = 0x0160 / 256.0;
		static constexpr double MAX_SPEED_SLIDE = 0x03F0 / 256.0;
		static constexpr double ACCEL_SLIDE_225 = 0x0010 / 256.0;
		static constexpr double ACCEL_SLIDE_45 = 0x0020 / 256.0;
		/* Jump physics */
		static constexpr double JUMP_VEL_STAND = 0x0370 / 256.0;  // vel_x_ <= 0x01.00
		static constexpr double JUMP_VEL_WALK = 0x0390 / 256.0;  // vel_x_ > 0x01.00
		static constexpr double JUMP_VEL_RUN = 0x03B0 / 256.0;  // vel_x_ > 0x02.00
		static constexpr double JUMP_VEL_SPRINT = 0x03F0 / 256.0;  // vel_x_ > 0x03.00
		static constexpr double STOMP_VEL_NORMAL = 0x0400 / 256.0;
		static constexpr double STOMP_VEL_WEAK = 0x0300 / 256.0;
		/* Mid-air physics */
		static constexpr double GRAVITY_LIGHT = 0x0010 / 256.0;  // JUMP held and vel_y_ < -0x02.00
		static constexpr double GRAVITY_HEAVY = 0x0050 / 256.0;
		static constexpr double VEL_TERM = 0x0450 / 256.0;
		static constexpr double ACCEL_AIR = 0x000E / 256.0;
		static constexpr double SKID_AIR = 0x0020 / 256.0;
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
	/// The status mario has for physics and collision calculations.
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