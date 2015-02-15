#pragma once

/* And it goes a little something like this...

// Create a Player (on init)

Player main_player;

InitPlayer(){
    main_player.init();
}

// Decide what format you are opening:
struct Cinnamon_Decoder *decoder = Cinnamon_CreateDecoder(Cinnamon_DefaultRate, Cinnamon_Stereo);

// Load the file into memory

// Decode the file:
void **decoded_buffer = nullptr;
unsigned long decoded_length = 0;
Cinnamon_Read(decoder, decoded_buffer, decoded_length, raw_buffer, raw_buffer_len);

// Create a Sound (in a JS constructor):
// Note that the last value is the same as the first to Cinnamon_CreateDecoder
Sound sound = main_player.load(decoded_buffer, decoded_length, Cinnamon_DefaultRate); 

// Destroy the decoder
Cinnamon_DestroyDecoder(decoder);

// Return the sound.

*/
