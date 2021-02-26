# Project_SY5_KMB


## La planification de notre projet

Nous avions réparti les différentes issues du projet en 3 Milestones : 
* La première version, où nous allions faire la "base" du shell, c'est à dire la boucle principale, l'analyse syntaxique de la requête ainsi que le lancement des instructions appropriées
* La deuxième version, où nous implémenterions une demi-douzaine de commande pour arriver à un résultat final correct
* La troisème version pour ajouter les options, peaufiner notre projet et ajouter d'autres fonctions non-requises

L'idée était d'avoir un shell déjà fonctionnel le plus rapidement possible, avant de commencer à ajouter les fonctionnalités de manière incrémentale, à la manière Scrum en quelque sorte.
Cependant, nous avons finalement opté sur l'approche inverse (à savoir inverser la première et la deuxième version), car nous n'avions pas une idée claire de ce que nous devions implémenter.
Ainsi, nous sommes arrivées rapidement à la fin de la première milestone, à savoir l'implémentation indépendante des versions tar de ls, mkdir, pwd, exit ainsi que de cd. Nous avons commencé à les ajouter aux options disponible de notre boucle principale, en même temps que nous construisions le mécanisme du shell.

## L'architecture de notre shell.

Notre architecture s'inspire d'un **modèle d'architecture de processus de flux** : nous voulions un code modulable, clair et avec peu de dépendances.

Il peut être retrouvé dans le fichier [*tsh_n1.png*](tsh_n1.png), réalisé avec LucidChart. 

Il y a ainsi 3 modules : 

* *loop.c* : ce module ci représente la boucle interractive du shell. On y effectue des tests préliminaires (de purification de l'entrée utilisateur).
    De plus, si besoin est, on transforme la string reçue en tableau de sous-chaînes, autrefois séparée par un | . On en profite pour vérifier qu'il n'y ai pas d'entrée mal formée du style `| cmd`.
    Nous envoyons ensuite ce tableau au module suivant pour les traitements supplémentaires.

* *launcher.c* : ici, on commence par gérer les pipes avec la fonction `managePipes` en prenant chaque paire de blocs de commande. Hélàs, nous n'avons pas réussi à rendre cette fonction récursive et notre tsh n'accepte qu'un seul pipe.
   Toutefois, une fois le pipe fait, le processus fils nouvellement créé continue le traitement du bloc de commande, constitué d'une commande, d'arguments et de possibles redirections.
   C'est la fonction `manageRedir` qui s'en charge : elle permet d'effectuer toutes les redirections simples (pas de `A > b > c` par exemple, où de `A 2> b 2>> c`), et effectue les redirections de flux adaptés.
   L'étape finale cette fonction est d'appeler la fonction `launcher`, qui va s'occuper de "spécialiser" le fils en fonction de la commande entrée, à l'aide d'un execv. On teste ici la nécéssité ou non d'utiliser nos fonctions "spéciales tar".
   Vous trouverez un schéma de l'architecture dans le fichier [*tsh_system.png*](tsh_system.png)

* *analyse_lexicale.c* : ce module possède des fonctions utiles pour la transformation de chaines de caractères en tableau par exemple

## Le résultat final que nous visons

Nous aurions voulu avoir des fonctions récurrentes un peu plus génériques, et plus d'analyse au niveau du shell lui même, par exemple pour analyser les chemins comportant des ".." et des expressions régulières en amont de nos commandes.
De plus, nous voulions ajouter quelques commandes supplémentaires pour les tars telles que tree, ainsi que gérer la co-existance des commandes tar et non-tar lors des pipes (en pouvant par exemple faire un grep sur le résultat d'une de nos fonctions).
Cependant, avant de faire ce projet nous ne nous attendions pas à ce qu'écrire des fonctions équivalentes pour les tar soit si difficile, avec beaucoup d'analyse syntaxique pour pouvoir naviguer au sein du tar.
