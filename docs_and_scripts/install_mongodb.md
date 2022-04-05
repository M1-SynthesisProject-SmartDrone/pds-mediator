# Etapes d'installation (Ubuntu 20.04)

## Ajout de la clé publique pour apt

`$ wget -qO - https://www.mongodb.org/static/pgp/server-5.0.asc | sudo apt-key add -`

> Si pas bon, il faut installer gnupg : `$ sudo apt-get install gnupg`

## Créer un list file pour mongodb

`$ echo "deb [ arch=amd64,arm64 ] https://repo.mongodb.org/apt/ubuntu focal/mongodb-org/5.0 multiverse" | sudo tee /etc/apt/sources.list.d/mongodb-org-5.0.list`

## Recharger apt

`$ sudo apt-get update`

## Installer le package

`$ sudo apt-get install -y mongodb-org`

Pour éviter que apt-upgrade pose problème dans le futur, on va fixer les dépendances :

```
echo "mongodb-org hold" | sudo dpkg --set-selections
echo "mongodb-org-database hold" | sudo dpkg --set-selections
echo "mongodb-org-server hold" | sudo dpkg --set-selections
echo "mongodb-org-shell hold" | sudo dpkg --set-selections
echo "mongodb-org-mongos hold" | sudo dpkg --set-selections
echo "mongodb-org-tools hold" | sudo dpkg --set-selections
```

## Lancer mongodb

`$ sudo systemctl start mongod`

## Rester mongodb

`$ sudo systemctl status mongod`

## Lancer une session mongodb

`$ mongosh`