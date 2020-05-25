[![Run on Repl.it](https://repl.it/badge/github/KeinR/Crossot)](https://repl.it/@KeinR/Crossot)

Will solve crosswords from sciencemusicvideos.com learning guides.

Usage:
./crossot.exe \<screenshot\> \<answers\>

where:
`screenshot` is a PERFECT (see *limitations*) screenshot of crossword from the PDF
`answers` is a text document with cut-and-pasted "possible answers" from the PDF; each one can be seperated
with a space ` ` carrage return `\r` newline `\n` or comma `,`, and any order and any number > 0.

Tested & suceeded with **20** different crosswords (see `./tests`) of ~46 total on the site (from when I counted), that's ~43% tested... Which I think is enough to prove its effectiveness.

### *Limitations:*
- For esoteric reasons¹, screenshots must be perfect- at least for the top, only the crossword and it's UNIFORM boarder must be visable; see `./tests` for examples.

### *Motivation:*
Just kinda' felt like it.

#### Update 5/25/2020
So Mr. W was giving us the assignments in Google Docs because of the quarentine, and because it was convinient I decided I might
as well try out Crossot with the original, source image as opposed to a screenshot. Well... It didn't work. After going through
the logs and analyzing the images I've found that the source image is far different from the screenshot. Perhaps the Windows
snipping tool does some filtering, perhaps the image changes when it's in pdf form. Whatever the case, I just want to reiterate,
***Crossot, as of right now, only works on screenshots***. That's something that may not change soon. If anyone wants to open up
a PR and get that fixed, I'd be a happy camper... I didn't really comment much though, and I made this when I was first learning C++...
I'll throw a MIT license on the source to make things a little easier.

\*

\*

\*

*¹The program takes the color of the top left pixel, then does a linear search for the first location where the color is NOT that. This results in the locating of the topleft-most box, which also happens to be #1. If there's varient color in the top section of the screenshot, it will mistake something else as the start box and burst into flames, the proceed to jump out of your computer and steal all your pokemon cards.*
