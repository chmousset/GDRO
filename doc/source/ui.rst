==============
User Interface
==============

The user interface is fully modular, thanks to a 'Tabset' widget, and a simple callback mechanism which makes it trivial to handle interactions with the user.
Each 'app' is displayed on its own tab, so it can use nearly all of the screen real estate.

Some apps are using a thread, which makes sure the periodic tasks are run when they have to.

All apps are seperated in their own source file, `app_*.c`

app_display
===========
This is the default app.
This simply displays the values of the different positions.
All the positions can be displayed in absolute or incremental mode.

app_thread
==========
This app is meant to setup a Step10V controller used for threading operation.
The spindle of a lathe is fitted with an encoder, and the lead screw is fitted with a servomotor, enabling threading on lathes that don't have threading gears.

app_settings
============
Allows to modify, save or load the configuration.
Note: After the type of interface has been modified, it's necessary to save the settings, then reset GDRO.