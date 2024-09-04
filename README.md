# Wraptris

An old C project from 2014.

Command prompt Tetris clone, but it "wraps around".

"Wraps around"? Simply put, the game board's left and right edges are connected, making the board more akin to the surface of a cylinder than a flat rectangle.

As a result, you can move each tetromino to the left or right as many times as you want without hitting an edge, instead looping around the other side.

The left display follows the curent tetromino, keeping it centered on screen.  
The right display is fixed relative to the board.

## Controls

| Key     |                                            Action |
| :------ | ------------------------------------------------: |
| W,S,A,D |                             Up, down, left, right |
| Enter   |                                            Select |
| P       | Earthquake! Shuffles the board. (because why not) |

## Disclaimers

The graphics mmight be extremely choppy. Each frame is generated by clearing the screen and then redrawing each letter individually. It's not pretty.

Options was never implemented. Selecting it is the same as selecting Exit.

---

2014, Erik Hallgren.
