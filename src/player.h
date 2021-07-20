#pragma once
#include "entity.h"

#include <memory>
#include <vector>
#include "input.h"
#include "rectangle.h"
#include "tilemap.h"

class Player : public Entity {
public:
	struct Physics {
		/* Grounded physics */
		static constexpr double MAX_SPEED_WALK = (double)0x0180 / 0x100;
		static constexpr double MAX_SPEED_RUN = (double)0x0280 / 0x100;
		static constexpr double MAX_SPEED_SPRINT = (double)0x0380 / 0x100;
		static constexpr double FORCED_SPEED = (double)0x0140 / 0x100;  // End-of-level walk
		static constexpr double FORCED_SPEED_2 = (double)0x0200 / 0x100;  // Airship boarding cutscene
		//static constexpr double ACCEL_GROUND = (double)0x000E / 0x100;
		static constexpr double ACCEL_GROUND = (double)8.0 / 9.0 / 16.0;
		//static constexpr double DECEL_GROUND_NORMAL = (double)0x0008 / 0x100;
		static constexpr double DECEL_GROUND_NORMAL = (double)24.0 / 40.0 / 16.0;
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
		static constexpr double ACCEL_AIR = ACCEL_GROUND;
		static constexpr double SKID_AIR = (double)0x0020 / 0x100;
	};
	struct CollisionInfo {
		bool collided;
		int row, col;
	};

	Player();
	Player(Graphics& graphics, const std::string& path_to_bmp, double pos_x = 0, double pos_y = 0);
	Player(Sprite* sprite, double pos_x = 0, double pos_y = 0);
	~Player();

	void HandleInputs(const Input& input);
	void Update(const Input& input, double delta_time, const Tilemap& tilemap);
	int Draw(Graphics& graphics, double offset_x = 0, double offset_y = 0);

	Rectangle GetColliderAbsoluteRect();
	Rectangle LeftCollision(double delta) const;
	Rectangle RightCollision(double delta) const;
	Rectangle TopCollision(double delta) const;
	Rectangle BottomCollision(double delta) const;
	CollisionInfo GetCollisionInfo(const Tilemap& tilemap, const Rectangle& rectangle);

private:
	enum class Status {
		GROUNDED = 1,
		SKIDDING = 2,
		SLIPPING = 4,
		SWIMMING = 8,
		INVULN = 16,
		INTANG = 32,
		FIXED = 64
	};
	/// @brief D-pad input vector, ranging from [-1, 1] for each component.
	///        Format is {horizontal, vertical}, with -1 being left/up and 1 being right/down.
	std::vector<int> dpad_vector_;
	/// @brief Direction the player is facing, where -1 is left, 1 is right.
	int dir_facing_;
	double speed_grounded_;
	double aerial_speed_cap_;
	/// @brief The bitwise status mario has for physics and collision calculations.  This is NOT powerup status.
	/// 
	/// 0 = default
	/// 1 = on ground (can jump)
	/// 2 = skidding (horizontal input is in opposite direction of horizontal motion)
	/// 4 = on ice (slippery physics)
	/// 8 = swimming
	/// 16 = invulnerable (e.g. from starman)
	/// 32 = intangible (e.g. inside pipe)
	/// 64 = uncontrollable, fixed motion (e.g. cutscene, inside pipe)
	/// 128 = UNUSED
	unsigned int status_;
	typedef std::map<std::string, std::unique_ptr<Sprite>> SpriteMap;
	SpriteMap sprite_map_;
};