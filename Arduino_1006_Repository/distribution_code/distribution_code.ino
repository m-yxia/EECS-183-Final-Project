/**
 * distribution_code.ino
 *
 * Maggie (Yi) Xia, Britney Cheng, Jasmine Wang and Cindy Gu
 * yixia, britc, jazzyw, and cindygu
 *
 * EECS 183: Final Project
 *
 * This project involves using the Arduino micro-controller link 
 * two sensors to an LED screen, so that you can play Space Invaders
 */

#include <gamma.h>
#include <RGBmatrixPanel.h>
#include <Adafruit_GFX.h>

// define the wiring of the LED screen
const uint8_t CLK  = 8;
const uint8_t LAT = A3;
const uint8_t OE = 9;
const uint8_t A = A0;
const uint8_t B = A1;
const uint8_t C = A2;

// define the wiring of the inputs
const int POTENTIOMETER_PIN_NUMBER = 5;
const int BUTTON_PIN_NUMBER = 10;

// global constant for the number of Invaders in the game
const int NUM_ENEMIES = 16;

// global constant for the middle value on the potentiometer
const int POT_MID = 511;

// a global variable that represents the LED screen
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);
int prev_pot = 511;

// the following functions are for printing messages
void print_level(int level);
void print_lives(int lives);
void game_over();

/* ~~~~~~~~~~~~~~ Color Class ~~~~~~~~~~~~~~ */
// sets the color of lights on the matrix
// default is black
class Color {
  public:
    int red;
    int green;
    int blue;

    // Constructors
    Color() {
      red = 0;
      green = 0;
      blue = 0;
    }
    
    Color(int r, int g, int b) {
      red = r;
      green = g;
      blue = b;
    }
    
    uint16_t to_333() const {
      return matrix.Color333(red, green, blue);
    }
};

// color constants
const Color BLACK(0, 0, 0);
const Color RED(4, 0, 0);
const Color ORANGE(6, 1, 0);
const Color YELLOW(4, 4, 0);
const Color GREEN(0, 4, 0);
const Color BLUE(0, 0, 4);
const Color PURPLE(1, 0, 2);
const Color WHITE(4, 4, 4);
const Color LIME(2, 4, 0);
const Color AQUA(0, 4, 4);


/* ~~~~~~~~~~~~~~ Invader Class ~~~~~~~~~~~~~~ */
// Class for the game's invaders
class Invader {
  public:
    // Constructors
    Invader() {
      x = 0;
      y = 0;
      strength = 0;
    }
    
    // sets values for private date members x and y
    Invader(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    
    // sets values for private data members
    Invader(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
    }
    
    // sets values for private data members
    void initialize(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    
    int get_y() const {
      return y;
    }
    
    int get_strength() const {
      return strength;
    }

    // Moves the Invader down the screen by one row
    // Modifies: y
    void move() {
      if (strength > 0) {
        y++;
      }
    }
    
    // draws the Invader if its strength is greater than 0
    // calls: draw_with_rgb
    void draw() {
      if (strength > 0) {
        if (strength == 7) {
          draw_with_rgb(WHITE, BLUE);
        }
        else if (strength == 6) {
          draw_with_rgb(PURPLE, BLUE);
        }
        else if (strength == 5) {
          draw_with_rgb(BLUE, BLUE);
        }
        else if (strength == 4) {
          draw_with_rgb(GREEN, BLUE);
        }
        else if (strength == 3) {
          draw_with_rgb(YELLOW, BLUE);
        }
        else if (strength == 2) {
          draw_with_rgb(ORANGE, BLUE);
        }
        else {
          draw_with_rgb(RED, BLUE);
        }
      }
    }
    
    // draws black where the Invader used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK, BLACK);
    }    
    
    // Invader is hit by a Cannonball.
    // Modifies: strength
    // calls: draw, erase
    void hit() {
      strength--;
      if (strength == 0) {
        erase();
      }
      else {
        draw();
      }
    }

  private:
    int x;
    int y;
    int strength;
    
    // draws the Invader
    void draw_with_rgb(Color body_color, Color eye_color) {
      int draw_x = x;
      int draw_y = y;
      matrix.drawPixel(draw_x + 1, draw_y, body_color.to_333());
      matrix.drawPixel(draw_x + 2, draw_y, body_color.to_333());
      draw_y++;
      matrix.drawPixel(draw_x, draw_y, body_color.to_333());
      matrix.drawPixel(draw_x + 1, draw_y, eye_color.to_333());
      matrix.drawPixel(draw_x + 2, draw_y, eye_color.to_333());
      matrix.drawPixel(draw_x + 3, draw_y, body_color.to_333());
      draw_y++;
      for (int i = draw_x; i < (draw_x + 4); i++) {
        matrix.drawPixel(i, draw_y, body_color.to_333());
      }
      draw_y++;
      matrix.drawPixel(draw_x, draw_y, body_color.to_333());
      matrix.drawPixel(draw_x + 3, draw_y, body_color.to_333());
    }
};


/* ~~~~~~~~~~~~~~ Cannonball Class ~~~~~~~~~~~~~~ */
class Cannonball {
  public:

