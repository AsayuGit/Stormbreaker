PROJET
(termes en majuscules au sens RFC 2119)

A/ ATTENDUS DU PROGRAMME A LIVRER
--- Le programme DOIT être écrit en langage C ;
--- Le programme DOIT avoir au moins deux modes de fonctionnements : génération de condensats (G pour generate) et recherche parmi une liste de condensats (L pour lookup) ;
---  Le programme PEUT avoir d'autres modes de fonctionnement, qui seront spécifiées dans le fichier README.

--- Le mode G DOIT avoir une fonctionnalité de lecture des chaînes de caractères d'entrée à partir d'un fichier (dit dictionnaire) ;
--- Le mode G DOIT accepter que le séparateur des chaînes de caractères d'entrées soit un caractère saut de ligne "\n" ;
--- Le mode G DOIT générer le condensat cryptographique de chacune des chaînes de caractères d'entrées ;
--- Le mode G DOIT avoir une fonctionnalité d'écriture de la table de correspondance chaine-condensat (T3C) sous forme de fichier ;
--- Le mode G DOIT proposer de générer les condensats avec des algorithmes alternatifs ;

-> Illustration :

                         args: mode Generate,
                               chemin vers la table de correspondance à écrire
                         │
                         │
                         │
                         ▼
                    ┌──────────┐
stdin: mots         │          │   stdout: log
          ─────────►│ generate ├─────────►
                    │          │   fichier: table de correspondance
                    └──────────┘

Le programme va lire l'entrée standard avec le dictionnaire, puis pour chaque ligne générer une entrée dans /home/user/project/rainbow.table contenant le clair et son condensat

$> cat /usr/share/wordlists/rockyou.txt | ./monProgramme -G -o /home/user/project/rainbow.table
FATAL: Cannont write file /home/user/project/rainbow.table

$> cat /usr/share/wordlists/rockyou.txt | ./monProgramme -G -o /home/user/project/rainbow.table
INFO Writing table...
INFO 10 hashes written...
INFO 50 hashes written...
INFO 100 hashes written...
INFO 123 hashes written.

-> Illustration d'une fonction optionnelle de définition d'algo alternatifs :
$> cat /usr/share/wordlists/rockyou.txt | ./monProgramme -G -o /home/user/project/rainbow.table --algorithm md5
INFO Writing table with MD5 algorithm...
INFO 10 hashes written...
INFO 50 hashes written...
INFO 100 hashes written...
INFO 123 hashes written.

--- Le mode L DOIT avoir une fonctionnalité de chargement en mémoire une T3C à partir d'un fichier ;
--- Le mode L DOIT avoir une fonctionnalité de recherche rapide d'une chaîne de caractère à partir d'un condensat ;
--- Le mode L DOIT accepter de recevoir les condensats par le flux standard d'entrée ;
--- Le mode L DOIT accepter que les condensats fournis soient séparés par un caractère saut de ligne "\n" ;
--- Le mode L DOIT accepter d'afficher les chaînes de caractères trouvées sur le flux standard de sortie ;

-> Illustration :

                                   args: mode lookup,
                                         chemin vers la table correspondance
                                   │
                                   │
                                   │
                                   ▼
                              ┌──────────┐
file: table correspondance    │          │   stdout: log, correspondances trouvées
                    ─────────►│  lookup  ├─────────►
stdin: hashs à retrouver      │          │
                              └──────────┘

Le programme s'initialise en chargeant en mémoire tous les hash contenu dans la rainbow.table en mémoire, puis pour chaque ligne qui lui est fournie en stdin il regarde si ce hash est déjà connu:

$> cat foundhashes.txt | ./monProgramme -L -i /home/user/project/rainbow.table
INFO Loading dict file: /home/user/project/rainbow.table
FATAL Cannont load file: /home/user/project/rainbow.table

$> cat foundhashes.txt | ./monProgramme -L -i /home/user/project/rainbow.table
INFO Loading dict file: /home/user/project/rainbow.table
INFO Loading 100 hashes...
INFO Loading 200 hashes...
INFO Loading 500 hashes...
INFO Loading 1000 hashes...
INFO 1453 hashes loaded.
MATCH b78034aacf3559fffbfcb545d9a9122efb93181f iloveu
MATCH 2cf20f3ebdeb8680949d83389bbdf9f242e95c00 titanic
MATCH fba9f1c9ae2a8afe7815c9cdd492512622a66302 777777
MATCH 2CF20F3EBDEB8680949D83389BBDF9F242E95C00 titanic


--- Pour tous les modes, le programme DOIT accepter de recevoir les fichiers d'entrée (dictionnaire, T3C) sous la forme d'un chemin donné en argument de ligne de commande ;
--- Pour tous les modes, le programme DOIT accepter de recevoir tous les paramètres utiles à sa bonne exécution en arguments de ligne de commande ;

--- Le programme PEUT exiger des pré-requis pour la compilation ou le fonctionnement du programme ;
--- Le programme NE DOIT PAS exiger d'autres pré-requis que : un OS linux, l'installation de bibliothèques via paquets des dépôts standards publics debian, Docker.


B/ CONDITIONS DE LIVRAISON
--- Le dépot DOIT être livré par transmission d’une URL gitlab ou github accessible publiquement ;
--- Le dépot DOIT contenir un fichier README.MD à la racine qui documente la compilation et l'utilisation du programme, explicite les pré-requis s'il en existe ;
--- Le dépot DOIT contenir un fichier makefile qui permet de compiler le programme ;
--- Le dépôt PEUT contenir un fichier Dockerfile qui permet de construire une image docker pour compiler le programme ;
--- Le dépôt PEUT contenir un fichier Dockerfile qui permet de construire une image docker pour exécuter le programme ;
--- Si le dépôt contient un ou des fichiers Dockerfile, le fichier README.MD DOIT indiquer comment construire la ou les image(s) et son/leur utilisation ;
--- Le programme livré DOIT être sur une branche nommée "master" ;
--- Les fichiers du code source DOIVENT être situés dans un dossier "src" à la racine du dépôt ;
--- Les documentations autre que le README.MD DOIVENT être situées dans un dossier doc à la racine du dépôt ;
--- Le dépôt PEUT contenir des tests ou une documentation de test ;
--- Le dépôt PEUT contenir un système d'intégration continue.

-> Exemple/illustration avec un projet fictif:
$> git clone https://github.com/myproject.git
$> cd myproject
$> git checkout master
$> cat README.MD
(...)
$> sudo docker build . -t myproject:latest
$> sudo docker run --rm -ti -v /home/user/myproject/build:/project/app/build myproject:latest make
$> chmod a+x build/myApp


C/ EVALUATION
Seront principalement pris en compte dans l'évaluation :
--- Le respect des attendus du programme à livrer exprimées supra ;
--- Le respect des conditions de livraisons exprimées supra ;
--- La qualité de la documentation ;
--- Le bon fonctionnement des éléments livrés ;
--- La démonstration de la maîtrise des notions enseignées : lecture/écriture de fichiers, paramètres passés en ligne de commande, tableaux, pointeurs, allocation dynamique de mémoire, structures.

Seront également pris en compte dans l'évaluation :
--- La performance du programme ;
--- La pertinence du découpage du programme en différents fichiers ;
--- La lisibilité du code ;
--- La créativité par des fonctionnalités additionnelles ;
--- La pertinence des fonctions additionnelles ;
--- La facilité d'utilisation.

--- Un imposant malus sera appliqué si dépôt ne permet pas au correcteur de compiler et d'exécuter le programme en moins de 15 minutes sur un OS linux en suivant les instructions contenues dans le fichier README.MD.
