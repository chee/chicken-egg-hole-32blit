#include "game.hpp"
#include "assets.hpp"

using namespace blit;

enum class Direction { up, down, left, right };

Direction opposite(Direction d) {
	if (d == Direction::up) {
		return Direction::down;
	} else if (d == Direction::down) {
		return Direction::up;
	} else if (d == Direction::left) {
		return Direction::right;
	} else {
		return Direction::left;
	}
}

class Thing {
	Rect sprite_rect1;
	Rect sprite_rect2;
	uint8_t size;
	Point origin;
	bool state = false;

  public:
	Point position;
	Thing(Rect sprite_rect, uint8_t size)
		: sprite_rect1(sprite_rect), sprite_rect2(sprite_rect), size(size) {
		position = Point(0, 0);
		origin = Point(size / 2, size / 2);
	}
	Thing(Rect sprite_rect1, Rect sprite_rect2, uint8_t size)
		: sprite_rect1(sprite_rect1), sprite_rect2(sprite_rect2), size(size) {
		position = Point(0, 0);
		origin = Point(size / 2, size / 2);
	}
	void render() {
		screen.sprite(state ? sprite_rect1 : sprite_rect2, position, origin);
	}
	void move(Direction d) {
		state = !state;
		if (d == Direction::left) {
			position.x -= size;
		} else if (d == Direction::right) {
			position.x += size;
		} else if (d == Direction::up) {
			position.y -= size;
		} else if (d == Direction::down) {
			position.y += size;
		}
	}
	void left() {
		position.x -= size;
		state = !state;
	}
	void right() {
		position.x += size;
		state = !state;
	}
	void up() {
		position.y -= size;
		state = !state;
	}
	void down() {
		position.y += size;
		state = !state;
	}
	void set(int32_t x, int32_t y) {
		position = Point(32 + (x * size), 32 + (y * size));
	}
	bool intersects(Thing &thing) { return thing.position == position; }
};

Thing chicken(Rect(0, 0, 4, 4), Rect(4, 0, 4, 4), 32);
Thing egg(Rect(4, 4, 4, 4), 32);
Thing hole(Rect(0, 4, 4, 4), 32);
Thing obstacle(Rect(0, 8, 4, 4), 32);

Thing *things[] = {&chicken, &egg, &hole, &obstacle};

void init() {
	set_screen_mode(ScreenMode::hires);
	screen.sprites = Surface::load(sprites);
	chicken.set(1, 2);
	egg.set(2, 2);
	hole.set(4, 0);
	obstacle.set(4, 1);
}

enum class GameState { play, win, lose };

GameState game_state = GameState::play;

void win() { game_state = GameState::win; }
void lose() { game_state = GameState::lose; }

void render(uint32_t time) {
	screen.clear();
	if (game_state == GameState::play) {
		screen.pen = Pen(0, 200, 60);
		Point position(15, 15);
		Point origin(16, 16);

		for (auto thing : things) {
			thing->render();
		}
	} else if (game_state == GameState::win) {
		screen.pen = Pen(255, 255, 255);
		screen.rectangle(Rect(0, 0, screen.bounds.w, screen.bounds.h));
		screen.pen = Pen(0, 0, 0);
		screen.text(
			"~* chicken egg hole *~", minimal_font,
			Point((screen.bounds.w / 2) - 50, (screen.bounds.h / 2) - 10));
	} else if (game_state == GameState::lose) {
		screen.pen = Pen(0, 0, 0);
		screen.rectangle(Rect(0, 0, screen.bounds.w, screen.bounds.h));
		screen.pen = Pen(255, 255, 255);
		screen.text(
			"chicken hole :-(", minimal_font,
			Point((screen.bounds.w / 2) - 35, (screen.bounds.h / 2) - 10));
	}
}

void update(uint32_t time) {
	Direction d;
	if (buttons.pressed & Button::DPAD_UP) {
		d = Direction::up;
	} else if (buttons.pressed & Button::DPAD_DOWN) {
		d = Direction::down;
	} else if (buttons.pressed & Button::DPAD_LEFT) {
		d = Direction::left;
	} else if (buttons.pressed & Button::DPAD_RIGHT) {
		d = Direction::right;
	} else {
		return;
	}

	chicken.move(d);
	if (chicken.intersects(obstacle)) {
		chicken.move(opposite(d));
	}
	if (chicken.intersects(egg)) {
		egg.move(d);
	}
	if (egg.intersects(obstacle)) {
		egg.move(opposite(d));
	}
	if (egg.intersects(chicken)) {
		chicken.move(opposite(d));
	}
	if (egg.intersects(hole)) {
		win();
	}
	if (chicken.intersects(hole)) {
		lose();
	}
}
