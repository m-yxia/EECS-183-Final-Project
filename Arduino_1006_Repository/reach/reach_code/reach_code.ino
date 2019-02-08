/**
 * reach_code.ino
 * 
 * EECS 183, Fall 2018
 * Final Project: Reach
 * 
 * Maggie (Yi) Xia, Cindy Gu, Jasmine Wang, Britney Cheng
 * yixia, cindygu, jazzyw, britc
 * 
 * Implementation of Tetris
 * A tile-matching puzzle video game
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

// define colors of each tetromino
const int CYANMINO = 1;
const int BLUEMINO = 2;
const int ORANGEMINO = 3;
const int YELLOWMINO = 4;
const int GREENMINO = 5;
const int REDMINO = 6;
const int PURPLEMINO = 7;

// a global variable that represents the LED screen
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// the following functions display messages on the screen
void print_level(int level);
void print_lines(int linesCleared);
void level_up();
void game_over();
void display_score(int score);

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
const Color WHITE(2, 2, 2);
const Color LIME(2, 4, 0);
const Color BERRY(255, 0, 127);
const Color CYAN(0,255,255);


// custom function for changing x and y input in matrix.drawPixel()
void new_drawPixel(int x, int y, Color c) {
  matrix.drawPixel(abs(31 - y), x, c.to_333());
}


/* ~~~~~~~~~~~~~~ PlacedPieces Class ~~~~~~~~~~~~~~ */
// representing already placed pieces allowing for collision check and clearing lines
class PlacedPieces {
  public:
    int grid[10][20];

    // default constructor  
    PlacedPieces() {
      clear_grid();  
    }
    
    // getters
    int get_width() {
      return width;
    }

    int get_height() {
      return height;
    }

    int get_color(int x, int y) {
      return grid[x][y];
    }

    // deletes row
    void delete_row(int del) {    
      for (int i = 0; i < width; i++) {
        grid[i][del] = 0;
      }      
    }

    // shifts grid to fill in deleted row
    void shift(int del) {
      for (int i = del; i > 0; i--) {
        for (int j = 0; j < width; j++) {
          grid[j][i] = grid[j][i - 1];
        }
      }
    }

    // returns true if line was cleared
    bool clear_lines(int pos) {
      bool can_clear = true;
      for (int i = 0; i < width; i++) {
        if (grid[i][pos] == 0) can_clear = false;
      }
      if (can_clear) {
        delete_row(pos);
        draw_grid();
        delay(20);
        shift(pos);
        return true;
      }
      return false;
    }

    // turns grid all black
    void clear_grid() {
      for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
          grid[i][j] = 0;
        }
      }
    }
    
    // draws 10x20 background grid
    void draw_grid() {
      for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
          if (grid[i][j] == CYANMINO) {
            new_drawPixel(i + offset, j + offset, CYAN);
          }
          else if (grid[i][j] == BLUEMINO) {
            new_drawPixel(i + offset, j + offset, BLUE);
          }
          else if (grid[i][j] == ORANGEMINO) {
            new_drawPixel(i + offset, j + offset, ORANGE);
          }
          else if (grid[i][j] == YELLOWMINO) {
            new_drawPixel(i + offset, j + offset, YELLOW);
          }
          else if (grid[i][j] == GREENMINO) {
            new_drawPixel(i + offset, j + offset, GREEN);
          }
          else if (grid[i][j] == REDMINO) {
            new_drawPixel(i + offset, j + offset, RED);
          }
          else if (grid[i][j] == PURPLEMINO) {
            new_drawPixel(i + offset, j + offset, PURPLE);
          }
          else {
            new_drawPixel(i + offset, j + offset, BLACK);
          }
        }
      }
    }
              
  private:
    int width = 10;
    int height = 20;

    // offset from LED matrix to playable screen
    int offset = 1;
};


// a global variable that represents the Tetris grid
PlacedPieces placed;


/* ~~~~~~~~~~~~~~ Tetromino Class ~~~~~~~~~~~~~~ */
class Tetromino {
  public:
    
    // Constructors
    Tetromino() {
      x = 0;
      y = 0;
      colorVal = 0;
      rotation = 0;
      set_points();
    }
    
    // sets values for private data members
    Tetromino(int x_arg, int y_arg, int colorVal_arg) {
      x = x_arg;
      y = y_arg;
      colorVal = colorVal_arg;
      rotation = 0;
      set_points();
    }
    
    // sets values for private data members
    void initialize(int x_arg, int y_arg, int colorVal_arg) {
      x = x_arg;
      y = y_arg;
      colorVal = colorVal_arg;
      rotation = 0;
      set_points();
    }

    // sets values for private data members given only color
    void initialize_with_color(int colorVal_arg) {    
      colorVal = colorVal_arg;
      rotation = 0;

      // centering piece to start dropping at top of tetris grid
      if ((colorVal == 1) || (colorVal == 2) || (colorVal == 3) ||
          (colorVal == 5) || (colorVal == 6) || (colorVal == 7)) {
        x = 3;
      } 
      else {
        x = 2;
      }
      if (colorVal == 1) {
        y = -3;
      }
      else {
        y = -2;
      }

      set_points();
    }
    
    // getters
    int get_x() const {
      return x;
    }
    
    int get_y() const {
      return y;
    }
    
    int get_colorVal() const {
      return colorVal;
    }

    int get_x1() {
      return x1;
    }

    int get_x2() {
      return x2;
    }

    int get_x3() {
      return x3;
    }

    int get_x4() {
      return x4;
    }

    int get_y1() {
      return y1;
    }

    int get_y2() {
      return y2;
    }

    int get_y3() {
      return y3;
    }

