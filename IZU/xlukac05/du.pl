%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 2. Uloha do predmetu IZU na VUT FIT 2011/2012          %
% Autor: Michal Lukac, xlukac05@stud.fit.vutbr.cz        %
% Datum: 11.4.2012                                       %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%delka linearniho seznamu
delka([],0).
delka([_|T],S) :- delka(T,SS), S is SS + 1.

%je prvek clenem lin. seznamu?
jePrvek([X|_],X).
jePrvek([_|T],X) :- jePrvek(T,X). 

%spojeni dvou linearnich seznamu
spoj([],L,L).
spoj([H|T],L,[H|TT]) :- spoj(T,L,TT).

%doplnte nasledujici predikaty:
% Vytvorte funkci pro rozdeleni linearniho seznamu na poloviny
divide_half(S,S1,S2) :- spoj(S1,S2,S), delka(S1,X), delka(S2,Y), Z is Y + 1, ((X = Z); X = Y).

% Vytvorte funkci pro odstraneni obecneho prvku z obecneho seznamu
remove_item_GEN([],_,[]).
remove_item_GEN([H|T],H,S) :- remove_item_GEN(T,H,S), !.
remove_item_GEN([H|T],X,[K|S]) :- is_list(H), remove_item_GEN(H,X,K), remove_item_GEN(T,X,S), !.
remove_item_GEN([H|T],X,[H|S]) :- remove_item_GEN(T,X,S), !.

% Vytvorte funkci pro reverzaci obecneho seznamu
reverse_GEN([],[]).
reverse_GEN([H|T],S) :- (is_list(H) -> (reverse_GENL(H,X),reverse_GEN(T,Y),spoj(Y,X,S));(reverse_GEN(T,X),spoj(X,[H],S))), !.
reverse_GEN(H,H).
reverse_GENL([H|T],[Y]) :- reverse_GEN(H,K), reverse_GEN(T,S), spoj(S,[K],Y), !.  
reverse_GENL(H,H).  

% Vytvorte funkci pro vlozeni prvku na n-tou pozici linearniho seznamu
insert_pos([],[],[],[]).
insert_pos([],_,C,[C]).
insert_pos(A,IN,IT,K) :- delka(A,X), X < IN, spoj(A,[IT],K), !. 
insert_pos(A,B,C,D) :- insert_pos1(A,B,C,D,1), !.
insert_pos1([H1|T1],IN,IT,K,AT) :- X is AT + 1,
                                (AT < IN -> (insert_pos1(T1,IN,IT,L2,X),spoj([],H1,C2),spoj([C2],L2,K));
                                (insert_pos2(T1,IN,IT,L2),spoj([H1],[IT],C),reverse_GEN(C,C2),spoj(C2,L2,K))).
insert_pos2([H1|T1],IN,IT,[H1|S]) :- insert_pos2(T1,IN,IT,S).
insert_pos2([],_,_,[]).

% Vytvorte funkci pro inkrementaci kazdeho prvku obecneho seznamu o hodnotu hloubky zanoreni prvku
increment_general([],[]).
increment_general([H|T],S) :- (is_list(H) -> (inc_gen(H,J,2));(inc_gen(H,J,1))), inc_gen(T,K,1),spoj([J],K,S) ,!.
inc_gen([H|T],S,X) :- (is_list(H) -> (V is X + 1,inc_gen(H,J,V));(inc_gen(H,J,X))),inc_gen(T,K,X),spoj([J],K,S) ,!.
inc_gen([],B,_):- spoj([],[],B).
inc_gen(P,S,X) :- S is P + X.
