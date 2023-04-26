not_member(_, []).
not_member(X, [H|T]) :- dif(X, H), not_member(X, T).
follows(A, B, Seen) :- not_member(B, Seen), follows(A, B).
follows(A, B, Seen) :- follows(A, X), not_member(X, Seen), follows(X, B, [A|Seen]).
follows_tx(A, B) :- follows(A, B, []).
followers(Name, L) :- setof(P, follows_tx(Name, P), L).
num_followers(Name, N) :- setof(P, follows_tx(Name, P), L), length(L, N).