    int get_y4() {
      return y4;
    }

    // returns true if moving results in collision
    bool will_crash(int move_x, int move_y) {
      int check_x[4] = { x1 + move_x, x2 + move_x, x3 + move_x, x4 + move_x };
      int check_y[4] = { y1 + move_y, y2 + move_y, y3 + move_y, y4 + move_y };

      for (int i : check_x) {
        if ((i < 0) || (i > 9)) {
          return true;
        }
      }
      
      for (int i : check_y) {
        if (i > 19) {
          return true;
        }
      }

      for (int i = 0; i < 4; i++) {
        if (check_y[i] > 0) {
          if (placed.grid[check_x[i]][check_y[i]] != 0) {
            return true;
          }
        }
      }

      return false;
    }

    // checks game over
    bool lose() {
      if (will_crash(0, 0)) {
        return true;
      }
      return false;
    }

    // move tetrominoes
    void move_down() {
      if ((colorVal > 0) && (!will_crash(0, 1))) {
        y++;
      }
      set_points();
    }

    void move_left() {
      if ((colorVal > 0) && (!will_crash(-1, 0))){
        x--;
      }
      set_points();
    }

    void move_right() {
      if ((colorVal > 0) && (!will_crash(1, 0))) {
        x++;
      }
      set_points();
    }    

    // rotating clockwise
    // increments rotation if successful
    void rotate_clock() {
      if (colorVal > 0) {
        rotation++;
        if (rotation == 4) {
          rotation = 0;
        }
        set_points();

        if (will_crash(0,0)) {
          rotation--;
          if (rotation == -1) {
            rotation = 3;
          }
          set_points();
        }
      }
    }

    // rotating counterclockwise
    // decrements rotation if successful
    void rotate_counter() {
      // if button is pressed, rotates the tetromino
      // based off of draw_with_rgb function
      if (colorVal > 0) {
        rotation--;
        if (rotation == -1) {
          rotation = 3;
        }
        set_points();

        if (will_crash(0,0)) {
          rotation++;
          if (rotation == 4) {
            rotation = 0;
          }
          set_points();
        }
      }
    }

    // inserts last position of a piece into placed before reset
    void mushmash() {
      placed.grid[x1][y1] = colorVal;
      placed.grid[x2][y2] = colorVal;
      placed.grid[x3][y3] = colorVal;
      placed.grid[x4][y4] = colorVal;
    }

    // draws tetrominos depending on color
    void draw() {
      if (colorVal == CYANMINO) {
        new_drawPixel(x1 + offset, y1 + offset, CYAN);
        new_drawPixel(x2 + offset, y2 + offset, CYAN);
        new_drawPixel(x3 + offset, y3 + offset, CYAN);
        new_drawPixel(x4 + offset, y4 + offset, CYAN);
      }
      else if (colorVal == BLUEMINO) {
        new_drawPixel(x1 + offset, y1 + offset, BLUE);
        new_drawPixel(x2 + offset, y2 + offset, BLUE);
        new_drawPixel(x3 + offset, y3 + offset, BLUE);
        new_drawPixel(x4 + offset, y4 + offset, BLUE);
      }
      else if (colorVal == ORANGEMINO) {
        new_drawPixel(x1 + offset, y1 + offset, ORANGE);
        new_drawPixel(x2 + offset, y2 + offset, ORANGE);
        new_drawPixel(x3 + offset, y3 + offset, ORANGE);
        new_drawPixel(x4 + offset, y4 + offset, ORANGE);
      }
      else if (colorVal == YELLOWMINO) {
        new_drawPixel(x1 + offset, y1 + offset, YELLOW);
        new_drawPixel(x2 + offset, y2 + offset, YELLOW);
        new_drawPixel(x3 + offset, y3 + offset, YELLOW);
        new_drawPixel(x4 + offset, y4 + offset, YELLOW);
      }
      else if (colorVal == GREENMINO) {
        new_drawPixel(x1 + offset, y1 + offset, GREEN);
        new_drawPixel(x2 + offset, y2 + offset, GREEN);
        new_drawPixel(x3 + offset, y3 + offset, GREEN);
        new_drawPixel(x4 + offset, y4 + offset, GREEN);
      }
      else if (colorVal == REDMINO) {
        new_drawPixel(x1 + offset, y1 + offset, RED);
        new_drawPixel(x2 + offset, y2 + offset, RED);
        new_drawPixel(x3 + offset, y3 + offset, RED);
        new_drawPixel(x4 + offset, y4 + offset, RED);
      }
      else if (colorVal == PURPLEMINO) {
        new_drawPixel(x1 + offset, y1 + offset, PURPLE);
        new_drawPixel(x2 + offset, y2 + offset, PURPLE);
        new_drawPixel(x3 + offset, y3 + offset, PURPLE);
        new_drawPixel(x4 + offset, y4 + offset, PURPLE);
      }
    }

    // for drawing tetronimoes in 'next' and 'hold' boxes
    void alt_draw(int color) {
      if (color == CYANMINO) {
        x--;
        rotation = 1;
      }
      else {
        rotation = 0;
      }
      set_points();
      draw();
    }

    // draws black where the Tetromino used to be
    void erase() {
      new_drawPixel(x1 + offset, y1 + offset, BLACK);
      new_drawPixel(x2 + offset, y2 + offset, BLACK);
      new_drawPixel(x3 + offset, y3 + offset, BLACK);
      new_drawPixel(x4 + offset, y4 + offset, BLACK);
    }
  
  private:
    int offset = 1;
    int x;
    int y;
    int x1;
    int x2;
    int x3;
    int x4;
    int y1;
    int y2;
    int y3;
    int y4;    
    int colorVal;
    int rotation;

