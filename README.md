# Morse_Code
Embededsystem-LPCExpresso-C++

Write a program that reads characters from UART until linefeed is pressed or 80 characters have been read,
whichever occurs first. The program then sends Morse coded text with the red led and pin A0 (port 0, pin
8). Red led is for visual inspection and the IO-pin is for morse code decoder.
Use ITU Morse code for the text. Replace characters that don’t have a Morse code symbol with ‘X’.
International Morse code is composed of five elements:
1. short mark, dot or "dit" (·) — "dot duration" is one time unit long
2. longer mark, dash or "dah" (–) — three time units long
3. inter-element gap between the dots and dashes within a character — one dot duration or one unit
long
4. short gap (between letters) — three time units long
5. medium gap (between words) — seven time units long
For more details see Wikipedia article: http://en.wikipedia.org/wiki/Morse_code
const int DOT = 1;
const int DASH = 3;
struct MorseCode {
char symbol;
unsigned char code[7];
};
const MorseCode ITU_morse[] = {
{ 'A', { DOT, DASH } }, // A
{ 'B', { DASH, DOT, DOT, DOT } }, // B
{ 'C', { DASH, DOT, DASH, DOT } }, // C
{ 'D', { DASH, DOT, DOT } }, // D
{ 'E', { DOT } }, // E
{ 'F', { DOT, DOT, DASH, DOT } }, // F
{ 'G', { DASH, DASH, DOT } }, // G
{ 'H', { DOT, DOT, DOT, DOT } }, // H
{ 'I', { DOT, DOT } }, // I
{ 'J', { DOT, DASH, DASH, DASH } }, // J
{ 'K', { DASH, DOT, DASH } }, // K
{ 'L', { DOT, DASH, DOT, DOT } }, // L
{ 'M', { DASH, DASH } }, // M
{ 'N', { DASH, DOT } }, // N
{ 'O', { DASH, DASH, DASH } }, // O
{ 'P', { DOT, DASH, DASH, DOT } }, // P
{ 'Q', { DASH, DASH, DOT, DASH } }, // Q
{ 'R', { DOT, DASH, DOT } }, // R
{ 'S', { DOT, DOT, DOT } }, // S
{ 'T', { DASH } }, // T
{ 'U', { DOT, DOT, DASH } }, // U
{ 'V', { DOT, DOT, DOT, DASH } }, // V
{ 'W', { DOT, DASH, DASH } }, // W
{ 'X', { DASH, DOT, DOT, DASH } }, // X
{ 'Y', { DASH, DOT, DASH, DASH } }, // Y
{ 'Z', { DASH, DASH, DOT, DOT } }, // Z
{ '1', { DOT, DASH, DASH, DASH, DASH } }, // 1
{ '2', { DOT, DOT, DASH, DASH, DASH } }, // 2
{ '3', { DOT, DOT, DOT, DASH, DASH } }, // 3
{ '4', { DOT, DOT, DOT, DOT, DASH } }, // 4
{ '5', { DOT, DOT, DOT, DOT, DOT } }, // 5
{ '6', { DASH, DOT, DOT, DOT, DOT } }, // 6
{ '7', { DASH, DASH, DOT, DOT, DOT } }, // 7
{ '8', { DASH, DASH, DASH, DOT, DOT } }, // 8
{ '9', { DASH, DASH, DASH, DASH, DOT } }, // 9
{ '0', { DASH, DASH, DASH, DASH, DASH } }, // 0
{ 0, { 0 } } // terminating entry - Do not remove!
};
The character at index k can be accessed with ITU_morse[k].symbol. If the character is zero then you are at
the end of the morse code table.
Code is an array of non-zero number that indicate the length of the marks in the code. To read the code at
index k: start with i=0, loop through ITU_morse[k].code[i] increasing I on each round until you see a zero
that marks the end of the code.
This must implement morse code sender class. The constructor takes two pointers to IO-pin object as
parameters: one for led and one for decoder output pin. There must be public methods for sending out a
string in morse code. Use method overloading to implement both string class and C string interfaces.
The functionality of this program will be tested against a Morse code decoder. Do not use dot length that is
shorter than 10 ms. Dot duration of 10 ms is about 100 wpm which is way too fast for a human to decode
from led blinks. Start with slower speed and test first with easy words like SOS.
Exercise 3
Improve your program so that the time unit is configurable. Program must have the following commands:
wpm <number>
- Set transmission speed in words per minute. Assume that average word length is 60 time units
long.
send <text to send>
- Send text that follows the command
set
- Print current settings (wpm and dot length in milliseconds)

