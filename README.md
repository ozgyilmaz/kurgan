# Kurgan Mud

Kurgan Mud is a derivative of ROM2.4b6 with MySQL integration.

A Mysql database is used for keeping data (rooms, mobiles, objects, resets, shops, helps, socials...).

Kurgan Mud has an ini parser (https://github.com/rxi/ini) to use a config file (./kurgan.ini).

Renewed random number generator to solve modula problem.

GMCP support (https://github.com/g7138580/protocol_gmcp) (not tested yet)

8bit color codes. Predefined colors are common web colors with names. You can find the
complete list from: https://www.w3schools.com/colors/colors_groups.asp
Check do_who() or do_look() in act_info.c for sample usages.

Don't forget to create a folder named "gods" in the root folder of project.