   // set points function
   // sets Tetromino color values depending on color and orientation
    void set_points() {
      if (colorVal > 0) {
        // cyan
        if (colorVal == CYANMINO) {
          if ((rotation == 0) || (rotation == 2)) {
            x1 = x + 0;
            x2 = x + 1;
            x3 = x + 2;
            x4 = x + 3;
            y1 = y + 3;
            y2 = y + 3;
            y3 = y + 3;
            y4 = y + 3;
          } 
          else if (rotation == 1) {
            x1 = x + 3;
            x2 = x + 3;
            x3 = x + 3;
            x4 = x + 3;
            y1 = y + 0;
            y2 = y + 1;
            y3 = y + 2;
            y4 = y + 3;
          }
          else if (rotation == 3) {
            x1 = x;
            x2 = x;
            x3 = x;
            x4 = x;
            y1 = y;
            y2 = y + 1;
            y3 = y + 2;
            y4 = y + 3;
          }
        }
        
        // blue
        else if (colorVal == BLUEMINO) {
          if (rotation == 0) {
            x1 = x + 1;
            x2 = x + 1;
            x3 = x + 2;
            x4 = x + 3;
            y1 = y + 2;
            y2 = y + 3;
            y3 = y + 3;
            y4 = y + 3;
          }
          else if (rotation == 1) {
            x1 = x + 1;
            x2 = x + 1;
            x3 = x + 1;
            x4 = x + 2;
            y1 = y + 1;
            y2 = y + 2;
            y3 = y + 3;
            y4 = y + 1;
          }
          else if (rotation == 2) {
            x1 = x + 0;
            x2 = x + 1;
            x3 = x + 2;
            x4 = x + 2;
            y1 = y + 2;
            y2 = y + 2;
            y3 = y + 2;
            y4 = y + 3;
          }
          else if (rotation == 3) {
            x1 = x + 1;
            x2 = x + 2;
            x3 = x + 2;
            x4 = x + 2;
            y1 = y + 3;
            y2 = y + 1;
            y3 = y + 2;
            y4 = y + 3;            
          }
        }
        
        // orange
        else if (colorVal == ORANGEMINO) {
          if (rotation == 0) {
            x1 = x + 1;
            x2 = x + 2;
            x3 = x + 3;
            x4 = x + 3;
            y1 = y + 3;
            y2 = y + 3;
            y3 = y + 3;
            y4 = y + 2;
          }
          else if (rotation == 1) {
            x1 = x + 2;
            x2 = x + 2;
            x3 = x + 2;
            x4 = x + 3;
            y1 = y + 1;
            y2 = y + 2;
            y3 = y + 3;
            y4 = y + 3;
          }
          else if (rotation == 2) {
            x1 = x + 1;
            x2 = x + 1;
            x3 = x + 2;
            x4 = x + 3;
            y1 = y + 2;
            y2 = y + 3;
            y3 = y + 2;
            y4 = y + 2;
          }
          else if (rotation == 3) {
            x1 = x + 1;
            x2 = x + 2;
            x3 = x + 2;
            x4 = x + 2;
            y1 = y + 1;
            y2 = y + 1;
            y3 = y + 2;
            y4 = y + 3;
          }
        }
        
        // yellow
        else if (colorVal == YELLOWMINO) {
          if ((rotation == 0) || (rotation == 1) || (rotation == 2) || (rotation == 3)) {
            x1 = x + 2;
            x2 = x + 3;
            x3 = x + 2;
            x4 = x + 3;
            y1 = y + 2;
            y2 = y + 2;
            y3 = y + 3;
            y4 = y + 3;
          }
        }
        
        // green
        else if (colorVal == GREENMINO) {
          if ((rotation == 0) || (rotation == 2)) {
            x1 = x + 2;
            x2 = x + 3;
            x3 = x + 1;
            x4 = x + 2;
            y1 = y + 2;
            y2 = y + 2;
            y3 = y + 3;
            y4 = y + 3;
          }
          else if ((rotation == 1) || (rotation == 3)) {
            x1 = x + 1;
            x2 = x + 1;
            x3 = x + 2;
            x4 = x + 2;
            y1 = y + 1;
            y2 = y + 2;
            y3 = y + 2;
            y4 = y + 3;
          }
        }
        
        // red
        else if (colorVal == REDMINO) {
          if ((rotation == 0) || (rotation == 2)) {
            x1 = x + 1;
            x2 = x + 2;
            x3 = x + 2;
            x4 = x + 3;
            y1 = y + 2;
            y2 = y + 2;
            y3 = y + 3;
            y4 = y + 3;
          }
          else if ((rotation == 1) || (rotation == 3)) {
            x1 = x + 1;
            x2 = x + 1;
            x3 = x + 2;
            x4 = x + 2;
            y1 = y + 2;
            y2 = y + 3;
            y3 = y + 1;
            y4 = y + 2;
          }
        }
        
        // purple
        else if (colorVal == PURPLEMINO) {
          if (rotation == 0) {
            x1 = x + 2;
            x2 = x + 1;
            x3 = x + 2;
            x4 = x + 3;
            y1 = y + 2;
            y2 = y + 3;
            y3 = y + 3;
            y4 = y + 3;
          }
          else if (rotation == 1) {
            x1 = x + 1;
            x2 = x + 1;
            x3 = x + 1;
            x4 = x + 2;
            y1 = y + 1;
            y2 = y + 2;
            y3 = y + 3;
            y4 = y + 2;            
          }
          else if (rotation == 2) {
            x1 = x + 1;
            x2 = x + 2;
            x3 = x + 2;
            x4 = x + 3;
            y1 = y + 2;
            y2 = y + 2;
            y3 = y + 3;
            y4 = y + 2;
          }
          else if (rotation == 3) {
            x1 = x + 2;
            x2 = x + 3;
            x3 = x + 3;
            x4 = x + 3;
            y1 = y + 2;
            y2 = y + 1;
            y3 = y + 2;
            y4 = y + 3;
          }
        }
      }
    }
};


