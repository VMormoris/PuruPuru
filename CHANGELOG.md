# Version v0.1.2
## Bug Fixes:
- Force extensions when exporting
- Links towards AcceptQuestNodes where omitted during export
- Empty outputs should no longer cause problems in game

## Features:
- Change extension of export file from `.puru` to `.epuru` to avoid confusion when refering to the tool vs engine files 

# Version v0.1.1
## Bug Fixes:
- Added back missing colors on nodes header
- Can connect output of `Accept Quest Node`

# Version v0.1.0
## Features:
- Supported Nodes:
    - `Entry Node`: The root for each character per act (file)
    - `Act Node`: Here you can define conversation between the main character and an NPC
    - `Fork Node`: A logic node that will make you go through one of it's output once and all the subsequent times you will go to the second one
    - `Set Variable Node`: Use this node to setup integer or boolean variable
    - `Branch Node`: Enables to select behaviour base on values of specific variables
    - `Dialogue Node`: Give a specified number of choice to the player that lead to different options
    - `Dice Node`: Every time we go through that node a random output will be selected
    - `Accept Quest Node`: Going through this signifies that quest has been accepeted. It also lets you define details about the quest and it's objectives
    - `Return Quest Node`: Can be used to signify a quest as Succeed or Failed
    - `Objective Node`: Can be used to signify an objective as Succeed or Failed
    - `Comment Node`: Can be used to give a human friendly name to a region (group of nodes). _[Ignored during export]_
    - __Game specific nodes:__
        - `FlavorMatch`: Checks wether the NPC's and the main character flavors are the same
        - `FlavorCheck`: Checks either the main Character or NPC for a specific flavor

## Known bugs:
- When you have a very big tree and you zoom out a lot, the graphics get broken
- Cannot remove dialogue options
- ~~Empty outputs will result in bugs in game~~