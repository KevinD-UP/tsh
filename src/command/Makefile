CC=gcc 
CFLAGS=-g	-Wall

ALL= tsh pwd rm mkdir ls touch mv rmdir cat cp

all : $(ALL)

tsh : loop.o cd.o exit.o analyse_lexicale.o in_tar.o print.o launcher.o tar.o 
		gcc loop.o cd.o exit.o analyse_lexicale.o in_tar.o print.o launcher.o tar.o -o tsh -lreadline

pwd : pwd_tar.o print.o
		gcc pwd_tar.o print.o -o pwd

rm : rm_tar.o print.o in_tar.o tar.o fonction_aux.o
		gcc rm_tar.o print.o in_tar.o tar.o fonction_aux.o -o rm

mkdir : mkdir_tar.o print.o in_tar.o tar.o fonction_aux.o
		gcc mkdir_tar.o print.o in_tar.o tar.o fonction_aux.o -o mkdir

ls : ls_tar.o print.o in_tar.o tar.o
		gcc ls_tar.o print.o in_tar.o tar.o -o ls

cat : cat_tar.o print.o in_tar.o tar.o fonction_aux.o
		gcc cat_tar.o print.o in_tar.o tar.o fonction_aux.o -o cat

touch : touch_tar.o print.o in_tar.o tar.o fonction_aux.o
		gcc touch_tar.o print.o in_tar.o tar.o fonction_aux.o -o touch

rmdir : rmdir_tar.o print.o in_tar.o tar.o fonction_aux.o
		gcc rmdir_tar.o print.o in_tar.o tar.o fonction_aux.o -o rmdir 

mv : mv_tar.o print.o in_tar.o tar.o fonction_aux.o
		gcc mv_tar.o print.o in_tar.o tar.o fonction_aux.o -o mv

cp : cp_tar.o print.o in_tar.o tar.o
		gcc cp_tar.o print.o in_tar.o tar.o fonction_aux.o -o cp

loop.o : loop.c ../header/analyse_lexicale.h ../header/print.h
cd.o : cd.c ../header/cd.h ../header/in_tar.h ../header/print.h 
exit.o : exit.c ../header/exit.h
in_tar.o : in_tar.c ../header/in_tar.h
print.o : print.c ../header/print.h
launcher.o : launcher.c ../header/launcher.h 
analyse_lexicale.o : analyse_lexicale.c ../header/analyse_lexicale.h
ls_tar.o : ls_tar.c ../header/ls_tar.h
cat_tar.o : cat_tar.c ../header/cat_tar.h 
tar.o : tar.c ../header/tar.h
mkdir_tar.o : mkdir_tar.c ../header/mkdir_tar.h
touch_tar.o : touch_tar.c ../header/touch_tar.h
pwd_tar.o : pwd_tar.c ../header/pwd_tar.h 
rm_tar.o : rm_tar.c ../header/rm_tar.h
rmdir_tar.o : rmdir_tar.c ../header/rmdir_tar.h
mv_tar.o : mv_tar.c ../header/mv_tar.h
cp_tar.o : cp_tar.c ../header/cp_tar.h
fonction_aux.o : fonction_aux.c ../header/fonction_aux.h

clean:
	rm -rf *.o *~  $(ALL)