    // default constructor
    Cannonball() {
      x = 0;
      y = 0;
      fired = false;
    }
    
    // resets private data members to initial values
    void reset() {
      x = 0;
      y = 0;
      fired = false;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    
    int get_y() const {
      return y;
    }

    bool has_been_fired() const {
      return fired;
    }
    
    // sets private data members
    void fire(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
      fired = true;
    }
    
    // moves the Cannonball and detects if it goes off the screen
    // Modifies: y, fired
    void move() {
      y--;
      if (y <= 0) {
        fired = false;
      }
    }
    
    // resets private data members to initial values
    void hit() {
      erase();
      x = 0;
      y = 0;
      fired = false;
    }
    
    // draws the Cannonball, if it is fired
    void draw() {
      if (fired) {
        matrix.drawPixel(x, y, ORANGE.to_333());
        matrix.drawPixel(x, y + 1, ORANGE.to_333());
      }
    }
    
    // draws black where the Cannonball used to be
    void erase() {
      matrix.drawPixel(x, y, BLACK.to_333());
      matrix.drawPixel(x, y + 1, BLACK.to_333());
    }

  private:
    int x;
    int y;
    bool fired;
};


/* ~~~~~~~~~~~~~~ Player Class ~~~~~~~~~~~~~~ */
// class for the player controlled by the user through the Arduino
class Player {
  public:

    // default constructor
    Player() {
      x = 0;
      y = 14;
      lives = 3;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    
    int get_y() const {
      return y;
    }
    
    int get_lives() const {
      return lives;
    }
    
    // setter
    void set_x(int x_arg) {
      x = x_arg;
    }
    
    // Modifies: lives
    void die() {
      lives = lives - 1;
    }
    
    // draws the Player
    // calls: draw_with_rgb
    void draw() {
      draw_with_rgb(AQUA);
    }
    
    // draws black where the Player used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK);
    }

  private:
    int x;
    int y;
    int lives;

    // sets private data members x and y to initial values
    // modifies: x and y
    void initialize(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    
    // draws the player
    void draw_with_rgb(Color color) {
      int draw_x = x;
      int draw_y = y;
      matrix.drawPixel(draw_x + 1, draw_y, color.to_333());
      draw_y++;
      for (int i = draw_x; i < (draw_x + 3); i++) {
        matrix.drawPixel(i, draw_y, color.to_333());
      }
    }
};


/* ~~~~~~~~~~~~~~ Game Class ~~~~~~~~~~~~~~ */
class Game {
  public:

    // default constructor
    Game() {
      level = 1;
      time = 0;
    }
    
    // sets up a new game of Space Invaders
    // Modifies: global variable matrix
    void gameSetup() {
       matrix.fillScreen(BLACK.to_333());
       print_lives(player.get_lives());
       delay(2000);
       matrix.fillScreen(BLACK.to_333());
       matrix.setCursor(0,0);
       print_level(level);
       delay(2000);
       matrix.fillScreen(BLACK.to_333());
       matrix.setCursor(0,0);
       player.set_x(15);
       player.draw();
       spawn(level);
       delay(3000);
    }
    