// custom function to return the max of four values
int max_4(int a, int b, int c, int d) {
  int x = max(a, b);
  int y = max(c, d);
  return max(x, y);
}


// reset function
void(* resetFunc) (void) = 0;


/* ~~~~~~~~~~~~~~ Game Class ~~~~~~~~~~~~~~ */
class Game {
  public:

    // Default constructor
    Game() {
      level = 1;
      lines = 0;
      score = 0;
      time = 0;
      pressed = false;
      held = false;
      started = false;
      ended = false;
    }

    // getters
    int get_level() {
      return level;
    }

    int get_lines() {
      return lines;
    }

    int get_score() {
      return score;
    }

    /* using NES Tetris randomizer to figure out next piece
     * rolls an 8 sided die with 1-7 being the the 7 tetrominoes and 8 being reroll,
     * If you get the same piece as the last piece you got, or you hit the reroll number, 
     * roll a 2nd 7 sided die to determine the piece.
     */
    int get_next_tetromino(int previousPiece) {      
      int pieceOne = random(1,8);
      int ret = 0;
      if (pieceOne == previousPiece || pieceOne == 8) {
        ret = random(1,7);
      }
      // if die lands on 8, rerolls with 7 sided die
      else {
        ret = pieceOne;
      }
      return ret;
    }        

    // move or rotate tetromino if pressed is false
    void can_move_down() {
      if (!pressed) {
        play.move_down();
      }
      score++;
      pressed = true;
    }

    void can_move_left() {
      if (!pressed) {
        play.move_left();
      }
      pressed = true;
    }

    void can_move_right() {
      if (!pressed) {
        play.move_right();
      }
      pressed = true;
    }

    void can_rotate_clock() {
      if (!pressed) {
        play.rotate_clock();
      }
      pressed = true;
    }
    
    void can_rotate_counter() {
      if (!pressed) {
        play.rotate_counter();
      }
      pressed = true;
    }

    // drops tetromino down if pressed is false
    void can_drop_down() {
      if (!pressed) {
        while(!play.will_crash(0, 1)) {
          play.erase();
          play.move_down();          
          play.draw();
          print_screen();
          delay(18);
        }     
      }
      score += 2;      
      pressed = true;
    }

    // holds tetronimo
    void can_hold() {
      if (!held) {
        if (!pressed) {
          int hold_color = play.get_colorVal();
          if (hold.get_colorVal() == 0) {
            play.erase();
            play.initialize_with_color(next.get_colorVal());
            next.erase();
            next.initialize(10, 4, get_next_tetromino(play.get_colorVal()));
            hold.initialize(10, 16, hold_color);
            play.draw();
            next.alt_draw(next.get_colorVal());
            hold.alt_draw(hold.get_colorVal());
          }
          else {
            play.erase();
            play.initialize_with_color(hold.get_colorVal());
            hold.erase();
            hold.initialize(10, 16, hold_color);
            hold.alt_draw(hold.get_colorVal());
          }
        }
        pressed = true;
      }
    }

    // adds points to score depending on number of lines cleared and level
    void add_to_score(int level_arg, int lines_cleared) {
      int incr_score = 0;
      if (lines_cleared == 1) {
        incr_score = (level_arg * 4);
      }
      else if (lines_cleared == 2) {
        incr_score = (level_arg * 10);
      }
      else if (lines_cleared == 3) {
        incr_score = (level_arg * 30);
      }
      else if (lines_cleared == 4) {
        incr_score = (level_arg * 120);
      }
      score += incr_score;
    }

    // initializes game screen
    void gameSetup() {
      title();      
    }

    // include input from microcontroller
    void update(char input) {

      // if the start button has not been pressed,
      // title continues to be displayed
      if (!started) {
        if (input == 's') {
          matrix.fillScreen(BLACK.to_333());
          print_screen();
          play.initialize_with_color(get_next_tetromino(play.get_colorVal()));
          next.initialize(10, 4, get_next_tetromino(play.get_colorVal()));
          hold.initialize(10, 16, 0);
          play.draw();
          next.alt_draw(next.get_colorVal());
          started = true;     
        }
      }
      
      // if the game has been started
      else {
        // lose condition
        if (play.lose()) {
          if (!ended) {
            delay(800);
            game_over();
            display_score(score);
            ended = true;
          }
          if (input == 'b') {
            resetFunc();
          }
        }
        else {
          print_screen();
          print_level(level);
          print_lines(lines);
          placed.draw_grid();      
          
          play.erase();    
          // piece moves down every set number of seconds depending on level
          unsigned long currentTime = millis();
          int max_seconds = 0;
    
          for (int i = level; i < level + 1; i++) {
            if (level < 11) {
              max_seconds = 1000 - ((i - 1) * 90);
            }
            else {
              max_seconds = 100 - (i * 4);
            }
            if (currentTime - time > max_seconds) {            
              play.move_down();
              time = currentTime;
             }    
           }

          
          // controller input
          if (input == 'D') {
            can_move_down();
          }
          else if (input == 'L') {      
            can_move_left();        
          }
          else if (input == 'R') {       
            can_move_right();        
          }
          else if (input == 'l') {        
            can_rotate_counter();        
          }
          else if (input == 'r') {        
            can_rotate_clock();        
          }  
          else if (input == 'A') {
            can_drop_down();    
          }
          else if (input == 'B') {
            can_hold();
            held = true;
          }
    
          // check wether lines are cleared right after last position is set
          int last_pos = max_4(play.get_y1(), play.get_y2(), play.get_y3(), play.get_y4());
          
          // recycles player tetromino for next piece
          if (play.will_crash(0, 1)) {
            play.mushmash();
    
            int count_false = 0;
            int num_cleared = 0;
            for (int n = 0; n < 4; n++) {
              if (placed.clear_lines(last_pos - count_false)) {
                erase_lines(lines);
                lines++;
                num_cleared++;
    
                // changes level after 10 lines cleared
                if ((lines % 10) == 0) {
                  erase_level(level);
                  level++;
                  
                  level_up();
                  delay(750);
                  placed.clear_grid();
                }
              }
              else {
                count_false++;
              }
            }
    
            // update score
            add_to_score(level, num_cleared);
    
            // reset piece
            play.initialize_with_color(next.get_colorVal());
            next.erase();
            next.initialize(10, 4, get_next_tetromino(play.get_colorVal()));
            play.draw();
            next.alt_draw(next.get_colorVal());
            held = false;
          }
    
          if (input == ' ') {
            pressed = false;
          }
          play.draw();
        }
      }
    }

