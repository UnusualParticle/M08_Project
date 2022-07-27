# M08_Project
Sorry! board game project for school.

# Assignment 
1. To play “Sorry”, you’ll need to create up to Four players. The game will prompt the user for the number of players (2-4).
2. Create a structure for ‘players’. The structure consists of the player’s name and their position.
3. Create an array that can contain up to four players.
4. Create two die for the players to roll.
5. The playing board has 60 spaces (with 60 being the winning space).
6. The dice have special conditions:
> 2. = Move two spaces
> 3. = Move three spaces
> 4. = Move back one space.
> 5. = Move back three spaces.
> 6. = Move six spaces.
> 7. = Swap spots with the last player / or nothing if player is in last.
> 8. = Move zero spaces (lose a turn).
> 9. = Move nine spaces.
> 10. = Move ten spaces.
> 11. = Swap spots with the leading player / or do nothing if player is in the lead.
> 12. = Start Over
7. Create an array to track the number of times each dice value is rolled (number of times a two through 
twelve was rolled). At the end of the game list the number of times each value was rolled.
8.  A player must roll a double to start.
9.  If a player lands on the same space as another, the other player must return to the beginning.
Example: If P1 lands on a space where P3 is, P3 would go back to the start.
10. A player must roll an EXACT number to enter the winning space.
11. Use a random generator to “roll” the dice, the user must press enter to roll.
12. Indicate the value of each die, the total of the roll, and the resultant move after each dice roll.
13. Depict the players’ positions on the screen after each round.
14. Once a player finishes, create a winning message announcing the winner.
15. Then ask the user if they would like to play again.
16. You must use at least three functions. Some function examples could be:
Roll dice, check for other player (when moving), display board.
16. Display the status/location of the players between sets of rolls.
17. Depict a Playing Board on the screen and display the Players’ position on the board. (Extra Credit)
(maybe try using a method to create the board and screen each time a player moves)
18. If a player rolls a double, they’ll get another roll (Extra Credit).
19. If a player rolls two doubles in a row, they Start Over (Extra Credit).
