
# Projet-Mini-Shell
Un interpréteur de commandes UNIX (shell) simplifié développé en C

## 🚀 Fonctionnalités

* **Exécution de commandes :** Lancement d'exécutables situés dans le PATH (ex: `ls`, `cat`, `grep`) ou via leur chemin absolu/relatif.
* **Gestion des arguments :** Prise en charge des commandes avec de multiples arguments (ex: `ls -l -a /home`).
* **Commandes internes (Built-ins) :**
    * `cd [dossier]` : Change le répertoire courant.
    * `pwd` : Affiche le chemin du répertoire courant.
    * `exit` : Quitte le minishell proprement.
* **Gestion des processus :** Utilisation de `fork()`, `execvp()`, et `wait()` pour la création et la synchronisation des processus enfants.

## 🛠️ Compilation

Le projet utilise un `Makefile` pour faciliter la compilation. Pour compiler le minishell, placez-vous à la racine du projet et tapez :

```bash
make
