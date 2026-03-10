# Exercise: Concurrency & I/O

This exercise exemplifies the use of Posix I/O and concurrency to implement a retro space shooter game on your Raspberry Pi. 

*In the following, the hand-in requirements, the game scenario, and the provided hardware and software will be presented. This is followed by the activities that you must do to solve the exercise.*

### Focus

* Working with multiple threads
* Thread synchronization with **mutexes** and **condition variables**
* GPIO access using Posix I/O

## Prerequisites

You must [fork](https://www.geeksforgeeks.org/git/how-to-fork-a-project-in-gitlab/) this repository to you own Gitlab group and clone it to you RPi from there.
On your RPi using VS Code's Git extension or using a terminal:
```bash
cd
git clone <your-space-shooter-repo>
```

To communicate with the OLED display, the required libraries must be installed on your RPI. You can find files and instructions here: [gitlab.au.dk/platforms/rpi5/ecesyshat/](https://gitlab.au.dk/platforms/rpi5/ecesyshat/-/tree/main/Libraries?ref_type=heads)

Note!!! **Do not download the library into your space shooter project directory**

Check that *CMake* and *CMakeTools* extensions for VS Code are installed on your Raspberry Pi. Do this in VS Code after establishing the remote connections. After opening the space shooter project in VSCode on your RPi, choose the Cmake extension, pick the pencil in configurations and scan for kits. After 5 seconds, pick the pencil again and choose the Clang toolchain. Now you can build and launch the project on your RPi.

## Approval

This exercise must be handed-in. The following requirements must be met:

* Use **mutexes** and **condition variables** (no semaphores).
* Provide working implementations of Activities 1 to 3.

You must hand-in a PDF on FeedbackFruits containing the following:
* Front page with group number and members plus **link to your gitlab repo**
* Design descriptions, diagrams and answers to questions in the exercise

Your Gitlab repository must contain:
* Code for your solution
* Makefile / CMakeLists.txt to build your project   

To have your hand-in approved you must also peer-review a fellow student's submission. You can find the review criteria at the end of this page. Your submission occupies 75% of the grade, the review 25%.

## Space Shooter

You are tasked with building a simple **space shooter game** on a Raspberry Pi, displayed on a small **OLED screen**. The spaceship is controlled by **four direction buttons**, and the player can fire bullets using a **shooting button**. Some of the code will be provided to kick-start your work, but you will have to implement access to the button gpios, control the space ship and bullets among others.

### Game Requirements

* The spaceship starts at the left center of the screen.
* Direction buttons move it left/right/up/down.
* Tapping the shooting button fires one bullet (per tap)
* Holding the shooting button must fire only one bullet.
* The bullets move to the right at fixed speed.
* Enemies moves in from the right to left at fixed speed (random vertical positions).
* As a bullet and an enemy collide (meet) → bullet and enemy disappears, score increases.
* If an enemy reaches the left side → game over
* There should be no tearing/flickering on the display

Beside these requirements, you are allowed to create additional rules :-)

---

### Hardware Design

The ECE-SYS-HAT provides six buttons (switches), three LEDs, accelerometer, and an OLED dislay.

#### Buttons

You'll see the names of the buttons printed on the printed circuit board (PCB) as seen below. We need this information to assign the buttons to the correct game functions. All buttons are active-low ('0' is actice).

![ECE-SYS-HAT PCB](figures/ece_hat_800px.jpg)

#### Display

The Display is an 128 x 32 pixel OLED display. The pixels are oriented as shown below.

![OLED Display](figures/bitmaps-display_legend.png)

To move the ship *left*, you must *increment* the x-position. To move the ship *up*, you must *decrement* the y-position.

The individual bitmaps (ship, bullet, enemy) use a similar orientation. The provided ship and enemy are 8x7 pixels and the bullet is 8x1 pixels.

![Ship Bitmap](figures/bitmaps-ship.png)

This means that placing the ship at (0,0) will place the ship's upper-left corner in the display's upper-left corner. Placing the ship at (0,28) will place the ship's bottom three pixel rows below the screen. You will get run-time warnings if you place bitmaps outside the screen area.

```
   01234567 
28 ###.....
29 #####...
30 ######..
31 ########
   ######..
   #####...
   ###.....  
```

---

### Software Design

Since we are training working with threads, many of the software classes also contains threads to perform work for the individual class. The classes as shown in the Class Diagram below. 

![Class Diagram](figures/cd_space_shooter.png)

The game is assembled and started in *space_shooter.cpp*

#### `Accelerometer`

This class is a stub, but can be used in later extensions of the game.

The idea is: 
  * Reads the accelerometer at fixed intervals.
  * Updates a global "gravity vector" that affects object motion.

#### `Bullet`

Controls adding bullets and moving them on the screen.

**YOU MUST IMPLEMENT THE CONTENT OF THIS CLASS**

Contains two threads:

  1. Update the bullet vector when with one new bullet if the shoot button has been tapped (pressed-released)
  2. Update the bullet position at a fixed speed

#### `Buttons`

Takes button (gpio) inputs and publishes them for the game.

**YOU MUST IMPLEMENT THE CONTENT OF THIS CLASS**

1) The *Constructor* must be updated so, the gpio inputs are requested and prepared for **level-triggered** reading

