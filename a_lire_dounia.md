# 06.07.26

COUCOU

jai juste recreer un repo propre, et jai juste une branche chloe pour linstant, ya tout sur main la pour linstant, ya pas grand chose donc tu peux add ou modifier ce que tu veux a la racine en vrai.
et jai fait qql .md avec qql infos, pour linstant jai pas commence a tester vrm des trucs avc du code, tu peux lire les .md mais jai juste vu ce que les commandes qui nous etaient demandee faisaient etcccc bref ta capte. xoxo


# 09.07.26


voila jai fait quoi ajd mdr c pas grand chose mais voila :
- Refait `organisation.md` avec une repartition plus claire: Chloe parsing, Dounia execution, et parties communes a definir ensemble.
- Ajoute les includes utiles dans `include/minishell.h` pour les fonctions autorisees du sujet, avec des petits commentaires sur le cote.
- Ajoute les premieres structs de parsing dans le `.h`: `t_token_type` et `t_token`.
- Ajoute les premiers prototypes pour les tokens: `new_token` et `add_token_back`.
- Cree un `Makefile` racine minimal qui compile avec `-Wall -Wextra -Werror`, compile aussi la libft, met les `.o` dans `objs/`, et evite les relinks inutiles.
- Modifie le `Makefile` de la libft pour mettre ses `.o` dans `libft/objs/`.
- Ajoute un `.gitignore` pour ne pas commit les fichiers generes par `make`.
- Commence la partie parsing avec `srcs/parsing/tokens_utils.c`.
- Code `new_token`: cree un token, copie sa value, met `next` a `NULL`.
- Code `add_token_back`: ajoute un token a la fin de la liste chainee.

et juste la je push tout sur main pcq je pense pas tu vas bc coder dici que tu reviennes donc jme suis dit c pas grave, et dans le main.c ya juste des trucs que jai besoin pour avancer au debut mes trucs, mais jme permet pcq jpense tu vas pas bc coder comme jai diiiit voilaaaa