  private:
    int level;
    int lines;
    int score;
    unsigned long curr_time;
    unsigned long past_time;
    bool pressed;
    bool held;
    bool started;
    bool ended;
    Tetromino play;
    Tetromino hold;
    Tetromino next;
    unsigned long time;
};

// global variable that represents the game Tetris
Game game;


/* ~~~~~~~~~~~~~~ setup ~~~~~~~~~~~~~~ */
// see https://www.arduino.cc/reference/en/language/structure/sketch/setup/
void setup() {
  Serial.begin(9600);  
  
  // ensure random() actually gives a believably random value
  int seed = analogRead(3);
  randomSeed(seed);
  random(2435262);

  char val = ' ';
  // setting up of microcontroller and reading of data
  if (Serial.available()) { 
    val = Serial.read();
  }

  matrix.begin();
  game.gameSetup();
}


/* ~~~~~~~~~~~~~~ loop ~~~~~~~~~~~~~~ */
// see https://www.arduino.cc/reference/en/language/structure/sketch/loop/
void loop() {
  // Data received from the serial port
  char val; 
  char input;

  // setting up of microcontroller and reading of data
  if (Serial.available()) { 
    val = Serial.read();
  }

  game.update(val);
}


/* ~~~~~~~~~~~~~~ write_number ~~~~~~~~~~~~~~ */
// function for displaying numbers
void write_number(int x, int y, int n, Color color) {
  if (n == 0) {
    for (int i = x + 1; i < x + 4; i++) {
      new_drawPixel(i, y, color);
      new_drawPixel(i, y + 4, color);
    }
    for (int i = y + 1; i < y + 4; i++) {
      new_drawPixel(x, i, color);
      new_drawPixel(x + 4, i, color);
    }
  }
  else if (n == 1) {
    new_drawPixel(x + 2, y, color);
    new_drawPixel(x + 1, y + 1, color);
    new_drawPixel(x + 2, y + 1, color);
    new_drawPixel(x, y + 2, color);
    new_drawPixel(x + 2, y + 2, color);
    new_drawPixel(x + 2, y + 3, color);
    for (int i = x; i < x + 5; i++) {
      new_drawPixel(i, y + 4, color);
    }
  }
  else if (n == 2) {
    for (int i = x; i < x + 4; i++) {
      new_drawPixel(i, y, color);
    }
    y++;
    new_drawPixel(x + 4, y, color);
    y++;
    for (int i = x + 1; i < x + 4; i++) {
      new_drawPixel(i, y, color);
    }
    y++;
    new_drawPixel(x, y, color);
    y++;
    for (int i = x + 1; i < x + 5; i++) {
      new_drawPixel(i, y, color);
    }
  }
  else if (n == 3) {
    for (int i = x; i < x + 4; i++) {
      new_drawPixel(i, y, color);
    }
    y++;
    new_drawPixel(x + 4, y, color);
    y++;
    for (int i = x + 1; i < x + 4; i++) {
      new_drawPixel(i, y, color);
    }
    y++;
    new_drawPixel(x + 4, y, color);
    y++;
    for (int i = x; i < x + 5; i++) {
      new_drawPixel(i, y, color);
    }
  }
  else if (n == 4) {
    for (int i = y; i < y + 3; i++) {
      new_drawPixel(x, i, color);
      new_drawPixel(x + 3, i, color);
    }
    y += 3;
    for (int i = x; i < x + 5; i++) {
      new_drawPixel(i, y, color);
    }
    y++;
    new_drawPixel(x + 3, y, color);
  }
  else if (n == 5) {
    for (int i = x; i < x + 5; i++) {
      new_drawPixel(i, y, color);
    }
    y++;
    new_drawPixel(x, y, color);
    y++;
    for (int i = x; i < x + 4; i++) {
      new_drawPixel(i, y, color);
    }
    y++;
    new_drawPixel(x + 4, y, color);
    y++;
    for (int i = x; i < x + 4; i++) {
      new_drawPixel(i, y, color);
    }
  }
  else if (n == 6) {
    for (int i = x + 1; i < x + 5; i++) {
      new_drawPixel(i, y, color);
    }
    y++;
    new_drawPixel(x, y, color);
    y++;
    for (int i = x; i < x + 4; i++) {
      new_drawPixel(i, y, color);
    }
    y++;
    new_drawPixel(x, y, color);
    new_drawPixel(x + 4, y, color);
    y++;
    for (int i = x + 1; i < x + 4; i++) {
      new_drawPixel(i, y, color);
    }
  }
  else if (n == 7) {
    for (int i = x; i < x + 5; i++) {
      new_drawPixel(i, y, color);
    }
    y++;
    new_drawPixel(x, y, color);
    new_drawPixel(x + 4, y, color);
    y++;
    new_drawPixel(x + 3, y, color);
    y++;
    new_drawPixel(x + 2, y, color);
    new_drawPixel(x + 2, y + 1, color);
  }
  else if (n == 8) {
    for (int i = x + 1; i < x + 4; i++) {
      new_drawPixel(i, y, color);
    }
    y++;
    new_drawPixel(x, y, color);
    new_drawPixel(x + 4, y, color);
    y++;
    for (int i = x + 1; i < x + 4; i++) {
      new_drawPixel(i, y, color);
    }
    y++;
    new_drawPixel(x, y, color);
    new_drawPixel(x + 4, y, color);
    y++;
    for (int i = x + 1; i < x + 4; i++) {
      new_drawPixel(i, y, color);
    }
  }
  else if (n == 9) {
    for (int i = x + 1; i < x + 4; i++) {
      new_drawPixel(i, y, color);
    }
    y++;
    new_drawPixel(x, y, color);
    new_drawPixel(x + 4, y, color);
    y++;
    for (int i = x + 1; i < x + 5; i++) {
      new_drawPixel(i, y, color);
    }
    y++;
    new_drawPixel(x + 4, y, color);
    y++;
    for (int i = x; i < x + 4; i++) {
      new_drawPixel(i, y, color);
    }
  }
}