    // advances the game simulation one step and renders the graphics
    // see spec for details of game
    // Modifies: global variable matrix
    void update(int potentiometer_value, bool button_pressed) {
      if (player.get_lives() > 0) {

        // Moving player
        int maxSpeed = 1;
        int dir = 0;
        int prev_dir = 0;
        
        if (-(potentiometer_value - POT_MID) > 5) {
          dir = 1;
        } else if (-(potentiometer_value - POT_MID) < -5) {
          dir = -1;
        }
        if (-(prev_pot - POT_MID) > 25) {
          prev_dir = 1;
        } else if (-(prev_pot - POT_MID) < -25) {
          prev_dir = -1;
        }

        // stop
        if ((prev_dir > dir) || (prev_dir < dir)) dir = 0;
        prev_pot = potentiometer_value;        

        // moves player by one x-coordinate
        player.erase();
        player.set_x(player.get_x() + (maxSpeed * dir));
        if (player.get_x() <= -1) {
          player.set_x(-1);
        }
        if (player.get_x() >= 30) {
          player.set_x(30);
        }
        player.draw();

        // sets a delay and kills the player
        for (int i = 0; i < NUM_ENEMIES; i++) {
          if ((enemies[i].get_x() == player.get_x() + 1 && enemies[i].get_y() + 3 == player.get_y())) {
            player.die();
            delay(1000);
            reset_level();       
          } 
          else if ((enemies[i].get_x() + 3 == player.get_x() + 1 && enemies[i].get_y() + 3 == player.get_y())) {
            player.die();
            delay(1000);
            reset_level();
          }
        }
      
        // Moving invaders
        unsigned long currentTime = millis();      
        if (currentTime - time > 2000) {
          bool playerHit = false;                    
          // top row of invaders only move when entire bottom row is dead
          bool bottom_row_dead = true;

          // moving invaders by erasing and drawing them again
          for (int i = 8; i < NUM_ENEMIES; i++) {
            if (enemies[i].get_strength() > 0) {
              bottom_row_dead = false;
              enemies[i].erase();
              enemies[i].move();
              enemies[i].draw();   
              if (enemies[i].get_y() >= 12) {
                playerHit = true;
              }
            }
          }
          if (bottom_row_dead) {
            for (int i = 0; i < 8; i++) {
              enemies[i].erase();
              enemies[i].move();
              enemies[i].draw();   
              if (enemies[i].get_y() >= 12) {
                playerHit = true;
              }
             }
          }

          // killing players and resetting the level
          if (playerHit) {
            player.die();
            delay(1000);
            reset_level();
          }
          time = currentTime;
        }
        
        // Firing cannonball
        if (button_pressed) {
          if (!ball.has_been_fired()) {
            ball.fire(player.get_x() + 1, player.get_y());
          }
        }

        // Moving cannonball
        if (ball.get_y() <= 0) {
          ball.hit();
        }
        else if (hit_enemy(ball.get_x(), ball.get_y())) {
          ball.hit();
        }
        else if (hit_enemy(ball.get_x(), ball.get_y() + 1)) {
          ball.hit();
        }
        else {
          ball.erase();
          ball.move();
          ball.draw();
        }

        // ensures enemies don't lose body parts
        for (int i = 0; i < NUM_ENEMIES; i++) enemies[i].draw();
     
      }
      else {
          game_over();
          delay(5000);
      }
      
      if (level_cleared()) {
        level++;
        reset_level();
      }
    }

