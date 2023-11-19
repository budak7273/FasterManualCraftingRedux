# Faster Manual Crafting Redux

_U6+ Version of [Faster Manual Crafting](https://ficsit.app/mod/FasterManualCrafting) by Turtlefight ported by Robb_

This mod will gradually speed up the crafting speed while you're hand crafting.
Initially, the crafting speed will be the same as the base game, and then continue to become faster for each crafted item.

This works with the following crafting stations:

* **Craft Bench** (including vehicle variant)
* **Equipment Workshop** (including vehicle variant)
* Probably™ work on manual crafting stations added by other mods

![Multiplayer Compatible](https://i.imgur.com/RJRFE89.png)

## Faster Crafting

By default the crafting speed will increase by 20% for every crafted item, up to 25x the normal vanilla crafting speed.
You can configure those values in the mod configuration.

![AccelerateGif](https://raw.githubusercontent.com/budak7273/FasterManualCraftingRedux/master/ModPageAssets/Preview.gif)

## Configuration

In the mod configuration (`Main Menu -> Mods -> Faster Manual Crafting Redux`) you can configure how fast the crafting speed will increase and what the maximum speed multiplier should be:

* **Speedup % :** How much faster the crafting gets with each crafted item (0.2 = 20%, 0.5 = 50%, etc...)
* **Max Speed Multiplier:** The hard limit how much faster the crafting can get (25 = 25x the normal vanilla crafting speed)
* **Max Crafts per Tick:** Limits the number of items that get crafted in a single crafting tick (0.25s) (0 = unlimited)
* **Max VFX Sparks:** Controls how many of the "spark particles" the game is allowed to display, which can cause lag

### Example:

So with the default of 0.2 - 25 the crafting speed would be as following:

* 1st crafted item: vanilla crafting speed
* 2nd crafted item: vanilla crafting speed / 1.2
* 3rd crafted item: vanilla crafting speed / 1.4
* ...
* 124th crafted item: vanilla crafting speed / 24.8
* 125th crafted item: vanilla crafting speed / 25
* 126th crafted item: vanilla crafting speed / 25
* ...

## Multiplayer

This mod is multiplayer compatible.
Make sure that the client and host have the same config settings, though.

## Help

In case something doesn't work correctly or you encounter a bug,
please post a report in the modding discord server so I can work on fixing it.
You can join the modding discord server here: <https://discord.gg/xkVJ73E>

### Known Bugs

* The To-Do list is not updated correctly while crafting
  (more info [here](https://discord.com/channels/555424930502541343/555782140533407764/1175237863701487648))

## Future

This mod relies on the base game Crafting Component's (unused) 'Fatigue' mechanic,
which this mod uses in reverse with some custom code to cause crafting to speed up instead of down.

If this unused mechanic were to be removed, I (Robb) may not be willing to update it,
but someone else is free to take on the torch instead; just message me on Discord.

## Credits

`Robb#6731` has updated the mod for Update 6 and added some additional UI elements.

This mod was originally created by `Turtlefight#6892` who published the original source code here:
<https://github.com/Turtlefight/SatisfactoryFasterManualCrafting>
