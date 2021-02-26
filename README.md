# Project_SY5_KMB

## Rappel du sujet : `tsh`, un shell pour les tarballs

Le but du projet est de faire tourner un shell qui permet à
l'utilisateur de traiter les tarballs comme s'il s'agissait de
répertoires, **sans que les tarballs ne soient désarchivés**. Le
format précis d'un tarball est décrit sur
[https://fr.wikipedia.org/wiki/Tar_(informatique)](https://fr.wikipedia.org/wiki/Tar_%28informatique%29).

Le shell demandé doit avoir les fonctionnalités suivantes :

* les commandes `cd` et `exit` doivent exister (avec leur comportement habituel)
* toutes les commandes externes doivent fonctionner normalement si leur déroulement n'implique pas l'utilisation d'un fichier (au sens large) dans un tarball
* `pwd` doit fonctionner y compris si le répertoire courant passe dans un tarball
* `mkdir`, `rmdir` et `mv` doivent fonctionner y compris avec des chemins impliquant des tarball quand ils sont utilisés sans option
* `cp` et `rm` doivent fonctionner y compris avec des chemins impliquant des tarball quand ils sont utilisés sans option ou avec l'option `-r`
* `ls` doit fonctionner y compris avec des chemins impliquant des tarball quand il est utilisé sans option ou avec l'option `-l`
* `cat` doit fonctionner y compris avec des chemins impliquant des tarball quand il est utilisé sans option
* les redirections de l'entrée, de la sortie et de la sortie erreur (y
  compris sur des fichiers d'un tarball) doivent fonctionner
* les combinaisons de commandes avec `|` doivent fonctionner

On ne vous demande pas de gérer les cas de tarballs imbriqués (un tarball dans un autre tarball).

Tous les processus lancés à partir de votre shell le seront en premier-plan.

## Modalité et environnement d'exécution

Le projet sera testé sur la distribution fournie par l'enseignante 
https://mirrors.ircam.fr/pub/mx/isos/ANTIX/Final/antiX-19/antiX-19.3_386-base.iso (AntiX 19.3). 
Pour que le projet fonctionne, il sera nécessaire d'installer gcc build-essential. Pour cela taper les commandes suivantes dans un terminale:

```
sudo apt update
```
puis
```
sudo apt install build-essential 
```
Il est également nécessaire d'installer la librairie readline via cette commande:
```
sudo apt-get install libreadline-dev
```

## Compilation et exécution

- Pour compiler le projet: 
depuis la racine du projet faire 
```
cd src/command
```
puis 
```
Make
```

- L'exécution du projet se fera via la commande suivant:
```
./tsh
```
- Néanmoins, afin de faciliter les tests de tsh est d'automatiser (à la manière de tests unitaires), nous proposons aussi un mode non-interactif
  Pour l'utiliser, nous proposons par défault le fichier demo.txt, mais n'importe quel fichier de la forme "cmd \n cmd2 \n" fonctionnera.
  Pour le lancer : 
```
./tsh [fichier_de_demo]
```

## Instruction supplémentaire post-épreuve sur machine

Il est nécessaire d'ajouter un `/` à la fin des chemins désignants des répertoires donnés en argument sur les commandes agissant sur des répertoires dans le tarball comme `mkdir` `cd` `rmdir` `cp -r` par exemple.

### Compte-rendu de l'epreuve

Rappel du sujet:
 - Nous vous demandons de faire en sorte que les lignes de commandes comportant un ou plusieurs pipes (|) se synchronisent correctement de sorte que tous les jobs soient au premier plan sans utilisation de sleep.
 - Nous vous demanderons également de faire en sorte qu'on puisse exécuter des commandes externes quand le répertoire courant se situe dans un tarball, y compris si on met dans la ligne de commande une redirection hors du tarball.

Au sujet des pipes: l'intégralité des sleep (et usleep) ont été retirés du code tout en gardant fonctionnel la fonctionnalité des pipes. Un bug a cependant été remarqué. 
Dans certaines combinaison de commande, une fois réalisé, le prompt ne réapparaît pas mais tsh continue de fonctionner et il est possible d'entrer une autre commande (existante ou non),
à ce moment la le prompt réapparaît.
Exemple:
```
laptop@localhost.localdomain@tsh:/home/kdang/project_sy5_kmb/src/command/ROOT.tar$ 
Usage : grep [OPTION]... MOTIFS [FICHIER]...
Exécutez « grep --help » pour obtenir des renseignements complémentaires.
ls
ROOT/    ROOT2/    
laptop@localhost.localdomain@tsh:/home/kdang/project_sy5_kmb/src/command/ROOT.tar$ 
```

Au sujet des commandes externes tout en étant dans un tarball: Toutes les commandes non demandés à implémenter sont utilisable dans le tar. Un bug est constaté dans les redirection hors tarball.
En effet le répertoire courant (donc dans le tarball n'est pas pris en compte).
Exemple: (nous constatons que la redirection crée bien le fichier file.txt mais un répertoire trop loin en arrière)
```
laptop@localhost.localdomain@tsh:/home/kdang/project_sy5_kmb/src/command$ cd ROOT.tar 
laptop@localhost.localdomain@tsh:/home/kdang/project_sy5_kmb/src/command/ROOT.tar$ echo "Bonjour à tous" > ../file.txt
laptop@localhost.localdomain@tsh:/home/kdang/project_sy5_kmb/src/command/ROOT.tar$ cd ../..
laptop@localhost.localdomain@tsh:/home/kdang/project_sy5_kmb/src$ ls
command  doc  file.txt  header
laptop@localhost.localdomain@tsh:/home/kdang/project_sy5_kmb/src$ 

```