    // draws invaders depending on level
    // Modifies: global variable matrix, enemies
    void spawn(int lvl) {

      // level ONE
      if (lvl == 1) {
        for (int i = 0; i < 32; i += 4) {
          enemies[(i / 4)].initialize(i, 0, 1);
        }
        for (int i = 8; i < 16; i++) {
          enemies[i].initialize(0, 0, 0);
        }
        for (int i = 0; i < 8; i++) {
          enemies[i].draw();
        }
      } 

      // level TWO
      else if (lvl == 2) {
        enemies[0].initialize(0, 0, 1);
        enemies[1].initialize(4, 0, 2);
        enemies[2].initialize(8, 0, 1);
        enemies[3].initialize(12, 0, 2);
        enemies[4].initialize(16, 0, 1);
        enemies[5].initialize(20, 0, 2);
        enemies[6].initialize(24, 0, 1);
        enemies[7].initialize(28, 0, 2);
        enemies[8].initialize(0, 4, 2);
        enemies[9].initialize(4, 4, 1);
        enemies[10].initialize(8, 4, 2);
        enemies[11].initialize(12, 4, 1);
        enemies[12].initialize(16, 4, 2);
        enemies[13].initialize(20, 4, 1);
        enemies[14].initialize(24, 4, 2);
        enemies[15].initialize(28, 4, 1);
        for (int i = 0; i < 16; i++) {
          enemies[i].draw();
        }
      } 

      // level THREE
      else if (lvl == 3) {
        enemies[0].initialize(0, 0, 1);
        enemies[1].initialize(4, 0, 2);
        enemies[2].initialize(8, 0, 3);
        enemies[3].initialize(12, 0, 4);
        enemies[4].initialize(16, 0, 5);
        enemies[5].initialize(20, 0, 1);
        enemies[6].initialize(24, 0, 2);
        enemies[7].initialize(28, 0, 3);
        enemies[8].initialize(0, 4, 4);
        enemies[9].initialize(4, 4, 5);
        enemies[10].initialize(8, 4, 1);
        enemies[11].initialize(12, 4, 2);
        enemies[12].initialize(16, 4, 3);
        enemies[13].initialize(20, 4, 4);
        enemies[14].initialize(24, 4, 5);
        enemies[15].initialize(28, 4, 1);
        for (int i = 0; i < 16; i++) {
          enemies[i].draw();
        }
      }
      
      // level FOUR
      else if (lvl == 4) {
        enemies[0].initialize(0, 0, 5);
        enemies[1].initialize(4, 0, 4);
        enemies[2].initialize(8, 0, 5);
        enemies[3].initialize(12, 0, 4);
        enemies[4].initialize(16, 0, 5);
        enemies[5].initialize(20, 0, 4);
        enemies[6].initialize(24, 0, 5);
        enemies[7].initialize(28, 0, 4);
        enemies[8].initialize(0, 4, 2);
        enemies[9].initialize(4, 4, 3);
        enemies[10].initialize(8, 4, 2);
        enemies[11].initialize(12, 4, 3);
        enemies[12].initialize(16, 4, 2);
        enemies[13].initialize(20, 4, 3);
        enemies[14].initialize(24, 4, 2);
        enemies[15].initialize(28, 4, 3);
        for (int i = 0; i < 16; i++) {
          enemies[i].draw();
        }
      } 

      // levels GREATER THAN 4 (random levels)
      else {
        enemies[0].initialize(0, 0, random(1, 8));
        enemies[1].initialize(4, 0, random(1, 8));
        enemies[2].initialize(8, 0, random(1, 8));
        enemies[3].initialize(12, 0, random(1, 8));
        enemies[4].initialize(16, 0, random(1, 8));
        enemies[5].initialize(20, 0, random(1, 8));
        enemies[6].initialize(24, 0, random(1, 8));
        enemies[7].initialize(28, 0, random(1, 8));
        enemies[8].initialize(0, 4, random(1, 8));
        enemies[9].initialize(4, 4, random(1, 8));
        enemies[10].initialize(8, 4, random(1, 8));
        enemies[11].initialize(12, 4, random(1, 8));
        enemies[12].initialize(16, 4, random(1, 8));
        enemies[13].initialize(20, 4, random(1, 8));
        enemies[14].initialize(24, 4, random(1, 8));
        enemies[15].initialize(28, 4, random(1, 8));
        for (int i = 0; i < 16; i++) {
          enemies[i].draw();
        }
      }
    }