/* ~~~~~~~~~~~~~~ print_screen ~~~~~~~~~~~~~~ */
// displays screen
void print_screen() {  
  //frame
  for (int i = 0; i < 12; i++) {
    new_drawPixel(i, 0, WHITE);
    new_drawPixel(i, 21, WHITE);
  }
  for (int i = 1; i < 21; i++) {
    new_drawPixel(0, i, WHITE);
    new_drawPixel(11, i, WHITE);
  }
  for (int i = 12; i < 16; i++) {
    new_drawPixel(i, 4, WHITE);
    new_drawPixel(i, 9, WHITE);
  }
  for (int i = 12; i < 16; i++) {
    new_drawPixel(i, 16, WHITE);
    new_drawPixel(i, 21, WHITE);
  }
  for (int i = 4; i < 10; i++) {
    new_drawPixel(15, i, WHITE);
  }
  for (int i = 16; i < 22; i++) {
    new_drawPixel(15, i, WHITE);
  }
  // n
  for (int i = 1; i < 4; i++) {
     new_drawPixel(13, i, ORANGE);
     new_drawPixel(15, i, ORANGE);
  }
  new_drawPixel(14, 1, ORANGE);
  // h
  for (int i = 13; i < 16; i++) {
     new_drawPixel(13, i, ORANGE);
  }
  new_drawPixel(14, 14, ORANGE);
  new_drawPixel(15, 14, ORANGE);
  new_drawPixel(15, 15, ORANGE);

  // LV
  for (int i = 23; i < 26; i++) {
    new_drawPixel(0, i, BERRY);
  }
  new_drawPixel(1, 25, BERRY);
  new_drawPixel(2, 25, BERRY);
  new_drawPixel(3, 23, BERRY);
  new_drawPixel(3, 24, BERRY);
  new_drawPixel(4, 25, BERRY);
  new_drawPixel(5, 23, BERRY);
  new_drawPixel(5, 24, BERRY);

  // LN
  for (int i = 28; i < 31; i++) {
    new_drawPixel(0, i, BERRY);
  }
  new_drawPixel(1, 30, BERRY);
  new_drawPixel(2, 30, BERRY);
  for (int i = 28; i < 31; i++) {
    new_drawPixel(3, i, BERRY);
    new_drawPixel(5, i, BERRY);
  }
  new_drawPixel(4, 28, BERRY);
}

/* ~~~~~~~~~~~~~~ print_level ~~~~~~~~~~~~~~ */
// displays level
void print_level(int level) {
  int lvl_tens = level / 10;
  int lvl_ones = level % 10;
  write_number(6, 22, lvl_tens, YELLOW);
  write_number(11, 22, lvl_ones, BLUE);
}

// removes level display from screen
void erase_level(int level) {
  int lvl_tens = level / 10;
  int lvl_ones = level % 10;  
  write_number(6, 22, lvl_tens, BLACK);
  write_number(11, 22, lvl_ones, BLACK);
}

/* ~~~~~~~~~~~~~~ print_lines ~~~~~~~~~~~~~~ */
// displays number of lines
void print_lines(int lns) {
  int lns_tens = lns / 10;
  int lns_ones = lns % 10;  
  write_number(6, 27, lns_tens, BLUE);
  write_number(11, 27, lns_ones, YELLOW);
}

// removes line display from screen
void erase_lines(int lns) {
  int lns_tens = lns / 10;
  int lns_ones = lns % 10;
  write_number(6, 27, lns_tens, BLACK);
  write_number(11, 27, lns_ones, BLACK);
}


/* ~~~~~~~~~~~~~~ draw_pixel_with_delay ~~~~~~~~~~~~~~ */
// draws pixels with a delay of 12
void draw_pixel_with_delay(int x, int y, Color color) {
  new_drawPixel(x, y, color);
  delay(10);
}