2) Create at thread that runs at fixed interval to:
  * Read the GPIO-connected direction + shooting buttons all **level-triggered**.
  * Publish when shooting button has been tapped (pressed-released)
  * Update the shared *game state*.

3)  Implement `ReadMoveButtons()` that returns the latest reading of the move buttons as the following values:
  * up: {0, -1}
  * down {0, 1}
  * left: {-1 ,0}
  * right: {1, 0}
  * None pressed: {0, 0}   

#### `Display`

Runs a thread at fixed interval to:
* Draw the game state (spaceship, bullets, enemies)
* Draw score, number of enemies and bullets

#### `Enemy`

Runs a thread at fixed interval to:
* Spawn new enemies
* Move enemy position
* Delete enemies if they move out of screen (left)

#### `GameController`

Runs the main game control loop thread to:
* Provide collision detection of bullets and enemies. 
* Delete bullet and enemy from `GameState` if they collide.

A collision is detected just as the face of a bullet and the face of an enemy intersect.

![](figures/bitmaps-collision.png)

Note! The game controller thread deletes bullets and enemies if they collide. Both are stored in vectors. Erasing an element in a vector will invalidate current iteraters using those vectors.

#### `GameState`

Contains the current state of the game. It is accessed by almost all classes and contains vectors of bullets, enemies, x-y positions, score and more. 

#### `I2CDriver`

Implements I2C communication to the OLED screen to be used by the OLED library.

#### `Ship`

Runs a thread at fixed interval to:
* Update ship position according to Move Button state

#### `Main` (space_shooter)

The main loop does the following:
* Initialize I2C and OLED library
* Instantiate objects
* Start threads
* Join threads

---

### Activities

To implement the missing parts of the game, the following activities are proposed:

#### Activity 1: Implement Buttons Class

**Task 1:**
Implement the constructor for the `Buttons` to allocate gpios and set their direction. Buttons must be read with **level-triggered** I/O.

**Task 2:** Implement `buttonsThread`. The thread must run at a fixed interval and read all gpios (**level-trigggered**). 
The direction (move) button state must be accessable using `Buttons::readMoveButtons()`. See details in the function. 
Shoot button taps (press-relase) inputs must be signalled imidiately using `GameState::shootButton_cv`. 

#### Activity 2: Implement Bullets Class

**Task 1:** Implement `Bullet::bulletInputThread()`. This thread must wait for the bullet button to be tapped. When this has occurred, a new bullet must be added to `GameState.bullets` with x-y position at the nose of the space ship.

