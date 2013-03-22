

***Global Functions***

RequireScript(script_name)
	- Compiles and executes a script. The base directory is the 'script' directory in the game's folder. The filesystem is not sandboxed, and by using '../' to specify one level up, any file is accessible. It is recommended to use unix style slashes (/) even on Windows.

GetVersion()
	- Returns a number specifying the version of TurboSphere that is running. For compatibility with Sphere 1.x games, it will return 2.0.

GetVersionString()
	- Returns a string specifying the version of TurboSphere being run, for example 'v0.2.13'.

SetTitle(title)
	- Sets the TurboSphere window's title caption to title.


***Time Functions***

GetTime()
	- Returns the number of milliseconds from a fixed point in time. The time returned usually uses the time TurboSphere was started as the epoch, but this should not be counted on.

Delay(ms)
	- Makes the engine sleep for ms milliseconds. The engine will idle in this time, and will cede the CPU time to other processes.


***Screen Functions***

GetScreenWidth()
	- Returns the width of the screen in pixels.

GetScreenHeight()
	- Returns the height of the screen in pixels.

SetResolution(w, h)
	- Sets the screen resolution to w pixels by h pixels.

FlipScreen()
	- Displays the contents of the video buffer on the screen. You must call this to make any graphics function display. The frequency at which this function is called can be considered the FPS the game is running at.


***Color Object***
	- var color_object = new Color(red, green, blue [, alpha])


***Primitives***

Point(x, y, color)
	- Fills the pixel at x, y with Color object color.

Rectangle(x, y, w, h, color)
	- Draws a rectangle at x, y, of size w by h, filled with Color object color.

GradientRectangle(x, y, w, h, color1, color2, color3, color4)
	- Draws a rectangle at x, y, of size w by h. The color is faded between Color objects color1, color2, color3, and color4. color1 is the upper left corner, color2 is the upper right, color3 is the lower right, and color4 is the lower left.

FilledCircle(x, y, r, color)
	- Draws a filled circle of radius r with the point x, y at its center. It is filled with Color object color.

OutlinedCircle(x, y, r, color)
	- Draws the outline of a circle with radius r with the point x, y at its center. The outline is Color object color.

Line(x1, y1, x2, y2, color)
	- Draws a line from point x1, y1, to x2, y2, of Color object color.


***Keyboard Functions***

GetKey()
	- Gets the next key in the keybuffer and returns the key constant. If no keys are in the keybuffer, it waits until a key is pressed.

AreKeysLeft()
	- Returns true if there are keys in the keybuffer, false if there are no keys in the keybuffer.

IsKeyPressed(key_const)
	- Returns true if the key key_const is pressed, false if not.

IsAnyKeyPressed()
	- Returns true if any key is pressed, false if not.

GetKeyString(key_const [, shift])
	- Returns the string representation of the key key_const. If shift is true, it returns the key as if the shift key is pressed. If shift is false or not passed, returns the key without shift. Control keys like control, shift, and alt return empty strings.


***Keyboard Constants***

Do not count on the numbers these constants represent to stay the same. Always use constants to ensure yout game will work on all platforms and versions.

KEY_ESCAPE     
KEY_F1         
KEY_F2         
KEY_F3         
KEY_F4         
KEY_F5         
KEY_F6         
KEY_F7         
KEY_F8         
KEY_F9         
KEY_F10        
KEY_F11        
KEY_F12           
KEY_F13           
KEY_F14        
KEY_F15           
KEY_TILDE      
KEY_0          
KEY_1          
KEY_2          
KEY_3          
KEY_4          
KEY_5          
KEY_6          
KEY_7          
KEY_8          
KEY_9          
KEY_MINUS      
KEY_EQUALS     
KEY_BACKSPACE  
KEY_TAB        
KEY_A          
KEY_B          
KEY_C          
KEY_D          
KEY_E          
KEY_F          
KEY_G          
KEY_H          
KEY_I          
KEY_J          
KEY_K          
KEY_L          
KEY_M          
KEY_N          
KEY_O          
KEY_P          
KEY_Q          
KEY_R          
KEY_S          
KEY_T          
KEY_U          
KEY_V          
KEY_W          
KEY_X          
KEY_Y          
KEY_Z          
KEY_SHIFT      
KEY_RSHIFT      
KEY_CTRL        
KEY_RCTRL       
KEY_ALT        
KEY_RALT        
KEY_SPACE      
KEY_OPENBRACE  
KEY_CLOSEBRACE 
KEY_SEMICOLON  
KEY_APOSTROPHE 
KEY_COMMA      
KEY_PERIOD     
KEY_SLASH      
KEY_BACKSLASH  
KEY_ENTER      
KEY_INSERT     
KEY_DELETE     
KEY_HOME       
KEY_END        
KEY_PAGEUP     
KEY_PAGEDOWN   
KEY_UP         
KEY_RIGHT      
KEY_DOWN       
KEY_LEFT       
KEY_NUM_0
KEY_NUM_1
KEY_NUM_2
KEY_NUM_3
KEY_NUM_4
KEY_NUM_5
KEY_NUM_6
KEY_NUM_7
KEY_NUM_8
KEY_NUM_9
KEY_NUM_PERIOD
KEY_NUM_DIVIDE
KEY_NUM_MINUS
KEY_NUM_EQUALS
KEY_RMETA
KEY_LMETA
KEY_RSUPER
KEY_LSUPER
KEY_BREAK
KEY_MENU
KEY_POWER
KEY_EURO
KEY_CAPSLOCK
KEY_NUMLOCK
KEY_SCROLLOCK

