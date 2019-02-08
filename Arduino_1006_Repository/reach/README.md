# Arduino Micro Arcade Reach Code
Team Name: Chicken Nuggets

Team Member Names: Maggie (Yi) Xia, Cindy Gu, Jasmine Wang, Britney Cheng

Team Member UniqNames: yixia, cindygu, jazzyw, britc

Link to Demo Video: https://www.dropbox.com/s/zj1kf5ws9xj6fi5/tetris.MOV?dl=0

DECEMBER 1
- Updated README file

DECEMBER 3
- Began working on reach implementation
- Created new Tetromino class
- Formatted display for Tetris
- Finished and tested Tetromino draw()
- Added Game getters
- Changed write_number() to 4x4 pixels
- Allocated screen space
- Added time variables in the game
- Updated reach folder with adafruit-gfx and rgb-matrix-panel
- Planned out LED Display (shown) with a pixel editor
 ![](../images/LED%20Display%20Plan.png)

DECEMBER 4
- Implemented get_next_tetromino() function
- Added minor gameplay adjustments
- Changed "Lv", "Ln", "n", "h" font sizes
- Shifted display frame up
- Updated LED Display

![](../images/Updated%20Tetris%20LED%20Display.png)

DECEMBER 5
- Started Tetromino rotate() function
- Modified Tetromino draw() function

DECEMBER 6
- Added PlacedPieces class for background grid
- Added constant variables for Tetromino colors
- Edited Tetromino class draw() function and tested
- Included randomSeed() to fix random()
- Modified print_screen() function
- Fixed get_next_tetromino function

DECEMBER 8
- Added PlacedPieces draw_grid() function, started clear_lines() function
- Added Tetromino set_points(), lose(), will_crash(), mushmash(), and drop() functions
- Tested and modified Tetromino move_down(), erase() and draw() functions
- Started Tetromino rotate() funciton
- Pieces only move within constraints of background grid
- Modified Game update() function
- Modified "Game Over" message to be even cooler
- Connected the controller to the game using Processing 3
- Fixed microcontroller input problem with delayed reaction
DECEMBER 9

- Fixed PlacedPieces clear_line() function
- Created Game score variable and add_to_score() function to calculate score
- Added Tetromino drop_down() function
- Created TETRIS title to be displayed at the beginning
- Call title() with delay

DECEMBER 10
- Fixed some display and speed issues with hard dropping tetrominos
- Modified gameplay speed
- Modified score writing

DECEMBER 11
- Included level up message
- Added 'next' and 'hold' game features
- Added rainbow lines in title()
- Added start and select controller features
- Added button control instructions

** Note: instructions for how to play Tetris using the controller is located in instructions.txt
