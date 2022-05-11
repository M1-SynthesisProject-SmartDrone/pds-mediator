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
$ sudo mkdir bin
$ cd build
$ cmake ..
$ make
```

## Help if error while trying to install libpq-dev

if you have the following error while trying to install libpq-dev :
```
Les paquets suivants contiennent des dépendances non satisfaites :
 libpq-dev : Dépend: libpq5 (= 13.6-0ubuntu0.21.10.1) mais 14.2-1.pgdg21.10+1 devra être installé
E: Impossible de corriger les problèmes, des paquets défectueux sont en mode « garder en l'état ».
```

consider install this dependancy before

```
sudo apt install libpq5=13.6-0ubuntu0.21.10.1

sudo apt-get install libpq-dev
```

and watch out to have the same version of Ubuntu for libpq5 that the one mentionned on the previous error

## Communication Protocol

Here is all the possible communication we can have with the database server.

### Save a trip

To save a trip, you have to send first this request:

```
{
    "requestType":"TR_SAVE"
}

```

And you will receive this :

```
{
	"responseType:" "RESP_TR_SAVE",
	"isLaunched" : boolean // true or false depending if it works or not
}
```

While the server is saving, you have to send this request to save a position & image:

```
{
    "requestType" : "REGISTER", 
    "tr_id" : 1,                  // id of the trip, integer
    "latitude" : 1234564,         // latitude, integer
    "longitude" : 1234561,        // longitude, integer
    "rotation" : 21,              // rotation, integer
    "time" : 164445158451488,     // time, integer
    collectionName" : "collectionName", 
    "image" : "ImageName"
}
```

When you want to stop saving the trip, you just have to send this request :
```
{
	"responseType" : "END_TR_SAVE",
	"tripName" : "NOMTRAJET"
}
```

And you will receive this :
```
{
	"responseType": "RESP_END_TR_SAVE",
	"isLaunched" : boolean // true or false depending if it works or not
}
```