/* ~~~~~~~~~~~~~~ title ~~~~~~~~~~~~~~ */
void title() {
  matrix.fillScreen(BLACK.to_333());

  // lines on the side of TETRIS title
  // left red
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 4; j++) {
      draw_pixel_with_delay(i, j, RED);
    }
  }
  
  // right orange
  for (int i = 14; i < 16; i++) {
    for (int j = 5; j < 10; j++) {
      draw_pixel_with_delay(i, j, ORANGE);
    }
  }
  
  // left yellow
  for (int i = 0; i < 2; i++) {
    for (int j = 11; j < 15; j++) {
      draw_pixel_with_delay(i, j, YELLOW);
    }
  }
  
  // right green
  for (int i = 14; i < 16; i++) {
    for (int j = 16; j < 21; j++) {
      draw_pixel_with_delay(i, j, GREEN);
    }
  }
  
  // left cyan
  for (int i = 0; i < 2; i++) {
    for (int j = 22; j < 26; j++) {
      draw_pixel_with_delay(i, j, CYAN);
    }
  }
  
  // right purple
  for (int i = 14; i < 16; i++) {
    for (int j = 27; j < 32; j++) {
      draw_pixel_with_delay(i, j, PURPLE);
    }
  }
  
  // right red
  for (int i = 14; i < 16; i++) {
    for (int j = 0; j < 4; j++) {
      draw_pixel_with_delay(i, j, RED);
    }
  }
  
  // left orange
  for (int i = 0; i < 2; i++) {
    for (int j = 5; j < 10; j++) {
      draw_pixel_with_delay(i, j, ORANGE);
    }
  }
  
  // right yellow
  for (int i = 14; i < 16; i++) {
    for (int j = 11; j < 15; j++) {
      draw_pixel_with_delay(i, j, YELLOW);
    }
  }
  
  // left green
  for (int i = 0; i < 2; i++) {
    for (int j = 16; j < 21; j++) {
      draw_pixel_with_delay(i, j, GREEN);
    }
  }
  
  // right cyan
  for (int i = 14; i < 16; i++) {
    for (int j = 22; j < 26; j++) {
      draw_pixel_with_delay(i, j, CYAN);
    }
  }
  
  // left purple
  for (int i = 0; i < 2; i++) {
    for (int j = 27; j < 32; j++) {
      draw_pixel_with_delay(i, j, PURPLE);
    }
  }
  
  // Displays "TETRIS" vertically
  // Drawing 'T'
  for (int i = 5; i < 11; i++) {
    // first T
    draw_pixel_with_delay(i, 0, RED);
    draw_pixel_with_delay(i, 1, RED);

    // second T
    draw_pixel_with_delay(i, 11, YELLOW);
    draw_pixel_with_delay(i, 12, YELLOW);
  }
  for (int i = 7; i < 9; i++) {
    // first T
    draw_pixel_with_delay(i, 2, RED);
    draw_pixel_with_delay(i, 3, RED);

    // second T
    draw_pixel_with_delay(i, 13, YELLOW);
    draw_pixel_with_delay(i, 14, YELLOW);
  }
  
  // Drawing 'E'
  // vertical part
  for (int i = 5; i < 10; i++) {
    draw_pixel_with_delay(6, i, ORANGE);
  }
  for (int i = 7; i < 10; i++) {
    // for 'E'
    draw_pixel_with_delay(i, 5, ORANGE);
    draw_pixel_with_delay(i, 9, ORANGE);

    // for 'S'
    draw_pixel_with_delay(i, 27, PURPLE);
  }

  // Drawing 'R'
  for (int i = 16; i < 21; i++) {
    draw_pixel_with_delay(6, i, GREEN);
  }
  for (int i = 7; i < 9; i++) {
    // for 'E'
    draw_pixel_with_delay(i, 7, ORANGE);
    
    // for 'R'
    draw_pixel_with_delay(i, 16, GREEN);
    draw_pixel_with_delay(i, 18, GREEN);
    draw_pixel_with_delay(i, 19, GREEN);

    // for 'S'
    draw_pixel_with_delay(i, 29, PURPLE);
  }
  for (int i = 17; i < 19; i++) {
    draw_pixel_with_delay(9, i, GREEN);
  }
  draw_pixel_with_delay(9, 20, GREEN);

  // Drawing 'I'
  for (int i = 22; i < 26; i++) {
    draw_pixel_with_delay(7, i, CYAN);
    draw_pixel_with_delay(8, i, CYAN);
  }

  // Drawing 'S'
  for (int i = 6; i < 9; i++) {
    draw_pixel_with_delay(i, 31, PURPLE);
  }
  draw_pixel_with_delay(6, 28, PURPLE);
  draw_pixel_with_delay(9, 30, PURPLE);
}

void level_up() {
  placed.clear_grid();
  placed.draw_grid();
  
  // writes "LVL"

  // draws two 'L's
  for (int i = 3; i < 7; i++) {
    new_drawPixel(2, i, RED); 
  }
  for (int i = 15; i < 19; i++) {
    new_drawPixel(2, i, RED); 
  }
  for (int i = 3; i < 5; i++) {
    new_drawPixel(i, 6, RED); 
    new_drawPixel(i, 18, RED); 
  }

  // draws 'V'
  for (int i = 2; i < 5; i += 2) {
    for (int j = 9; j < 12; j++) {
      new_drawPixel(i, j, RED);
    }
  }
  new_drawPixel(3, 12, RED);

  // writes "UP"
  for (int i = 6; i < 10; i += 3) {
    for (int j = 5; j < 9; j++) {
      new_drawPixel(i, j, RED);
    }
  }
  for (int i = 7; i < 9; i++) {
    new_drawPixel(i, 9, RED);
    new_drawPixel(i, 11, RED);
    new_drawPixel(i, 14, RED);
  }
  for (int i = 12; i < 18; i++) {
    new_drawPixel(6, i, RED);
  }
  for (int i = 12; i < 14; i++) {
    new_drawPixel(9, i, RED);
  }

  delay(750);
  placed.clear_grid();
}

