# Pds Mediator

This repository contains the source code for the server responsible for storing and retrieving elements on the database.

## Install

Simply launch the script docs_and_scripts/install_dependencies.sh.

If you have certain parts installed (such as mongodb driver, which is pretty slow to compile), go in the folder and launch only required scripts or commands.

In order to launch the program, you must duplicate the file "config/config.cfg.TEMPLATE", calling the duplicate
"config/config.cfg" and put your wanted settings.

This config.cfg file is unique to this computer and should (will) never be committed.

## How to run

Launch the script "./compile.sh" while in this folder (the "drone_controller" one)
and run the application created in the "build folder".

If the script doesn't work for any reason, you can compile yourself by typing :

```
$ sudo mkdir build
$ cd build
$ cmake ..
$ make
```