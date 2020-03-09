Will solve crosswords from sciencemusicvideos.com learning guides.

Usage:
./cross.exe \<screenshot\> \<answers\>

where:
`screenshot` is a PERFECT (see *limitations*) screenshot of crossword from the PDF
`answers` is a text document with cut-and-pasted "possible answers" from the PDF; each one can be seperated
with a space ` ` carrage return `\r` newline `\n` or comma `,`, and any order and any number > 0.

Tested & suceeded with **20** different crosswords (see `./tests`) of ~46 total on the site (from when I counted), that's ~43% tested... Which I think is enough to prove its effectiveness.

### *Limitations:*
- For esoteric reasons¹, screenshots must be perfect- at least for the top, only the crossword and it's UNOFORM boarder must be visable; see `./tests` for examples.

### *Motivation:*
Just kinda' felt like it.

\*

\*

\*

*¹The program takes the color of the top left pixel, then does a linear search for the first location where the color is NOT that. This results in the locating of the topleft-most box, which also happens to be #1. If there's varient color in the top section of the screenshot, it will mistake something else as the start box and burst into flames, the proceed to jump out of your computer and steal all your pokemon cards.*
