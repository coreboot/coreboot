team('Google', 'adurbin@chromium.org').
team('Google', 'adurbin@google.com').
team('Google', 'dhendrix@chromium.org').
team('Google', 'dhendrix@google.com').
team('Google', 'dlaurie@chromium.org').
team('Google', 'dlaurie@google.com').
team('Google', 'gabeblack@chromium.org').
team('Google', 'gabeblack@google.com').
team('Google', 'hungte@chromium.org').
team('Google', 'hungte@google.com').
team('Google', 'jeremyt@chromium.org').
team('Google', 'nsylvain@google.com').
team('Google', 'rminnich@google.com').
team('Google', 'shawnn@chromium.org').
team('Google', 'vapier@chromium.org').
team('Google', 'vbendeb@chromium.org').
team('Google', 'venkateshs@google.com').
team('Google', 'vpalatin@chromium.org').
team('Google', 'yjlou@chromium.org').

same_team(Email1, Email2) :-
  team(Team, Email1),
  team(Team, Email2).

submit_rule(S) :-
  gerrit:default_submit(DefaultResults),
  DefaultResults =.. [submit | Inner],
  check_author_approval(Inner, Result),
  S =.. [submit | Result].

%% same userid, even if name/email are different, is ok.
check_author_approval(In, Out) :-
  gerrit:commit_author(Author),
  gerrit:commit_committer(Committer),
  Committer = Author,
  Author \= user(anonymous), !,
  Out = In.

%% same team is ok.
check_author_approval(In, Out) :-
  gerrit:commit_author(_, _, AuthorEmail),
  gerrit:commit_committer(_, _, CommitterEmail),
  same_team(AuthorEmail, CommitterEmail), !,
  Out = In.

%% different name/email, author did give positive review
check_author_approval(In, Out) :-
  gerrit:commit_author(Author),
  gerrit:commit_label(label('Code-Review', Review), Reviewer),
  Reviewer = Author,
  Review >= 1, !,
  Out = [label('Author-Must-Review', ok(Author)) | In].

%% different name/email, author did not give positive review
check_author_approval(In, Out) :-
  gerrit:commit_author(Author),
  gerrit:commit_committer(Committer),
  Committer \= Author,
  Out = [label('Author-Must-Review', need(Author)) | In].