  /* ~~~~~~~~~~~~~~ hit_enemy ~~~~~~~~~~~~~~ */
    // checks if there is an enemy at (x, y)
    bool hit_enemy(int x, int y) {
      int nearestInvader = x - (x % 4);
      int invaderIndex = nearestInvader / 4;
      bool cannonHit = false;

      // if invader is still alive
      if (enemies[invaderIndex].get_strength() > 0) {

  // set cannonHit as true if the enemy coordinates are equal to cannonball's coordinates
        if (enemies[invaderIndex].get_x() == x && enemies[invaderIndex].get_y() + 1 == y) {
          cannonHit = true;
        }
        else if (enemies[invaderIndex].get_x() == x && enemies[invaderIndex].get_y() + 2 == y) {
          cannonHit = true;
        }
        else if (enemies[invaderIndex].get_x() == x && enemies[invaderIndex].get_y() + 3 == y) {
          cannonHit = true;
        }
        else if (enemies[invaderIndex].get_x() + 1 == x && enemies[invaderIndex].get_y() == y) {
          cannonHit = true;
        }
        else if (enemies[invaderIndex].get_x() + 1 == x && enemies[invaderIndex].get_y() + 1 == y) {
          cannonHit = true;
        }
        else if (enemies[invaderIndex].get_x() + 1 == x && enemies[invaderIndex].get_y() + 2 == y) {
          cannonHit = true;
        }
        else if (enemies[invaderIndex].get_x() + 1 == x && enemies[invaderIndex].get_y() + 3 == y) {
          cannonHit = true;
        }
        else if (enemies[invaderIndex].get_x() + 2 == x && enemies[invaderIndex].get_y() == y) {
          cannonHit = true;
        }
        else if (enemies[invaderIndex].get_x() + 2 == x && enemies[invaderIndex].get_y() + 1 == y) {
          cannonHit = true;
        }
        else if (enemies[invaderIndex].get_x() + 2 == x && enemies[invaderIndex].get_y() + 2 == y) {
          cannonHit = true;
        }
        else if (enemies[invaderIndex].get_x() + 2 == x && enemies[invaderIndex].get_y() + 3 == y) {
          cannonHit = true;
        }
        else if (enemies[invaderIndex].get_x() + 3 == x && enemies[invaderIndex].get_y() + 1 == y) {
          cannonHit = true;
        }
        else if (enemies[invaderIndex].get_x() + 3 == x && enemies[invaderIndex].get_y() + 2 == y) {
          cannonHit = true;
        }
        else if (enemies[invaderIndex].get_x() + 3 == x && enemies[invaderIndex].get_y() + 3 == y) {
          cannonHit = true;
        }
        if (cannonHit) {
          enemies[invaderIndex].hit();
        }
      }
      
      if (enemies[invaderIndex + 8].get_strength() > 0) {
        if (enemies[invaderIndex + 8].get_x() == x && enemies[invaderIndex + 8].get_y() + 3 == y) {
          cannonHit = true;
        }
        else if (enemies[invaderIndex + 8].get_x() + 1 == x && enemies[invaderIndex + 8].get_y() + 2 == y) {
          cannonHit = true;
        }
        else if (enemies[invaderIndex + 8].get_x() + 2 == x && enemies[invaderIndex + 8].get_y() + 2 == y) {
          cannonHit = true;
        }
        else if (enemies[invaderIndex + 8].get_x() + 3 == x && enemies[invaderIndex + 8].get_y() + 3 == y) {
          cannonHit = true;
        }
        if (cannonHit) {
          enemies[invaderIndex + 8].hit();
        }
      }
      
      return cannonHit;
    }

  private:
    int level;
    unsigned long time;
    Player player;
    Cannonball ball;
    Invader enemies[NUM_ENEMIES];

    // check if Player defeated all Invaders in current level
    bool level_cleared() {
      for (int i = 0; i < NUM_ENEMIES; i++) {
        if (enemies[i].get_strength() > 0) {
          return false;
        }
      }
      return true;
    }

    // set up a level
    void reset_level() {
      if (player.get_lives() > 0) {
        gameSetup();
      }
    }
};

// a global variable that represents the game Space Invaders
Game game;

// see https://www.arduino.cc/reference/en/language/structure/sketch/setup/
void setup() {
  Serial.begin(9600);
  Serial.println("My Sketch has started");
  pinMode(BUTTON_PIN_NUMBER, INPUT);
  matrix.begin();
  game.gameSetup();
}

// see https://www.arduino.cc/reference/en/language/structure/sketch/loop/
void loop() {
  unsigned long timer = millis();  
  bool button_pressed;
  bool smooth_button = true;

  // smoothing fluctuating button_pressed values
  while ((millis() - timer) <= (1000/26)) {
    button_pressed = !(digitalRead(BUTTON_PIN_NUMBER) == HIGH);
    if (!button_pressed) {
      smooth_button = false;
    }
  }
  int potentiometer_value = analogRead(POTENTIOMETER_PIN_NUMBER);

  Serial.print(button_pressed);
  Serial.print(" ");
  Serial.print(smooth_button);
  Serial.print(" ");
  Serial.println(potentiometer_value);

  game.update(potentiometer_value, smooth_button);
}

// displays Level
void print_level(int level) {
  String lvl = "Level: ";
  String currentLvl = lvl + level;
  Serial.println(currentLvl);
  matrix.setTextColor(PURPLE.to_333());
  matrix.println(currentLvl);
}

// displays number of lives
void print_lives(int lives) {
  String numLives = "Lives: ";
  String currentLives = numLives + lives;
  Serial.println(currentLives);
  matrix.setTextColor(LIME.to_333());
  matrix.println(currentLives);
}

// displays "game over"
void game_over() {
  matrix.fillScreen(BLACK.to_333());
  Serial.println("Game Over");
  matrix.setTextColor(RED.to_333());
  matrix.println("Game Over");
  while (true) {};
}