/* ~~~~~~~~~~~~~~ game_over ~~~~~~~~~~~~~~ */
// displays "game over"
void game_over() {
  matrix.fillScreen(BLACK.to_333());
  
  // Displays "GAME" and "OVER" vertically
  for (int i = 4; i < 7; i++) {
    draw_pixel_with_delay(2, i, RED);
    draw_pixel_with_delay(9, i, RED);
    draw_pixel_with_delay(13, i, RED);
  }
  for (int i = 3; i < 6; i++) {
    draw_pixel_with_delay(i, 3, RED);
    draw_pixel_with_delay(i, 7, RED);
    draw_pixel_with_delay(i, 10, RED);
  }
  
  for (int i = 5; i < 7; i++) {
    draw_pixel_with_delay(6, i, RED);
  }
  draw_pixel_with_delay(4, 5, RED);
  draw_pixel_with_delay(5, 5, RED);
  
  for (int i = 11; i < 15; i++) {
    draw_pixel_with_delay(2, i, RED);
    draw_pixel_with_delay(6, i, RED);
  }
  for (int i = 2; i < 7; i++) {
    draw_pixel_with_delay(i, 12, RED);
    draw_pixel_with_delay(i, 24, RED);
    draw_pixel_with_delay(i, 28, RED);
  }
  for (int i = 17; i < 22; i++) {
    draw_pixel_with_delay(2, i, RED);
    draw_pixel_with_delay(6, i, RED);
    draw_pixel_with_delay(9, i, RED);
  }
  draw_pixel_with_delay(3, 18, RED);
  draw_pixel_with_delay(4, 19, RED);
  draw_pixel_with_delay(5, 18, RED);  
  for (int i = 2; i < 6; i++) {
    draw_pixel_with_delay(i, 26, RED);
  }
  for (int i = 24; i < 29; i++) {
    draw_pixel_with_delay(2, i, RED);
    draw_pixel_with_delay(9, i, RED);
  }

  // for "OVER"
  for (int i = 10; i < 13; i++) {
    draw_pixel_with_delay(i, 3, RED);
    draw_pixel_with_delay(i, 7, RED);
    draw_pixel_with_delay(i, 24, RED);
    draw_pixel_with_delay(i, 26, RED);
  }
  
  for (int i = 10; i < 13; i++) {
    // draws part of letter 'V'
    draw_pixel_with_delay(9, i, RED);
    draw_pixel_with_delay(13, i, RED);

    // for 'E' in "OVER"
    draw_pixel_with_delay(i, 19, RED);
  }
  draw_pixel_with_delay(10, 13, RED);
  draw_pixel_with_delay(11, 14, RED);
  draw_pixel_with_delay(12, 13, RED);

  // draws parts of 'E' for "OVER"
  for (int i = 9; i < 14; i++) {
    draw_pixel_with_delay(i, 17, RED);
    draw_pixel_with_delay(i, 21, RED);
  }

  // for letter 'R'
  draw_pixel_with_delay(13, 25, RED);
  draw_pixel_with_delay(12, 27, RED);
  draw_pixel_with_delay(13, 28, RED);

  delay(1500);
}

// displays score
void display_score(int score) {
  matrix.fillScreen(BLACK.to_333());
  
  int ten_thousands = score / 10000;
  int thousands = score / 1000 % 10;
  int hundreds = score / 100 % 10;
  int tens = score / 10 % 10;
  int ones = score % 10;

   // writes word "SCORE" 
  for (int i = 3; i < 6; i++) {
    draw_pixel_with_delay(i, 3, GREEN);
    draw_pixel_with_delay(i, 5, GREEN);
    draw_pixel_with_delay(i, 13, GREEN);
    draw_pixel_with_delay(i, 17, GREEN);
    draw_pixel_with_delay(i, 19, GREEN);
    draw_pixel_with_delay(i, 21, GREEN); 
  }
  for (int i = 2; i < 6; i += 3) {
    draw_pixel_with_delay(2, i, GREEN);
  }
  for (int i = 1; i < 5; i += 3) {
    draw_pixel_with_delay(6, i, GREEN);
  }
  for (int i = 8; i < 11; i++) {
    draw_pixel_with_delay(2, i, GREEN); 
  }
  for (int i = 14; i < 17; i++) {
    draw_pixel_with_delay(2, i, GREEN);
    draw_pixel_with_delay(6, i, GREEN);
  }
  for (int i = 19; i < 24; i++) {
    draw_pixel_with_delay(2, i, GREEN);
  }
  draw_pixel_with_delay(6, 20, GREEN);
  draw_pixel_with_delay(6, 23, GREEN);
  draw_pixel_with_delay(5, 22, GREEN);
  for (int i = 25; i < 30; i++) {
    draw_pixel_with_delay(2, i, GREEN);
  }
  for (int i = 3; i < 7; i++) {
    draw_pixel_with_delay(i, 1, GREEN);
    draw_pixel_with_delay(i, 7, GREEN);
    draw_pixel_with_delay(i, 11, GREEN);
    draw_pixel_with_delay(i, 25, GREEN);
    draw_pixel_with_delay(i, 27, GREEN);
    draw_pixel_with_delay(i, 29, GREEN);
  }
  
  // writes numerical score
  write_number(9, 1, ten_thousands, BLUE);
  write_number(9, 7, thousands, YELLOW);
  write_number(9, 13, hundreds, BLUE);
  write_number(9, 19, tens, YELLOW);
  write_number(9, 25, ones, BLUE);

  delay(1500);  
}