**Task 2:** Implement `Bullet::bulletUpdateThread()`. This thread must run at a fixed 25 ms interval and must update all `GameState.bullets`, so that they moves one pixel to the left. Changing the update interval effects the bullet movement speed. 

#### Activity 3: Analyse and fix synchronization issues

**Task 1:** Analyze how threads are currently synchronized. Draw an activity diagram showing the flow of the individual threads and the interaction between them. Argue whether synchronization is sufficient or not.

**Task 2:** Fix synchronization issues and test if requirements have been met.

Consider the following:
* Where is synchronization needed?
* Does your current implementation supports all requirements? Ex: Holding shooting button only emits ONE bullet? Emeny and bullet always dissapear when colliding?

If your current implementation does not fulfill the requirements 100%, then design and implement the required synchronization. It's considered unsafe to simply adjust times to try and fix synchronization issues. 

#### Optional Activities

If you finish up early, you are welcome to implement some of the folowing ideas:

* Use **double-buffering** for OLED updates to prevent flickering.
* If Ship and Enemy collide, game is over.
button).
* Introduce different bullet types and/or directions depending on which shooting button is pressed.
* Add a **pause** function (e.g., pressing right shooting

## Peer-review Evaluation Criteria

During peer-reviewing, you will be evaluated according to this rubrics. Read them like this: 
* Full fullfillment - You did as expected
* Partial fullfillment - You almost did as expected, but had deviations from **Full** similar to the described
* Insufficient - You did not quite do as a expected and had deviations similar to the descriptions in  **Partial** and **Insufficient**
</br>

| Goal                                       | Full fulfillment of goal                            | Partial fulfillment of goal                              | Insufficient fulfillment of goal                 |
|-------------------------------------------|---------------------------------------|---------------------------------------|------------------------------|
| Activity 1.1: Buttons Ctor | <ul><li> Correct GPIOs requested </li><li> Correct GPIO directions </li><li> File descriptors closed in destructor </li></ul> | <ul><li> Minor errors GPIO request / setting direction </li><li> Forget to close file descriptor  </li></ul>| <ul><li> Missing or incomplete GPIO request / direction setting </li></ul>|
| Activity 1.2: Buttons Thread | <ul><li> Thread runs at fixed time interval </li><li> Reads and updates the button states </li><li> Notifies if shoot button has been toggled </li></ul> | <ul><li> Shoot button shoots contineously when holding the button </li></ul> | <ul><li> Thread Busy waits without delay </li></ul> |
| Activity 2.1: Bullet Input Thread | <ul><li> When button is tapped, a new bullet is created </li><li> Bullet is positioned at nose of ship </li><li> bullet is created as soon as button is tapped </li></ul> | <ul><li> time delay from button press to new bullet created </li><li> button is positioned at wrong place </li></ul> | <ul><li> Holding shoot button, several bullets are created  </li><li> Pressing shooting button does not create new bullets </li></ul> |
| Activity 2.2: Bullet Update Thread | <ul><li> Uses apropriate locking </li><li> Uses apropriate signalling  updates bullet positions </li></ul> | <ul><li> Minor errors in locking / signalling </li></ul> | <ul><li> Major errors in or missing locking / signalling </li></ul>|
| Activity 3.1: Analyze sync. | <ul><li> Activity diagram showing synchronization between threads </li><li> Discusses whether the synchronization is sufficient </li></ul> | <ul><li> Activity diagrams shows some of the synchronization </li><li> Poor discussion  </li></ul> | <ul><li> Wrong or very incomplete diagram </li><li> Wrong or missing discussion </li></ul> |
| Activity 3.2: Fix sync. issues  </li></ul> | <ul><li> Game is responsive and without severe lagging </li><li> Collision detection always work </li><li> Synchronization acheived through signalling  </li></ul> | <ul><li> Some synchronization issues fx. button delays </li><li> From time to time bullet hits are not detected </li><li> Synchronization only partially through signallig  </li></ul> | <ul><li> Bullet hits are often not detected </li><li> Synchronization through adjusting timing  </li></ul> |