***Mouse***

GetMouseX()
	- Gets the X coordinate of the cursor.

GetMouseY()
	- Gets the Y coordinate of the cursor.

IsMouseButttonPressed(mouse_const)
	- Checks if the mouse button mouse_const is pressed.

***Mouse Constants***

Do not count on the numbers these constants represent to stay the same. Always use constants to ensure yout game will work on all platforms and versions.

MOUSE_LEFT
MOUSE_MIDDLE
MOUSE_RIGHT


***Font Object***
	- var bmp_font_object = new Font(font_filename)


GetSystemFont()
	- returns a new Font object containing the system bmp font.


***TrueType Font Object***
	- var ttf_font_object = new TTFFont(ttf_font_filename)


***Sound Object***
	- var sound_object = new Sound(filename);
	
sound_object.play(loop)
	- Plays the sound. If loop is a boolean true, sound_object will loop playing until stopped. If loop is a number, sound_object will play for 'loop' number of times.

sound_object.isPlaying()
	- Returns true if sound_object is currently playing, false otherwise.


***Surfaces***
	- var surface_object = new Surface(w, h, color)
	- var surface_object = new Surface(filename)

surface_object.blit(x, y)
	- Blits surface_object with the top left corner at point x, y.

surface_object.zoomBlit(x, y, factor)
	- Blits the surface at x, y, zoomed by factor. 1 is normal size.

surface_object.stretchBlit(x, y, h_factor, v_factor)
	- Blits the surface at x, y, vertically zoomed by a factor of v_factor and horizontally zoomed by a factor of h_factor. 1 is normal size.

surface_object.getPixel(x, y)
	- Returns Color object representing the pixel of surface_object at x, y.

surface_object.setPixel(x, y, color)
	- Sets the pixel x, y of surface_object to Color object color.

surface_object.createSurface()
	- Clones surface_object.

surface_object.createImage()
	- Returns an Image object with the same properties as surface_object.

surface_object.blitSurface(surface, x, y)
	- blits surface onto surface_object at point x, y.

surface_object.blitSurface(image, x, y)
	- blits image onto surface_object at point x, y.


***Images***
	- var image_object = new Image(w, h, color)
	- var image_object = new Image(filename)

image_object.blit(x, y)
	- Blits image_object with the top left corner at point x, y.
	
image_object.zoomBlit(x, y, factor)
	- Blits image_object at x, y, zoomed by factor. 1 is normal size.

image_object.stretchBlit(x, y, h_factor, v_factor)
	- Blits image_object at x, y, vertically zoomed by a factor of v_factor and horizontally zoomed by a factor of h_factor. 1 is normal size.

image_object.getPixel(x, y)
	- Returns Color object representing the pixel of image_object at x, y.

image_object.setPixel(x, y, color)
	- Sets the pixel x, y of image_object to Color object color.

image_object.createSurface()
	- Returns a surface object with the same properties as image_object.

image_object.createImage()
	- Clones image_object.

image_object.blitSurface(surface, x, y)
	- blits surface onto image_object at point x, y.

image_object.blitSurface(image, x, y)
	- blits image onto image_object at point x, y.


***Alpha, Temporary Functions, and Workaround Features***

TS_Test()
	- Performs an unspecified function from TurboSphere. This function is used to test features without any interference from JS. This is often used to test for memory leaks without considering JS-side memory, passing values into functions that don't yet have a representation in JS, or simply for testing functions that aren't exposed to JS yet. While this function may be used for testing and to see new features that don't quite work yet, there are absolutely no exxpectations for this function. Likely a function bound to TS_Test will either work, leak a lot of memory, or crash TurboSphere.

